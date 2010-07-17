#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

class QLabel;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * parent = 0);
    ~MainWindow();

private:
    void initActions();
    void initSystemTray();

private slots:
    void on_leAddTask_returnPressed();
    void toggleHideDoneTasks();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void updateStatusBar();

protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    QAction * hideDoneAction_;
    QLabel * lblStatusBar_;

private:
    Ui::MainWindow *ui;
};
