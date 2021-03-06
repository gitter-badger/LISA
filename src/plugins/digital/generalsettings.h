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

#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <QPointer>

namespace Digital {
namespace Internal {

namespace Ui {
    class GeneralSettings;
}

class GeneralSettings
        : public Core::IOptionsPage
{
    Q_OBJECT

public:
    GeneralSettings();

    bool matches(const QString& searchKeyWord) const;
    QWidget* widget();
    void apply();
    void finish();

private:
    Ui::GeneralSettings* m_page;
    QPointer<QWidget> m_widget;
};

} // namespace Internal
} // namespace Digital

#endif // GENERALSETTINGS_H
