#pragma once

#include <QLineEdit>

class AutocompleteLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit AutocompleteLineEdit(QWidget *parent = 0);

private slots:
    void onTextChanged(const QString & text);
    void doneCompletion();

private:
    bool eventFilter(QObject *obj, QEvent *ev);
};
