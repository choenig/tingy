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
#include "taskmodel.h"

#include <core/clock.h>
#include <util/log.h>

//
// TaskModel

TaskModel * TaskModel::instance_ = 0;

TaskModel::TaskModel()
{
	if (instance_ == 0) instance_ = this;

	connect(Clock::instance(), SIGNAL(dateChanged(QDate)), this, SLOT(handleDateChanged()));
}

TaskModel::~TaskModel()
{
	if (instance_ == this) instance_ = 0;

	foreach (StorageEngine * storageEngine, storageEngines_) {
		delete storageEngine;
	}
}

void TaskModel::addStorageEngine(StorageEngine * storageEngine)
{
	storageEngines_ << storageEngine;
}

void TaskModel::init()
{
    Q_ASSERT(tasks_.isEmpty());

    const QHash<TaskId, Task> tasks = storageEngines_.syncTasks();
    foreach (const Task & task, tasks)
    {
        tasks_[task.getId()] = task;
        emit taskAdded(task);

        if (task.isOverdue()) {
            overdueTasks_ << task.getId();
            if (overdueTasks_.size() == 1) emit hasOverdueTasks(true);
        }
    }
}

void TaskModel::addTask(const Task & task)
{
    if (!task.isValid()) return;

    if (tasks_.contains(task.getId())) {
        qFatal("duplicate task in TaskModel %s", (const char*)task.getId().toString().constData());
        return;
    }

    if (!storageEngines_.addTask(task)) {
        log(Log::Fatal) << "Error adding Task to storage engines" << task.getId();
        // fixme inform user that something bad happened
        return;
    }

    tasks_[task.getId()] = task;
    emit taskAdded(task);

    if (task.isOverdue()) {
        overdueTasks_ << task.getId();
        if (overdueTasks_.size() == 1) emit hasOverdueTasks(true);
    }
}

void TaskModel::updateTask(const Task & task)
{
    if (!task.isValid()) return;

    Task oldTask = tasks_.value(task.getId());
    if (task == oldTask) return;

    if (!storageEngines_.updateTask(task, oldTask.isDone() != task.isDone()))  {
        log(Log::Fatal) << "Error updating Task in storage engines" << task.getId();
        // fixme inform user that something bad happened
        return;
    }

    tasks_[task.getId()] = task;
    emit taskUpdated(task, oldTask.isDone() != task.isDone());

    if (overdueTasks_.contains(task.getId()) != task.isOverdue()) {
        if (task.isOverdue()) {
            overdueTasks_ << task.getId();
            if (overdueTasks_.size() == 1) emit hasOverdueTasks(true);
        } else {
             overdueTasks_.remove(task.getId());
             if (overdueTasks_.isEmpty()) emit hasOverdueTasks(false);
        }
    }
}

void TaskModel::removeTask(const TaskId & taskId)
{
    if (!tasks_.contains(taskId)) return;

    if (!storageEngines_.removeTask(taskId)) {
        log(Log::Fatal) << "Error removing task from storage engines" << taskId;
        // fixme inform user that something bad happened
        return;
    }

    tasks_.remove(taskId);
    emit taskRemoved(taskId);

    overdueTasks_.remove(taskId);
    if (overdueTasks_.isEmpty()) emit hasOverdueTasks(false);
}

void TaskModel::handleDateChanged()
{
	QSet<TaskId> oldOverdueTasks = overdueTasks_;

	foreach (const Task & task, tasks_) {
		if (overdueTasks_.contains(task.getId()) != task.isOverdue()) {
			if (task.isOverdue()) overdueTasks_ << task.getId();
			else                  overdueTasks_.remove(task.getId());
		}
	}

	if (oldOverdueTasks.isEmpty() != overdueTasks_.isEmpty()) {
		emit hasOverdueTasks(!overdueTasks_.isEmpty());
	}
}
