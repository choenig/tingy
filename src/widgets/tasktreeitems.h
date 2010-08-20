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

	void update(const QDate & date, const QString & string) {
		date_ = date;
		string_ = string;
		init();
	}

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
