#pragma once

#include <storage/storageengine.h>

#include <QHash>
#include <QObject>

class CalDavNetworkManager;
class QNetworkReply;

class CalDavStorage : public QObject, public StorageEngine
{
    Q_OBJECT
public:
    CalDavStorage(QObject *parent = 0);

public:
    virtual QList<Task> loadTasks();
    virtual bool saveTasks(const QList<Task> & tasks);

    virtual bool addTask(const Task & task);
    virtual bool updateTask(const Task & task, bool doneChanged);
    virtual bool removeTask(const TaskId & taskId);

private slots:
    void checkForChanges();

private:
    CalDavNetworkManager * networkMgr_;
    QHash<TaskId, QString> knownEtags_;
};