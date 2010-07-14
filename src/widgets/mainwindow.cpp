#include "mainwindow.h"

#include <core/taskmodel.h>

#include <QCloseEvent>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->leFilter->setInfoText("Filter");
    ui->leAddTask->setInfoText("Add Task");

    initActions();
    initSystemTray();
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
     hideDoneAction_->setChecked(true);

     // initialy hide done tasks
     toggleHideDoneTasks();
}

void MainWindow::toggleHideDoneTasks()
{
    ui->taskTree->hideDoneTasks(hideDoneAction_->isChecked());
}

void MainWindow::initSystemTray()
{
    QSystemTrayIcon * trayIcon = new QSystemTrayIcon(QIcon(":images/yellowBall.png"), this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,  SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    QMenu * contextMenu = new QMenu;
    contextMenu->addAction("Quit", qApp, SLOT(quit()));
    trayIcon->setContextMenu(contextMenu);

    trayIcon->show();
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        setVisible(!isVisible());
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    // ignore the close event ...
    event->ignore();

    // ... then hide the widget
    hide();
}
