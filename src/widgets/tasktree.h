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

#include <QTreeWidget>

class Task;
class TaskId;
class TaskTreeItem;
class TopLevelItem;

//
// TaskTree

class TaskTree : public QTreeWidget
{
	Q_OBJECT

public:
	TaskTree(QWidget * parent = 0);

public slots:
	void showDoneTasks(bool hide);
	void highlightDate(const QDate & date);
	void filterItems(const QString & filter);

private slots:
	void init();
	void addTask(const Task & task);
	void updateTask(const Task & task, bool doneChanged);
	void removeTask(const TaskId & taskId);

	void slotItemDoubleClicked(QTreeWidgetItem * item, int column);
	void slotItemChanged(QTreeWidgetItem * item, int column);

	void handleDateChange();
	void updateClock();

protected:
	virtual void drawBranches(QPainter * painter, const QRect & rect, const QModelIndex & index) const;
	virtual void paintEvent(QPaintEvent *event);

	virtual void contextMenuEvent(QContextMenuEvent * e);

	// drag and drop
	virtual QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const;
	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);
	virtual void dragLeaveEvent(QDragLeaveEvent * event);
	virtual void dropEvent(QDropEvent *event);
	virtual Qt::DropActions supportedDropActions () const;
	virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);

private:
	TaskTreeItem * findTaskTreeItem(const TaskId & taskId) const;
	TopLevelItem * getTopLevelItemForTask(const Task & task) const;
	void reparentUpdatedItem(TaskTreeItem * updatedTaskTreeItem);
};
