#include "ftp.h"

#include <core/settings.h>
#include <util/log.h>

#include <QDebug>
#include <QRegExp>

namespace {
QString toString(QFtp::Command cmd)
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
}

QFtp * Ftp::ftp = 0;

Ftp::Ftp() : QObject(), isNested(ftp != 0)
{
    if (!isNested) {
        ftp = new QFtp();
    }

    connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(_q_commandFinished(int,bool)));
    connect(ftp, SIGNAL(listInfo(QUrlInfo)),        this, SLOT(_q_listInfo(QUrlInfo)));

    if (!isNested) {
        myIds << ftp->connectToHost(Settings::NetworkStorage::Hostname());
        myIds << ftp->login(Settings::NetworkStorage::Username(),
                            QByteArray::fromBase64(Settings::NetworkStorage::Password().toLatin1()));
        myIds << ftp->cd(Settings::NetworkStorage::Taskdir());
        loop.exec();
    }
}

Ftp::~Ftp()
{
    if (isNested) return;

    myIds << ftp->close();
    loop.exec();

    delete ftp;
    ftp = 0;
}

QList<QUrlInfo> Ftp::ls(const QRegExp & fileRe)
{
    lsRegExp = fileRe;

    myIds << ftp->list();
    loop.exec();

    return lsInfo;
}

void Ftp::put(const QString & filename, QIODevice * ioDevice)
{
    myIds << ftp->put(ioDevice, filename);
    loop.exec();
}

void Ftp::get(const QString & filename, QIODevice * ioDevice)
{
    myIds << ftp->get(filename, ioDevice);
    loop.exec();
}

void Ftp::remove(const QString & filename)
{
    myIds << ftp->remove(filename);
    loop.exec();
}

void Ftp::_q_commandFinished(int id, bool error)
{
    if (!myIds.contains(id)) return;

    myIds.remove(id);

    log << "_q_commandFinished();" << id << error << toString(ftp->currentCommand());

    if (error) {
        qDebug() << ftp->errorString();
        loop.exit(1);
    } else if (myIds.isEmpty()) {
        loop.exit(0);
    }
}

void Ftp::_q_listInfo(const QUrlInfo & urlInfo)
{
    if (myIds.contains(ftp->currentId())) {
        if (lsRegExp.exactMatch(urlInfo.name())) lsInfo << urlInfo;
    }
}