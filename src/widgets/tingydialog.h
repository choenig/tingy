#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>

class QPaintEvent;

class TingyDialog : public QDialog
{
public:
    TingyDialog(QWidget * parent, const QString & title);
    virtual ~TingyDialog() {}

protected:
    virtual void paintEvent(QPaintEvent * paintEvent);

protected:
    QWidget * centralWidget();

private:
    QLabel * lblSubject_;
    QWidget * centralWidget_;
    QDialogButtonBox * btnGroup_;
};
