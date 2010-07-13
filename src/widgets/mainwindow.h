#pragma once

#include <QMainWindow>

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget * parent = 0);
    ~MainWindow();

private slots:
    void on_leAddTask_returnPressed();
    void initActions();
    void toggleHideDoneTasks();

private:
    QAction * hideDoneAction_;

private:
    Ui::MainWindow *ui;
};
