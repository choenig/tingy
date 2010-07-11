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
    QTime effort;
};

