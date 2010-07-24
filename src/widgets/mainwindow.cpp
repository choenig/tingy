#include "mainwindow.h"

#include <core/clock.h>
#include <core/taskmodel.h>
#include <util/qxtglobalshortcut.h>
#include <widgets/quickadddialog.h>

#include <QCloseEvent>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QEvent>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->leAddTask->setPlaceholderText("Add new Task");
    connect(ui->dateBeam, SIGNAL(dateHovered(QDate)), this, SLOT(showDateInStatusbar(QDate)));

    // init stuff
    initActions();
    initSystemTray();    
    initStatusBar();

	QxtGlobalShortcut * globalShortcut = new QxtGlobalShortcut(QKeySequence("Ctrl+Alt+Home"), this);
	globalShortcut->setEnabled(true);
	connect(globalShortcut, SIGNAL(activated()), this, SLOT(globalShortcutTriggered()));
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

void MainWindow::hideEvent(QHideEvent *event)
{
    lastPos = pos();
    QMainWindow::hideEvent(event);
}

void MainWindow::showEvent(QShowEvent * event)
{
    if (!lastPos.isNull()) move(lastPos);
    QMainWindow::showEvent(event);
}

bool MainWindow::event(QEvent * event)
{
//    if (event->type() == QEvent::WindowStateChange) {
//        if (windowState() == Qt::WindowMinimized) {
//            hide();
//        }
//    }

    return QMainWindow::event(event);
}

void MainWindow::on_leAddTask_returnPressed()
{
    Task newTask = Task::createFromString(ui->leAddTask->text());
    if (!newTask.isValid()) return;

    TaskModel::instance()->addTask(newTask);

    ui->leAddTask->clear();

    statusBar()->showMessage("Task added successfully", 5 * 1000);
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
            this,      SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(TaskModel::instance(), SIGNAL(hasOverdueTasks(bool)),
            this,      SLOT(updateTrayIcon(bool)));

    // init context menu
    QMenu * contextMenu = new QMenu;
    contextMenu->addAction(QIcon(":images/exit.png"), "Quit Tingy", qApp, SLOT(quit()));
    trayIcon_->setContextMenu(contextMenu);    

    trayIcon_->show();
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason != QSystemTrayIcon::Trigger) return;

	if (isVisible() && !QApplication::focusWidget()) {
		// get the window to the front ...
		activateWindow();
		raise();
	} else {
		// ... or toggle visibility
		setVisible(!isVisible());
	}
}

void MainWindow::updateTrayIcon(bool hasOverdueTasks)
{
	trayIcon_->setIcon( hasOverdueTasks ? QIcon(":images/NOK.png") : QIcon(":images/OK.png") );
}

void MainWindow::initStatusBar()
{
    lblStatusBarTimestamp_ = new QLabel;
    statusBar()->addPermanentWidget(lblStatusBarTimestamp_);

    QTimer * timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatusBarTime()));
    timer->start(1000);
}

void MainWindow::updateStatusBarTime()
{
    lblStatusBarTimestamp_->setText(Clock::currentDateTime().toString("dd.MM.yyyy   hh:mm:ss"));
}

void MainWindow::showDateInStatusbar(const QDate & date)
{
    if (date.isNull()) statusBar()->clearMessage();
    else               statusBar()->showMessage(QLocale().toString(date, "dddd, dd.MM.yyyy"));
}

void MainWindow::globalShortcutTriggered()
{
    QuickAddDialog dlg;
    connect(&dlg, SIGNAL(showMessage(QString,QString)), this, SLOT(showTrayMessage(QString,QString)));
    dlg.execDlg();
}

void MainWindow::showTrayMessage(const QString & title, const QString & msg)
{
    trayIcon_->showMessage(title, msg, QSystemTrayIcon::NoIcon);
}


