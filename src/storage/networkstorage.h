/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#pragma once

#include <storage/storageengine.h>

#include <QObject>

class Task;
class TaskId;
class NetworkStoragePrivate;
class QTimer;

class NetworkStorage : public QObject, public StorageEngine
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkStorage);

public:
    NetworkStorage();
    ~NetworkStorage();

public:
    virtual QList<Task> loadTasks();
    virtual bool saveTasks(const QList<Task> & tasks);
    virtual bool addTask(const Task & task);
    virtual bool updateTask(const Task & task, bool doneChanged);
    virtual bool removeTask(const TaskId & taskId);

private slots:
    void checkForChanges();

private:
    QTimer * checkForChangesTimer_;

private:
    NetworkStoragePrivate * const d_ptr;
};
