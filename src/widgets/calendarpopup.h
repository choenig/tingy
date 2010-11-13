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

#include <QWidget>
#include <QDate>

class QLabel;

namespace Ui { class CalendarPopup; }

class CalendarPopup : public QWidget
{
    Q_OBJECT

public:
    CalendarPopup(QWidget * parent, int colCount = 7, int rowCount = 5);
    ~CalendarPopup();

    void reset();

signals:
    void dateSelected(const QDate & date);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void keyPressEvent(QKeyEvent * keyEvent);
    virtual void wheelEvent(QWheelEvent * wheelEvent);

private:
    void updateVisibleDates(const QDate & dateInFirstRow);
    void moveSelectedIndex(int days);
    QLabel * getDateLabel(QPoint pos);
    QLabel * getKWLable(QPoint pos);
    QLabel * getDayOfWeekLable(QPoint pos);

private:
    QPoint selectedIdx_;
    QDate  selectedDate_;
    QDate  currentStartDate_;
    int    colCount_;
    int    rowCount_;


private:
    Ui::CalendarPopup * ui;
};
