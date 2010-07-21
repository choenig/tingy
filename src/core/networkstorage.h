#pragma once

#include <QObject>

class QUrlInfo;

class NetworkStorage : public QObject
{
    Q_OBJECT
public:
    NetworkStorage(QObject *parent = 0);

private:
    void listTaskFiles();

private slots:
    void listInfo(const QUrlInfo & i);
    void commandFinished(int id, bool error);
};
