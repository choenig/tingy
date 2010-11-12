#include "networkstorage.h"

#include <core/clock.h>
#include <core/ftp.h>
#include <core/settings.h>
#include <core/taskmodel.h>
#include <storage/filestorage.h>
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

    bool saveTasks(const QList<Task> & tasks)
    {
        Ftp ftp;

        bool ok = true;
        foreach (const Task & task, tasks) {
            ok = saveTaskToFile(task) && ok;
        }

        return ok;
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

    bool saveTaskToFile(const Task & task)
    {
        QTemporaryFile tempFile;
        tempFile.open();
        Task::saveToFile(tempFile.fileName(), task);

        const QString filename = filenameFromTaskId(task.getId()) + (task.isDone() ? ".done" : "");

        Ftp ftp;
        bool ok = ftp.put(filename, &tempFile);

        availableTasks_[filename] = QDateTime::currentDateTime();

        return ok;
    }

    bool removeTask(const QString & filename)
    {
        if (!availableTasks_.contains(filename)) return false;

        Ftp ftp;
        bool ok = ftp.remove(filename);

        availableTasks_.remove(filename);

        return ok;
    }


    void checkForChanges()
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
                    TaskModel::instance()->addTask(task);
                }
                continue;
            }

            // task changed
            if (oldAvailableTasks[filename] != availableTasks_[filename]) {
                Task task = loadTaskFromFile(filename);
                if (task.isValid()) {
                    TaskModel::instance()->updateTask(task);
                }
                continue;
            }
        }

        // check for removed tasks
        foreach (const QString & filename, oldAvailableTasks.keys()) {
            if (!availableTasks_.contains(filename)) {
                TaskId taskId = TaskId::fromString(QString(filename).remove(".task"));
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

NetworkStorage::NetworkStorage()
    : QObject(), checkForChangesTimer_(0), d_ptr(new NetworkStoragePrivate(this))
{
    checkForChangesTimer_ = new QTimer(this);
    connect(checkForChangesTimer_, SIGNAL(timeout()), this, SLOT(checkForChanges()));
    checkForChangesTimer_->start(30 * 1000);
}

NetworkStorage::~NetworkStorage()
{
    delete d_ptr;
}

QList<Task> NetworkStorage::loadTasks()
{
    Q_D(NetworkStorage);
    return d->loadAllAvailableTasks();
}

bool NetworkStorage::saveTasks(const QList<Task> & tasks)
{
    Q_D(NetworkStorage);
    return d->saveTasks(tasks);
}

bool NetworkStorage::addTask(const Task & task)
{
    Q_D(NetworkStorage);
    return d->saveTaskToFile(task);
}

bool NetworkStorage::updateTask(const Task & task, bool doneChanged)
{
    Q_D(NetworkStorage);

    // remove 'other' file
    if (doneChanged) d->removeTask(filenameFromTaskId(task.getId()) + (task.isDone() ? "" : ".done"));

    // save new task
    return d->saveTaskToFile(task);
}

bool NetworkStorage::removeTask(const TaskId & taskId)
{
    Q_D(NetworkStorage);
   return  d->removeTask(filenameFromTaskId(taskId));
}

void NetworkStorage::checkForChanges()
{
    log << "checking for changes ...";

    // check for changes
    Q_D(NetworkStorage);
    d->checkForChanges();

    log << "checking for changes ... [done]";
}
