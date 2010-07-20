#pragma once

#include <QDialog>

namespace Ui { class QuickAddDialog; }

class QuickAddDialog : public QDialog
{
    Q_OBJECT

public:
    QuickAddDialog(QWidget *parent = 0);
    ~QuickAddDialog();

private slots:
    void addNewTask();

private:
    Ui::QuickAddDialog *ui;
};
