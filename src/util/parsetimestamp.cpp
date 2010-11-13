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
#include "parsetimestamp.h"

#include <core/clock.h>
#include <core/effort.h>

#include <QDateTime>
#include <QRegExp>
#include <QString>


QDate parseDate(const QString & string)
{
    const QDate today = Clock::currentDate();

    if (QString("today").startsWith(string) ||
        QString("heute").startsWith(string)) return today;

    if (QString("tomorrow").startsWith(string) ||
        QString("morgen").startsWith(string)) return today.addDays(1);

    if (string.startsWith("+") || string.startsWith("-")) {
        int mins = parseYWDHMTime(string.mid(1));
        if (mins >= 0) {
            return today.addDays((string.startsWith("+")?1:-1) * mins / 60 / 24);
        }
    }

    // accepts a date like 12. or 12.03. or 12.03.1978
    QRegExp reDate("([0-9]{1,2})\\.(([0-9]{1,2})\\.(([0-9]{2,4}))?)?");
    if (reDate.exactMatch(string)) {
        int day   = reDate.cap(1).toInt();
        int month = reDate.cap(3).toInt();
        int year  = reDate.cap(4).toInt();

        if (day   == 0) day   = today.day();
        if (month == 0) month = today.month();
        if (year  == 0) year  = today.year();

        return QDate(year, month, day);
    }

    return QDate();
}

qint32 parseYWDHMTime(const QString & string)
{
    QRegExp reTimedef("(([0-9]*)y)? *(([0-9]*)w)? *(([0-9]*)d)? *(([0-9]*)h)? *(([0-9]*)m)? *");

    if (!reTimedef.exactMatch(string)) return -1;

    qint32 mins = 0;
    mins += reTimedef.cap( 2).toInt() * 60 * 24 * 365;
    mins += reTimedef.cap( 4).toInt() * 60 * 24 * 7;
    mins += reTimedef.cap( 6).toInt() * 60 * 24;
    mins += reTimedef.cap( 8).toInt() * 60;
    mins += reTimedef.cap(10).toInt();
    return mins;
}

