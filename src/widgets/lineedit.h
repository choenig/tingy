#pragma once

#include <QLineEdit>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    LineEdit(QWidget *parent = 0);

	void setLeftIcon(const QPixmap & icon);
	void setRightIcon(const QPixmap & icon);

    void setInfoText(const QString & infoText);
    QString infoText() const { return infoText_; }

protected:
    void paintEvent(QPaintEvent * event);
    void mousePressEvent( QMouseEvent * e );

private:
	QPixmap leftIcon_;
	QPixmap rightIcon_;
    QString infoText_;
};
