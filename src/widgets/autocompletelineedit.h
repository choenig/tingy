#pragma once

#include <QLineEdit>

class CalendarPopup;
class QDate;
class TextPopup;

class AutocompleteLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    AutocompleteLineEdit(QWidget * parent = 0);

    void setInfoText(const QString & infoText);
    QString infoText() const { return infoText_; }

protected:
    void paintEvent(QPaintEvent * event);

private slots:
    void onTextChanged(const QString & text);
    void doneTextCompletion(const QString & text);
    void doneCalCompletion(const QDate & date);

private:
    TextPopup * textPopup_;
    CalendarPopup * calPopup_;

    QString infoText_;
};
