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
#include "taskeditwidget.h"

#include <core/clock.h>
#include <util/util.h>

#include <QCalendarWidget>
#include <QDebug>
#include <QLocale>
#include <QPainter>

#include "ui_taskeditwidget.h"

TaskEditWidget::TaskEditWidget(QWidget *parent)
    : QDialog(parent), ui(new Ui::TaskEditWidget)
{
    ui->setupUi(this);

    // init the Priority combo box
    ui->cbPrio->addItem(QPixmap(":/images/highPriority.png"),   "Hoch",    Priority::High);
    ui->cbPrio->addItem(QPixmap(":/images/normalPriority.png"), "Normal",  Priority::Normal);
    ui->cbPrio->addItem(QPixmap(":/images/lowPriority.png"),    "Niedrig", Priority::Low);

    ui->calDue->setDisplayFormat("dd.MM.yyyy");
    ui->calPlanned->setDisplayFormat("dd.MM.yyyy");
    ui->calDue->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    ui->calPlanned->calendarWidget()->setFirstDayOfWeek(Qt::Monday);

    // connect main signals
    connect(ui->btnGroup, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->btnGroup, SIGNAL(rejected()), this, SLOT(reject()));
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

void TaskEditWidget::paintEvent(QPaintEvent * paintEvent)
{
    // draw the 'blue' background
    QPainter p(this);
    p.fillRect(QRect(ui->lblSubject->geometry().topLeft(),
                     QPoint(ui->lblDone->geometry().right() + 4, ui->btnGroup->geometry().bottom())),
               QColor(ui->lblSubject->palette().color(QPalette::Window)));

    QDialog::paintEvent(paintEvent);
}
