#pragma once

#include <QRegExp>
#include <QTreeWidget>

class TextPopup : public QTreeWidget
{
    Q_OBJECT
public:
    TextPopup(QWidget *parent = 0);

    void setChoices(const QStringList & choices, const QRegExp & re = QRegExp());

signals:
    void textSelected(const QString & text);

protected:
    virtual void keyPressEvent(QKeyEvent * event);

private slots:
    void doneTextCompletion();

private:
    QRegExp reValidationMask_;
};

