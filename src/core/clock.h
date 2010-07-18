#pragma once

class QDateTime;
class QDate;
class QTime;

class Clock
{
public:
    static void init(const QDateTime & now);

    static QDateTime currentDateTime();
    static QDate currentDate();
    static QTime currentTime();

    // returns the amount of msecs that are left today
    static int msecsTillTomorrow();
};
