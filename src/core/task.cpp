#include "task.h"

#include <core/clock.h>
#include <util/parsetimestamp.h>
#include <util/util.h>

#include <QDebug>
#include <QRegExp>
#include <QSettings>
#include <QStringList>

QT_REGISTER_TYPE(Task);

Task::Task(const TaskId & taskId)
    : id_(taskId), lastChanged_(Clock::currentDateTime())
{
}

bool Task::isNull() const
{
    return id_.isNull() && creationTimestamp_.isNull() && title_.isNull() && description_.isNull();
}

bool Task::isValid() const
{
    return !id_.isNull() && creationTimestamp_.isValid() && !title_.isEmpty();
}

void Task::updateLastChanged()
{
    lastChanged_ = Clock::currentDateTime();
}

void Task::setPriority(const Priority & priority)
{
    priority_ = priority;
    updateLastChanged();
}

void Task::setTitle(const QString & title)
{
    title_ = title;
    updateLastChanged();
}

void Task::setDescription(const QString & description)
{
    description_ = description;
    updateLastChanged();
}

QDate Task::getEffectiveDate() const
{
    if (isDone()) return QDate(); // done items don't have an effective date
    if (plannedDate_.isValid()) return plannedDate_;
    return dueDate_;
}

void Task::setDueDate(const QDate & dueDate)
{
    dueDate_ = dueDate;
    updateLastChanged();
}

bool Task::isOverdue() const
{
    const QDate effDate = getEffectiveDate();
    return !isDone() && effDate.isValid() && effDate < Clock::currentDate();
}

void Task::setPlannedDate(const QDate & plannedDate)
{
    plannedDate_ = plannedDate;
    updateLastChanged();
}

void Task::resetPlannedDate()
{
    plannedDate_ = QDate();
    updateLastChanged();
}

void Task::setEffort(const Effort & effort)
{
    effort_ = effort;
    updateLastChanged();
}


void Task::setDone(const QDateTime & timestamp)
{
    doneTimestamp_ = timestamp;
    updateLastChanged();
}


QString Task::toString() const
{
    return title_ + " /// " + description_; // RFI enhance
}

bool Task::operator==(const Task & rhs) const
{
    return id_                == rhs.id_                &&
           creationTimestamp_ == rhs.creationTimestamp_ &&
           lastChanged_       == rhs.lastChanged_       &&
           priority_          == rhs.priority_          &&
           title_             == rhs.title_             &&
           description_       == rhs.description_       &&
           dueDate_           == rhs.dueDate_           &&
           plannedDate_       == rhs.plannedDate_       &&
           effort_            == rhs.effort_            &&
           doneTimestamp_     == rhs.doneTimestamp_;
}

bool Task::operator<(const Task & rhs) const
{
	const Task & lhs = *this;

	// done tasks are ordered by doneTimestamp
	if (lhs.isDone() && rhs.isDone()) {
		return lhs.getDoneTimestamp() > rhs.getDoneTimestamp();
	}

	// check effective date
	QDate lhsDate = lhs.getEffectiveDate();
	QDate rhsDate = rhs.getEffectiveDate();
	if (lhsDate != rhsDate) return lhsDate < rhsDate;

	// check duedate
	lhsDate = lhs.getDueDate();
	rhsDate = rhs.getDueDate();
	if (lhsDate.isValid() && rhsDate.isValid()) {
		if (lhsDate != rhsDate) return lhsDate < rhsDate;
	} else {
		if (lhsDate != rhsDate) return !lhsDate.isNull();
	}

	// finally compare creation timestamp
	return lhs.getCreationTimestamp() < rhs.getCreationTimestamp();
}

Task Task::createFromString(const QString & string)
{
    Task task;
    task.id_ = TaskId::createUniqueId();
    task.creationTimestamp_ = Clock::currentDateTime();
    task.lastChanged_ = Clock::currentDateTime();
    task.title_ = string;

    // parse date like "*today"
    QRegExp reDue("\\*([^ ]+)");
    if (task.title_.indexOf(reDue) >= 0) {
        const QDate dueDate = parseDate(reDue.cap(1));
        if (dueDate.isValid()) {
            task.title_.remove(reDue);
            task.dueDate_ = dueDate;
        }
    }

    // parse effort like "$1h45m"
    QRegExp reEffort("\\$([^ ]+)");
    if (task.title_.indexOf(reEffort) >= 0) {
        const Effort effort = Effort::fromString(reEffort.cap(1));
        if (effort.isValid()) {
            task.title_.remove(reEffort);
            task.effort_ = effort;
        }
    }

    // parse priority like "!+"
    QRegExp rePriority("\\!([+-])");
    if (task.title_.indexOf(rePriority) >= 0) {
        task.setPriority(rePriority.cap(1) == "+" ? Priority::High : Priority::Low);
        task.title_.remove(rePriority);
    }

    // split title and description
    QStringList sl = task.title_.split("///");
    if (sl.size() == 2) {
        task.title_ = sl.value(0);
        task.description_ = sl.value(1);
    }

    // replace '//' by newlines
    task.title_.replace(QRegExp("\\s*//\\s*"), "\n");
    task.description_.replace(QRegExp("\\s*//\\s*"), "\n");

    task.title_       = task.title_.trimmed();
    task.description_ = task.description_.trimmed();

    return task;
}

void Task::saveToFile(const QString & filename, const Task & task)
{
    // Version History
    // 1: changed 'done' from bool to timestamp
    // 2: added 'title'
    // 3: added 'lastChanged'
    QSettings settings(filename, QSettings::IniFormat);
    settings.setValue("fileStorageVersion", 3);
    settings.setValue("task/id",                task.getId().toString());
    settings.setValue("task/creationTimestamp", task.getCreationTimestamp().toString(Qt::ISODate));
    settings.setValue("task/lastChanged",       task.getLastChanged().toString(Qt::ISODate));
    settings.setValue("task/priority",          task.getPriority().toInt());
    settings.setValue("task/title",             task.getTitle());
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

    Task task(TaskId::fromString(settings.value("task/id").toString()));

    // creationTimestamp
    task.creationTimestamp_ = QDateTime::fromString(settings.value("task/creationTimestamp").toString(), Qt::ISODate);

    // lastChanged
    if (version >= 3) task.lastChanged_ = QDateTime::fromString(settings.value("task/lastChanged_").toString(), Qt::ISODate);
    else              task.lastChanged_ = Clock::currentDateTime();

    // priority
    task.priority_ = (Priority::Level)settings.value("task/priority").toInt();

    // title and description
    if (version >= 2) {
        task.title_ = settings.value("task/title").toString();
        task.description_ = settings.value("task/description").toString();
    } else {
        task.title_ = settings.value("task/description").toString();
    }

    // dueDate
    task.dueDate_ = QDate::fromString(settings.value("task/dueDate").toString(), Qt::ISODate);

    // plannedDate
    task.plannedDate_ = QDate::fromString(settings.value("task/plannedDate").toString(), Qt::ISODate);

    // effort
    task.effort_ = Effort(settings.value("task/effort").toUInt());

    // doneTimestamp
    if (version == 0) task.doneTimestamp_ = settings.value("task/done").toBool() ? Clock::currentDateTime() : QDateTime();
    else              task.doneTimestamp_ = QDateTime::fromString(settings.value("task/done").toString(), Qt::ISODate);

    return task;
}
