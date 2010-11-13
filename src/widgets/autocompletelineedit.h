/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of Tingy.
**
** Tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with Tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#pragma once

#include <widgets/lineedit.h>

class CalendarPopup;
class QDate;
class TextPopup;

class AutocompleteLineEdit : public LineEdit
{
    Q_OBJECT
public:
    AutocompleteLineEdit(QWidget * parent = 0);

private slots:
    void onTextChanged(const QString & text);
    void doneTextCompletion(const QString & text);
    void doneCalCompletion(const QDate & date);

private:
    TextPopup * textPopup_;
    CalendarPopup * calPopup_;
};
