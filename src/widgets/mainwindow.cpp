#include "mainwindow.h"

#include <core/taskmodel.h>

#include "ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->leFilter->setInfoText("Filter");
    ui->leAddTask->setInfoText("Add Task");

    QTimer::singleShot(0, this, SLOT(init()));
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

    TaskModel::instance()->saveToFile();
}

void MainWindow::init()
{
    TaskModel::instance()->restoreFromFile();
}
