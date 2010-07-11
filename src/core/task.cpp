#include "task.h"

#include <util/parsetimestamp.h>

#include <QDebug>
#include <QRegExp>

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
           importance_   == rhs.importance_  &&
           description_  == rhs.description_ &&
           dueDate_      == rhs.dueDate_     &&
           plannedDate_  == rhs.plannedDate_ &&
           effort_       == rhs.effort_;
}

Task Task::createFromString(const QString & string)
{
    Task retval;
    retval.id_ = TaskId::createId();
    retval.description_ = string;

    QRegExp reDue("\\*([^ ]+)");
    if (retval.description_.indexOf(reDue) >= 0) {
        const QDate dueDate = parseDate(reDue.cap(1));
        if (dueDate.isValid()) {
            retval.description_.remove(reDue);
            retval.dueDate_ = dueDate;
        }
    }

    QRegExp reEffort("\\$([^ ]+)");
    if (retval.description_.indexOf(reEffort) >= 0) {
        const QTime effortTime = parseTime(reEffort.cap(1));
        if (effortTime.isValid()) {
            retval.description_.remove(reEffort);
            retval.effort_ = effortTime;
        }
    }

    return retval;
}
