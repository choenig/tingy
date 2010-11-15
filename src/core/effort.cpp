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
#include "effort.h"

#include <util/parsetimestamp.h>

#include <QStringList>

Effort::Effort(quint32 minutes)
    : effortInMinutes_(minutes)
{
}

QString Effort::toString() const
{
    const int d = effortInMinutes_ / (60 * 24);
    const int h = effortInMinutes_ % (60 * 24) / 60;
    const int m = effortInMinutes_ %             60;

    QStringList retval;
    if (d > 0) retval << QString("%1d").arg(d);
    if (h > 0) retval << QString("%1h").arg(h);
    if (m > 0) retval << QString("%1m").arg(m);
    return retval.join(" ");
}

Effort Effort::fromString(const QString & str)
{
    Effort retval;
    retval.effortInMinutes_ = parseYWDHMTime(str);
    return retval;
}
