#include "networkstorage.h"

#include <core/clock.h>
#include <core/taskmodel.h>
#include <core/filestorage.h>

#include <QDebug>
#include <QEventLoop>
#include <QFtp>
#include <QMap>
#include <QRegExp>
#include <QSettings>
#include <QStringList>
#include <QTemporaryFile>
#include <QTimer>
#include <QUrlInfo>

namespace {
QString host = "ftp.tingy.de";
QString username = "tingy";
QString password = "Ym9VblJpcDc=";
QString taskdir  = "/home/tingy/tasks";
}

class NetworkStoragePrivate
{
public:
    NetworkStoragePrivate() : ftp(0) {
    }

    ~NetworkStoragePrivate() {
        delete ftp;
    }

    void init()
    {
        Q_Q(NetworkStorage);

        ftp = new QFtp();
        q->connect(ftp, SIGNAL(listInfo(QUrlInfo)), q, SLOT(_q_listInfo(QUrlInfo)));
        q->connect(ftp, SIGNAL(commandFinished(int,bool)), q, SLOT(_q_commandFinished(int,bool)));
    }

    bool isConnected() {
        return ftp->state() == QFtp::LoggedIn;
    }

    void connect()
    {
        ftp->connectToHost(host);
        waitForId = ftp->login(username, QByteArray::fromBase64(password.toLatin1()));
        loop.exec();
    }

    void reloadListOfAvailableTasks()
    {
        availableTasks.clear();
        ftp->cd(taskdir);
        waitForId = ftp->list();
        loop.exec();
    }

    void loadAvailableTasks()
    {
        foreach (const QString & fileName, availableTasks.keys()) {
            loadTaskFromFile(fileName, true);
        }
    }

    void loadTaskFromFile(const QString & fileName, bool added)
    {
        QTemporaryFile tempFile;
        tempFile.open();

        waitForId = ftp->get(fileName, &tempFile);

        loop.exec();

        tempFile.close();

        Task task = Task::loadFromFile(tempFile.fileName());
        if (task.isValid()) {
            if (added) TaskModel::instance()->addTask(task);
            else       TaskModel::instance()->updateTask(task);
        }
    }

    void saveTaskToFile(const Task & task)
    {
        QTemporaryFile tempFile;
        tempFile.open();
        Task::saveToFile(tempFile.fileName(), task);

        ftp->cd(taskdir);

        QString fileName = task.getId().toString() + ".task";
        waitForId = ftp->put(&tempFile, fileName);

        loop.exec();
    }

    void removeTask(const TaskId & taskId)
    {
        waitForId = ftp->remove(taskId.toString() + ".task");
        loop.exec();
    }

//    void checkForChanges()
//    {
//        QMap<QString, QDateTime> oldAvailableTasks = availableTasks;

//        reloadListOfAvailableTasks();

//        foreach (const QString & fileName, availableTasks.keys())
//        {
//            if (!oldAvailableTasks.contains(fileName)) {
//                loadTaskFromFile(fileName, true);
//                continue;
//            }

//            if (oldAvailableTasks[fileName] != availableTasks[fileName]) {
//                loadTaskFromFile(fileName, false);
//                continue;
//            }
//        }

//        foreach (const QString & fileName, oldAvailableTasks.keys()) {
//            if (!availableTasks.contains(fileName)) {
//                // fixme file has been removed on server
//                qDebug() << "NOT YET IMPLEMENTED file has been removed on server";
//            }
//        }
//    }

    void _q_listInfo(const QUrlInfo & urlInfo)
    {
        QRegExp re("*.task", Qt::CaseSensitive, QRegExp::Wildcard);
        if (re.exactMatch(urlInfo.name())) {
            availableTasks[urlInfo.name()] = urlInfo.lastModified();
        }
    }

    void _q_commandFinished(int id, bool error)
    {
        if (error) {
            qDebug() << ftp->errorString();
            loop.quit();
            return;
        }

        if (id == waitForId) {
            loop.quit();
        }
    }

public:
    QFtp * ftp;
    int waitForId;
    QMap<QString, QDateTime> availableTasks;


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

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)));
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)));

//    checkForChangesTimer_ = new QTimer(this);
//    connect(checkForChangesTimer_, SIGNAL(timeout()), this, SLOT(checkForChanges()));
//    checkForChangesTimer_->start(10 * 1000);
}

NetworkStorage::~NetworkStorage()
{
    delete d_ptr;
}

void NetworkStorage::restoreFromFiles()
{
    restoreInProgress_ = true;

    Q_D(NetworkStorage);

    // make sure we are connected
    if (!d->isConnected()) d->connect();

    // reload task files
    d->reloadListOfAvailableTasks();
    d->loadAvailableTasks();

    restoreInProgress_ = false;
}


void NetworkStorage::addTask(const Task & task)
{
    if (restoreInProgress_) return;

    Q_D(NetworkStorage);

    // make sure we are connected
    if (!d->isConnected()) d->connect();

    d->saveTaskToFile(task);
}

void NetworkStorage::updateTask(const Task & task)
{
    if (restoreInProgress_) return;

    Q_D(NetworkStorage);

    // make sure we are connected
    if (!d->isConnected()) d->connect();

    d->saveTaskToFile(task);
}

void NetworkStorage::removeTask(const TaskId & taskId)
{
    if (restoreInProgress_) return;

    Q_D(NetworkStorage);

    // make sure we are connected
    if (!d->isConnected()) d->connect();

    d->removeTask(taskId);
}

void NetworkStorage::checkForChanges()
{
//    if (restoreInProgress_) return;

//    Q_D(NetworkStorage);

//    // make sure we are connected
//    if (!d->isConnected()) d->connect();

//    d->checkForChanges();
}
