#include "taskid.h"

#include <util/util.h>

QT_REGISTER_TYPE(TaskId)

TaskId TaskId::fromString(const QString &idString)
{
    TaskId t;
    t.uuid_ = QUuid(idString);
    return t;
}

TaskId TaskId::createUniqueId()
{
    TaskId retval;
    retval.uuid_ = QUuid::createUuid();
    return retval;
}
