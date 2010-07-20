#include "quickadddialog.h"

#include <core/taskmodel.h>
#include <widgets/autocompletelineedit.h>

#include "ui_quickadddialog.h"

QuickAddDialog::QuickAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickAddDialog)
{
    ui->setupUi(this);
    ui->leAddTask->setPlaceholderText("Add new Task");

//    setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->pbBox, SIGNAL(accepted()), this, SLOT(addNewTask()));
    connect(ui->pbBox, SIGNAL(rejected()), this, SLOT(close()));
}

QuickAddDialog::~QuickAddDialog()
{
    delete ui;
}

void QuickAddDialog::addNewTask()
{
    if (ui->leAddTask->text().isEmpty()) return;
    TaskModel::instance()->addTask(Task::createFromString(ui->leAddTask->text()));
    accept();
}
