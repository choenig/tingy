#ifndef TASKEDITWIDGET_H
#define TASKEDITWIDGET_H

#include <QWidget>

namespace Ui {
    class TaskEditWidget;
}

class TaskEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskEditWidget(QWidget *parent = 0);
    ~TaskEditWidget();

private:
    Ui::TaskEditWidget *ui;
};

#endif // TASKEDITWIDGET_H
