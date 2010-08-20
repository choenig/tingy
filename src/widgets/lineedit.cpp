#include "lineedit.h"

#include <QMouseEvent>
#include <QPainter>

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void LineEdit::setLeftIcon(const QPixmap & icon)
{
	leftIcon_ = icon;
	QMargins margins = textMargins();
	margins.setLeft(icon.width() + 2);
	setTextMargins(margins);
}

void LineEdit::setRightIcon(const QPixmap & icon)
{
	rightIcon_ = icon;
	QMargins margins = textMargins();
	margins.setRight(icon.width() + 6);
	setTextMargins(margins);
}

void LineEdit::setInfoText(const QString & infoText)
{
    infoText_ = infoText;
    update();
}

void LineEdit::paintEvent(QPaintEvent * event)
{
    QLineEdit::paintEvent(event);

    QPainter p(this);

    // draw the default text if needed
    bool showDefaultText = text().isEmpty() && !infoText_.isEmpty();
    if (showDefaultText) {
        p.setPen(Qt::gray);
        const QMargins margins = textMargins();

        p.drawText(QRect(margins.left() + 8,
                         margins.top(),
                         width() - margins.left() - margins.right() - 8,
                         height() - margins.top() - margins.bottom()),
                   infoText_,
                   Qt::AlignLeft | Qt::AlignVCenter);
    }

	// draw left icon
	if (!leftIcon_.isNull()) {
		int offset = 1 + (height() - leftIcon_.height() ) / 2;
		p.drawPixmap(offset, offset, leftIcon_);
	}

	// draw right icon
	if (!showDefaultText && !rightIcon_.isNull()) {
		p.drawPixmap(width() - rightIcon_.width() - 7, 1 + (height() - rightIcon_.height()) / 2, rightIcon_);
	}
}

void LineEdit::mousePressEvent( QMouseEvent * e )
{
	if (!rightIcon_.isNull() && !text().isEmpty()) {
		if (e->pos().x() > (width() - rightIcon_.width() - 10)) {
			clear();
		}
	}

	QLineEdit::mousePressEvent(e);
}
