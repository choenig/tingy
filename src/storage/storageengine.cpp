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
	foreach(StorageEngine * storageEngine, *this) { ok = ok && storageEngine->saveTasks(tasks); }
	return ok;
}

bool StorageEngines::addTask(const Task & task)
{
	bool ok = true;
	foreach(StorageEngine * storageEngine, *this) { ok = ok && storageEngine->addTask(task); }
	return ok;
}

bool StorageEngines::updateTask(const Task & task, bool doneChanged)
{
	bool ok = true;
	foreach(StorageEngine * storageEngine, *this) { ok = ok && storageEngine->updateTask(task, doneChanged); }
	return ok;
}

bool StorageEngines::removeTask(const TaskId & taskId)
{
	bool ok = true;
	foreach(StorageEngine * storageEngine, *this) { ok = ok && storageEngine->removeTask(taskId); }
	return ok;
}

