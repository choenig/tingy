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
	TopLevelItem(QTreeWidget * treeWidget, QTreeWidgetItem * itmBefore, const QString & string, const QDate & date);
	TopLevelItem(QTreeWidget * treeWidget, const QString & string, const QDate & date);

	QString getString() const { return string_; }
	QDate getDate() const { return date_; }

private:
	void init(const QString& changeList);
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
	void setTask(const Task & task) { task_ = task; init(); }

private:
	void init();
	virtual bool operator<(const QTreeWidgetItem & rhs) const;

private:
	Task task_;
};

class TopLevelItemDelegate : public QItemDelegate
{
public:
	TopLevelItemDelegate(QTreeWidget * parent = 0);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	QTreeWidget* parent_;
};
