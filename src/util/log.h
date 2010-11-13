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
#pragma once

#include <QDateTime>
#include <QMap>

namespace Log {
enum Severity {
    Trace   = 0,
    Debug   = 2,
    Info    = 5,
    Warning = 7,
    Error   = 8,
    Fatal   = 9
};
}

class LogLine
{
public:
    LogLine(const char * file, int line);
    ~LogLine();

    LogLine & operator()(Log::Severity severity);

    template <class T>
    LogLine & operator<<(const T & t) { return operator <<((QString)t.toString()); }

    LogLine & operator<<(const QString & str);
    LogLine & operator<<(const char    * str);
    LogLine & operator<<(int i);
    LogLine & operator<<(bool b);

private:
    QDateTime timestamp_;
    Log::Severity severity_;
    QString  file_;
    int       line_;
    QString   string_;
};

#define log LogLine(__FILE__, __LINE__)

namespace Log {
    void dumpLog();
}
