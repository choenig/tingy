#include "tasktree.h"

#include <core/task.h>
#include <core/taskmodel.h>

#include <QTreeWidgetItem>

TaskTree::TaskTree(QWidget *parent)
    : QTreeWidget(parent)
{
    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
}

void TaskTree::addTask(const Task & task)
{
    QStringList data;
    data << task.toString() << task.getId().toString();

    new QTreeWidgetItem(this, data);
}
