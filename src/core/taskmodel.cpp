#include "taskmodel.h"

#include <QFile>
#include <QDir>

#include <QDebug>

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

bool TaskModel::saveToFile() const
{
    QFile f(QDir::homePath() + "/.myTasks/tasks.data");
    if (!f.open(QFile::WriteOnly)) return false;

    QDataStream out(&f);
    out << tasks_;

    f.close();
    return true;
}

bool TaskModel::restoreFromFile()
{
    QFile f(QDir::homePath() + "/.myTasks/tasks.data");
    if (!f.open(QFile::ReadOnly)) return false;

    // first remove all available tasks
    foreach (const TaskId & taskId, tasks_.keys()) {
        tasks_.remove(taskId);
        emit taskRemoved(taskId);
    }

    QDataStream in(&f);
    in >> tasks_;
    f.close();

    foreach (const TaskId & taskId, tasks_.keys()) {
        emit taskAdded(tasks_.value(taskId));
    }

    return true;
}

void TaskModel::addTask(const Task & task)
{
    if (tasks_.contains(task.getId())) {
        qFatal("duplicate task in TaskModel %s", (const char*)task.getId().toString().constData());
        return;
    }

    tasks_[task.getId()] = task;

    qDebug() << "Added Task:" << task.toString();
    emit taskAdded(task);
}

void TaskModel::updateTask(const Task & task)
{
    Task oldTask = tasks_.value(task.getId());

    if (oldTask == task) return;

    tasks_[task.getId()] = task;

    qDebug() << "Added Task:" << task.toString();
    emit taskUpdated(task);
}

void TaskModel::removeTask(const TaskId & taskId)
{
    if (!tasks_.contains(taskId)) return;

    // remember task for debug output
    Task oldTask = tasks_.value(taskId);

    tasks_.remove(taskId);

    qDebug() << "removed Task:" << oldTask.toString();
    emit taskRemoved(taskId);
}
