#pragma once

#include <QDataStream>
#include <QHash>
#include <QUuid>

class TaskId
{
public:
    bool isNull() const { return uuid_.isNull(); }

    QString toString() const { return uuid_.toString(); }

    bool operator==(const TaskId & rhs) const { return uuid_ == rhs.uuid_; }
    bool operator!=(const TaskId & rhs) const { return !operator==(rhs); }

    static TaskId fromString(const QString & idString) { TaskId t; t.uuid_ = QUuid(idString); return t;}
    static TaskId createId();

private:
    QUuid uuid_;

    friend uint qHash(const TaskId& taskId);
    friend QDataStream & operator<<(QDataStream & out, const TaskId & taskId);
    friend QDataStream & operator>>(QDataStream & in, TaskId & taskId );
};

inline uint qHash(const TaskId& taskId) {
    return qHash(taskId.uuid_);
}

inline QDataStream & operator<<(QDataStream & out, const TaskId & taskId) {
    return operator<<(out, taskId.uuid_);
}
inline QDataStream & operator>>(QDataStream & in, TaskId & taskId ) {
    return operator>>(in, taskId.uuid_);
}

