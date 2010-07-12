#include "filestorage.h"

#include <core/task.h>
#include <core/taskmodel.h>

#include <QFile>
#include <QDir>

FileStorage::FileStorage()
: QObject()
{
    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)));
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)));
}

void FileStorage::restoreFromFile()
{
    QFile f(QDir::homePath() + "/.myTasks/tasks.data");
    if (!f.open(QFile::ReadOnly)) return;

    // first remove all available tasks
    foreach (const Task & task, tasks_) {
        TaskModel::instance()->removeTask(task.getId());
   }

    QDataStream in(&f);
    QList<Task> tasks;
    in >> tasks;
    f.close();

    foreach (const Task & task, tasks) {
        TaskModel::instance()->addTask(task);
    }
}

void FileStorage::addTask(const Task & task)
{
    tasks_ << task;
    saveToFile();
}

void FileStorage::updateTask(const Task & task)
{
    removeTask(task.getId());
    addTask(task);
}

void FileStorage::removeTask(const TaskId & taskId)
{
    foreach (const Task & task, tasks_) {
        if (task.getId() == taskId) {
            tasks_.removeAll(task);
            return;
        }
    }
    saveToFile();
}

void FileStorage::saveToFile()
{
    QFile f(QDir::homePath() + "/.myTasks/tasks.data");
    if (!f.open(QFile::WriteOnly)) return;

    QDataStream out(&f);
    out << tasks_;

    f.close();
}
