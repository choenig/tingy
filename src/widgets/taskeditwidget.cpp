/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#include "taskeditwidget.h"

#include <core/clock.h>
#include <util/util.h>

#include <QCalendarWidget>
#include <QDebug>
#include <QLocale>
#include <QPainter>

#include "ui_taskeditwidget.h"

TaskEditWidget::TaskEditWidget(QWidget *parent)
    : TingyDialog(parent, "Task bearbeiten"), ui(new Ui::TaskEditWidget)
{
    ui->setupUi(centralWidget());

    resize(600,350);

    // init the Priority combo box
    ui->cbPrio->addItem(QPixmap(":/images/highPriority.png"),   "Hoch",    Priority::High);
    ui->cbPrio->addItem(QPixmap(":/images/normalPriority.png"), "Normal",  Priority::Normal);
    ui->cbPrio->addItem(QPixmap(":/images/lowPriority.png"),    "Niedrig", Priority::Low);

    ui->calDue->setDisplayFormat("dd.MM.yyyy");
    ui->calPlanned->setDisplayFormat("dd.MM.yyyy");
    ui->calDue->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    ui->calPlanned->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
}

TaskEditWidget::~TaskEditWidget()
{
    delete ui;
}

Task TaskEditWidget::exec(const Task & task)
{
    // init dates
    ui->calDue->setDate(Clock::currentDate());
    ui->calPlanned->setDate(Clock::currentDate());

    const QString timeformat = dot("dddd * dd.MM.yyyy * hh:mm:ss");
    // write the task into the GUI
    ui->lblCreated->setText(QLocale().toString(task.getCreationTimestamp(), timeformat));
    ui->cbPrio->setCurrentIndex(ui->cbPrio->findData(task.getPriority().toInt()));
    ui->leTitle->setText(task.getTitle());
    ui->leDescription->setPlainText(task.getDescription());
    ui->leEffort->setText(task.getEffort().toString());
    ui->chkDue->setChecked(task.getDueDate().isValid());
    ui->calDue->setDate(task.getDueDate());
    ui->chkPlanned->setChecked(task.getPlannedDate().isValid());
    ui->calPlanned->setDate(task.getPlannedDate());
    ui->chkDone->setChecked(task.isDone());
    ui->lblDoneTimestamp->setText(QLocale().toString(task.getDoneTimestamp(), "[" + timeformat + "]"));
    ui->lblDoneTimestamp->setVisible(task.isDone());

    if (QDialog::exec() != QDialog::Accepted) {
        return Task();
    }

    // load task from GUI
    Task retval = task;
    retval.setPriority((Priority::Level)ui->cbPrio->itemData(ui->cbPrio->currentIndex()).toInt());
    retval.setTitle(ui->leTitle->text().trimmed());
    retval.setDescription(ui->leDescription->toPlainText().trimmed());
    retval.setEffort(Effort::fromString(ui->leEffort->text()));
    retval.setDueDate(ui->chkDue->isChecked() ? ui->calDue->date() : QDate());
    retval.setPlannedDate(ui->chkPlanned->isChecked() ? ui->calPlanned->date() : QDate());
    retval.setDone(task.isDone() ? task.getDoneTimestamp() : ui->chkDone->isChecked() ? Clock::currentDateTime() : QDateTime());
    return retval;
}
