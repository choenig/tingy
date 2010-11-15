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

#include <core/task.h>

//
// StorageEngine

class StorageEngine
{
public:
    virtual ~StorageEngine() {}

    virtual QList<Task> loadTasks()                              =0;
    virtual bool saveTasks(const QList<Task> & tasks)            =0;

    virtual bool addTask(const Task & task)                      =0;
    virtual bool updateTask(const Task & task, bool doneChanged) =0;
    virtual bool removeTask(const TaskId & taskId)               =0;
};

//
// StorageEngines

class StorageEngines : public QList<StorageEngine*>, public StorageEngine
{
public:
    QHash<TaskId, Task> syncTasks();

    virtual QList<Task> loadTasks();
    virtual bool saveTasks(const QList<Task> & tasks);

    virtual bool addTask(const Task & task);
    virtual bool updateTask(const Task & task, bool doneChanged);
    virtual bool removeTask(const TaskId & taskId);
};
