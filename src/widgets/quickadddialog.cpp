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
    ui->leAddTask->setInfoText("Neuen Task hinzufügen");
    connect(ui->leAddTask, SIGNAL(returnPressed()), this, SLOT(addNewTask()));

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

QuickAddDialog::~QuickAddDialog()
{
    delete ui;
}

void QuickAddDialog::showDlg()
{
    QDesktopWidget wdg;
    const QRect screenrect = wdg.screenGeometry(QCursor::pos());

    const QPoint startPos = QPoint(screenrect.x() + (screenrect.width() - width()) / 2,
                                   screenrect.y());

    ui->leAddTask->clear();
    move(startPos);
    show();

//    QPropertyAnimation anim(this, "pos");
//    anim.setStartValue(startPos);
//    anim.setEndValue(startPos + QPoint(0, height()));

//    QEventLoop loop;
//    connect(&anim, SIGNAL(finished()), &loop, SLOT(quit()));

//    anim.start();
//    loop.exec();

//    exec();

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
        emit showMessage("Added new Task", task.getDescription());
        TaskModel::instance()->addTask(task);
        accept();
    } else {
        reject();
    }
}
