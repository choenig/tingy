#pragma once

#include <QObject>

class NetworkStoragePrivate;

class NetworkStorage : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkStorage);

public:
    NetworkStorage(QObject *parent = 0);

public slots:
    void restoreFromFiles();

private:
    Q_PRIVATE_SLOT(d_func(), void _q_listInfo(QUrlInfo));
    Q_PRIVATE_SLOT(d_func(), void _q_commandFinished(int,bool));
    NetworkStoragePrivate * const d_ptr;
};
