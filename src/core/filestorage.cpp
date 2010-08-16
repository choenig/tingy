#include "filestorage.h"

#include <core/clock.h>
#include <core/settings.h>
#include <core/task.h>
#include <core/taskmodel.h>
#include <util/log.h>

#include <QFile>
#include <QDir>
#include <QDebug>

namespace {
QString filenameFromTask(const TaskId & taskId) {
    return taskId.toString() + ".task";
}
}

FileStorage::FileStorage(QObject * parent)
: QObject(parent), fileDir_(Settings::dataPath() + "tasks")
{
    if (!fileDir_.exists()) {
        fileDir_.mkpath(fileDir_.absolutePath());
    }

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)),        this, SLOT(addTask(Task)),         Qt::QueuedConnection);
    connect(tm, SIGNAL(taskUpdated(Task,bool)), this, SLOT(updateTask(Task,bool)), Qt::QueuedConnection);
    connect(tm, SIGNAL(taskRemoved(TaskId)),    this, SLOT(removeTask(TaskId)),    Qt::QueuedConnection);
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

    Task::saveToFile(fileDir_.absolutePath() + QDir::separator() + filenameFromTask(task.getId()), task);
}

void FileStorage::updateTask(const Task & task, bool doneChanged)
{
    const QString filename = fileDir_.absolutePath() + QDir::separator() + filenameFromTask(task.getId());

    // remove 'other' file
    if (doneChanged) QDir().remove(filename + (task.isDone() ? "" : ".done"));

    // update task file
    Task::saveToFile(filename + (task.isDone() ? ".done" : ""), task);
}

void FileStorage::removeTask(const TaskId & taskId)
{
    QFile f(fileDir_.absolutePath() + QDir::separator() + filenameFromTask(taskId));
    if (!f.exists()) {
        log << "cannot find task "<< taskId.toString() << " to remove";
        return;
    }

    f.remove();
}
