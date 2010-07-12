#include "filterlineedit.h"

//
// FilterLineEdit

FilterLineEdit::FilterLineEdit(QWidget * parent)
    : QLineEdit(parent)
{
}

QString FilterLineEdit::infoText() const
{
    return infoText_;
}

void FilterLineEdit::setInfoText(const QString infoText)
{
    infoText_ = infoText;

    if (text().isEmpty()) updateInfoText(true);
}

void FilterLineEdit::focusInEvent(QFocusEvent * event)
{
    QLineEdit::focusInEvent(event);
    if (text() == infoText_) updateInfoText(false);
}

void FilterLineEdit::focusOutEvent(QFocusEvent * event)
{
    QLineEdit::focusOutEvent(event);
    if (text().isEmpty()) updateInfoText(true);
}

void FilterLineEdit::updateInfoText(bool show)
{
    QPalette p = palette();
    if (show) {
        p.setColor(QPalette::Text, Qt::gray);
        setPalette(p);
        setText(infoText_);
    } else {
        p.setColor(QPalette::Text, Qt::black);
        setPalette(p);
        clear();
    }
}
