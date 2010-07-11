#include "tasktree.h"

#include <core/task.h>
#include <core/taskmodel.h>

#include <QTreeWidgetItem>
#include <QHeaderView>

TaskTree::TaskTree(QWidget *parent)
    : QTreeWidget(parent)
{
    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
}

void TaskTree::addTask(const Task & task)
{
    QStringList data;
    data << task.toString();
    if (task.getEffort().isValid()) data << task.getEffort().toString("hh:mm") ;
    else                            data << "";

    if (task.getDueDate().isValid()) data << task.getDueDate().toString("dd.MM.yyyy");
    else                             data << "";

    new QTreeWidgetItem(this, data);

    // fixme: move this out of here
    setColumnWidth(0, 300);
    setColumnWidth(1, 60);
    setColumnWidth(2, 50);
}
