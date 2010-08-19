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
#if QT_VERSION >= 0x040700
    : offsetInMSecs_(QDateTime::currentDateTime().msecsTo(now))
#else
    : offsetInMSecs_(QDateTime::currentDateTime().secsTo(now)*1000)
#endif
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
    QDateTime now = currentDateTime();
#if QT_VERSION >= 0x040700
    return now.msecsTo(QDateTime(now.date().addDays(1)));
#else
    return now.secsTo(QDateTime(now.date().addDays(1)))*1000;
#endif
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
