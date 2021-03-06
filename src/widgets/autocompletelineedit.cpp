/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#include "autocompletelineedit.h"

#include <widgets/calendarpopup.h>
#include <widgets/textpopup.h>

#include <QPainter>

AutocompleteLineEdit::AutocompleteLineEdit(QWidget *parent)
    : LineEdit(parent)
{
    textPopup_ = new TextPopup(this);
    connect(textPopup_, SIGNAL(textSelected(QString)), this, SLOT(doneTextCompletion(QString)));

    calPopup_ = new CalendarPopup(this);
    connect(calPopup_, SIGNAL(dateSelected(QDate)), this, SLOT(doneCalCompletion(QDate)));

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
}

void AutocompleteLineEdit::onTextChanged(const QString & text)
{
    QWidget * wdgt = 0;


    if (text.endsWith("!")) {
        textPopup_->setChoices(QStringList() << "+" << "-", QRegExp("[+-]"));
        wdgt = textPopup_;
    } else if (text.endsWith("$") || text.endsWith("*+")) {
        textPopup_->setChoices(QStringList() << "m" << "h" << "d" << "w" << "y", QRegExp("[0-9]+([mhdwy]?[0-9]*)*"));
        wdgt = textPopup_;
    } else if (text.endsWith("*") || text.endsWith("@")) {
        calPopup_->reset();
        wdgt = calPopup_;
    }

    if (!wdgt) return;

    QFontMetrics fm(font());
    int cursorPosPx = fm.width(this->text()) + 7;
    wdgt->move(this->mapToGlobal(QPoint(cursorPosPx, this->height() - 4)));
    wdgt->setFocus();
    wdgt->show();
}

void AutocompleteLineEdit::doneTextCompletion(const QString & txt)
{
    // do not append the selected text, if its already there
    // dunno if this is a good solution
    if (text().endsWith(txt)) {
        setText(text() + " ");
    } else {
        setText(text() + txt + " ");
    }
}

void AutocompleteLineEdit::doneCalCompletion(const QDate &date)
{
    setText(text() + date.toString("dd.MM.yyyy") + " ");
}

