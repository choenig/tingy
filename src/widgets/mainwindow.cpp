/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#include "mainwindow.h"

#include <core/clock.h>
#include <core/settings.h>
#include <core/taskmodel.h>
#include <qtextlib/qxtglobalshortcut.h>
#include <util/log.h>
#include <util/util.h>
#include <widgets/aboutdialog.h>
#include <widgets/infodialog.h>
#include <widgets/quickadddialog.h>

#include <QCloseEvent>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QEvent>
#include <QShortcut>
#include <QPushButton>

#include "ui_mainwindow.h"

bool getGridPos(const QGridLayout * layout, const QWidget * w, int & x, int & y)
{
    for (int r = 0 ; r < layout->rowCount() ; ++r) {
        for (int c = 0 ; c < layout->columnCount() ; ++c) {
            if (layout->itemAtPosition(r, c)->widget() == w) {
                x = r;
                y = c;
                return true;
            }
        }
    }

    return false;
}

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent), quickAddDlg_(0), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->dateBeam, SIGNAL(dateHovered(QDate)), this, SLOT(showDateInStatusbar(QDate)));
    connect(ui->dateBeam, SIGNAL(dateHovered(QDate)), ui->taskTree, SLOT(highlightDate(QDate)));

    ui->leAddTask->setInfoText("Neuen Task hinzufügen");
    ui->leAddTask->setLeftIcon(QPixmap(":/images/add.png"));
    ui->leAddTask->setRightIcon(QPixmap(":/images/clear.png"));

    {
        const QPixmap infoIcon(":/images/info.png");

        QGridLayout * centralLayout = (QGridLayout*)centralWidget()->layout();

        int r,c;
        getGridPos(centralLayout, ui->dummyWidget, r, c);
        delete ui->dummyWidget;

        QToolBar * bar = new QToolBar();
        bar->setIconSize(infoIcon.size());
        bar->addAction(infoIcon, "Info über tingy", this, SLOT(showInfoDialog()));

        centralLayout->addWidget(bar, r, c, 1, 1);
    }

    // init stuff
    initActions();
    initSystemTray();

    // init quickAddDlg
    quickAddDlg_ = new QuickAddDialog();
    connect(quickAddDlg_, SIGNAL(showMessage(QString,QString)), this, SLOT(showTrayMessage(QString,QString)));

    // init global shortcut
    QxtGlobalShortcut * globalShortcut = new QxtGlobalShortcut(QKeySequence(Settings::QuickTingyShortcut()), this);
    globalShortcut->setEnabled(true);
    connect(globalShortcut, SIGNAL(activated()), this, SLOT(globalShortcutTriggered()));

    // dump log on Ctrl+Shift+D
    new QShortcut(QKeySequence("Ctrl+Shift+D"), this, SLOT(dumpLog()));

    // hide window on Escape
    new QShortcut(QKeySequence("Esc"), this, SLOT(hide()));

    // initialy hide done tasks
    toggleShowDoneTasks();
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
    lastWindowPos_ = pos();
    QMainWindow::hideEvent(event);
}

void MainWindow::showEvent(QShowEvent * event)
{
    if (!lastWindowPos_.isNull()) move(lastWindowPos_);
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

void MainWindow::showInfoDialog()
{
    InfoDialog * dlg = new InfoDialog();
	connect(dlg, SIGNAL(finished(int)), dlg, SLOT(deleteLater()));
    dlg->show();
}

void MainWindow::initActions()
{
    ui->mainToolBar->setIconSize(QSize(40, 20));

    // showDoneTasks
    showDoneTasksAction_ = ui->mainToolBar->addAction(QIcon(":/images/done.png"), "Abgeschlossene Tasks einblenden",
                                                      this, SLOT(toggleShowDoneTasks()));
    showDoneTasksAction_->setCheckable(true);

    ui->mainToolBar->addSeparator();

    // about action
    ui->mainToolBar->addAction(QIcon(":/images/about.png"), "Info über tingy", this, SLOT(showAboutDialog()));

    // spacer used to 'squeeze' the filterLineEdit
    QWidget * spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
    ui->mainToolBar->addWidget(spacerWidget);

    // filterLineEdit
    LineEdit * filterLineEdit = new LineEdit(this);
    filterLineEdit->setInfoText("Filter");
    filterLineEdit->setLeftIcon(QPixmap(":/images/mag.png"));
    filterLineEdit->setRightIcon(QPixmap(":/images/clear.png"));
    connect(filterLineEdit, SIGNAL(textChanged(QString)), ui->taskTree, SLOT(filterItems(QString)));
    filterLineEdit->setMaximumWidth(250);
    ui->mainToolBar->addWidget(filterLineEdit);

    // spacerWidget
    QWidget * borderSpacerWidget = new QWidget();
    borderSpacerWidget->setMinimumWidth(2);
    ui->mainToolBar->addWidget(borderSpacerWidget);

    new QShortcut(QKeySequence("Ctrl+F"), filterLineEdit, SLOT(setFocus()));
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
    contextMenu->addAction(QIcon(":/images/exit.png"), "tingy beenden", qApp, SLOT(quit()));
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

void MainWindow::showDateInStatusbar(const QDate & date)
{
    if (date.isNull()) statusBar()->clearMessage();
    else               statusBar()->showMessage(QLocale().toString(date, dot("dddd * dd.MM.yyyy")));
}

void MainWindow::globalShortcutTriggered()
{
    if (quickAddDlg_->isVisible()) quickAddDlg_->hide();
    else                           quickAddDlg_->showDlg();
}

void MainWindow::showTrayMessage(const QString & title, const QString & msg)
{
    trayIcon_->showMessage(title, msg, QSystemTrayIcon::NoIcon);
}

void MainWindow::dumpLog()
{
    Log::dumpLog();
}

void MainWindow::showAboutDialog()
{
    AboutDialog dlg;
    dlg.exec();
}
