#include "filestorage.h"

#include <core/task.h>
#include <core/taskmodel.h>

#include <QFile>
#include <QDir>
#include <QDebug>

FileStorage::FileStorage()
: QObject(), fileDir_(QDir::homePath() + QDir::separator() + ".myTasks"), restoreInProgress_(false)
{
    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)));
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)));
}

void FileStorage::restoreFromFiles()
{
    QList<Task> tasks;

    const QStringList files = fileDir_.entryList(QStringList("*.task"));
    foreach (const QString & fileName, files) {
        QFile f(fileDir_.absolutePath() + QDir::separator() + fileName);
        if (f.open(QFile::ReadOnly)) {
            QDataStream in(&f);
            Task task;
            in >> task;
            tasks << task;
            f.close();
        }
    }

    if (tasks.isEmpty()) return;

    restoreInProgress_ = true;
    {
        // first remove all available tasks
        TaskModel::instance()->clear();

        foreach (const Task & task, tasks) {
            TaskModel::instance()->addTask(task);
        }
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
    QFile f(fileDir_.absolutePath() + QDir::separator() + task.getId().toString() + ".task");
    if (!f.open(QFile::WriteOnly)) return;

    QDataStream out(&f);
    out << task;

    f.close();
}
