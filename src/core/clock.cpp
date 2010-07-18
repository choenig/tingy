#include "clock.h"

#include <QDateTime>

namespace {
qint64 offsetInMSecs = 0;
}

void Clock::init(const QDateTime & now)
{
    offsetInMSecs = QDateTime::currentDateTime().msecsTo(now);
}

QDateTime Clock::currentDateTime()
{
    return QDateTime::currentDateTime().addMSecs(offsetInMSecs);
}

QDate Clock::currentDate()
{
    return currentDateTime().date();
}

QTime Clock::currentTime()
{
    return currentDateTime().time();
}

int Clock::msecsTillTomorrow()
{
    const QDateTime now = currentDateTime();
    return now.msecsTo(QDateTime(now.date().addDays(1)));
}
