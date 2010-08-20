#pragma once

#include <widgets/lineedit.h>

class CalendarPopup;
class QDate;
class TextPopup;

class AutocompleteLineEdit : public LineEdit
{
    Q_OBJECT
public:
    AutocompleteLineEdit(QWidget * parent = 0);

private slots:
    void onTextChanged(const QString & text);
    void doneTextCompletion(const QString & text);
    void doneCalCompletion(const QDate & date);

private:
    TextPopup * textPopup_;
    CalendarPopup * calPopup_;
};
