#include "task.h"


bool Task::isValid() const
{
    return !id_.isNull() && !description_.isEmpty();
}

QString Task::toString() const
{
    return description_;
}

bool Task::operator==(const Task & rhs) const
{
    return id_           == rhs.id_          &&
            importance_  == rhs.importance_  &&
            description_ == rhs.description_ &&
            dueDate_     == rhs.dueDate_     &&
            plannedDate_ == rhs.plannedDate_ &&
            effort       == rhs.effort;
}

Task Task::createFromString(const QString & string)
{
    Task retval;
    retval.id_ = TaskId::createId();
    retval.description_ = string;
    return retval;
}
