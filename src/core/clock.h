#pragma once

#include <QObject>

class QDateTime;
class QDate;
class QTime;

class Clock : public QObject
{
    Q_OBJECT
public:

    static void init(const QDateTime & now);

    static QDateTime currentDateTime();
    static QDate currentDate();
    static QTime currentTime();

    static int msecsTillTomorrow();
};
