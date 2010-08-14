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
    void updateTask(const Task & task);
    void removeTask(const TaskId & taskId);

private:
    void saveToFile(const Task & task);

private:
    QDir fileDir_;

    // list of tasks used to handle initial restore
    QList<Task> newTasks_;
};
