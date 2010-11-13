/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of Tingy.
**
** Tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with Tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
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
                            Settings::NetworkStorage::Password());
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

bool Ftp::put(const QString & filename, QIODevice * ioDevice)
{
    myIds << ftp->put(ioDevice, filename);
    return loop.exec() == 0;
}

bool Ftp::get(const QString & filename, QIODevice * ioDevice)
{
    myIds << ftp->get(filename, ioDevice);
    return loop.exec() == 0;
}

bool Ftp::remove(const QString & filename)
{
    myIds << ftp->remove(filename);
    return loop.exec() == 0;
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
