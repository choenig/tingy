#include "storageengine.h"

//
// StorageEngines

QList<Task> StorageEngines::loadTasks()
{
	Q_ASSERT(0); // restoreFromFiles() not implemented
	return QList<Task>();
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
