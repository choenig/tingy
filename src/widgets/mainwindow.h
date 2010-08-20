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
    void updateStatusBarTime();
    void globalShortcutTriggered();
    void showTrayMessage(const QString & title, const QString & msg);
    void showDateInStatusbar(const QDate & date);
    void dumpLog();

private:
    void initActions();
    void initSystemTray();
    void initStatusBar();

private:
    QuickAddDialog * quickAddDlg_;
    QAction * showDoneTasksAction_;
    QLabel  * lblStatusBarTimestamp_;
    QSystemTrayIcon * trayIcon_;
    QPoint lastPos;

private:
    Ui::MainWindow * ui;
};
