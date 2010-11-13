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

#include <storage/storageengine.h>

#include <QHash>
#include <QObject>

class CalDavNetworkManager;
class QNetworkReply;

class CalDavStorage : public QObject, public StorageEngine
{
    Q_OBJECT
public:
    CalDavStorage(QObject *parent = 0);

public:
    virtual QList<Task> loadTasks();
    virtual bool saveTasks(const QList<Task> & tasks);

    virtual bool addTask(const Task & task);
    virtual bool updateTask(const Task & task, bool doneChanged);
    virtual bool removeTask(const TaskId & taskId);

private slots:
    void checkForChanges();

private:
    CalDavNetworkManager * networkMgr_;
    QHash<TaskId, QString> knownEtags_;
};
