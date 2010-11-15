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
#include "storageengine.h"

#include <QHash>
#include <QList>
#include <QMap>

namespace {

void mergeTasks(QHash<TaskId,Task> & tasks, QList<Task> tasksToAdd)
{
	foreach (const Task & task, tasksToAdd)
	{
		if (tasks.contains(task.getId())) {
			if (task.getLastChanged() > tasks[task.getId()].getLastChanged()) {
				tasks[task.getId()] = task;
			}
		} else {
			tasks[task.getId()] = task;
		}
	}
}

}

//
// StorageEngines

QHash<TaskId, Task> StorageEngines::syncTasks()
{
	QHash<TaskId, Task> tasks;

	// remember which tasks came from which tasklist
	QMap<StorageEngine*, QHash<TaskId,Task>> loadTaskLists;

	// first gather and merge all tasks from all storage engines ...
	foreach (StorageEngine * storageEngine, *this) {
		QList<Task> newTasks = storageEngine->loadTasks();
		mergeTasks(loadTaskLists[storageEngine], newTasks);
		mergeTasks(tasks, newTasks);
	}

	// .. then prepare for save ...
	QMap<StorageEngine*, QList<Task>> saveTaskLists;
	foreach (const Task & task, tasks)
	{
		foreach (StorageEngine * storageEngine, *this)
		{
			const QHash<TaskId,Task> & currentLoadList = loadTaskLists[storageEngine];
			if (currentLoadList.contains(task.getId())) {
				if (currentLoadList[task.getId()] != task) {
					saveTaskLists[storageEngine] << task;
				}
			} else {
					saveTaskLists[storageEngine] << task;
			}
		}
	}

	// finally sync all storage engines
	foreach (StorageEngine * storageEngine, saveTaskLists.keys()) {
		storageEngine->saveTasks(saveTaskLists[storageEngine]);
	}

	return tasks;
}

QList<Task> StorageEngines::loadTasks()
{
	QList<Task> tasks;
	foreach(StorageEngine * storageEngine, *this) { tasks += storageEngine->loadTasks(); }
	return tasks;
}

bool StorageEngines::saveTasks(const QList<Task> &tasks)
{
	bool ok = true;
	foreach(StorageEngine * storageEngine, *this) { ok = storageEngine->saveTasks(tasks) && ok; }
	return ok;
}

bool StorageEngines::addTask(const Task & task)
{
	bool ok = true;
	foreach(StorageEngine * storageEngine, *this) { ok = storageEngine->addTask(task) && ok; }
	return ok;
}

bool StorageEngines::updateTask(const Task & task, bool doneChanged)
{
	bool ok = true;
	foreach(StorageEngine * storageEngine, *this) { ok = storageEngine->updateTask(task, doneChanged) && ok; }
	return ok;
}

bool StorageEngines::removeTask(const TaskId & taskId)
{
	bool ok = true;
	foreach(StorageEngine * storageEngine, *this) { ok = storageEngine->removeTask(taskId) && ok; }
	return ok;
}

