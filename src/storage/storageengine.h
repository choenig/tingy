#pragma once

#include <core/task.h>

//
// StorageEngine

class StorageEngine
{
public:
    virtual QList<Task> loadTasks()                              =0;
    virtual bool saveTasks(const QList<Task> & tasks)            =0;
    virtual bool addTask(const Task & task)                      =0;
    virtual bool updateTask(const Task & task, bool doneChanged) =0;
    virtual bool removeTask(const TaskId & taskId)               =0;
};

//
// StorageEngines

class StorageEngines : public QList<StorageEngine*>, public StorageEngine
{
public:
    virtual QList<Task> loadTasks();
    virtual bool saveTasks(const QList<Task> & tasks);
    virtual bool addTask(const Task & task);
    virtual bool updateTask(const Task & task, bool doneChanged);
    virtual bool removeTask(const TaskId & taskId);
};
