#pragma once

#include <QUuid>
#include <QHash>

class TaskId
{
public:
    bool isNull() const { return uuid_.isNull(); }
    QString toString() const { return uuid_.toString(); }

    bool operator==(const TaskId & rhs) const { return uuid_ == rhs.uuid_; }
    bool operator!=(const TaskId & rhs) const { return !operator==(rhs); }

    static TaskId createId();

private:
    QUuid uuid_;

    friend uint qHash(const TaskId& taskId);
};

inline uint qHash(const TaskId& taskId)
{
    return qHash(taskId.uuid_);
}
