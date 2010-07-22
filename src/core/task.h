#pragma once

#include <core/effort.h>
#include <core/priority.h>
#include <core/taskid.h>

#include <QDate>
#include <QString>

//
// Task

class Task
{
public:
    Task() {}

    bool isNull() const;
    bool isValid() const;

    TaskId getId() const { return id_; }
    void setId(const TaskId & taskId) { id_ = taskId; }

    QDateTime getCreationTimestamp() const { return creationTimestamp_; }
    void setCreationTimestamp(const QDateTime & timestamp) { creationTimestamp_ = timestamp; }

    Priority getPriority() const { return priority_; }
    void setPriority(const Priority & priority) { priority_ = priority; }

    QString getDescription() const { return description_; }
    void setDescription(const QString & description) { description_ = description; }

    // returns the date this task is scheduled, which is either the planned or the due date
    QDate getEffectiveDate() const { return plannedDate_.isValid() ? plannedDate_ : dueDate_; }

    QDate getDueDate() const { return dueDate_; }
    void setDueDate(const QDate & dueDate) { dueDate_ = dueDate; }

    bool isOverdue() const;

    QDate getPlannedDate() const { return plannedDate_; }
    void setPlannedDate(const QDate & plannedDate) { plannedDate_ = plannedDate; }

    Effort getEffort() const { return effort_; }
    void setEffort(const Effort & effort) { effort_ = effort; }

    bool isDone() const { return doneTimestamp_.isValid(); }
    void setDone(const QDateTime & now) { doneTimestamp_ = now; }
    QDateTime getDoneTimestamp() const { return doneTimestamp_; }

    QString toString() const;

    bool operator==(const Task & rhs) const;
    bool operator!=(const Task & rhs) const { return !operator==(rhs); }

    static Task createFromString(const QString & string);

public:
    static void saveToFile(const QString & filename, const Task & task);
    static Task loadFromFile(const QString & filePath);

private:
    TaskId id_;
    QDateTime creationTimestamp_;
    Priority priority_;
    QString description_;
    QDate dueDate_;
    QDate plannedDate_;
    Effort effort_;
    QDateTime doneTimestamp_;

    friend QDataStream & operator<<( QDataStream & out, const Task & task);
    friend QDataStream & operator>>( QDataStream & out, Task & task);
};

//
// stream operators

inline QDataStream & operator<<(QDataStream & out, const Task & task) {
    out
        << task.id_
        << task.creationTimestamp_
        << task.priority_
        << task.description_
        << task.dueDate_
        << task.plannedDate_
        << task.effort_
        << task.doneTimestamp_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Task & task) {
    in
        >> task.id_
        >> task.creationTimestamp_
        >> task.priority_
        >> task.description_
        >> task.dueDate_
        >> task.plannedDate_
        >> task.effort_
        >> task.doneTimestamp_;
    return in;
}
