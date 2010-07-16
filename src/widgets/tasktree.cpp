#include "tasktree.h"

#include <core/task.h>
#include <core/taskmodel.h>
#include <widgets/tasktreeitems.h>

#include <QContextMenuEvent>
#include <QDebug>
#include <QHash>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include <QLocale>

namespace {

TopLevelItem * doneTopLevelItem;
QList<TopLevelItem*> topLevelItems;

void setTopLevelItemsHidden(bool hide, bool ignoreWithChildren = false)
{
    foreach (TopLevelItem * item , topLevelItems) {
        if (ignoreWithChildren && item->childCount() > 0) continue;
        item->setHidden(hide);
    }
}

void updateTopLevelItems(TaskTree * tree)
{
    const QDate today = QDate::currentDate();

    QMutableListIterator<TopLevelItem*> it(topLevelItems);
    it.next()->update(QDate(1970,1,1),   "Überfällig");
    it.next()->update(today,             QString("Heute, %1") .arg(QLocale().toString(today,            "dddd, dd.MM.yyyy")));
    it.next()->update(today.addDays(1),  QString("Morgen, %1").arg(QLocale().toString(today.addDays(1), "dddd, dd.MM.yyyy")));
    it.next()->update(today.addDays(2),  "Nächste Woche");
    it.next()->update(today.addDays(7),  "Nächster Monat");
    it.next()->update(today.addDays(30), "Zukunft ...");
}

void initTopLevelItems(TaskTree * tree)
{
    for (int i = 0 ; i < 6; ++i) {
        topLevelItems <<   new TopLevelItem(tree);
    }
    updateTopLevelItems(tree);

    doneTopLevelItem = new TopLevelItem(tree);
    doneTopLevelItem->update(QDate(), "Done");

    // initially hide all items
    setTopLevelItemsHidden(true);
}

}

//
// TaskTree

TaskTree::TaskTree(QWidget *parent)
	: QTreeWidget(parent)
{
	setRootIsDecorated(false);
	setIndentation(0);

	// used for top level items
	TopLevelItemDelegate * dlg = new TopLevelItemDelegate(this);
	for (int i = 0; i < 2 ; ++i) setItemDelegateForColumn(i, dlg);

	// drag and drop
	setAcceptDrops( true );
	setDragEnabled( true );

	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)));
    connect(tm, SIGNAL(taskRemoved(TaskId)), this, SLOT(removeTask(TaskId)));

    initTopLevelItems(this);

    QTimer * dayChangeTimer = new QTimer(this);
    dayChangeTimer->setInterval(QDateTime::currentDateTime().secsTo(QDateTime(QDate::currentDate().addDays(1), QTime(0,0))));
    connect(dayChangeTimer, SIGNAL(timeout()), this, SLOT(handleDayChange()));

    QTimer::singleShot(0, this, SLOT(init()));
}

void TaskTree::init()
{
    header()->setResizeMode(0, QHeaderView::ResizeToContents);
    header()->setResizeMode(1, QHeaderView::Stretch);
}

void TaskTree::hideDoneTasks(bool hide)
{
    doneTopLevelItem->setHidden(hide);
}

void TaskTree::addTask(const Task & task)
{
    if (task.isDone()) {
        new TaskTreeItem(doneTopLevelItem, task);
        sortItems(0, Qt::AscendingOrder);
        return;
    }

    TopLevelItem * parent = getTopLevelItemForTask(task);
    if (parent) {
        parent->setHidden(false);
        new TaskTreeItem(parent, task);
    } else {
        new TaskTreeItem(this, task);
    }
    sortItems(0, Qt::AscendingOrder);

}

void TaskTree::updateTask(const Task & task)
{
    TaskTreeItem * updatedTaskTreeItem = findTaskTreeItem(task.getId());
    if (!updatedTaskTreeItem) return;

    // remember old task to check what changed
    const Task oldTask = updatedTaskTreeItem->getTask();

    updatedTaskTreeItem->setTask(task);

    // if effectiveDate or isDone changed, the TaskTreeItem has to be moved probably
    if (oldTask.getEffectiveDate() != task.getEffectiveDate() || oldTask.isDone() != task.isDone())
    {
        reparentUpdatedItem(updatedTaskTreeItem);
        sortItems(0, Qt::AscendingOrder);
    }
}

void TaskTree::removeTask(const TaskId & taskId)
{
    TaskTreeItem * tti = findTaskTreeItem(taskId);
    if (!tti) return;

    // remember the parent to hide it if needed
    QTreeWidgetItem * parent = tti->parent();

    // remove the TaskTreeItem
    delete tti;

    // then hide the parent if it has no children anymore
    if (parent && parent->childCount() == 0) parent->setHidden(true);
}

void TaskTree::slotItemDoubleClicked(QTreeWidgetItem * item, int column)
{
    TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(item);
    if (!tti) return;

    if (column == 1) {
        bool ok;
        QString newDescription = QInputDialog::getText(this, "Update Description", "Update description",
                                                       QLineEdit::Normal, tti->getTask().getDescription(),&ok);
        if (ok && !newDescription.isEmpty() && newDescription != tti->getTask().getDescription()) {
            Task newTask = tti->getTask();
            newTask.setDescription(newDescription);
            TaskModel::instance()->updateTask(newTask);
        }
    }
}

void TaskTree::slotItemChanged(QTreeWidgetItem * item, int column)
{
    TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(item);
    if (!tti) return;

    // only look for changes of the 'done'-checkbox
    if (column != 0) return;

    bool doneIsChecked = tti->checkState(0) == Qt::Checked;
    if (doneIsChecked != tti->getTask().isDone()) {
        // checkstate changed
        Task newTask = tti->getTask();
        newTask.setDone(doneIsChecked ? QDateTime::currentDateTime() : QDateTime());
        TaskModel::instance()->updateTask(newTask);
    }
}

void TaskTree::handleDayChange()
{
    updateTopLevelItems(this);

    QTreeWidgetItemIterator it(this);
    while (*it) {
        TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(*it);
        if (tti) reparentUpdatedItem(tti);
        ++it;
    }
}

void TaskTree::drawBranches(QPainter * /*painter*/, const QRect & /*rect*/, const QModelIndex & /*index*/) const
{
    // do nothing here as we don't want branches
}

void TaskTree::contextMenuEvent(QContextMenuEvent * e)
{
    QTreeWidgetItem* twi = itemAt(e->pos());
    if (!twi) return;

    if (twi->type() != TaskTreeItem::Type) return;

    Task task = static_cast<TaskTreeItem*>(twi)->getTask();

    QMenu contextMenu;
    QAction * removeTaskAct = contextMenu.addAction("Remove Task");

    QAction * resetPlannedAct = contextMenu.addAction("Reset planned status");
    resetPlannedAct->setEnabled(task.getPlannedDate().isValid());

    QAction *act = contextMenu.exec(e->globalPos());
    if (!act) return;


    if (act == removeTaskAct)
    {
        TaskModel::instance()->removeTask(task.getId());
    }
    else if (act == resetPlannedAct)
    {
        task.setPlannedDate(QDate());
        TaskModel::instance()->updateTask(task);
    }
}

// called when DnD is started
QMimeData * TaskTree::mimeData(const QList<QTreeWidgetItem *> items) const
{
	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);
	TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(items.first());
	if (tti) {
		// DnD is not allowed for 'done' tasks
		if (!tti->getTask().isDone()) {
			out << tti->getTask();
		}
	}

	if (data.isEmpty()) return 0;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("myTasks/Task", data);
	return mimeData;
}

void TaskTree::dragEnterEvent(QDragEnterEvent * e)
{
	if (e->mimeData()->hasFormat("myTasks/Task")) {
		setTopLevelItemsHidden(false);
		e->accept();
	} else {
		e->ignore();
	}
}

void TaskTree::dragMoveEvent(QDragMoveEvent * e)
{
	if (e->mimeData()->hasFormat("myTasks/Task"))
	{
		QTreeWidgetItem * item = itemAt(e->answerRect().topLeft());
		// accept if ...

		// ... there is no item (means remove changelist)
		if (!item) {
			e->accept();
			return;
		}
		// ... there is a TopLevelItem below
		if (item->type() == TopLevelItem::Type) {
			TopLevelItem * tli = dynamic_cast<TopLevelItem*>(item);
			if (tli->getDate().isValid()) {
				e->accept();
				return;
			}
		}

		// ... there is a TaskTreeItem and it has a dueDate
		if (item->type() == TaskTreeItem::Type) {
			TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(item);
			if (tti->getTask().getEffectiveDate().isValid() && !tti->getTask().isDone()) {
				e->accept();
				return;
			}
		}
	}

	// otherwise ignore
	e->ignore();
}

void TaskTree::dragLeaveEvent(QDragLeaveEvent * event)
{
	QTreeWidget::dragLeaveEvent(event);
	setTopLevelItemsHidden(true, true);
}

void TaskTree::dropEvent(QDropEvent *event)
{
	QTreeWidget::dropEvent(event);
	setTopLevelItemsHidden(true, true);
}

Qt::DropActions TaskTree::supportedDropActions () const
{
    // returns what actions are supported when dropping
    return Qt::CopyAction;
}

bool TaskTree::dropMimeData(QTreeWidgetItem *parent, int /*index*/, const QMimeData * data, Qt::DropAction /*action*/)
{
    setTopLevelItemsHidden(true, true);

    QDate plannedDate;
    if (parent) {
        if (parent->type() == TopLevelItem::Type) {
            plannedDate = static_cast<TopLevelItem*>(parent)->getDate();
        } else if (parent->type() == TaskTreeItem::Type)  {
            TaskTreeItem * tti = static_cast<TaskTreeItem*>(parent);
            if (tti->getTask().getEffectiveDate().isValid()) {
                plannedDate = tti->getTask().getEffectiveDate();
            }
        }

        if (!plannedDate.isValid()) return false;
    }

	// Create a QByteArray from the mimedata associated with foo/bar
	QByteArray ba = data->data("myTasks/Task");
	QDataStream ds(&ba, QIODevice::ReadOnly);
	Task task;
	ds >> task;

	// reset the planned date if it equals the due date
	if (task.getDueDate() == plannedDate) task.setPlannedDate(QDate());
	else                                  task.setPlannedDate(plannedDate);

	TaskModel::instance()->updateTask(task);
	return true;
}

TaskTreeItem * TaskTree::findTaskTreeItem(const TaskId & taskId) const
{
    QTreeWidgetItemIterator it(const_cast<TaskTree*>(this));
    while (*it) {
        TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(*it);
        if (tti && tti->getTask().getId() == taskId) {
            return tti;
        }
        ++it;
    }

    return 0;
}

TopLevelItem * TaskTree::getTopLevelItemForTask(const Task & task) const
{
    TopLevelItem * parent = 0;
    if (task.isDone()) {
        parent = doneTopLevelItem;
    } else if (task.getEffectiveDate().isValid()) {
        foreach (TopLevelItem * tli, topLevelItems) {
            if (tli->getDate() <= task.getEffectiveDate()) {
                parent = tli;
                // do not break here!
            }
        }
    }
    return parent;
}

void TaskTree::reparentUpdatedItem(TaskTreeItem * updatedTaskTreeItem)
{
    QTreeWidgetItem * parent = updatedTaskTreeItem->parent();
    if (parent) {
        parent->removeChild(updatedTaskTreeItem);
        if (parent->childCount() == 0 && parent != doneTopLevelItem) parent->setHidden(true);
    } else {
        invisibleRootItem()->removeChild(updatedTaskTreeItem);
    }

    parent = getTopLevelItemForTask(updatedTaskTreeItem->getTask());
    if (parent) {
        parent->addChild(updatedTaskTreeItem);
        if (parent != doneTopLevelItem) parent->setHidden(false);
    } else {
        invisibleRootItem()->insertChild(0, updatedTaskTreeItem);
    }

    sortItems(0, Qt::AscendingOrder);
}
