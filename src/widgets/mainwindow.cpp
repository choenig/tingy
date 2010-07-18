#include "mainwindow.h"

#include <core/clock.h>
#include <core/taskmodel.h>

#include <QCloseEvent>
#include <QApplication>
#include <QLabel>
#include <QTimer>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->leAddTask->setPlaceholderText("Add new Task");

    connect (TaskModel::instance(), SIGNAL(hasOverdueTasks(bool)), this, SLOT(updateTrayIcon(bool)));

    initActions();
    initSystemTray();
    initStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    // ignore the close event ...
    event->ignore();

    // ... then hide the widget
    hide();
}

void MainWindow::on_leAddTask_returnPressed()
{
    Task newTask = Task::createFromString(ui->leAddTask->text());
    if (!newTask.isValid()) return;

    TaskModel::instance()->addTask(newTask);

    ui->leAddTask->clear();

    statusBar()->showMessage("Task added successfully", 5*1000);
}

void MainWindow::initActions()
{
     hideDoneAction_ = ui->mainToolBar->addAction(QIcon(":images/hideDone.png"), "Hide done tasks", this, SLOT(toggleHideDoneTasks()));
     hideDoneAction_->setCheckable(true);

     // initialy hide done tasks
     hideDoneAction_->setChecked(true);
     toggleHideDoneTasks();
}

void MainWindow::toggleHideDoneTasks()
{
    ui->taskTree->hideDoneTasks(hideDoneAction_->isChecked());
}

void MainWindow::initSystemTray()
{
    trayIcon_ = new QSystemTrayIcon(QIcon(":images/OK.png"), this);
    connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,  SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    QMenu * contextMenu = new QMenu;
    contextMenu->addAction("Quit", qApp, SLOT(quit()));
    trayIcon_->setContextMenu(contextMenu);

    trayIcon_->show();
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger) {
		if (isVisible() && !QApplication::focusWidget()) {
			activateWindow();
			raise();
		} else {
			setVisible(!isVisible());
		}
	}
}

void MainWindow::updateTrayIcon(bool hasOverdueTasks)
{
	trayIcon_->setIcon( hasOverdueTasks ? QIcon(":images/NOK.png") : QIcon(":images/OK.png") );
}

void MainWindow::initStatusBar()
{
    lblStatusBar_ = new QLabel;
    statusBar()->addPermanentWidget(lblStatusBar_);

    QTimer * timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatusBar()));
    timer->start(1000);
}

void MainWindow::updateStatusBar()
{
    lblStatusBar_->setText(Clock::currentDateTime().toString("dd.MM.yyyy   hh:mm:ss"));
}

