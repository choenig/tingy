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

private slots:
    void restoreFromFiles();

    void addTask(const Task & task);
    void updateTask(const Task & task);
    void removeTask(const TaskId & taskId);

private:
    void saveToFile(const Task & task);
    Task loadFromFile(const QString & filePath);

private:
    QDir fileDir_;
    bool restoreInProgress_;
};
