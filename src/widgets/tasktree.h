#pragma once

#include <QTreeWidget>

class Task;

////
//// TaskTree

class TopLevelItem;

class TaskTree : public QTreeWidget
{
	Q_OBJECT

public:
	TaskTree(QWidget * parent = 0);

private slots:
	void addTask(const Task & task);
	void updateTask(const Task & task);
	void slotItemDoubleClicked(QTreeWidgetItem * item, int column);

protected:
	virtual void drawBranches(QPainter * painter, const QRect & rect, const QModelIndex & index) const;

	virtual void contextMenuEvent(QContextMenuEvent * e);

	// drag and drop
	virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
	virtual QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const;
	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);
	virtual Qt::DropActions supportedDropActions () const;
};
