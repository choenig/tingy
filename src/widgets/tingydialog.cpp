#include <tingydialog.h>

#include <QDialogButtonBox>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDebug>

TingyDialog::TingyDialog(QWidget * parent, const QString & title)
    : QDialog(parent)
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    // subject
    {
        lblSubject_ = new QLabel(this);
        lblSubject_->setMinimumSize(QSize(0, 30));
        lblSubject_->setText(title);

        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(184, 210, 76, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        lblSubject_->setPalette(palette);

        QFont font;
        font.setBold(true);
        font.setWeight(75);
        lblSubject_->setFont(font);
        lblSubject_->setAutoFillBackground(true);
        lblSubject_->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lblSubject_);
    }

    // central widget
    {
        centralWidget_ = new QWidget(this);
        verticalLayout->addWidget(centralWidget_);
    }

    // button group
    {
        btnGroup_ = new QDialogButtonBox(this);
        btnGroup_->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(btnGroup_);

        // connect main signals
        connect(btnGroup_, SIGNAL(accepted()), this, SLOT(accept()));
        connect(btnGroup_, SIGNAL(rejected()), this, SLOT(reject()));
    }

}

QWidget * TingyDialog::centralWidget()
{
    return centralWidget_;
}


void TingyDialog::paintEvent(QPaintEvent * paintEvent)
{
    QFormLayout * formLayout = dynamic_cast<QFormLayout*>(centralWidget_->layout());
    if (formLayout && formLayout->rowCount() > 0)
    {
        QLayoutItem * layoutItem = formLayout->itemAt(0, QFormLayout::LabelRole);
        QWidget * lbl = layoutItem->widget();
        if (lbl) {
            // draw the 'blue' background
            QPainter p(this);
            p.fillRect(QRect(lblSubject_->geometry().topLeft(),
                             QPoint( lbl->mapTo(this, lbl->rect().bottomRight()).x() + formLayout->horizontalSpacing()/2,
                                    btnGroup_->geometry().bottom())),
                       QColor(lblSubject_->palette().color(QPalette::Window)));
        }
    }


   QDialog::paintEvent(paintEvent);
}



