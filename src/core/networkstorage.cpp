#include "networkstorage.h"

#include <core/clock.h>
#include <core/filestorage.h>
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

class NetworkStoragePrivate
{
public:
    NetworkStoragePrivate() : ftp(0) {}
    ~NetworkStoragePrivate() { delete ftp; }

    void init()
    {
        Q_Q(NetworkStorage);

        ftp = new QFtp();
        q->connect(ftp, SIGNAL(listInfo(QUrlInfo)),        q, SLOT(_q_listInfo(QUrlInfo)));
        q->connect(ftp, SIGNAL(commandFinished(int,bool)), q, SLOT(_q_commandFinished(int,bool)));
    }

    void loadAllAvailableTasks()
    {
        _q_reconnect();

        _q_reloadListOfAvailableTasks();

        ftp->cd( Settings::NetworkStorage::Taskdir() );

        foreach (const QString & fileName, availableTasks.keys()) {
            Task task = _q_loadTaskFromFile(fileName);
            if (task.isValid()) TaskModel::instance()->addTask(task);
        }

       _q_disconnect();
    }

    void saveTaskToFile(const Task & task)
    {
        _q_reconnect();

        QTemporaryFile tempFile;
        tempFile.open();

        Task::saveToFile(tempFile.fileName(), task);

        const QString filename = filenameFromTaskId(task.getId());

        ftp->cd(Settings::NetworkStorage::Taskdir());
        waitFor( ftp->put(&tempFile, filename) );

        availableTasks[filename] = QDateTime();

        _q_disconnect();
    }

    void removeTask(const TaskId & taskId)
    {
        _q_reconnect();

        const QString filename = filenameFromTaskId(taskId);
        if (availableTasks.contains(filename)) {
            ftp->cd( Settings::NetworkStorage::Taskdir() );
            waitFor( ftp->remove(filename) );
            availableTasks.remove(filename);
        }

        _q_disconnect();
    }

    void moveToAttic(const TaskId & taskId)
    {
        _q_reconnect();

        const QString filename = filenameFromTaskId(taskId);

        ftp->cd(Settings::NetworkStorage::Taskdir());
        waitFor( ftp->rename(filename, filename + ".attic") );

        _q_disconnect();
    }

    void checkForChanges()
    {
        _q_reconnect();

        // remember old list
        const QMap<QString, QDateTime> oldAvailableTasks = availableTasks;

        // reload list of tasks
        _q_reloadListOfAvailableTasks();

        // check for new and updated tasks
        foreach (const QString & filename, availableTasks.keys())
        {
            // task is new, add it
            if (!oldAvailableTasks.contains(filename)) {
                Task task = _q_loadTaskFromFile(filename);
                if (task.isValid()) TaskModel::instance()->addTask(task);
                continue;
            }

            // task changed
            if (oldAvailableTasks[filename] != availableTasks[filename]) {
                Task task = _q_loadTaskFromFile(filename);
                if (task.isValid()) TaskModel::instance()->updateTask(task);
                continue;
            }
        }

        // check for removed tasks
        foreach (const QString & filename, oldAvailableTasks.keys()) {
            if (!availableTasks.contains(filename)) {
                TaskModel::instance()->removeTask(TaskId::fromString(QString(filename).remove(".task")));
            }
        }

        _q_disconnect();
    }

    void waitFor(int id) {
        waitForId = id;
        loop.exec();
    }

private:
    void _q_reconnect()
    {
        if (ftp->state() == QFtp::LoggedIn) ftp->close();

        ftp->connectToHost(Settings::NetworkStorage::Hostname());
        waitFor( ftp->login(Settings::NetworkStorage::Username(),
                            QByteArray::fromBase64(Settings::NetworkStorage::Password().toLatin1())) );
    }

    void _q_disconnect()
    {
        waitFor( ftp->close() );
    }

    void _q_reloadListOfAvailableTasks()
    {
        availableTasks.clear();

        ftp->cd( Settings::NetworkStorage::Taskdir() );
        waitFor( ftp->list() );
    }

    Task _q_loadTaskFromFile(const QString & fileName)
    {
        QTemporaryFile tempFile;
        tempFile.open();

        waitFor( ftp->get(fileName, &tempFile) );

        tempFile.close();

        const Task task = Task::loadFromFile(tempFile.fileName());
        if (task.isValid()) return task;
        return Task();
    }

    void _q_listInfo(const QUrlInfo & urlInfo)
    {
        const QRegExp re("*.task", Qt::CaseSensitive, QRegExp::Wildcard);
        if (re.exactMatch(urlInfo.name())) {
            availableTasks[urlInfo.name()] = urlInfo.lastModified();
        }
    }

    QString _q_toString(QFtp::Command cmd)
    {
        switch (cmd) {
        case QFtp::None:            return "None";
        case QFtp::SetTransferMode: return "SetTransferMode";
        case QFtp::SetProxy:        return "SetProxy";
        case QFtp::ConnectToHost:   return "ConnectToHost";
        case QFtp::Login:           return "Login";
        case QFtp::Close:           return "Close";
        case QFtp::List:            return "List";
        case QFtp::Cd:              return "Cd";
        case QFtp::Get:             return "Get";
        case QFtp::Put:             return "Put";
        case QFtp::Remove:          return "Remove";
        case QFtp::Mkdir:           return "Mkdir";
        case QFtp::Rmdir:           return "Rmdir";
        case QFtp::Rename:          return "Rename";
        case QFtp::RawCommand:      return "RawCommand";
        }
        return "";
    }

    void _q_commandFinished(int id, bool error)
    {
        log << "_q_commandFinished();" << id << error << _q_toString(ftp->currentCommand());

        if (error) {
            qDebug() << ftp->errorString();
            loop.exit(1);
        } else if (id == waitForId) {
            loop.exit(0);
        }
    }

private:
    QFtp * ftp;
    QMap<QString, QDateTime> availableTasks;

    int waitForId;
    QEventLoop loop;

public:
    Q_DECLARE_PUBLIC(NetworkStorage)
    NetworkStorage * q_ptr;
};

#include "moc_networkstorage.cpp"

//
// NetworkStorage

NetworkStorage::NetworkStorage(QObject *parent)
    : QObject(parent), restoreInProgress_(false), checkForChangesTimer_(0), d_ptr(new NetworkStoragePrivate)
{
    Q_D(NetworkStorage);
    d->q_ptr = this;
    d->init();

    // use Qt::QueuedConnection to make sure to loop the event loop on every action to have instant gui updates before
    // updating the network files
    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)), Qt::QueuedConnection);
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)), Qt::QueuedConnection);
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)), Qt::QueuedConnection);

    checkForChangesTimer_ = new QTimer(this);
    connect(checkForChangesTimer_, SIGNAL(timeout()), this, SLOT(checkForChanges()));
    checkForChangesTimer_->start(60 * 1000);
}

NetworkStorage::~NetworkStorage()
{
    delete d_ptr;
}

void NetworkStorage::restoreFromFiles()
{
    restoreInProgress_ = true;

    // reload all task files
    Q_D(NetworkStorage);
    d->loadAllAvailableTasks();

    restoreInProgress_ = false;
}

void NetworkStorage::addTask(const Task & task)
{
    if (restoreInProgress_) return;

    Q_D(NetworkStorage);
    d->saveTaskToFile(task);
}

void NetworkStorage::updateTask(const Task & task)
{
    if (restoreInProgress_) return;

    Q_D(NetworkStorage);
    d->saveTaskToFile(task);
}

void NetworkStorage::removeTask(const TaskId & taskId)
{
    if (restoreInProgress_) return;

    Q_D(NetworkStorage);
    d->removeTask(taskId);
}

void NetworkStorage::checkForChanges()
{
    log << "checking for changes ...";

    if (restoreInProgress_) return;

    restoreInProgress_ = true;

    // check for changes
    Q_D(NetworkStorage);
    d->checkForChanges();

    restoreInProgress_ = false;

    log << "checking for changes ... [done]";
}
