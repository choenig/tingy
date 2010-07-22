#pragma once

#include <core/task.h>

#include <QDir>
#include <QObject>

class TaskId;

class FileStorage : public QObject
{
    Q_OBJECT

public:
    FileStorage();

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
    bool restoreInProgress_;
};
