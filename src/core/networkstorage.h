#pragma once

#include <core/storageengine.h>

#include <QObject>

class Task;
class TaskId;
class NetworkStoragePrivate;
class QTimer;

class NetworkStorage : public QObject, public StorageEngine
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkStorage);

public:
    NetworkStorage();
    ~NetworkStorage();

public:
    virtual QList<Task> loadTasks();
    virtual bool saveTasks(const QList<Task> & tasks);
    virtual bool addTask(const Task & task);
    virtual bool updateTask(const Task & task, bool doneChanged);
    virtual bool removeTask(const TaskId & taskId);

private slots:
    void checkForChanges();

private:
    QTimer * checkForChangesTimer_;

private:
    NetworkStoragePrivate * const d_ptr;
};
