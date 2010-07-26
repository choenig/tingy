#include "taskmodel.h"

#include <core/clock.h>

TaskModel * TaskModel::instance_ = 0;

TaskModel::TaskModel()
{
	if (instance_ == 0) instance_ = this;

	connect(Clock::instance(), SIGNAL(dateChanged(QDate)), this, SLOT(handleDateChanged()));
}

TaskModel::~TaskModel()
{
	if (instance_ == this) instance_ = 0;
}

void TaskModel::clear()
{
    foreach (const TaskId & taskId, tasks_.keys()) {
        removeTask(taskId);
    }
}

void TaskModel::addTask(const Task & task)
{
    if (tasks_.contains(task.getId())) {
        qFatal("duplicate task in TaskModel %s", (const char*)task.getId().toString().constData());
        return;
    }

    tasks_[task.getId()] = task;
    emit taskAdded(task);

    if (task.isOverdue()) {
        overdueTasks_ << task.getId();
        if (overdueTasks_.size() == 1) {
            emit hasOverdueTasks(true);
        }
    }
}

void TaskModel::updateTask(const Task & task)
{
    Task oldTask = tasks_.value(task.getId());
    if (task == oldTask) return;

    tasks_[task.getId()] = task;
    emit taskUpdated(task);

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
