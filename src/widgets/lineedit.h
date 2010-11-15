/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
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
