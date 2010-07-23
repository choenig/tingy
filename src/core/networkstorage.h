#pragma once

#include <QObject>

class Task;
class TaskId;
class NetworkStoragePrivate;
class QTimer;

class NetworkStorage : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkStorage);

public:
    NetworkStorage(QObject *parent = 0);
    ~NetworkStorage();

public slots:
    void restoreFromFiles();

private slots:
    void addTask(const Task & task);
    void updateTask(const Task & task);
    void removeTask(const TaskId & taskId);
    void checkForChanges();

private:
    bool restoreInProgress_;
    QTimer * checkForChangesTimer_;

private:
    Q_PRIVATE_SLOT(d_func(), void _q_listInfo(QUrlInfo));
    Q_PRIVATE_SLOT(d_func(), void _q_commandFinished(int,bool));
    NetworkStoragePrivate * const d_ptr;
};
