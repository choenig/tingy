#include "taskeditwidget.h"

#include <core/clock.h>

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
    ui->cbPrio->addItem("Hoch",    Priority::High);
    ui->cbPrio->addItem("Normal",  Priority::Normal);
    ui->cbPrio->addItem("Niedrig", Priority::Low);

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

    const QString timeformat = QString("dddd %1 dd.MM.yyyy %1 hh:mm:ss").arg(QString::fromUtf8("\xe2\x80\xa2"));
    // write the task into the GUI
    ui->lblCreated->setText(QLocale().toString(task.getCreationTimestamp(), timeformat));
    ui->cbPrio->setCurrentIndex(ui->cbPrio->findData(task.getPriority().toInt()));
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
               QColor("#C1C1FF"));

    QDialog::paintEvent(paintEvent);
}
