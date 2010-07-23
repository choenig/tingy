#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

class QLabel;
class QDate;

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

private slots:
    void on_leAddTask_returnPressed();
    void toggleHideDoneTasks();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void updateTrayIcon(bool ok);
    void updateStatusBarTime();
    void globalShortcutTriggered();
    void showTrayMessage(const QString & title, const QString & msg);
    void showDateInStatusbar(const QDate & date);

private:
    void initActions();
    void initSystemTray();
    void initStatusBar();

private:
    QAction * hideDoneAction_;
    QLabel  * lblStatusBarTimestamp_;
    QSystemTrayIcon * trayIcon_;
    QPoint lastPos;

private:
    Ui::MainWindow * ui;
};
