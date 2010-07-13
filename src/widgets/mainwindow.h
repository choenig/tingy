#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

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

protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    QAction * hideDoneAction_;

private:
    Ui::MainWindow *ui;
};
