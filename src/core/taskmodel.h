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
#include <core/task.h>
#include <core/taskid.h>

#include <QObject>
#include <QHash>
#include <QSet>

class TaskModel : public QObject
{
    Q_OBJECT

public:
    TaskModel();
    ~TaskModel();

    static TaskModel * instance() { return instance_; }

    void addStorageEngine(StorageEngine * storageEngine);

public slots:
    void init();
    void addTask(const Task & task);
    void updateTask(const Task & task);
    void removeTask(const TaskId & taskId);

signals:
    void taskAdded(const Task & task);
    void taskUpdated(const Task & task, bool doneChanged);
    void taskRemoved(const TaskId & taskId);
    void hasOverdueTasks(bool b);

private slots:
	void handleDateChanged();

private:
    QHash<TaskId, Task> tasks_;
    QSet<TaskId> overdueTasks_;

    StorageEngines storageEngines_;

private:
    static TaskModel * instance_;
};
