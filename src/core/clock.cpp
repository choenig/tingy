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
#include "clock.h"

#include <QDateTime>
#include <QTimer>

Clock * Clock::instance_ = 0;

Clock::Clock()
    : offsetInMSecs_(0)
{
    if (instance_ == 0) instance_ = this;
    init();
}

Clock::Clock(const QDateTime & now)
    : offsetInMSecs_(QDateTime::currentDateTime().msecsTo(now))
{
    if (instance_ == 0) instance_ = this;
    init();
}

Clock::~Clock()
{
    if (instance_ == this) instance_ = 0;
}

void Clock::init()
{
    lastDate_ = currentDate();

    dateChangeTimer_ = new QTimer(this);
    connect(dateChangeTimer_, SIGNAL(timeout()), this, SLOT(handleDateChange()));
    dateChangeTimer_->setSingleShot(true);
    dateChangeTimer_->start(msecsTillTomorrow());

    QTimer * highFreqTimer = new QTimer(this);
    connect(highFreqTimer, SIGNAL(timeout()), this, SLOT(handleDateChange()));
    highFreqTimer->start(60*1000);
}

qint64 Clock::msecsTillTomorrow() const
{
    const QDateTime now = currentDateTime();
    return now.msecsTo(QDateTime(now.date().addDays(1)));
}

QDateTime Clock::currentDateTime()
{
    return QDateTime::currentDateTime().addMSecs(instance_->offsetInMSecs_);
}

QDate Clock::currentDate()
{
    return currentDateTime().date();
}

QTime Clock::currentTime()
{
    return currentDateTime().time();
}

void Clock::handleDateChange()
{
    if (currentDate() == lastDate_) return;

    lastDate_ = currentDate();

    // restart timer
    dateChangeTimer_->start(msecsTillTomorrow());

    // inform clients
    emit dateChanged(currentDate());
}
