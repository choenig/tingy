#include "networkstorage.h"

#include <core/clock.h>
#include <core/filestorage.h>
#include <core/ftp.h>
#include <core/settings.h>
#include <core/taskmodel.h>
#include <util/log.h>

#include <QDebug>
#include <QEventLoop>
#include <QFtp>
#include <QMap>
#include <QRegExp>
#include <QTemporaryFile>
#include <QTimer>
#include <QUrlInfo>

namespace {

QString filenameFromTaskId(const TaskId & taskId) {
    return taskId.toString() + ".task";
}

}

//
// NetworkStoragePrivate

class NetworkStoragePrivate
{
public:
    QList<Task> loadAllAvailableTasks()
    {
        Ftp ftp;

        _q_reloadListOfAvailableTasks();

        QList<Task> retval;
        foreach (const QString & fileName, availableTasks.keys()) {
            Task task = _q_loadTaskFromFile(fileName);
            if (task.isValid()) retval << task;
        }

       return retval;
    }

    void saveTaskToFile(const Task & task)
    {
        QTemporaryFile tempFile;
        tempFile.open();
        Task::saveToFile(tempFile.fileName(), task);

        const QString filename = filenameFromTaskId(task.getId());

        Ftp ftp;
        ftp.put(filename, &tempFile);

        availableTasks[filename] = QDateTime::currentDateTime();
    }

    void removeTask(const TaskId & taskId)
    {
        const QString filename = filenameFromTaskId(taskId);
        if (!availableTasks.contains(filename)) return;

        Ftp ftp;
        ftp.remove(filename);
        availableTasks.remove(filename);
    }


    void checkForChanges(QList<Task> & changedTasks)
    {
        // remember old list
        const QMap<QString, QDateTime> oldAvailableTasks = availableTasks;

        Ftp ftp;

        // reload list of tasks
        _q_reloadListOfAvailableTasks();

        // check for new and updated tasks
        foreach (const QString & filename, availableTasks.keys())
        {
            // task is new, add it
            if (!oldAvailableTasks.contains(filename)) {
                Task task = _q_loadTaskFromFile(filename);
                if (task.isValid()) {
                    changedTasks << task;
                    TaskModel::instance()->addTask(task);
                }
                continue;
            }

            // task changed
            if (oldAvailableTasks[filename] != availableTasks[filename]) {
                Task task = _q_loadTaskFromFile(filename);
                if (task.isValid()) {
                    changedTasks << task;
                    TaskModel::instance()->updateTask(task);
                }
                continue;
            }
        }

        // check for removed tasks
        foreach (const QString & filename, oldAvailableTasks.keys()) {
            if (!availableTasks.contains(filename)) {
                TaskId taskId = TaskId::fromString(QString(filename).remove(".task"));
                changedTasks << Task(taskId);
                TaskModel::instance()->removeTask(taskId);
            }
        }
    }

private:
    void _q_reloadListOfAvailableTasks()
    {
        availableTasks.clear();

        Ftp ftp;
        QList<QUrlInfo> urlInfos = ftp.ls(QRegExp("*.task", Qt::CaseSensitive, QRegExp::Wildcard));
        foreach (const QUrlInfo & urlInfo, urlInfos) {
            availableTasks[urlInfo.name()] = urlInfo.lastModified();
        }
    }

    Task _q_loadTaskFromFile(const QString & fileName)
    {
        QTemporaryFile tempFile;
        tempFile.open();

        Ftp ftp;
        ftp.get(fileName, &tempFile);

        tempFile.close();

        const Task task = Task::loadFromFile(tempFile.fileName());
        if (task.isValid()) return task;
        return Task();
    }

private:
    QMap<QString, QDateTime> availableTasks;

public:
    Q_DECLARE_PUBLIC(NetworkStorage)
    NetworkStorage * q_ptr;
};

#include "moc_networkstorage.cpp"

//
// NetworkStorage

NetworkStorage::NetworkStorage(QObject *parent)
    : QObject(parent), checkForChangesTimer_(0), d_ptr(new NetworkStoragePrivate)
{
    Q_D(NetworkStorage);
    d->q_ptr = this;

    // use Qt::QueuedConnection to make sure to loop the event loop on every action to have instant gui updates before
    // updating the network files
    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)), Qt::QueuedConnection);
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)), Qt::QueuedConnection);
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)), Qt::QueuedConnection);

    checkForChangesTimer_ = new QTimer(this);
    connect(checkForChangesTimer_, SIGNAL(timeout()), this, SLOT(checkForChanges()));
    checkForChangesTimer_->start(30 * 1000);
}

NetworkStorage::~NetworkStorage()
{
    delete d_ptr;
}

void NetworkStorage::restoreFromFiles()
{
    newTasks_.clear();

    // reload all task files
    Q_D(NetworkStorage);
    newTasks_ = d->loadAllAvailableTasks();
    TaskModel::instance()->init(newTasks_);
}

void NetworkStorage::addTask(const Task & task)
{
    if (newTasks_.contains(task)) {
        newTasks_.removeOne(task);
        return;
    }

    Q_D(NetworkStorage);
    d->saveTaskToFile(task);
}

void NetworkStorage::updateTask(const Task & task)
{
    if (newTasks_.contains(task)) {
        newTasks_.removeOne(task);
        return;
    }

    Q_D(NetworkStorage);
    d->saveTaskToFile(task);
}

void NetworkStorage::removeTask(const TaskId & taskId)
{
    foreach (const Task & task, newTasks_) {
        if (task.getId() == taskId) {
            newTasks_.removeOne(task);
            return;
        }
    }

    Q_D(NetworkStorage);
    d->removeTask(taskId);
}

void NetworkStorage::checkForChanges()
{
    log << "checking for changes ...";

    // seems initial reload is in progress
    if (!newTasks_.isEmpty()) return;

    // check for changes
    Q_D(NetworkStorage);
    d->checkForChanges(newTasks_);

    log << "checking for changes ... [done]";
}
