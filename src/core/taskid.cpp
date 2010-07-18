#include "taskid.h"

TaskId TaskId::createUniqueId()
{
    TaskId retval;
    retval.uuid_ = QUuid::createUuid();
    return retval;
}
