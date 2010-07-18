#pragma once

#include <core/task.h>

#include <QDialog>

namespace Ui { class TaskEditWidget; }

class TaskEditWidget : public QDialog
{
    Q_OBJECT

public:
    TaskEditWidget(QWidget *parent = 0);
    ~TaskEditWidget();

    Task exec(const Task & task);

protected:
    virtual void paintEvent(QPaintEvent * paintEvent);

private:
    Ui::TaskEditWidget *ui;
};
