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
#include <QString>

//
// Effort

class Effort
{
public:
    explicit Effort(quint32 minutes = 0);

    bool isNull()  const { return effortInMinutes_ == 0; }
    bool isValid() const { return !isNull(); }

    QString toString() const;
    static Effort fromString(const QString & str);

    bool operator==(const Effort & rhs) const { return effortInMinutes_ == rhs.effortInMinutes_; }
    bool operator!=(const Effort & rhs) const { return !operator==(rhs); }

    quint32 toMinutes() const { return effortInMinutes_; }

private:
    quint32 effortInMinutes_;

    friend QDataStream & operator<<( QDataStream & out, const Effort & effort);
    friend QDataStream & operator>>( QDataStream & out, Effort & effort);
};

//
// stream operators

inline QDataStream & operator<<(QDataStream & out, const Effort & effort) {
    out << effort.effortInMinutes_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Effort & effort) {
    in >> effort.effortInMinutes_;
    return in;
}
