#include "clock.h"

#include <QDateTime>
#include <QTimer>

Clock * Clock::instance_ = 0;

Clock::Clock()
    :  offsetInMSecs_(0)
{
    if (instance_ == 0) instance_ = this;
    init();
}

Clock::Clock(const QDateTime &now)
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
    dateChangeTimer_ = new QTimer(this);
    connect(dateChangeTimer_, SIGNAL(timeout()), this, SLOT(handleDateChange()));
    dateChangeTimer_->setSingleShot(true);
    dateChangeTimer_->start(msecsTillTomorrow());
}

qint64 Clock::msecsTillTomorrow() const
{
    QDateTime now = currentDateTime();
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
    // restart timer
    dateChangeTimer_->start(msecsTillTomorrow());

    // inform clients
    emit dateChanged(currentDate());
}
