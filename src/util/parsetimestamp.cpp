#include "parsetimestamp.h"

#include <QDateTime>
#include <QRegExp>
#include <QString>
#include <QDebug>

namespace {

qint32 parseTimeDef(const QString & string)
{
    QRegExp reTimedef("(([0-9]*)y)? *(([0-9]*)w)? *(([0-9]*)d)? *(([0-9]*)h)? *(([0-9]*)m)? *");

    if (!reTimedef.exactMatch(string)) return -1;

    qint32 secs = 0;
    secs += reTimedef.cap( 2).toInt() * 60 * 60 * 24 * 365;
    secs += reTimedef.cap( 4).toInt() * 60 * 60 * 24 * 7;
    secs += reTimedef.cap( 6).toInt() * 60 * 60 * 24;
    secs += reTimedef.cap( 8).toInt() * 60 * 60;
    secs += reTimedef.cap(10).toInt() * 60;
    return secs;
}

}

QDate parseDate(const QString & string)
{
    const QDate today = QDate::currentDate();

    if (string.isEmpty()                    ||
        QString("today").startsWith(string) ||
        QString("heute").startsWith(string)) return today;

    if (QString("tomorrow").startsWith(string) ||
        QString("morgen").startsWith(string)) return today.addDays(1);

    if (string.startsWith("+")) {
        int secs = parseTimeDef(string.mid(1));
        if (secs >= 0) {
            return today.addDays(secs / 3600 / 24);
        }
    }

    return QDate();
}

QTime parseTime(const QString & string)
{
    int secs = parseTimeDef(string);
    if (secs >= 0) {
        return QTime().addSecs(secs);
    }
    return QTime();
}

