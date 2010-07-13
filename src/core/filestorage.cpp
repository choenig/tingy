#include "filestorage.h"

#include <core/task.h>
#include <core/taskmodel.h>

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>

FileStorage::FileStorage()
: QObject(), fileDir_(QDir::homePath() + QDir::separator() + ".myTasks/tasks"), restoreInProgress_(false)
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
        Task task = loadFromFile(fileDir_.absolutePath() + QDir::separator() + fileName);
        TaskModel::instance()->addTask(task);
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
    QSettings settings(fileDir_.absolutePath() + QDir::separator() + task.getId().toString() + ".task", QSettings::IniFormat);
    settings.setValue("task/id", task.getId().toString());
    settings.setValue("task/creationTimestamp", task.getCreationTimestamp().toString(Qt::ISODate));
    settings.setValue("task/importance", task.getImportance().toInt());
    settings.setValue("task/description", task.getDescription());
    settings.setValue("task/dueDate", task.getDueDate().toString(Qt::ISODate));
    settings.setValue("task/plannedDate", task.getPlannedDate().toString(Qt::ISODate));
    settings.setValue("task/effort", task.getEffort().toString(Qt::ISODate));
    settings.setValue("task/done", task.isDone());
}

Task FileStorage::loadFromFile(const QString & filePath)
{
    Task task;

    QSettings settings(filePath, QSettings::IniFormat);
    task.setId(TaskId::fromString(settings.value("task/id").toString()));
    task.setCreationTimestamp(QDateTime::fromString(settings.value("task/creationTimestamp").toString(), Qt::ISODate));
    task.setImportance((Importance::Level)settings.value("task/importance").toInt());
    task.setDescription(settings.value("task/description").toString());
    task.setDueDate(QDate::fromString(settings.value("task/dueDate").toString(),Qt::ISODate));
    task.setPlannedDate(QDate::fromString(settings.value("task/plannedDate").toString(),Qt::ISODate));
    task.setEffort(QTime::fromString(settings.value("task/effort").toString(),Qt::ISODate));
    task.setDone(settings.value("task/done").toBool());

    return task;
}
