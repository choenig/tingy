#pragma once

#include <QLineEdit>

class QDate;

class AutocompleteLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    AutocompleteLineEdit(QWidget *parent = 0);

private slots:
    void onTextChanged(const QString & text);

    void doneTextCompletion(const QString & text);
    void doneCalCompletion(const QDate & date);
};