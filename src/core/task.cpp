#include "task.h"

#include <core/clock.h>
#include <util/parsetimestamp.h>
#include <util/util.h>

#include <QDebug>
#include <QRegExp>
#include <QSettings>

QT_REGISTER_TYPE(Task);

bool Task::isNull() const
{
    return id_.isNull() && creationTimestamp_.isNull() && description_.isNull();
}

bool Task::isValid() const
{
    return !id_.isNull() && creationTimestamp_.isValid() && !description_.isEmpty();
}

QDate Task::getEffectiveDate() const
{
    if (isDone()) return QDate(); // done items don't have an effective date
    if (plannedDate_.isValid()) return plannedDate_;
    return dueDate_;
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

void Task::saveToFile(const QString & filename, const Task & task)
{
    QSettings settings(filename, QSettings::IniFormat);
    settings.setValue("fileStorageVersion", 1);
    settings.setValue("task/id",                task.getId().toString());
    settings.setValue("task/creationTimestamp", task.getCreationTimestamp().toString(Qt::ISODate));
    settings.setValue("task/priority",          task.getPriority().toInt());
    settings.setValue("task/description",       task.getDescription());
    settings.setValue("task/dueDate",           task.getDueDate().toString(Qt::ISODate));
    settings.setValue("task/plannedDate",       task.getPlannedDate().toString(Qt::ISODate));
    settings.setValue("task/effort",            task.getEffort().toMinutes());
    settings.setValue("task/done",              task.getDoneTimestamp().toString(Qt::ISODate));
}

Task Task::loadFromFile(const QString & filePath)
{
    const QSettings settings(filePath, QSettings::IniFormat);
    const int version = settings.value("fileStorageVersion").toInt();

    Task task;
    task.setId(TaskId::fromString(settings.value("task/id").toString()));
    task.setCreationTimestamp(QDateTime::fromString(settings.value("task/creationTimestamp").toString(), Qt::ISODate));
    task.setPriority((Priority::Level)settings.value("task/priority").toInt());
    task.setDescription(settings.value("task/description").toString());
    task.setDueDate(QDate::fromString(settings.value("task/dueDate").toString(), Qt::ISODate));
    task.setPlannedDate(QDate::fromString(settings.value("task/plannedDate").toString(), Qt::ISODate));
    task.setEffort(Effort(settings.value("task/effort").toUInt()));
    if (version == 0) task.setDone(settings.value("task/done").toBool() ? Clock::currentDateTime() : QDateTime());
    else              task.setDone(QDateTime::fromString(settings.value("task/done").toString(), Qt::ISODate));
    return task;
}
