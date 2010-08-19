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
    NetworkStoragePrivate(NetworkStorage * q_ptr) : q_ptr(q_ptr)
    {
    }

    QList<Task> loadAllAvailableTasks()
    {
        Ftp ftp;

        availableTasks_ = lsAvailableTasks();

        QList<Task> retval;
        foreach (const QString & fileName, availableTasks_.keys()) {
            Task task = loadTaskFromFile(fileName);
            if (task.isValid()) retval << task;
        }

       return retval;
    }

    Task loadTaskFromFile(const QString & fileName)
    {
        // try to enforce a really uniq tempfileName as QSettings tries to cache and seems not to reload the file in some cases
        QString templateName = QDir::tempPath() + QDir::separator() + "tingy." + QString::number(qrand()%1000);
        QTemporaryFile tempFile(templateName);
        tempFile.open();

        Ftp ftp;
        ftp.get(fileName, &tempFile);

        tempFile.close();

        const Task task = Task::loadFromFile(tempFile.fileName());
        if (task.isValid()) return task;
        return Task();
    }

    void saveTaskToFile(const Task & task)
    {
        QTemporaryFile tempFile;
        tempFile.open();
        Task::saveToFile(tempFile.fileName(), task);

        const QString filename = filenameFromTaskId(task.getId()) + (task.isDone() ? ".done" : "");

        Ftp ftp;
        ftp.put(filename, &tempFile);

        availableTasks_[filename] = QDateTime::currentDateTime();
    }

    void removeTask(const QString & filename)
    {
        if (!availableTasks_.contains(filename)) return;

        Ftp ftp;
        ftp.remove(filename);

        availableTasks_.remove(filename);
    }


    void checkForChanges(QList<Task> & changedTasks)
    {
        // remember old list
        const QMap<QString, QDateTime> oldAvailableTasks = availableTasks_;

        Ftp ftp;

        // reload list of tasks
        availableTasks_ = lsAvailableTasks();

        // check for new and updated tasks
        foreach (const QString & filename, availableTasks_.keys())
        {
            // task is new, add it
            if (!oldAvailableTasks.contains(filename)) {
                Task task = loadTaskFromFile(filename);
                if (task.isValid()) {
                    changedTasks << task;
                    TaskModel::instance()->addTask(task);
                }
                continue;
            }

            // task changed
            if (oldAvailableTasks[filename] != availableTasks_[filename]) {
                Task task = loadTaskFromFile(filename);
                if (task.isValid()) {
                    changedTasks << task;
                    TaskModel::instance()->updateTask(task);
                }
                continue;
            }
        }

        // check for removed tasks
        foreach (const QString & filename, oldAvailableTasks.keys()) {
            if (!availableTasks_.contains(filename)) {
                TaskId taskId = TaskId::fromString(QString(filename).remove(".task"));
                changedTasks << Task(taskId);
                TaskModel::instance()->removeTask(taskId);
            }
        }
    }

    QMap<QString, QDateTime> lsAvailableTasks()
    {
        QMap<QString, QDateTime> retval;

        Ftp ftp;
        QList<QUrlInfo> urlInfos = ftp.ls( QRegExp("*.task", Qt::CaseSensitive, QRegExp::Wildcard) );
        foreach (const QUrlInfo & urlInfo, urlInfos) {
            retval[urlInfo.name()] = urlInfo.lastModified();
        }

        return retval;
    }

private:
    QMap<QString, QDateTime> availableTasks_;

public:
    Q_DECLARE_PUBLIC(NetworkStorage)
    NetworkStorage * q_ptr;
};

#include "moc_networkstorage.cpp"

//
// NetworkStorage

NetworkStorage::NetworkStorage(QObject *parent)
    : QObject(parent), checkForChangesTimer_(0), d_ptr(new NetworkStoragePrivate(this))
{
    // use Qt::QueuedConnection to make sure to loop the event loop on every action to have instant gui updates before
    // updating the network files
    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)),        this, SLOT(addTask(Task)),         Qt::QueuedConnection);
    connect(tm, SIGNAL(taskUpdated(Task,bool)), this, SLOT(updateTask(Task,bool)), Qt::QueuedConnection);
    connect(tm, SIGNAL(taskRemoved(TaskId)),    this, SLOT(removeTask(TaskId)),    Qt::QueuedConnection);

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

void NetworkStorage::updateTask(const Task & task, bool doneChanged)
{
    if (newTasks_.contains(task)) {
        newTasks_.removeOne(task);
        return;
    }

    Q_D(NetworkStorage);

    // remove 'other' file
    if (doneChanged) d->removeTask(filenameFromTaskId(task.getId()) + (task.isDone() ? "" : ".done"));

    // save new task
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
    d->removeTask(filenameFromTaskId(taskId));
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
