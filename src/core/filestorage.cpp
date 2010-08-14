#include "filestorage.h"

#include <core/clock.h>
#include <core/settings.h>
#include <core/task.h>
#include <core/taskmodel.h>
#include <util/log.h>

#include <QFile>
#include <QDir>
#include <QDebug>

FileStorage::FileStorage(QObject * parent)
: QObject(parent), fileDir_(Settings::dataPath() + "tasks")
{
    if (!fileDir_.exists()) {
        fileDir_.mkpath(fileDir_.absolutePath());
    }

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)),     this, SLOT(addTask(Task)),      Qt::QueuedConnection);
    connect(tm, SIGNAL(taskUpdated(Task)),   this, SLOT(updateTask(Task)),   Qt::QueuedConnection);
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)), Qt::QueuedConnection);
}

void FileStorage::restoreFromFiles()
{
    newTasks_.clear();
    const QStringList files = fileDir_.entryList(QStringList("*.task"));
    foreach (const QString & fileName, files) {
        Task task = Task::loadFromFile(fileDir_.absolutePath() + QDir::separator() + fileName);
        if (task.isValid()) newTasks_ << task;
    }

    TaskModel::instance()->init(newTasks_);
}

void FileStorage::addTask(const Task & task)
{
    if (newTasks_.contains(task)) {
        newTasks_.removeOne(task);
        return;
    }

    saveToFile(task);
}

void FileStorage::updateTask(const Task & task)
{
    saveToFile(task);
}

void FileStorage::removeTask(const TaskId & taskId)
{
    QFile f(fileDir_.absolutePath() + QDir::separator() + taskId.toString() + ".task");
    if (!f.exists()) {
        log << "cannot find task "<< taskId.toString() << " to remove";
        return;
    }

    f.remove();
}

void FileStorage::saveToFile(const Task & task)
{
    Task::saveToFile(fileDir_.absolutePath() + QDir::separator() + task.getId().toString() + ".task", task);
}
