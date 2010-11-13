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

#include <QDataStream>
#include <QHash>
#include <QUuid>

//
// TaskId

class TaskId
{
public:
    bool isNull() const { return uuid_.isNull(); }

    QString toString() const { return uuid_.toString(); }

    bool operator==(const TaskId & rhs) const { return uuid_ == rhs.uuid_; }
    bool operator!=(const TaskId & rhs) const { return !operator==(rhs); }

    static TaskId fromString(const QString & idString);
    static TaskId createUniqueId();

private:
    QUuid uuid_;

    friend uint qHash(const TaskId& taskId);
    friend QDataStream & operator<<(QDataStream & out, const TaskId & taskId);
    friend QDataStream & operator>>(QDataStream & in, TaskId & taskId );
};

//
// qHash

inline uint qHash(const TaskId& taskId) {
    return qHash(taskId.uuid_);
}

//
// stream operators

inline QDataStream & operator<<(QDataStream & out, const TaskId & taskId) {
    return operator<<(out, taskId.uuid_);
}
inline QDataStream & operator>>(QDataStream & in, TaskId & taskId ) {
    return operator>>(in, taskId.uuid_);
}

