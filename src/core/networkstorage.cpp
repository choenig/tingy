#include "networkstorage.h"

#include <QDebug>
#include <QUrlInfo>
#include <QFtp>
#include <QStringList>

QFtp * ftp = 0;
int lsId;
QStringList taskFiles;

NetworkStorage::NetworkStorage(QObject *parent)
    : QObject(parent)
{
    ftp = new QFtp(this);
    connect(ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(listInfo(QUrlInfo)));
    connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(commandFinished(int,bool)));

    ftp->connectToHost("www.hoenig.cc");
    ftp->login("tingy", QByteArray::fromBase64("Ym9VblJpcDc="));

    listTaskFiles();
}

void NetworkStorage::listTaskFiles()
{
    taskFiles.clear();
    ftp->cd("/home/tingy/tasks");
    lsId = ftp->list();
}

void NetworkStorage::listInfo(const QUrlInfo &i)
{
    taskFiles << i.name();
}

void NetworkStorage::commandFinished(int id, bool error)
{
    if (id == lsId) {
        qDebug() << taskFiles.join(", ");
    }
}
