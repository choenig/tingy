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
