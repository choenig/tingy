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
    ui->leAddTask->setLeftIcon(QPixmap(":/images/add.png"));
    ui->leAddTask->setRightIcon(QPixmap(":/images/clear.png"));
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
        emit showMessage("Added new Task", task.getTitle());
        TaskModel::instance()->addTask(task);
        accept();
    } else {
        reject();
    }
}
