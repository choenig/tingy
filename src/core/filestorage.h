#pragma once

#include <core/task.h>

#include <QObject>

class TaskId;

class FileStorage : public QObject
{
    Q_OBJECT

public:
    FileStorage();

private slots:
    void restoreFromFile();

    void addTask(const Task & task);
    void updateTask(const Task & task);
    void removeTask(const TaskId & taskId);

private:
    void saveToFile();

private:
    QList<Task> tasks_;
};
