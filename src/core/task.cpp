/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#include "task.h"

#include <core/clock.h>
#include <util/parsetimestamp.h>
#include <util/util.h>

#include <QDebug>
#include <QFile>
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

	// sort by priority
	if (lhs.getPriority() != rhs.getPriority()) return lhs.getPriority() > rhs.getPriority();

	// finally compare creation timestamp
	return lhs.getCreationTimestamp() < rhs.getCreationTimestamp();
}

Task Task::createFromString(const QString & string)
{
    Task task;
    task.id_                = TaskId::createUniqueId();
    task.creationTimestamp_ = Clock::currentDateTime();
    task.lastChanged_       = Clock::currentDateTime();
    task.title_             = string;

    // parse due date like "*today"
    QRegExp reDue("\\*([^ ]+)");
    if (task.title_.indexOf(reDue) >= 0) {
        const QDate dueDate = parseDate(reDue.cap(1));
        if (dueDate.isValid()) {
            task.title_.remove(reDue);
            task.dueDate_ = dueDate;
        }
    }

    // parse planned date like "*today"
    QRegExp rePlanned("\\@([^ ]+)");
    if (task.title_.indexOf(rePlanned) >= 0) {
        const QDate plannedDate = parseDate(rePlanned.cap(1));
        if (plannedDate.isValid()) {
            task.title_.remove(rePlanned);
            task.plannedDate_ = plannedDate;
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
    // the ':' is used to not replace things like http://www
    const QRegExp newLineRegExp("\\s*[^:]//\\s*");
    task.title_.replace(newLineRegExp, "\n");
    task.description_.replace(newLineRegExp, "\n");

    task.title_       = task.title_.trimmed();
    task.description_ = task.description_.trimmed();

    return task;
}

bool Task::saveToFile(const QString & filename, const Task & task)
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

    settings.sync();
    return settings.status() == QSettings::NoError;
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
    if (task.lastChanged_.isNull()) task.lastChanged_ = Clock::currentDateTime();

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

#define NL "\n"

QString Task::toICal() const
{
    QString taskString;

    taskString += "BEGIN:VCALENDAR" NL;
    taskString += "VERSION:2.0" NL;
    taskString += "PRODID:tingy" NL;
    taskString += "BEGIN:VTODO" NL;

    taskString += "UID:"+getId().toString() + NL;
    taskString += "DTSTAMP:" + getCreationTimestamp().toString("yyyyMMdd'T'hhmmss'Z'") + NL;
    taskString += "CREATED:" + getCreationTimestamp().toString("yyyyMMdd'T'hhmmss'Z'") + NL;
    taskString += "LAST-MODIFIED:" + getLastChanged().toString("yyyyMMdd'T'hhmmss'Z'") + NL;
    taskString += "PRIORITY:" + QString::number(2 - getPriority().toInt()) + NL;
    taskString += "SUMMARY:"     + getTitle().replace('\n', "\\n") + NL;
    taskString += "DESCRIPTION:" + getDescription().replace('\n', "\\n") + NL;

    taskString += "CLASS:PRIVATE" NL;

    if (getDueDate().isValid()) {
        taskString += "DUE;VALUE=DATE:" + getDueDate().toString("yyyyMMdd") + NL;
    }

    if (getPlannedDate().isValid()) {
        taskString += "RDATE;VALUE=DATE:" + getPlannedDate().toString("yyyyMMdd") + NL;
    }

    if (getEffort().isValid()) {
        taskString += "DURATION:PT" + getEffort().toString().toUpper() + NL;      // must not be used together with DUE
    }

    if (isDone()) {
        taskString += "COMPLETED:" + getDoneTimestamp().toString("yyyyMMdd'T'hhmmss'Z'") + NL;
        taskString += "STATUS:COMPLETED" NL;
    }

    taskString += "END:VTODO" NL;
    taskString += "END:VCALENDAR" NL;

    return taskString;
}

namespace {

void parse(TaskId & id, const QString & key, const QString & src) {
    QRegExp re(key+":(.*)\\n");
    re.setMinimal(true);
    if (src.indexOf(re) == -1) return;
    id = TaskId::fromString(re.cap(1));
}

void parse(Priority & priority, const QString & key, const QString & src) {
    QRegExp re(key+":(.*)\\n");
    re.setMinimal(true);
    if (src.indexOf(re) == -1) return;
    priority = (Priority::Level)(2-re.cap(1).toInt());
}

void parse(Effort & effort, const QString & key, const QString & src) {
    QRegExp re(key+"(.*)Z\\n");
    re.setMinimal(true);
    if (src.indexOf(re) == -1) return;
    effort = Effort::fromString(re.cap(1));
}

void parse(QDateTime & timestamp, const QString & key, const QString & src) {
    QRegExp re(key+":(.*)Z\\n");
    re.setMinimal(true);
    if (src.indexOf(re) == -1) return;
    timestamp = QDateTime::fromString(re.cap(1), "yyyyMMddThhmmss");
}

void parse(QDate & date,  const QString & key, const QString & src) {
    QRegExp re(key+";VALUE=DATE:(.*)\\n");
    re.setMinimal(true);
    if (src.indexOf(re) == -1) return;
    date = QDate::fromString(re.cap(1), "yyyyMMdd");
}

void parse(QString & str, const QString & key, const QString & src) {
    QRegExp re(key+":(.*)\\n");
    re.setMinimal(true);
    if (src.indexOf(re) == -1) return;
    str = re.cap(1);
    str.replace("\\n", "\n");
}
}

Task Task::fromICal(const QString & taskString)
{
//    qDebug() << taskString;
//    QFile file(filename);
//    file.open(QFile::ReadOnly);
//    QString taskString = file.readAll();
//    file.close();

    Task task;
    parse(task.id_               , "UID",           taskString);
    parse(task.creationTimestamp_, "CREATED",       taskString);
    parse(task.lastChanged_      , "LAST-MODIFIED", taskString);
    parse(task.priority_         , "PRIORITY",      taskString);
    parse(task.title_            , "SUMMARY",       taskString);
    parse(task.description_      , "DESCRIPTION",   taskString);
    parse(task.dueDate_          , "DUE",           taskString);
    parse(task.plannedDate_      , "RDATE",         taskString);
    parse(task.effort_           , "DURATION:PT",   taskString);
    parse(task.doneTimestamp_    , "COMPLETED",     taskString);

    return task;
}
