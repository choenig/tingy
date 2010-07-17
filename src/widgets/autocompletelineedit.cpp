#include "autocompletelineedit.h"

#include <widgets/calendarpopup.h>
#include <widgets/textpopup.h>

namespace {
TextPopup * textPopup;
CalendarPopup * calPopup;
}

AutocompleteLineEdit::AutocompleteLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    textPopup = new TextPopup(this);
    connect(textPopup, SIGNAL(textSelected(QString)), this, SLOT(doneTextCompletion(QString)));

    calPopup = new CalendarPopup(this);
    connect(calPopup, SIGNAL(dateSelected(QDate)), this, SLOT(doneCalCompletion(QDate)));

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
}

void AutocompleteLineEdit::onTextChanged(const QString & text)
{
    QWidget * wdgt = 0;

    if (text.endsWith("!")) {
        QStringList choices; choices << "+" << "-";
        textPopup->setChoices(choices, QRegExp("[+-]"));
        wdgt = textPopup;
    } else if (text.endsWith("$")) {
        QStringList choices; choices << "m" << "h"<< "d"<< "w"<< "y";
        textPopup->setChoices(choices, QRegExp("[0-9]+([mhdwy]?[0-9]*)*"));
        wdgt = textPopup;
    } else if (text.endsWith("*")) {
        calPopup->reset();
        wdgt = calPopup;
    }

    if (!wdgt) return;

    QFontMetrics fm(textPopup->font());
    int cursorPosPx = fm.width(this->text()) + 7;
    wdgt->move(this->mapToGlobal(QPoint(cursorPosPx, this->height() - 4)));
    wdgt->setFocus();
    wdgt->show();
}

void AutocompleteLineEdit::doneTextCompletion(const QString & txt)
{
    // do not append the selected text, if its already there
    // dunno if this is a good solution
    if (text().endsWith(txt)) {
        setText(text() + " ");
    } else {
        setText(text() + txt + " ");
    }
}

void AutocompleteLineEdit::doneCalCompletion(const QDate &date)
{
    setText(text() + date.toString("dd.MM.yyyy") + " ");
}

