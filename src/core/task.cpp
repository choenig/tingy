#include "task.h"

#include <core/clock.h>
#include <util/parsetimestamp.h>

#include <QDebug>
#include <QRegExp>

bool Task::isValid() const
{
    return !id_.isNull() && !description_.isEmpty();
}

QString Task::toString() const
{
    return description_; // fixme enhance
}

bool Task::operator==(const Task & rhs) const
{
    return id_                == rhs.id_                &&
           creationTimestamp_ == rhs.creationTimestamp_ &&
           priority_          == rhs.priority_        &&
           description_       == rhs.description_       &&
           dueDate_           == rhs.dueDate_           &&
           plannedDate_       == rhs.plannedDate_       &&
           effort_            == rhs.effort_            &&
           doneTimestamp_     == rhs.doneTimestamp_;
}

Task Task::createFromString(const QString & string)
{
    Task task;
    task.id_ = TaskId::createId();
    task.creationTimestamp_ = Clock::currentDateTime();
    task.description_ = string;

    QRegExp reDue("\\*([^ ]+)");
    if (task.description_.indexOf(reDue) >= 0) {
        const QDate dueDate = parseDate(reDue.cap(1));
        if (dueDate.isValid()) {
            task.description_.remove(reDue);
            task.dueDate_ = dueDate;
        }
    }

    // fixme
    QRegExp reEffort("\\$([^ ]+)");
    if (task.description_.indexOf(reEffort) >= 0) {
        const Effort effort = parseEffort(reEffort.cap(1));
        if (effort.isValid()) {
            task.description_.remove(reEffort);
            task.effort_ = effort;
        }
    }

    QRegExp rePriority("\\!([+-])");
    if (task.description_.indexOf(rePriority) >= 0) {
        task.setPriority(rePriority.cap(1) == "+" ? Priority::High : Priority::Low);
        task.description_.remove(rePriority);
    }

    task.description_ = task.description_.simplified();

    return task;
}
