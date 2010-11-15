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
#pragma once

#include <QDataStream>

//
// Priority

class Priority
{
public:
    enum Level {
        High   =  1,
        Normal =  0,
        Low    = -1
    };

public:
    Priority(Level level = Normal) : level_(level) {}

    QString toTrString() const;

    int toInt() { return (int)level_; }

    bool operator==(const Priority & rhs) const { return level_ == rhs.level_; }
    bool operator!=(const Priority & rhs) const { return !operator==(rhs); }

    bool operator<(const Priority & rhs) const { return level_ < rhs.level_; }
    bool operator>(const Priority & rhs) const { return level_ > rhs.level_; }

private:
    Level level_;

    friend QDataStream & operator<<(QDataStream & out, const Priority & priority);
    friend QDataStream & operator>>(QDataStream & in, Priority & priority );
};

//
// stream operators

inline QDataStream & operator<<(QDataStream & out, const Priority & priority) {
    out << (qint32)priority.level_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Priority & priority ) {
    qint32 lvl;
    in >> lvl;
    priority.level_ = (Priority::Level)lvl;
    return in;
}
