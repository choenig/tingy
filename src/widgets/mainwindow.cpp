#include "mainwindow.h"

#include <core/clock.h>
#include <core/taskmodel.h>
#include <qtextlib/qxtglobalshortcut.h>
#include <util/log.h>
#include <widgets/quickadddialog.h>

#include <QCloseEvent>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QEvent>
#include <QShortcut>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent), quickAddDlg_(0), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->leAddTask->setInfoText("Neuen Task hinzufügen");
    ui->leAddTask->setLeftIcon(QPixmap(":/images/add.png"));
    ui->leAddTask->setRightIcon(QPixmap(":/images/clear.png"));

    ui->pbInfo->setIcon(QPixmap(":/images/info.png"));

    connect(ui->dateBeam, SIGNAL(dateHovered(QDate)), this, SLOT(showDateInStatusbar(QDate)));
    connect(ui->dateBeam, SIGNAL(dateHovered(QDate)), ui->taskTree, SLOT(highlightDate(QDate)));

    quickAddDlg_ = new QuickAddDialog();
    connect(quickAddDlg_, SIGNAL(showMessage(QString,QString)), this, SLOT(showTrayMessage(QString,QString)));

    // init stuff
    initActions();
    initSystemTray();    
    initStatusBar();

	QxtGlobalShortcut * globalShortcut = new QxtGlobalShortcut(QKeySequence("Ctrl+Alt+Home"), this);
	globalShortcut->setEnabled(true);
	connect(globalShortcut, SIGNAL(activated()), this, SLOT(globalShortcutTriggered()));

	new QShortcut(QKeySequence("Ctrl+Shift+D"), this, SLOT(dumpLog()));

	// hide window on Escape
	new QShortcut(QKeySequence("Esc"), this, SLOT(hide()));
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
    ui->leAddTask->setFocus();
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

void MainWindow::on_pbInfo_clicked()
{
    QString msg;
    msg =
            "<b>Informationen</b>"
            "<table>"
            "<tr>"
            "  <th>&nbsp;&nbsp;Tag&nbsp;&nbsp;</th>"
            "  <th align=left>Beschreibung</th>"
            "  <th align=left>Beispiele</th>"
            "</tr>"
            "<tr>"
            "  <td align=center>*</td>"
            "  <td>Due Date</td>"
            "  <td>*12.03.1978<br>*today<br>*tomorrow<br>*+5d</td>"
            "</tr>"
            "<tr>"
            "  <td align=center>!</td>"
            "  <td>Wichtigkeit</td>"
            "  <td>!+<br>!-</td>"
            "</tr>"
            "<tr>"
            "  <td align=center>$</td>"
            "  <td>Aufwand</td>"
            "  <td>$1h45m</td>"
            "</tr>"
            "<tr>"
            "  <td align=center>//</td>"
            "  <td>Zeilenumbruch</td>"
            "  <td>Neue // Zeile</td>"
            "</tr>"
            "<tr>"
            "  <td align=center>///</td>"
            "  <td>Trenner zwischen Titel und Beschreibung</td>"
            "  <td>Titel /// Beschreibung</td>"
            "</tr>"
            "</table>"
            ;

    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}

void MainWindow::initActions()
{
    showDoneTasksAction_ = ui->mainToolBar->addAction(QIcon(":/images/done.png"), "Abgeschlossene Tasks einblenden",
                                                      this, SLOT(toggleShowDoneTasks()));
    showDoneTasksAction_->setCheckable(true);

    // spacer used to 'squeeze' the filterLineEdit
    QWidget * spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
    ui->mainToolBar->addWidget(spacerWidget);

    LineEdit * filterLineEdit = new LineEdit(this);
    filterLineEdit->setInfoText("Filter");
    filterLineEdit->setLeftIcon(QPixmap(":/images/mag.png"));
    filterLineEdit->setRightIcon(QPixmap(":/images/clear.png"));
    connect(filterLineEdit, SIGNAL(textChanged(QString)), ui->taskTree, SLOT(filterItems(QString)));
    filterLineEdit->setMaximumWidth(250);
    ui->mainToolBar->addWidget(filterLineEdit);

    new QShortcut(QKeySequence("Ctrl+F"), filterLineEdit, SLOT(setFocus()));

    // initialy hide done tasks
    toggleShowDoneTasks();
}

void MainWindow::toggleShowDoneTasks()
{
    ui->taskTree->showDoneTasks(showDoneTasksAction_->isChecked());
}

void MainWindow::initSystemTray()
{
    trayIcon_ = new QSystemTrayIcon(QIcon(":/images/OK.png"), this);
    connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,      SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(TaskModel::instance(), SIGNAL(hasOverdueTasks(bool)),
            this,      SLOT(updateTrayIcon(bool)));

    // init context menu
    QMenu * contextMenu = new QMenu;
    contextMenu->addAction(QIcon(":/images/exit.png"), "Quit Tingy", qApp, SLOT(quit()));
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
	trayIcon_->setIcon( hasOverdueTasks ? QIcon(":/images/NOK.png") : QIcon(":/images/OK.png") );
}

void MainWindow::initStatusBar()
{
//    lblStatusBarTimestamp_ = new QLabel;
//    statusBar()->addPermanentWidget(lblStatusBarTimestamp_);

//    QTimer * timer = new QTimer;
//    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatusBarTime()));
//    timer->start(1000);
}

void MainWindow::updateStatusBarTime()
{
    lblStatusBarTimestamp_->setText(Clock::currentDateTime().toString("dd.MM.yyyy   hh:mm:ss"));
}

void MainWindow::showDateInStatusbar(const QDate & date)
{
    if (date.isNull()) statusBar()->clearMessage();
    else               statusBar()->showMessage(QLocale().toString(date, "dd.MM.yyyy, dddd"));
}

void MainWindow::globalShortcutTriggered()
{
    quickAddDlg_->showDlg();
}

void MainWindow::showTrayMessage(const QString & title, const QString & msg)
{
    trayIcon_->showMessage(title, msg, QSystemTrayIcon::NoIcon);
}

void MainWindow::dumpLog()
{
    Log::dumpLog();
}
