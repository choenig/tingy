#include "taskeditwidget.h"

#include <core/clock.h>

#include <QDebug>
#include <QLocale>
#include <QPainter>

#include "ui_taskeditwidget.h"

TaskEditWidget::TaskEditWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskEditWidget)
{
    ui->setupUi(this);

    // init the Priority combo box
    ui->cbPrio->addItem("Hoch",    Priority::High);
    ui->cbPrio->addItem("Normal",  Priority::Normal);
    ui->cbPrio->addItem("Niedrig", Priority::Low);

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

    // write the task into the GUI
    ui->lblCreated->setText(QLocale().toString(task.getCreationTimestamp(), "dddd, dd.MM.yyyy, hh:mm:ss"));
    ui->cbPrio->setCurrentIndex(ui->cbPrio->findData(task.getPriority().toInt()));
    ui->leDescription->setPlainText(task.getDescription());
    ui->leEffort->setText(task.getEffort().toString());
    ui->chkDue->setChecked(task.getDueDate().isValid());
    ui->calDue->setDate(task.getDueDate());
    ui->chkPlanned->setChecked(task.getPlannedDate().isValid());
    ui->calPlanned->setDate(task.getPlannedDate());
    ui->chkDone->setChecked(task.isDone());
    ui->lblDoneTimestamp->setVisible(task.isDone());
    ui->lblDoneTimestamp->setText(QLocale().toString(task.getDoneTimestamp(), "[dddd, dd.MM.yyyy, hh:mm:ss]"));

    if (QDialog::exec() != QDialog::Accepted) {
        return Task();
    }

    // load task from GUI
    Task retval = task;
    retval.setPriority((Priority::Level)ui->cbPrio->itemData(ui->cbPrio->currentIndex()).toInt());
    retval.setDescription(ui->leDescription->toPlainText());
    retval.setEffort(Effort::fromString(ui->leEffort->text()));
    retval.setDueDate(ui->chkDue->isChecked() ? ui->calDue->date() : QDate());
    retval.setPlannedDate(ui->chkPlanned->isChecked() ? ui->calPlanned->date() : QDate());
    retval.setDone(ui->chkDone->isChecked() ? Clock::currentDateTime() : QDateTime());
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
