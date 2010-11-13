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
#pragma once

#include <QEventLoop>
#include <QFtp>
#include <QObject>
#include <QRegExp>
#include <QSet>

class Ftp : public QObject
{
    Q_OBJECT

public:
    Ftp();
    ~Ftp();

    QList<QUrlInfo> ls(const QRegExp & fileRe);
    bool put(const QString & filename, QIODevice * ioDevice);
    bool get(const QString &filename, QIODevice *ioDevice);
    bool remove(const QString &filename);

private slots:
    void _q_commandFinished(int id, bool error);
    void _q_listInfo(const QUrlInfo & urlInfo);

private:
    static QFtp * ftp;
    bool isNested;

    QEventLoop loop;
    QSet<int> myIds;

    QRegExp lsRegExp;
    QList<QUrlInfo> lsInfo;
};
