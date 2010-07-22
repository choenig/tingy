#include "networkstorage.h"

#include <core/clock.h>
#include <core/taskmodel.h>
#include <core/filestorage.h>

#include <QDebug>
#include <QEventLoop>
#include <QFtp>
#include <QMap>
#include <QStringList>
#include <QUrlInfo>
#include <QTemporaryFile>
#include <QSettings>

class NetworkStoragePrivate
{
public:
    NetworkStoragePrivate()
    {
    }

    ~NetworkStoragePrivate()
    {
        delete ftp;
    }

    void init()
    {
        Q_Q(NetworkStorage);

        ftp = new QFtp();
        q->connect(ftp, SIGNAL(listInfo(QUrlInfo)), q, SLOT(_q_listInfo(QUrlInfo)));
        q->connect(ftp, SIGNAL(commandFinished(int,bool)), q, SLOT(_q_commandFinished(int,bool)));
    }

    bool isConnected()
    {
        return ftp->state() == QFtp::LoggedIn;
    }

    void connect()
    {
        ftp->connectToHost("ftp.tingy.de");
        waitForId = ftp->login("tingy", QByteArray::fromBase64("Ym9VblJpcDc="));
        loop.exec();
    }

    void reloadTaskList()
    {
        taskFiles.clear();
        ftp->cd("/home/tingy/tasks");
        waitForId = ftp->list();
        loop.exec();
    }

    void loadTaskFiles()
    {
        if (taskFiles.isEmpty()) return;

        QObject deleteHelper;

        QMap<QString, QTemporaryFile*> tempFiles;
        foreach (const QString & file, taskFiles.keys()) {
            tempFiles[file] = new QTemporaryFile(&deleteHelper);
            tempFiles[file]->open();
            waitForId = ftp->get(file, tempFiles[file], QFtp::Binary);
        }

        loop.exec();

        foreach (const QString & file, taskFiles.keys()) {
            tempFiles[file]->close();
            Task task = Task::loadFromFile(tempFiles[file]->fileName());
            if (task.isValid()) TaskModel::instance()->addTask(task);
        }
    }

    void saveToFile(const Task & task)
    {
        QTemporaryFile tempFile;
        tempFile.open();
        Task::saveToFile(tempFile.fileName(), task);
        tempFile;

        QString fileName = task.getId().toString() + ".task";
        waitForId = ftp->put(&tempFile, fileName);
        qDebug() << waitForId;

        loop.exec();
    }

    void _q_listInfo(const QUrlInfo & urlInfo)
    {
        taskFiles[urlInfo.name()] = urlInfo.lastModified();
    }

    void _q_commandFinished(int id, bool error)
    {
        if (error) {
            qDebug() << ftp->errorString();
            loop.quit();
            return;
        }

        qDebug() << "done" << id;
        if (id == waitForId) {
            loop.quit();
        }
    }

public:
    QFtp * ftp;
    int waitForId;
    QMap<QString, QDateTime> taskFiles;


    QEventLoop loop;

public:
    Q_DECLARE_PUBLIC(NetworkStorage)
    NetworkStorage * q_ptr;
};

#include "moc_networkstorage.cpp"


NetworkStorage::NetworkStorage(QObject *parent)
    : QObject(parent), restoreInProgress_(false), d_ptr(new NetworkStoragePrivate)
{
    Q_D(NetworkStorage);
    d->q_ptr = this;
    d->init();

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)));
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)));
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
    d->loadTaskFiles();

    restoreInProgress_ = false;
}


void NetworkStorage::addTask(const Task & task)
{
    if (restoreInProgress_) return;

    Q_D(NetworkStorage);

    // make sure we are connected
//    if (!d->isConnected()) d->connect();

    d->saveToFile(task);

}

void NetworkStorage::updateTask(const Task & task)
{
    if (restoreInProgress_) return;

    // fixme implement
}

void NetworkStorage::removeTask(const TaskId & taskId)
{
    if (restoreInProgress_) return;

    // fixme implement
}
