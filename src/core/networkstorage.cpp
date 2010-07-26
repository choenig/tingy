#include "networkstorage.h"

#include <core/clock.h>
#include <core/filestorage.h>
#include <core/settings.h>
#include <core/taskmodel.h>

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

class NetworkStoragePrivate
{
public:
    NetworkStoragePrivate() : ftp(0) {}

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
        ftp->connectToHost(Settings::NetworkStorage::Hostname());
        waitForId = ftp->login(Settings::NetworkStorage::Username(),
                               QByteArray::fromBase64(Settings::NetworkStorage::Password().toLatin1()));

        loop.exec();
    }

    void reloadListOfAvailableTasks()
    {
        availableTasks.clear();

        ftp->cd( Settings::NetworkStorage::Taskdir() );
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

        ftp->cd(Settings::NetworkStorage::Taskdir());

        QString filename = task.getId().toString() + ".task";
        waitForId = ftp->put(&tempFile, filename);

        loop.exec();

        availableTasks[filename] = QDateTime();
    }

    void removeTask(const TaskId & taskId)
    {
        const QString filename = taskId.toString() + ".task";

        if (availableTasks.contains(filename)) {
            waitForId = ftp->remove(filename);
            loop.exec();

            availableTasks.remove(filename);
        }
    }

    void checkForChanges()
    {
        // remember old list
        const QMap<QString, QDateTime> oldAvailableTasks = availableTasks;

        // reload list of tasks
        reloadListOfAvailableTasks();

        // check for new and updated tasks
        foreach (const QString & filename, availableTasks.keys())
        {
            // task is new, add it
            if (!oldAvailableTasks.contains(filename)) {
                loadTaskFromFile(filename, true);
                continue;
            }

            // task changed
            if (oldAvailableTasks[filename] != availableTasks[filename]) {
                loadTaskFromFile(filename, false);
                continue;
            }
        }

        // check for removed tasks
        foreach (const QString & filename, oldAvailableTasks.keys()) {
            if (!availableTasks.contains(filename)) {
                TaskModel::instance()->removeTask(TaskId::fromString(QString(filename).remove(".task")));
            }
        }
    }

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

    checkForChangesTimer_ = new QTimer(this);
    connect(checkForChangesTimer_, SIGNAL(timeout()), this, SLOT(checkForChanges()));
    checkForChangesTimer_->start(10 * 1000);
}

NetworkStorage::~NetworkStorage()
{
    delete d_ptr;
}

void NetworkStorage::restoreFromFiles()
{
    Q_D(NetworkStorage);

    // make sure we are connected
    if (!d->isConnected()) d->connect();

    restoreInProgress_ = true;

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
    if (restoreInProgress_) return;

    Q_D(NetworkStorage);

    // make sure we are connected
    if (!d->isConnected()) d->connect();

    restoreInProgress_ = true;

    // check for changes
    d->checkForChanges();

    restoreInProgress_ = false;
}
