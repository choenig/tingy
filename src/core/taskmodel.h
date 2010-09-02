#pragma once

#include <core/storageengine.h>
#include <core/task.h>
#include <core/taskid.h>

#include <QObject>
#include <QHash>
#include <QSet>

class StorageEngines : public QList<StorageEngine*>, public StorageEngine
{
public:
    virtual QList<Task> loadTasks();
    virtual bool saveTasks(const QList<Task> & tasks);
    virtual bool addTask(const Task & task);
    virtual bool updateTask(const Task & task, bool doneChanged);
    virtual bool removeTask(const TaskId & taskId);
};

class TaskModel : public QObject
{
    Q_OBJECT

public:
    TaskModel();
    ~TaskModel();

    static TaskModel * instance() { return instance_; }

    void addStorageEngine(StorageEngine * storageEngine);

public slots:
    void init();
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

    StorageEngines storageEngines_;

private:
    static TaskModel * instance_;
};
