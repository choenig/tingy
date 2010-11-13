/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of Tingy.
**
** Tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with Tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

class QDate;
class QLabel;
class QuickAddDialog;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent * event);
    virtual void hideEvent(QHideEvent * event);
    virtual void showEvent(QShowEvent * event);
    virtual bool event(QEvent *event);

private slots:
    void on_leAddTask_returnPressed();
    void on_pbInfo_clicked();
    void toggleShowDoneTasks();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void updateTrayIcon(bool ok);
    void globalShortcutTriggered();
    void showTrayMessage(const QString & title, const QString & msg);
    void showDateInStatusbar(const QDate & date);
    void dumpLog();

private:
    void initActions();
    void initSystemTray();

private:
    QuickAddDialog * quickAddDlg_;
    QAction * showDoneTasksAction_;
    QLabel  * lblStatusBarTimestamp_;
    QSystemTrayIcon * trayIcon_;
    QPoint lastWindowPos;

private:
    Ui::MainWindow * ui;
};
