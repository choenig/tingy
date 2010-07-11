#include "taskid.h"

#include <QtGlobal>
#include <cstdlib>
#include <stdlib.h>

TaskId TaskId::createId()
{
    TaskId retval;
    retval.uuid_ = QUuid::createUuid();
    return retval;
}
