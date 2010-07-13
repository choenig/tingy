#include "mainwindow.h"

#include <core/taskmodel.h>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->leFilter->setInfoText("Filter");
    ui->leAddTask->setInfoText("Add Task");

    initActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_leAddTask_returnPressed()
{
    Task newTask = Task::createFromString(ui->leAddTask->text());
    if (!newTask.isValid()) return;

    TaskModel::instance()->addTask(newTask);

    ui->leAddTask->clear();
}

void MainWindow::initActions()
{
     hideDoneAction_ = ui->mainToolBar->addAction(QIcon(":images/hideDone.png"), "Hide done tasks", this, SLOT(toggleHideDoneTasks()));
     hideDoneAction_->setCheckable(true);
}

void MainWindow::toggleHideDoneTasks()
{
    ui->taskTree->hideDoneTasks(hideDoneAction_->isChecked());
}

