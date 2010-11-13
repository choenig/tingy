/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of Tingy.
**
** Tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with Tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#pragma once

#include <core/taskid.h>

#include <QDate>
#include <QMap>
#include <QSet>
#include <QWidget>

class Task;

class DateBeam : public QWidget
{
    Q_OBJECT
public:
    DateBeam(QWidget *parent = 0);

public:
    virtual QSize sizeHint() const;

signals:
    void dateHovered(const QDate & date);

protected:
    virtual void enterEvent(QEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void paintEvent(QPaintEvent * event);

    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void dropEvent(QDropEvent *);

private slots:
    void addTask(const Task & task);
    void updateTask(const Task & task, bool doneChanged);
    void removeTask(const TaskId & taskId);

private:
    int cellWidth_;
    int hoveredCell_;
    bool mouseIn_;
    QMap<QDate, QSet<TaskId> > datesWithTasks_;
};
