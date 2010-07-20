#include "task.h"

#include <core/clock.h>
#include <util/parsetimestamp.h>

#include <QDebug>
#include <QRegExp>

bool Task::isNull() const
{
    return id_.isNull() && creationTimestamp_.isNull() && description_.isNull();
}

bool Task::isValid() const
{
    return !id_.isNull() && creationTimestamp_.isValid() && !description_.isEmpty();
}

bool Task::isOverdue() const
{
    const QDate effDate = getEffectiveDate();
    return !isDone() && effDate.isValid() && effDate < Clock::currentDate();
}

QString Task::toString() const
{
    return description_; // RFI enhance
}

bool Task::operator==(const Task & rhs) const
{
    return id_                == rhs.id_                &&
           creationTimestamp_ == rhs.creationTimestamp_ &&
           priority_          == rhs.priority_          &&
           description_       == rhs.description_       &&
           dueDate_           == rhs.dueDate_           &&
           plannedDate_       == rhs.plannedDate_       &&
           effort_            == rhs.effort_            &&
           doneTimestamp_     == rhs.doneTimestamp_;
}

Task Task::createFromString(const QString & string)
{
    Task task;
    task.id_ = TaskId::createUniqueId();
    task.creationTimestamp_ = Clock::currentDateTime();
    task.description_ = string;

    // parse date like "*today"
    QRegExp reDue("\\*([^ ]+)");
    if (task.description_.indexOf(reDue) >= 0) {
        const QDate dueDate = parseDate(reDue.cap(1));
        if (dueDate.isValid()) {
            task.description_.remove(reDue);
            task.dueDate_ = dueDate;
        }
    }

    // parse effort like "$1h45m"
    QRegExp reEffort("\\$([^ ]+)");
    if (task.description_.indexOf(reEffort) >= 0) {
        const Effort effort = Effort::fromString(reEffort.cap(1));
        if (effort.isValid()) {
            task.description_.remove(reEffort);
            task.effort_ = effort;
        }
    }

    // parse priority like "!+"
    QRegExp rePriority("\\!([+-])");
    if (task.description_.indexOf(rePriority) >= 0) {
        task.setPriority(rePriority.cap(1) == "+" ? Priority::High : Priority::Low);
        task.description_.remove(rePriority);
    }

    task.description_ = task.description_.simplified();

    return task;
}
