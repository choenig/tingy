#include "networkstorage.h"

#include <core/clock.h>
#include <core/taskmodel.h>

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

    void init() {
        Q_Q(NetworkStorage);

        ftp = new QFtp(q);
        q->connect(ftp, SIGNAL(listInfo(QUrlInfo)), q, SLOT(_q_listInfo(QUrlInfo)));
        q->connect(ftp, SIGNAL(commandFinished(int,bool)), q, SLOT(_q_commandFinished(int,bool)));
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
            loadFromFile(tempFiles[file]->fileName());
        }

    }

    void loadFromFile(const QString & fileName)
    {
        QSettings settings(fileName, QSettings::IniFormat);

        const int version = settings.value("fileStorageVersion").toInt();

        Task task;
        task.setId(TaskId::fromString(settings.value("task/id").toString()));
        task.setCreationTimestamp(QDateTime::fromString(settings.value("task/creationTimestamp").toString(), Qt::ISODate));
        task.setPriority((Priority::Level)settings.value("task/priority").toInt());
        task.setDescription(settings.value("task/description").toString());
        task.setDueDate(QDate::fromString(settings.value("task/dueDate").toString(),Qt::ISODate));
        task.setPlannedDate(QDate::fromString(settings.value("task/plannedDate").toString(),Qt::ISODate));
        task.setEffort(Effort(settings.value("task/effort").toUInt()));
        if (version == 0) {
            task.setDone(settings.value("task/done").toBool() ? Clock::currentDateTime() : QDateTime());
        } else {
            task.setDone(QDateTime::fromString(settings.value("task/done").toString(),Qt::ISODate));
        }

        if (task.isValid()) TaskModel::instance()->addTask(task);
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
    : QObject(parent), d_ptr(new NetworkStoragePrivate)
{
    Q_D(NetworkStorage);
    d->q_ptr = this;
    d->init();

    d->connect();
    d->reloadTaskList();

}

void NetworkStorage::restoreFromFiles()
{
    Q_D(NetworkStorage);
    d->loadTaskFiles();
    qDebug() << "done";
}
