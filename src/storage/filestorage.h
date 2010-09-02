#pragma once

#include <core/task.h>
#include <storage/storageengine.h>

#include <QDir>

class TaskId;

class FileStorage : public StorageEngine
{
public:
    FileStorage();

public:
    virtual QList<Task> loadTasks();
    virtual bool saveTasks(const QList<Task> & tasks);
    virtual bool addTask(const Task & task);
    virtual bool updateTask(const Task & task, bool doneChanged);
    virtual bool removeTask(const TaskId & taskId);

private:
    QDir fileDir_;
};
