#pragma once

#include <core/task.h>

class StorageEngine
{
public:
    virtual QList<Task> loadTasks()                              =0;
    virtual bool saveTasks(const QList<Task> & tasks)            =0;
    virtual bool addTask(const Task & task)                      =0;
    virtual bool updateTask(const Task & task, bool doneChanged) =0;
    virtual bool removeTask(const TaskId & taskId)               =0;
};
