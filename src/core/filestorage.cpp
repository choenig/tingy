#include "filestorage.h"

#include <core/clock.h>
#include <core/task.h>
#include <core/taskmodel.h>

#include <QFile>
#include <QDir>
#include <QDebug>

FileStorage::FileStorage()
: QObject(), fileDir_(QDir::homePath() + QDir::separator() + ".tingy/tasks"), restoreInProgress_(false)
{
    if (!fileDir_.exists()) {
        fileDir_.mkpath(fileDir_.absolutePath());
    }

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)));
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)));
}

void FileStorage::restoreFromFiles()
{
    restoreInProgress_ = true;

    // first remove all available tasks ...
    TaskModel::instance()->clear();

    // ... then load the tasks from files
    const QStringList files = fileDir_.entryList(QStringList("*.task"));
    foreach (const QString & fileName, files) {
        Task task = Task::loadFromFile(fileDir_.absolutePath() + QDir::separator() + fileName);
        if (task.isValid()) TaskModel::instance()->addTask(task);
    }

    restoreInProgress_ = false;
}

void FileStorage::addTask(const Task & task)
{
    if (restoreInProgress_) return;

    saveToFile(task);
}

void FileStorage::updateTask(const Task & task)
{
    if (restoreInProgress_) return;

    saveToFile(task);
}

void FileStorage::removeTask(const TaskId & taskId)
{
    if (restoreInProgress_) return;

    QFile f(fileDir_.absolutePath() + QDir::separator() + taskId.toString() + ".task");
    if (!f.exists()) {
        qWarning("cannot find task %s to remove", (const char*)taskId.toString().constData());
        return;
    }

    f.remove();
}

void FileStorage::saveToFile(const Task & task)
{
    Task::saveToFile(fileDir_.absolutePath() + QDir::separator() + task.getId().toString() + ".task", task);
}

