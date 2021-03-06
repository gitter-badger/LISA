/***********************************************************************
 *
 * LISA: Lightweight Integrated System for Amateur Radio
 * Copyright (C) 2013 - 2014
 *      Norman Link (DM6LN)
 *
 * This file is part of LISA.
 *
 * LISA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LISA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You can find a copy of the GNU General Public License in the file
 * LICENSE.GPL contained in the root directory of this project or
 * under <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/

#include "database.h"
#include <QDebug>
#include "qsoentry.h"

using namespace Logbook::Internal;

Database::Database(QObject* parent)
   : QObject(parent)
{
    m_database = QSqlDatabase::addDatabase(QString::fromLatin1("QSQLITE"));
    m_database.setHostName(QString::fromLatin1("localhost"));

    // TODO: store in appdata
    m_database.setDatabaseName(QString::fromLatin1("logbook.sql"));
}

bool Database::open()
{
    // open database
    bool isOpen = m_database.open();

    if (isOpen) {
        // create necessary tables if they do not exist
        createTables();

        /*QSqlTableModel model(this, m_database);
        model.setTable(QString::fromLatin1("logbook"));

        int row = 0;
        model.insertRows(row, 1);
        model.setData(model.index(row, 0), 03544);
        model.setData(model.index(row, 1), QString::fromLatin1("DM6LN"));
        model.setData(model.index(row, 2), QString::fromLatin1("DM5RS"));
        if (!model.submitAll()) {
            QSqlError err = model.lastError();
            qDebug() << err.text();
        }*/

        QSqlTableModel model(this, m_database);
        model.setTable(QString::fromLatin1("logbook"));
        model.select();

        for (int i = 0; i < model.rowCount(); i++) {
            const QSqlRecord& record = model.record(i);

            for (int j = 0; j < record.count(); j++) {
                qDebug() << "Field " << j << ": " << record.field(j).value();
            }
        }
    }

    //QSqlQuery query(m_database);

    //QsoEntry newEntry(this);
    //newEntry.setProperty("CallsignFrom", "DM6LN");
    //newEntry.CallsigTo = "DM5RS";

    QsoEntry newEntry(this);
    newEntry.setId(0);
    newEntry.setCallsign(QString::fromLatin1("DM5RS"));
    newEntry.setOperator(QString::fromLatin1("DM6LN"));

    updateOrInsert(newEntry);

    return isOpen;
}

void Database::createTables()
{
    QSqlQuery query(m_database);

    //QStringList tables = m_database.tables();

    // create table if it does not already exist
    if (!query.exec(QString::fromLatin1("CREATE TABLE IF NOT EXISTS logbook ("
                                              "ID INTEGER PRIMARY KEY,"
                                              "CallsignFrom VARCHAR(32),"
                                              "CallsignTo VARCHAR(32)"
                                              ")"))) {
        qWarning() << "Could not create table";
    }
}

void Database::updateOrInsert(const QsoEntry& entry)
{
    const QSqlRecord& record = entry.getRecord();

    QSqlTableModel model(this, m_database);
    model.setTable(QString::fromLatin1("logbook"));

    // first, check if this entry already exists

}

void Database::close()
{
    m_database.close();
}
