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
#pragma once

#include <QObject>
#include <QDate>

class QTime;
class QTimer;

class Clock : public QObject
{
    Q_OBJECT
public:
    Clock();
    Clock(const QDateTime & now);
    ~Clock();

    static Clock * instance() { return instance_; }

public:
    static QDateTime currentDateTime();
    static QDate currentDate();
    static QTime currentTime();

signals:
    void dateChanged(const QDate & date);

private slots:
    void handleDateChange();

private:
    qint64 msecsTillTomorrow() const;
    void init();

private:
    qint64 offsetInMSecs_;
    QTimer * dateChangeTimer_;
    QDate lastDate_;

private:
    static Clock * instance_;
};
