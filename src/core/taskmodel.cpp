#include "taskmodel.h"

#include <core/clock.h>
#include <util/log.h>

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

    QHash<TaskId, Task> tasks;

	// first gather all tasks from all storage engines ...
	foreach (StorageEngine * storageEngine, storageEngines_)
	{
		foreach (const Task & task, storageEngine->loadTasks())
		{
			if (tasks.contains(task.getId())) {
				if (task.getLastChanged() > tasks[task.getId()].getLastChanged()) {
					tasks[task.getId()] = task;
				}
			} else {
				tasks.insert(task.getId(), task);
			}
		}
	}

    // ... then add tasks
    foreach (const Task & task, tasks)
    {
        tasks_[task.getId()] = task;
        emit taskAdded(task);

        if (task.isOverdue()) {
            overdueTasks_ << task.getId();
            if (overdueTasks_.size() == 1) emit hasOverdueTasks(true);
        }
    }

    // finally sync all storage engines
    storageEngines_.saveTasks(tasks.values());
}

void TaskModel::addTask(const Task & task)
{
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
