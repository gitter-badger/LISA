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

#ifndef LOGBOOKWINDOW_H
#define LOGBOOKWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include "logbookproxymodel.h"

#include <QtSql>

namespace Logbook {
namespace Internal {

class LogbookWindow
        : public QWidget
{
    Q_OBJECT

public:
    LogbookWindow(QWidget *parent = 0);
    ~LogbookWindow();

private:
    QTableView* m_logbookView;

    QSqlDatabase m_database;
    QSqlRelationalTableModel* m_model;
    LogbookProxyModel* m_proxyModel;
};

} // namespace Internal
} // namespace Logbook

#endif // LOGBOOKWINDOW_H
