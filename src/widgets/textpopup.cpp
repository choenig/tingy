#include "textpopup.h"

#include <QLineEdit>
#include <QHeaderView>
#include <QKeyEvent>
#include <QDebug>

int initialCursorPos;

TextPopup::TextPopup(QWidget *parent) :
    QTreeWidget(parent)
{
    setWindowFlags(Qt::Popup);
    setFocusPolicy(Qt::NoFocus);
    setFocusProxy(parent);
    setMouseTracking(true);
    setColumnCount(1);
    setUniformRowHeights(true);
    setRootIsDecorated(false);
    setEditTriggers(QTreeWidget::NoEditTriggers);
    setSelectionBehavior(QTreeWidget::SelectRows);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    header()->hide();

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(doneTextCompletion()));
}

void TextPopup::setChoices(const QStringList & choices, const QRegExp & re)
{
    reValidationMask_ = re;
    if (reValidationMask_.isValid()) {
        initialCursorPos = static_cast<QLineEdit*>(parentWidget())->cursorPosition();
    }

    setUpdatesEnabled(false);
    clear();
    for (int i = 0; i < choices.count(); ++i) {
        QTreeWidgetItem * item = new QTreeWidgetItem(this);
        item->setText(0, choices[i]);
    }
    setCurrentItem(topLevelItem(0));
    resizeColumnToContents(0);
    adjustSize();
    setUpdatesEnabled(true);

    int h = sizeHintForRow(0) * qMin(7, choices.count()) + 3;
    resize(width(), h);
}

void TextPopup::keyPressEvent(QKeyEvent * event)
 {
     bool consumed = false;

     int key = static_cast<QKeyEvent*>(event)->key();
     switch (key) {
     case Qt::Key_Enter:
     case Qt::Key_Return:
         doneTextCompletion();

     case Qt::Key_Escape:
         hide();
         consumed = true;

     case Qt::Key_Shift:
     case Qt::Key_Control:
     case Qt::Key_Alt:
     case Qt::Key_Meta:
     case Qt::Key_Mode_switch:
         // modifiers should not close the popup
         break;

     case Qt::Key_Up:
     case Qt::Key_Down:
     case Qt::Key_Home:
     case Qt::Key_End:
     case Qt::Key_PageUp:
     case Qt::Key_PageDown:
         break;

     default: {
         parent()->event(event);

         QLineEdit * ple = static_cast<QLineEdit*>(parentWidget());
         QString t = ple->text().mid(initialCursorPos, ple->cursorPosition());
         if (!reValidationMask_.exactMatch(t))
            hide();
         break;
     }
     }

     if (!consumed) {
         QTreeWidget::keyPressEvent(event);
     }
}

void TextPopup::doneTextCompletion()
 {
     hide();
     QTreeWidgetItem *item = currentItem();
     if (item) emit textSelected(item->text(0));
 }
