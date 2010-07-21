#include "quickadddialog.h"

#include <core/taskmodel.h>
#include <widgets/autocompletelineedit.h>

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QEventLoop>

#include "ui_quickadddialog.h"

QuickAddDialog::QuickAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickAddDialog)
{
    ui->setupUi(this);
    ui->leAddTask->setPlaceholderText("Add new Task");
    connect(ui->leAddTask, SIGNAL(returnPressed()), this, SLOT(addNewTask()));

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

QuickAddDialog::~QuickAddDialog()
{
    delete ui;
}

void QuickAddDialog::execDlg()
{
    QDesktopWidget wdg;
    const QRect r = wdg.screenGeometry(QCursor::pos());

    const QPoint startPos = QPoint(r.x() + (r.width() - width()) / 2, r.y() - height());

    move(startPos);
    show();

//    QPropertyAnimation anim(this, "pos");
//    anim.setStartValue(startPos);
//    anim.setEndValue(startPos + QPoint(0, height()));

//    QEventLoop loop;
//    connect(&anim, SIGNAL(finished()), &loop, SLOT(quit()));

//    anim.start();
//    loop.exec();

    exec();

//    move(startPos);
}

void QuickAddDialog::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
}

void QuickAddDialog::addNewTask()
{
    if (ui->leAddTask->text().isEmpty()) return;
    Task task = Task::createFromString(ui->leAddTask->text());
    if (task.isValid()) {
        TaskModel::instance()->addTask(task);
        emit showMessage("Added new Task", task.getDescription());
        accept();
    } else {
        reject();
    }
}
