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
    return description_; // fixme enhance
}

bool Task::operator==(const Task & rhs) const
{
    return id_                == rhs.id_                &&
           creationTimestamp_ == rhs.creationTimestamp_ &&
           importance_        == rhs.importance_        &&
           description_       == rhs.description_       &&
           dueDate_           == rhs.dueDate_           &&
           plannedDate_       == rhs.plannedDate_       &&
           effort_            == rhs.effort_            &&
           done_              == rhs.done_;
}

Task Task::createFromString(const QString & string)
{
    Task task;
    task.id_ = TaskId::createId();
    task.creationTimestamp_ = QDateTime::currentDateTime();
    task.description_ = string;

    QRegExp reDue("\\*([^ ]+)");
    if (task.description_.indexOf(reDue) >= 0) {
        const QDate dueDate = parseDate(reDue.cap(1));
        if (dueDate.isValid()) {
            task.description_.remove(reDue);
            task.dueDate_ = dueDate;
        }
    }

    QRegExp reEffort("\\$([^ ]+)");
    if (task.description_.indexOf(reEffort) >= 0) {
        const QTime effortTime = parseTime(reEffort.cap(1));
        if (effortTime.isValid()) {
            task.description_.remove(reEffort);
            task.effort_ = effortTime;
        }
    }

    QRegExp reImportance("\\!([+-])");
    if (task.description_.indexOf(reImportance) >= 0) {
        task.setImportance(reImportance.cap(1) == "+" ? Importance::High : Importance::Low);
        task.description_.remove(reImportance);
    }

    task.description_ = task.description_.simplified();

    return task;
}
