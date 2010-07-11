#pragma once

#include <QLineEdit>

class FilterLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString infoText READ infoText WRITE setInfoText)

public:
    FilterLineEdit(QWidget * parent = 0);

    QString infoText() const;
    void setInfoText(const QString infoText);

protected:
    virtual void focusInEvent(QFocusEvent * event);
    virtual void focusOutEvent(QFocusEvent * event);

private:
    void updateInfoText(bool show);

private:
    QString infoText_;
};
