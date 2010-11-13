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

#include <core/task.h>

#include <QTreeWidgetItem>
#include <QItemDelegate>
#include <QDate>

class TaskTree;

//
// TopLevelItem

class TopLevelItem : public QTreeWidgetItem
{
public:
	enum { Type = QTreeWidgetItem::UserType + 23 };

public:
	TopLevelItem(QTreeWidget * treeWidget);

	void invokeContextMenu(const QPoint & pos);

	void update(const QDate & date, const QString & string);

	QString getString() const { return string_; }
	QDate getDate() const { return date_; }

private:
	void init();
	virtual bool operator<(const QTreeWidgetItem & rhs) const;

private:
	QString string_;
	QDate date_;
};

//
// TaskTreeItem

class TaskTreeItem : public QTreeWidgetItem
{
public:
	enum { Type = QTreeWidgetItem::UserType + 24 };

public:
	TaskTreeItem(TaskTree * tree, const Task & task);
	TaskTreeItem(TopLevelItem * topLevelItem, const Task & task);

	Task getTask() const { return task_; }
	void setTask(const Task & task);

	void invokeContextMenu(const QPoint & pos);

	void update();

	void highlightDate(const QDate & date);
	void filterYourself(const QString &filterText);

	QList<QPoint> getFilterHits() const { return filterHits; }

private:
	virtual bool operator<(const QTreeWidgetItem & other) const;

private:
	Task task_;
	QList<QPoint> filterHits;
};

//
// TopLevelItemDelegate

class TopLevelItemDelegate : public QItemDelegate
{
public:
	TopLevelItemDelegate(QTreeWidget * parent = 0) : QItemDelegate(parent) {}
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

//
// TaskTreeItemDelegate

class TaskTreeItemDelegate : public QItemDelegate
{
public:
	TaskTreeItemDelegate(QTreeWidget * parent = 0) : QItemDelegate(parent) {}
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

protected:
	void drawDisplay(QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text) const;

private:
	mutable QList<QPoint> filters;
};
