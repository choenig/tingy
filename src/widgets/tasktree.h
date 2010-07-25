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
	void hideDoneTasks(bool hide);
	void highlightDate(const QDate & date);

private slots:
	void init();
	void addTask(const Task & task);
	void updateTask(const Task & task);
	void removeTask(const TaskId & taskId);

	void slotItemDoubleClicked(QTreeWidgetItem * item, int column);
	void slotItemChanged(QTreeWidgetItem * item, int column);

	void handleDayChange();
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
