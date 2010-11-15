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
#include "textpopup.h"

#include <QLineEdit>
#include <QHeaderView>
#include <QKeyEvent>
#include <QDebug>

int initialCursorPos;

TextPopup::TextPopup(QWidget *parent) :
    QTreeWidget(parent)
{
    setWindowFlags(Qt::Popup);
    setFocusPolicy(Qt::NoFocus);
    setFocusProxy(parent);
    setMouseTracking(true);
    setColumnCount(1);
    setUniformRowHeights(true);
    setRootIsDecorated(false);
    setEditTriggers(QTreeWidget::NoEditTriggers);
    setSelectionBehavior(QTreeWidget::SelectRows);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    header()->hide();

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(doneTextCompletion()));
}

void TextPopup::setChoices(const QStringList & choices, const QRegExp & re)
{
    reValidationMask_ = re;
    if (reValidationMask_.isValid()) {
        initialCursorPos = static_cast<QLineEdit*>(parentWidget())->cursorPosition();
    }

    setUpdatesEnabled(false);
    clear();
    for (int i = 0; i < choices.count(); ++i) {
        QTreeWidgetItem * item = new QTreeWidgetItem(this);
        item->setText(0, choices[i]);
    }
    setCurrentItem(topLevelItem(0));
    resizeColumnToContents(0);
    adjustSize();
    setUpdatesEnabled(true);

    int h = sizeHintForRow(0) * qMin(7, choices.count()) + 3;
    resize(width(), h);
}

void TextPopup::keyPressEvent(QKeyEvent * event)
 {
     bool consumed = false;

     int key = static_cast<QKeyEvent*>(event)->key();
     switch (key) {
     case Qt::Key_Enter:
     case Qt::Key_Return:
         doneTextCompletion();

     case Qt::Key_Escape:
         hide();
         consumed = true;

     case Qt::Key_Shift:
     case Qt::Key_Control:
     case Qt::Key_Alt:
     case Qt::Key_Meta:
     case Qt::Key_Mode_switch:
         // modifiers should not close the popup
         break;

     case Qt::Key_Up:
     case Qt::Key_Down:
     case Qt::Key_Home:
     case Qt::Key_End:
     case Qt::Key_PageUp:
     case Qt::Key_PageDown:
         break;

     default: {
         parent()->event(event);

         QLineEdit * ple = static_cast<QLineEdit*>(parentWidget());
         QString t = ple->text().mid(initialCursorPos, ple->cursorPosition());
         if (!reValidationMask_.exactMatch(t))
            hide();
         break;
     }
     }

     if (!consumed) {
         QTreeWidget::keyPressEvent(event);
     }
}

void TextPopup::doneTextCompletion()
 {
     hide();
     QTreeWidgetItem *item = currentItem();
     if (item) emit textSelected(item->text(0));
 }
