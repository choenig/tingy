#pragma once

#include <core/taskid.h>
#include <core/importance.h>

#include <QDate>
#include <QString>
#include <QTime>

class Task
{
public:
    Task() : done_(false) {}

    bool isValid() const;

    TaskId getId() const { return id_; }
    void setId(const TaskId & taskId) { id_ = taskId; }

    QDateTime getCreationTimestamp() const { return creationTimestamp_; }
    void setCreationTimestamp(const QDateTime & timestamp) { creationTimestamp_ = timestamp; }

    Importance getImportance() const { return importance_; }
    void setImportance(const Importance & importance) { importance_ = importance; }

    QString getDescription() const { return description_; }
    void setDescription(const QString & description) { description_ = description; }

    QDate getDueDate() const { return dueDate_; }
    void setDueDate(const QDate & dueDate) { dueDate_ = dueDate; }

    QDate getPlannedDate() const { return plannedDate_; }
    void setPlannedDate(const QDate & plannedDate) { plannedDate_ = plannedDate; }

    QTime getEffort() const { return effort_; }
    void setEffort(const QTime & effort) { effort_ = effort; }

    bool isDone() const { return done_; }
    void setDone(bool done) { done_ = done; }

    QString toString() const;

    bool operator==(const Task & rhs) const;
    bool operator!=(const Task & rhs) const { return !operator==(rhs); }

    static Task createFromString(const QString & string);

private:
    TaskId id_;
    QDateTime creationTimestamp_;
    Importance importance_;
    QString description_;
    QDate dueDate_;
    QDate plannedDate_; // adjustable by drag and drop
    QTime effort_;
    bool done_;

    friend QDataStream & operator<<( QDataStream & out, const Task & task);
    friend QDataStream & operator>>( QDataStream & out, Task & task);
};

inline QDataStream & operator<<(QDataStream & out, const Task & task) {
    out
        << task.id_
        << task.creationTimestamp_
        << task.importance_
        << task.description_
        << task.dueDate_
        << task.plannedDate_
        << task.effort_
        << task.done_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Task & task) {
    in
        >> task.id_
        >> task.creationTimestamp_
        >> task.importance_
        >> task.description_
        >> task.dueDate_
        >> task.plannedDate_
        >> task.effort_
        >> task.done_;
    return in;
}
