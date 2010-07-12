#pragma once

#include <QTreeWidget>

class Task;

//
// TaskTree

class TaskTree : public QTreeWidget
{
    Q_OBJECT

public:
    TaskTree(QWidget * parent = 0);

private slots:
    void addTask(const Task & task);
};
