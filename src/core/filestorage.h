#pragma once

#include <core/task.h>

#include <QDir>
#include <QObject>

class TaskId;

class FileStorage : public QObject
{
    Q_OBJECT

public:
    FileStorage(QObject * parent = 0);

public slots:
    void restoreFromFiles();

private slots:
    void addTask(const Task & task);
    void updateTask(const Task & task, bool doneChanged);
    void removeTask(const TaskId & taskId);

private:
    QDir fileDir_;

    // list of tasks used to handle initial restore
    QList<Task> newTasks_;
};
