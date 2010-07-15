#include "taskeditwidget.h"
#include "ui_taskeditwidget.h"

TaskEditWidget::TaskEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskEditWidget)
{
    ui->setupUi(this);
}

TaskEditWidget::~TaskEditWidget()
{
    delete ui;
}
