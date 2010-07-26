#pragma once

#include <QDialog>

namespace Ui { class QuickAddDialog; }

class QuickAddDialog : public QDialog
{
    Q_OBJECT

public:
    QuickAddDialog(QWidget *parent = 0);
    ~QuickAddDialog();

    void showDlg();

signals:
    void showMessage(const QString & title, const QString & msg);

protected:
    virtual void showEvent(QShowEvent * event);

private slots:
    void addNewTask();

private:
    Ui::QuickAddDialog *ui;
};
