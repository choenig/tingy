#pragma once

#include <core/taskid.h>
#include <core/importance.h>

#include <QDate>
#include <QString>
#include <QTime>

class Task
{
public:
    Task() {}

    bool isValid() const;

    TaskId getId() const { return id_; }
    QString getDescription() const { return description_; }
    QDate getDueDate() const { return dueDate_; }
    QTime getEffort() const { return effort_; }

    QString toString() const;

    bool operator==(const Task & rhs) const;
    bool operator!=(const Task & rhs) const { return !operator==(rhs); }

    static Task createFromString(const QString & string);

private:
    TaskId id_;
    Importance importance_;
    QString description_;
    QDate dueDate_;
    QDate plannedDate_; // adjustable by drag and drop
    QTime effort_;

    friend QDataStream & operator<<( QDataStream & out, const Task & task);
    friend QDataStream & operator>>( QDataStream & out, Task & task);
};

inline QDataStream & operator<<(QDataStream & out, const Task & task) {
    out
        << task.id_
        << task.importance_
        << task.description_
        << task.dueDate_
        << task.plannedDate_
        << task.effort_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Task & task) {
    in
        >> task.id_
        >> task.importance_
        >> task.description_
        >> task.dueDate_
        >> task.plannedDate_
        >> task.effort_;
    return in;
}
