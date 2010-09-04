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
    Task(const TaskId & taskId = TaskId());

    bool isNull() const;
    bool isValid() const;

    TaskId getId() const { return id_; }

    QDateTime getCreationTimestamp() const { return creationTimestamp_; }

    QDateTime getLastChanged() const { return lastChanged_; }
    void updateLastChanged();

    Priority getPriority() const { return priority_; }
    void setPriority(const Priority & priority);

    QString getTitle() const { return title_; }
    void setTitle(const QString & title);

    QString getDescription() const { return description_; }
    void setDescription(const QString & description);

    // returns the date this task is scheduled, which is either the planned or the due date
    QDate getEffectiveDate() const;

    QDate getDueDate() const { return dueDate_; }
    void setDueDate(const QDate & dueDate);

    bool isOverdue() const;

    QDate getPlannedDate() const { return plannedDate_; }
    void setPlannedDate(const QDate & plannedDate);
    void resetPlannedDate();

    Effort getEffort() const { return effort_; }
    void setEffort(const Effort & effort);

    bool isDone() const { return doneTimestamp_.isValid(); }
    void setDone(const QDateTime & timestamp);
    QDateTime getDoneTimestamp() const { return doneTimestamp_; }

    QString toString() const;

    bool operator==(const Task & rhs) const;
    bool operator!=(const Task & rhs) const { return !operator==(rhs); }
    bool operator<(const Task & rhs) const;
    bool operator>=(const Task & rhs) const { return !operator<(rhs); }

    static Task createFromString(const QString & string);

public:
    static void saveToFile(const QString & filename, const Task & task);
    static Task loadFromFile(const QString & filePath);

private:
    TaskId    id_;
    QDateTime creationTimestamp_;
    QDateTime lastChanged_;
    Priority  priority_;
    QString   title_;
    QString   description_;
    QDate     dueDate_;
    QDate     plannedDate_;
    Effort    effort_;
    QDateTime doneTimestamp_;

    friend QDataStream & operator<<( QDataStream & out, const Task & task);
    friend QDataStream & operator>>( QDataStream & out, Task & task);
};

//
// stream operators

inline QDataStream & operator<<(QDataStream & out, const Task & task) {
    out << task.id_
        << task.creationTimestamp_
        << task.lastChanged_
        << task.priority_
        << task.title_
        << task.description_
        << task.dueDate_
        << task.plannedDate_
        << task.effort_
        << task.doneTimestamp_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Task & task) {
    in  >> task.id_
        >> task.creationTimestamp_
        >> task.lastChanged_
        >> task.priority_
        >> task.title_
        >> task.description_
        >> task.dueDate_
        >> task.plannedDate_
        >> task.effort_
        >> task.doneTimestamp_;
    return in;
}
