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
#include "calendarpopup.h"

#include <core/clock.h>

#include <QDateTime>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>

#include "ui_calendarpopup.h"

CalendarPopup::CalendarPopup(QWidget * parent, int colCount, int rowCount)
    : QWidget(parent), colCount_(colCount), rowCount_(rowCount), ui(new Ui::CalendarPopup)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);
    setFocusPolicy(Qt::NoFocus);
    setFocusProxy(parent);

    reset();
}

CalendarPopup::~CalendarPopup()
{
    delete ui;
}

void CalendarPopup::reset()
{
    const QDate now = Clock::currentDate();

    updateVisibleDates(now.addDays(-colCount_));

    // select today
    selectedIdx_ = QPoint();
    moveSelectedIndex(now.dayOfWeek() - 1 + colCount_);
}

void CalendarPopup::updateVisibleDates(const QDate & dateInFirstRow)
{
    currentStartDate_ = dateInFirstRow.addDays( -(dateInFirstRow.dayOfWeek()-1) );

    // init dates
    const QDate now = Clock::currentDate();
    for (int r = 0; r < rowCount_; ++r)  {
        for (int c = 0; c < colCount_; ++c)
        {
            const QDate date = currentStartDate_.addDays(r*colCount_ + c);
            QLabel * lbl = getDateLabel(QPoint(c, r));

            // set text
            lbl->setText(date.toString("d"));

            // set font
            QFont f = lbl->font();
            f.setBold(date == now);
            lbl->setFont(f);
        }
    }

    // update kalender wochen
    const int kw0 = dateInFirstRow.weekNumber();
    for (int r = 0; r < rowCount_; ++r) {
        getKWLable(QPoint(0,r))->setText(QString::number(kw0 + r));
    }
}

void CalendarPopup::moveSelectedIndex(int offsetInDays)
{
    int newX = selectedIdx_.x() + offsetInDays % colCount_;
    int newY = selectedIdx_.y() + offsetInDays / colCount_;

    if (newX < 0) {
        newY -= 1;
        newX += colCount_;
    }
    if (newX > colCount_-1) {
        newY += 1;
        newX -= colCount_;
    }

    if (newY < 0)          updateVisibleDates(currentStartDate_.addDays(newY * colCount_));
    if (newY > rowCount_-1) updateVisibleDates(currentStartDate_.addDays((newY-rowCount_+1) * colCount_));

    selectedIdx_ = QPoint(qMax(0, qMin(colCount_-1, newX)), qMax(0, qMin(rowCount_-1, newY)));

    // update selected date
    selectedDate_ = currentStartDate_.addDays(selectedIdx_.x() + selectedIdx_.y()*colCount_);
    ui->lblSelectedDate->setText(QLocale().toString(selectedDate_, "dd.  MMMM  yyyy"));

    // update label colors
    for (int r = 0; r < rowCount_; ++r)  {
        for (int c = 0; c < colCount_; ++c)
        {
            const QDate date = currentStartDate_.addDays(r*colCount_ + c);
            QLabel * lbl = getDateLabel(QPoint(c, r));

            // set text color
            QPalette pal = lbl->palette();
            if (date.month() != selectedDate_.month()) pal.setColor(QPalette::Foreground, Qt::darkGray);
            else if (date.dayOfWeek() >= 6)            pal.setColor(QPalette::Foreground, Qt::darkBlue);
            else                                       pal.setColor(QPalette::Foreground, Qt::black);
            lbl->setPalette(pal);
        }
    }
}

void CalendarPopup::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    // draw background of headlines and KW
    {
        QRegion region;
        region += ui->lblSelectedDate->geometry().united(ui->lblDayOfWeek1->geometry());       // Mo-Sa
        region += ui->lblKW0->geometry().united(ui->lblKW4->geometry());                       // KWs
        for (int r = 0; r < rowCount_; ++r) region += getDateLabel(QPoint(5, r))->geometry();   // Saturdays
        for (int r = 0; r < rowCount_; ++r) region += getDateLabel(QPoint(6, r))->geometry();   // Sundays
        QPainterPath pp; pp.addRegion(region);
        p.fillPath(pp, Qt::lightGray);
    }

    // highlight selection
    {
        QRegion region;
        for (int c = 0; c < colCount_; ++c) region += getDateLabel(QPoint(c, selectedIdx_.y()))->geometry(); // row
        for (int r = 0; r < rowCount_; ++r) region += getDateLabel(QPoint(selectedIdx_.x(), r))->geometry(); // col
        region += getKWLable(selectedIdx_)->geometry();
        region += getDayOfWeekLable(selectedIdx_)->geometry();
        QPainterPath pp; pp.addRegion(region);
        p.fillPath(pp, QColor(0xff,0xff,0xff,0x96));
    }

    // hilight selected date
    p.drawRect( getDateLabel(selectedIdx_)->geometry() );

    QWidget::paintEvent(event);
}

void CalendarPopup::keyPressEvent(QKeyEvent * keyEvent)
{
    int moveSelectedDate = 0;

    switch (keyEvent->key()) {
    case Qt::Key_Down:
        moveSelectedDate = 7;
        break;
    case Qt::Key_Up:
        moveSelectedDate = -7;
        break;
    case Qt::Key_Right:
        moveSelectedDate = 1;
        break;
    case Qt::Key_Left:
        moveSelectedDate = -1;
        break;
    case Qt::Key_PageDown:
        moveSelectedDate =  7 * 4;
        break;
    case Qt::Key_PageUp:
        moveSelectedDate = -7 * 4;
        break;
    case Qt::Key_Home:
        reset();
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        emit dateSelected(selectedDate_);
        // fall through
    case Qt::Key_Escape:
        hide();
        return;

    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Alt:
    case Qt::Key_Meta:
    case Qt::Key_Mode_switch:
        // modifiers should not close the popup
        break;

    default:
        hide();
        parent()->event(keyEvent); // propagate the last keyEvent to our parent, so it doesn't get lost
    }

    if (moveSelectedDate != 0) {
        moveSelectedIndex(moveSelectedDate);
        update();
    }
}

void CalendarPopup::wheelEvent(QWheelEvent * wheelEvent)
{
    QWidget::wheelEvent(wheelEvent);
    moveSelectedIndex((wheelEvent->delta() > 0 ? -1 : 1) * 7 );
    update();
}

QLabel * CalendarPopup::getDateLabel(QPoint pos)
{
    switch (pos.x() + pos.y()*colCount_) {
    case  0: return ui->lblDay0;
    case  1: return ui->lblDay1;
    case  2: return ui->lblDay2;
    case  3: return ui->lblDay3;
    case  4: return ui->lblDay4;
    case  5: return ui->lblDay5;
    case  6: return ui->lblDay6;
    case  7: return ui->lblDay7;
    case  8: return ui->lblDay8;
    case  9: return ui->lblDay9;
    case 10: return ui->lblDay10;
    case 11: return ui->lblDay11;
    case 12: return ui->lblDay12;
    case 13: return ui->lblDay13;
    case 14: return ui->lblDay14;
    case 15: return ui->lblDay15;
    case 16: return ui->lblDay16;
    case 17: return ui->lblDay17;
    case 18: return ui->lblDay18;
    case 19: return ui->lblDay19;
    case 20: return ui->lblDay20;
    case 21: return ui->lblDay21;
    case 22: return ui->lblDay22;
    case 23: return ui->lblDay23;
    case 24: return ui->lblDay24;
    case 25: return ui->lblDay25;
    case 26: return ui->lblDay26;
    case 27: return ui->lblDay27;
    case 28: return ui->lblDay28;
    case 29: return ui->lblDay29;
    case 30: return ui->lblDay30;
    case 31: return ui->lblDay31;
    case 32: return ui->lblDay32;
    case 33: return ui->lblDay33;
    case 34: return ui->lblDay34;
    default:
        return 0;
    }
}

QLabel * CalendarPopup::getKWLable(QPoint pos)
{
    switch (pos.y()) {
    case  0: return ui->lblKW0;
    case  1: return ui->lblKW1;
    case  2: return ui->lblKW2;
    case  3: return ui->lblKW3;
    case  4: return ui->lblKW4;
    default: return 0;
    }
}

QLabel * CalendarPopup::getDayOfWeekLable(QPoint pos)
{
    switch (pos.x()) {
    case  0: return ui->lblDayOfWeek1;
    case  1: return ui->lblDayOfWeek2;
    case  2: return ui->lblDayOfWeek3;
    case  3: return ui->lblDayOfWeek4;
    case  4: return ui->lblDayOfWeek5;
    case  5: return ui->lblDayOfWeek6;
    case  6: return ui->lblDayOfWeek7;
    default: return 0;
    }
}
