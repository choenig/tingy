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
#include "tasktree.h"

#include <core/clock.h>
#include <core/task.h>
#include <core/taskmodel.h>
#include <util/log.h>
#include <util/util.h>
#include <widgets/taskeditwidget.h>
#include <widgets/tasktreeitems.h>

#include <QContextMenuEvent>
#include <QDebug>
#include <QHeaderView>

#include <QPainter>
#include <QScrollBar>
#include <QTextDocument>
#include <QTimer>

namespace {

QTextDocument docTimestamp; // fixme

TopLevelItem * doneTopLevelItem;    // fixme
QList<TopLevelItem*> topLevelItems; // fixme

void setTopLevelItemsHidden(bool hide, bool ignoreWithChildren = false)
{
    foreach (TopLevelItem * item , topLevelItems) {
        if (ignoreWithChildren && item->childCount() > 0) continue;
        item->setHidden(hide);
    }
}

void updateNext(TaskTree * tree, QMutableListIterator<TopLevelItem*> & it, const QDate & date, const QString & string)
{
    if (!it.hasNext()) {
        it.insert(new TopLevelItem(tree));
        it.previous();
    }

    it.next()->update(date, string);
}

void updateTopLevelItems(TaskTree * tree)
{
    QMutableListIterator<TopLevelItem*> it(topLevelItems);
    updateNext(tree, it, QDate(1970,1,1), "Überfällig");

    // add entries for current week
    const QDate today = Clock::currentDate();
    const int offset = today.dayOfWeek();
    for (int i = offset ; i <= 7 ; ++i) {
        const QDate date = today.addDays(i-offset);
        const QString str = QLocale().toString(date, dot(QString(date == today ? "Heute * " : "") + "dddd' * 'dd.MM.yyyy"));
        updateNext(tree, it, date, str);
    }

    // add 'next week'
    const QDate nextWeekDate = today.addDays(7-today.dayOfWeek()+1);
    updateNext(tree, it, nextWeekDate, "Nächste Woche");

    // add 'future'
    updateNext(tree, it, nextWeekDate.addDays(7), "Zukunft ...");

    while (it.hasNext()) {
        it.next();
        it.remove();
    }
}

void initTopLevelItems(TaskTree * tree)
{
    updateTopLevelItems(tree);

    doneTopLevelItem = new TopLevelItem(tree);
    doneTopLevelItem->update(QDate(), "Abgeschlossen");

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
	setItemDelegateForColumn(0, new TopLevelItemDelegate(this));
	setItemDelegateForColumn(1, new TaskTreeItemDelegate(this));

	// drag and drop
	setAcceptDrops( true );
	setDragEnabled( true );

	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)),        this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task,bool)), this, SLOT(updateTask(Task,bool)));
    connect(tm, SIGNAL(taskRemoved(TaskId)),    this, SLOT(removeTask(TaskId)));

    initTopLevelItems(this);

    // update the tree on day change
    connect(Clock::instance(), SIGNAL(dateChanged(QDate)), this, SLOT(handleDateChange()));

    // update the viewport when scrolling (this is a workaround for the bg image beeing garbeled)
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), viewport(), SLOT(update()));

    // update timestamp
    QTimer * t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(updateClock()));
    t->start(1000);

    // init tsDoc
    docTimestamp.setDefaultFont(font());
    docTimestamp.setDocumentMargin(0);
    docTimestamp.setDefaultTextOption(QTextOption(Qt::AlignHCenter));

    QTimer::singleShot(0, this, SLOT(init()));
}

void TaskTree::init()
{
    header()->setResizeMode(0, QHeaderView::ResizeToContents);
    header()->setResizeMode(1, QHeaderView::Stretch);
}

void TaskTree::showDoneTasks(bool show)
{
    doneTopLevelItem->setHidden(!show);
}

void TaskTree::highlightDate(const QDate & date)
{
    for (QTreeWidgetItemIterator it(this) ; *it ; ++it)
    {
        if ((*it)->type() == TaskTreeItem::Type) {
            static_cast<TaskTreeItem*>(*it)->highlightDate(date);
        }
    }
}

void TaskTree::filterItems(const QString & filterText)
{
    for (QTreeWidgetItemIterator it(this) ; *it ; ++it)
    {
        if ((*it)->type() == TaskTreeItem::Type) {
            static_cast<TaskTreeItem*>(*it)->filterYourself(filterText);
        }
    }
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

void TaskTree::updateTask(const Task & task, bool doneChanged)
{
    Q_UNUSED(doneChanged)

    TaskTreeItem * updatedTaskTreeItem = findTaskTreeItem(task.getId());
    if (!updatedTaskTreeItem) return;

    // remember old task to check what changed
    const Task oldTask = updatedTaskTreeItem->getTask();

    // update the task
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
        TaskEditWidget * tew = new TaskEditWidget;
        Task newTask = tew->exec(tti->getTask());
        if (newTask.isValid()) {
            TaskModel::instance()->updateTask(newTask);
        }
    }
}

void TaskTree::slotItemChanged(QTreeWidgetItem * item, int column)
{
    // only look for changes of the 'done'-checkbox
    if (column != 0) return;

    TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(item);
    if (!tti) return;

    bool doneIsChecked = tti->checkState(0) == Qt::Checked;
    if (doneIsChecked != tti->getTask().isDone()) {
        // checkstate changed
        Task newTask = tti->getTask();
        newTask.setDone(doneIsChecked ? Clock::currentDateTime() : QDateTime());
        TaskModel::instance()->updateTask(newTask);
    }
}

void TaskTree::handleDateChange()
{
    log << "TaskTree::handleDateChange();";

    // first gather all items up ...
    QList<TaskTreeItem *> allTaskItems;
    for (QTreeWidgetItemIterator it(this) ; *it ; ++it)
    {
        if ((*it)->type() == TaskTreeItem::Type) {
            allTaskItems << static_cast<TaskTreeItem*>(*it);
        }
    }

    updateTopLevelItems(this);

    // ... then reparent and update them
    foreach (TaskTreeItem * tti, allTaskItems) {
        reparentUpdatedItem(tti);
        tti->update();
    }
}

void TaskTree::updateClock()
{
    const QDateTime now = Clock::currentDateTime();
    QString timestamp = dot(now.toString("'<b>'dd.MM.yyyy'</b> * KW%1 * <b>'hh:mm:ss'</b>'")).arg(now.date().weekNumber());

    docTimestamp.setHtml("<font color=\"#8c8c8c\">" + timestamp + "</font>");
    viewport()->update(0, viewport()->height()-docTimestamp.size().height(), viewport()->width(), docTimestamp.size().height());

}

void TaskTree::drawBranches(QPainter * /*painter*/, const QRect & /*rect*/, const QModelIndex & /*index*/) const
{
    // do nothing here as we don't want branches
}

void TaskTree::paintEvent(QPaintEvent * event)
{
    QPainter p(viewport());

    QPixmap pm(":/images/grass.png");
    for (int i = 0; i <= viewport()->width()/pm.width(); ++i) {
        p.drawPixmap(pm.rect().translated(i*pm.width(), viewport()->height()-pm.height()), pm);
    }

    p.translate(0, viewport()->height()-docTimestamp.size().height()+1);
    docTimestamp.drawContents(&p);
    docTimestamp.setTextWidth(viewport()->width());

    QTreeWidget::paintEvent(event);
}

void TaskTree::contextMenuEvent(QContextMenuEvent * e)
{
    QTreeWidgetItem* twi = itemAt(e->pos());
    if (!twi) return;

    switch (twi->type()) {
    case TaskTreeItem::Type:
        static_cast<TaskTreeItem*>(twi)->invokeContextMenu(e->globalPos());
        break;
    case TopLevelItem::Type:
        static_cast<TopLevelItem*>(twi)->invokeContextMenu(e->globalPos());
        break;
    default:
        break;
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
	mimeData->setData("tingy/Task", data);
	return mimeData;
}

void TaskTree::dragEnterEvent(QDragEnterEvent * e)
{
	if (e->mimeData()->hasFormat("tingy/Task")) {
		setTopLevelItemsHidden(false);
		e->accept();
	} else {
		e->ignore();
	}
}

void TaskTree::dragMoveEvent(QDragMoveEvent * e)
{
	if (e->mimeData()->hasFormat("tingy/Task"))
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
	QByteArray ba = data->data("tingy/Task");
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
    for (QTreeWidgetItemIterator it(const_cast<TaskTree*>(this)) ; *it ; ++it)
    {
        if ((*it)->type() == TaskTreeItem::Type) {
            TaskTreeItem * tti = static_cast<TaskTreeItem*>(*it);
            if (tti->getTask().getId() == taskId) {
                return tti;
            }
        }
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
        if (parent != doneTopLevelItem && parent->childCount() == 0) parent->setHidden(true);
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
