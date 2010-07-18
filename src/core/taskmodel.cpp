#include "taskmodel.h"

TaskModel * TaskModel::instance_ = 0;

TaskModel::TaskModel()
    : isFirstInstance_(instance_ == 0)
{
    if (isFirstInstance_) instance_ = this;
}

TaskModel::~TaskModel()
{
    if (isFirstInstance_) instance_ = 0;
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

    if (oldTask == task) return;

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

    // remember task for debug output
    Task oldTask = tasks_.value(taskId);

    tasks_.remove(taskId);
    emit taskRemoved(taskId);

    overdueTasks_.remove(taskId);
    if (overdueTasks_.isEmpty()) emit hasOverdueTasks(false);
}
