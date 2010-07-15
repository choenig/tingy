#include "autocompletelineedit.h"

#include <QTreeWidget>
#include <QHeaderView>
#include <QKeyEvent>

QTreeWidget * popup;

AutocompleteLineEdit::AutocompleteLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));

    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setColumnCount(1);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->header()->hide();

    popup->installEventFilter(this);

    connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(doneCompletion()));
}


void AutocompleteLineEdit::onTextChanged(const QString & text)
{
    QStringList choices;
    if (text.endsWith("!")) {
        choices << "+" << "-";
    } else if (text.endsWith("*")) {
        choices << "today" << "tomorrow" << "+2d";
    }

    if (choices.isEmpty()) return;

    popup->setUpdatesEnabled(false);
    popup->clear();
    for (int i = 0; i < choices.count(); ++i) {
        QTreeWidgetItem * item = new QTreeWidgetItem(popup);
        item->setText(0, choices[i] + " ");
    }
    popup->setCurrentItem(popup->topLevelItem(0));
    popup->resizeColumnToContents(0);
    popup->adjustSize();
    popup->setUpdatesEnabled(true);

    int h = popup->sizeHintForRow(0) * qMin(7, choices.count()) + 3;
    popup->resize(popup->width(), h);

    QFontMetrics fm(popup->font());
    int x = fm.width(this->text()) + 7;

    popup->move(this->mapToGlobal(QPoint(x, this->height() - 4)));
    popup->setFocus();
    popup->show();
}

bool AutocompleteLineEdit::eventFilter(QObject *object, QEvent *event)
 {
     if (object != popup) return false;

     if (event->type() == QEvent::MouseButtonPress) {
         popup->hide();
         this->setFocus();
         return true;
     }

     if (event->type() == QEvent::KeyPress)
     {
         bool consumed = false;
         int key = static_cast<QKeyEvent*>(event)->key();
         switch (key) {
         case Qt::Key_Enter:
         case Qt::Key_Return: {
             doneCompletion();
             consumed = true;
         }
         case Qt::Key_Escape:
             this->setFocus();
             popup->hide();
             consumed = true;

         case Qt::Key_Up:
         case Qt::Key_Down:
         case Qt::Key_Home:
         case Qt::Key_End:
         case Qt::Key_PageUp:
         case Qt::Key_PageDown:
             break;

         default:
             this->setFocus();
             this->event(event);
             popup->hide();
             break;
         }

         return consumed;
     }

     return false;
 }

void AutocompleteLineEdit::doneCompletion()
 {
     popup->hide();
     setFocus();
     QTreeWidgetItem *item = popup->currentItem();
     if (item) {
         setText(text() + item->text(0));
//         QMetaObject::invokeMethod(editor, "returnPressed");
     }
 }

