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

FileStorage::FileStorage()
: fileDir_(Settings::dataPath() + "tasks")
{
    if (!fileDir_.exists()) fileDir_.mkpath(fileDir_.absolutePath());
}

QList<Task> FileStorage::loadTasks()
{
    QList<Task> retval;
    const QStringList files = fileDir_.entryList(QStringList("*.task"));
    foreach (const QString & fileName, files) {
        Task task = Task::loadFromFile(fileDir_.absolutePath() + QDir::separator() + fileName);
        if (task.isValid()) retval << task;
    }

    return retval;
}

bool FileStorage::saveTasks(const QList<Task> & tasks)
{
    bool ok = true;
    foreach (const Task & task, tasks) {
        ok = Task::saveToFile(fileDir_.absolutePath() + QDir::separator() + filenameFromTask(task.getId()), task)
             && ok;
    }
    return ok;
}

bool FileStorage::addTask(const Task & task)
{
    return Task::saveToFile(fileDir_.absolutePath() + QDir::separator() + filenameFromTask(task.getId()), task);
}

bool FileStorage::updateTask(const Task & task, bool doneChanged)
{
    const QString filename = fileDir_.absolutePath() + QDir::separator() + filenameFromTask(task.getId());

    // remove 'other' file
    if (doneChanged) QDir().remove(filename + (task.isDone() ? "" : ".done"));

    // update task file
    return Task::saveToFile(filename + (task.isDone() ? ".done" : ""), task);
}

bool FileStorage::removeTask(const TaskId & taskId)
{
    QFile f(fileDir_.absolutePath() + QDir::separator() + filenameFromTask(taskId));
    if (!f.exists()) {
        log(Log::Error) << "cannot find task "<< taskId << " to remove";
        return false;
    }

    return f.remove();
}