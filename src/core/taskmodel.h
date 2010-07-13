#pragma once

#include <core/task.h>
#include <core/taskid.h>

#include <QObject>
#include <QHash>

class TaskModel : public QObject
{
    Q_OBJECT

public:
    TaskModel();
    ~TaskModel();

    static TaskModel * instance() { return instance_; }

    bool saveToFile() const;
    bool restoreFromFile();

public slots:
    void clear();
    void addTask(const Task & task);
    void updateTask(const Task & task);
    void removeTask(const TaskId & taskId);

signals:
    void taskAdded(const Task & task);
    void taskUpdated(const Task & task);
    void taskRemoved(const TaskId & taskId);

private:
    QHash<TaskId, Task> tasks_;

private:
    bool isFirstInstance_;
    static TaskModel * instance_;
};
