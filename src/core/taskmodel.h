#pragma once

#include <core/task.h>
#include <core/taskid.h>

#include <QObject>
#include <QHash>
#include <QSet>

class TaskModel : public QObject
{
    Q_OBJECT

public:
    TaskModel();
    ~TaskModel();

    static TaskModel * instance() { return instance_; }

    void init(const QList<Task> & tasks);

public slots:
    void addTask(const Task & task);
    void updateTask(const Task & task);
    void removeTask(const TaskId & taskId);

signals:
    void taskAdded(const Task & task);
    void taskUpdated(const Task & task, bool doneChanged);
    void taskRemoved(const TaskId & taskId);
    void hasOverdueTasks(bool b);

private slots:
	void handleDateChanged();

private:
    QHash<TaskId, Task> tasks_;
    QSet<TaskId> overdueTasks_;

private:
    static TaskModel * instance_;
};
