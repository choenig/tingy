#include "tasktree.h"

#include <core/task.h>
#include <core/taskmodel.h>

#include <QContextMenuEvent>
#include <QDate>
#include <QDebug>
#include <QHash>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemDelegate>
#include <QPainter>
#include <QTreeWidgetItem>

//
// TopLevelItem

class TopLevelItem : public QTreeWidgetItem
{
public:
	enum { Type = QTreeWidgetItem::UserType + 23 };

public:
	TopLevelItem(QTreeWidget * treeWidget, QTreeWidgetItem * itmBefore, const QString & string, const QDate & date)
		: QTreeWidgetItem(treeWidget, itmBefore, Type), string_(string), date_(date)
	{
		init(string);
	}

	TopLevelItem(QTreeWidget * treeWidget, const QString & string, const QDate & date)
		: QTreeWidgetItem(treeWidget, Type), string_(string), date_(date)
	{
		init(string);
	}

	QString getString() const { return string_; }
	QDate getDate() const { return date_; }

private:
	void init(const QString& changeList)
	{
		setExpanded(true);
		setFirstColumnSpanned(true);
		setText(0, changeList);
		setSizeHint(0, QSize(0, 40));
	}

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
	TaskTreeItem(TaskTree * tree, const Task & task)
		: QTreeWidgetItem(tree, 0, Type), task_(task)
	{
		init();
	}

	TaskTreeItem(TopLevelItem * topLevelItem, const Task & task)
		: QTreeWidgetItem(topLevelItem, Type), task_(task)
	{
		init();
	}

	Task getTask() { return task_; }
	void setTask(const Task & task) { task_ = task; init(); }

private:
	void init()
	{
		setText(0, task_.getDescription());
		setText(1, task_.getEffort().isValid()  ? task_.getEffort().toString("hh:mm")       : QString());
		setText(2, task_.getDueDate().isValid() ? task_.getDueDate().toString("dd.MM.yyyy") : QString());

		setCheckState(0, Qt::Unchecked);

		QColor color = Qt::black;
		if      (task_.getDueDate().isNull())               color = Qt::darkBlue;
		else if (task_.getDueDate() <  QDate::currentDate()) color = Qt::darkRed;
		else if (task_.getDueDate() == QDate::currentDate()) color = Qt::darkGreen;
		setForeground(0, color);
		setForeground(1, color);
		setForeground(2, color);
	}

private:
	Task task_;
};

class TopLevelItemDelegate : public QItemDelegate
{
public:
	TopLevelItemDelegate(QTreeWidget * parent = 0) :QItemDelegate(parent), parent_(parent) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		TopLevelItem* tli = dynamic_cast<TopLevelItem*>(static_cast<QTreeWidgetItem*>(index.internalPointer()));
		if (!tli) {
			QItemDelegate::paint(painter, option, index);
			return;
		}

		painter->save();
		QFont f(painter->font());
		f.setBold(true);
		painter->setFont(f);
		painter->drawText(option.rect.adjusted(10,0,0,-5), Qt::AlignBottom, tli->getString());
		{
			QPoint off(0,-6);
			QLinearGradient gradient(0, 0, 300, 0);
			gradient.setColorAt(0, Qt::blue);
			gradient.setColorAt(1, Qt::white);
			painter->fillRect(QRect(option.rect.bottomLeft()+off, option.rect.bottomRight()+off), QBrush(gradient));
		}
		painter->restore();
	}

private:
	QTreeWidget* parent_;
};

//
// TaskTree

namespace {

QMap<QDate, TopLevelItem*> topLevelItems;

void initTopLevelItems(TaskTree * tree)
{
    const QDate today = QDate::currentDate();

    topLevelItems[QDate(1970,1,1)]  = new TopLevelItem(tree, "Overdue",    QDate(1970,1,1));
    topLevelItems[today]            = new TopLevelItem(tree, "Today",      today);
    topLevelItems[today.addDays(1)] = new TopLevelItem(tree, "Tomorrow",   today.addDays(1));
    topLevelItems[today.addDays(3)] = new TopLevelItem(tree, "Next week",  today.addDays(3));
    topLevelItems[today.addDays(7)] = new TopLevelItem(tree, "Future ...", today.addDays(7));
    topLevelItems[QDate()]          = new TopLevelItem(tree, "Done",       QDate());

    // initially hide all items
    foreach (const QDate & date, topLevelItems.keys()) {
        topLevelItems[date]->setHidden(true);
    }
}

}

TaskTree::TaskTree(QWidget *parent)
	: QTreeWidget(parent)
{
	setRootIsDecorated(false);
	setIndentation(10);

	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem *, int)));

	// used for top level items
	setItemDelegateForColumn(0, new TopLevelItemDelegate(this));

	// drag and drop
	setAcceptDrops( true );
	setDragEnabled( true );

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)), this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task)), this, SLOT(updateTask(Task)));

    initTopLevelItems(this);
}

void TaskTree::addTask(const Task & task)
{
    // fixme this is copy paste code
    TopLevelItem * parent = 0;
    if (task.isDone()) {
        parent = topLevelItems[QDate()];
    } else if (task.getDueDate().isValid()) {
        foreach (const QDate & date, topLevelItems.keys()) {
            if (date <= task.getDueDate()) {
                parent = topLevelItems[date];
            }
        }
    }

    if (parent) {
        // make sure parent is shown
        parent->setHidden(false);
        // and add new item
        new TaskTreeItem(parent, task);
    } else {
        new TaskTreeItem(this, task);
    }


    // RFI: move this out of here
    setColumnWidth(0, 300);
    setColumnWidth(1, 60);
    setColumnWidth(2, 50);
}

void TaskTree::updateTask(const Task & task)
{
    QTreeWidgetItemIterator it(this);
    while (*it) {
        TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(*it);
        if (tti) {
            if (tti->getTask().getId() == task.getId())
            {
                Task oldTask = tti->getTask();
                if (oldTask.getDueDate() != task.getDueDate())
                {
                    QTreeWidgetItem * parent = tti->parent();
                    parent->removeChild(tti);
                    if (parent->childCount() == 0) parent->setHidden(true);

                    parent = 0;
                    // fixme this is copy paste code
                    if (task.isDone()) {
                        parent = topLevelItems[QDate()];
                    } else if (task.getDueDate().isValid()) {
                        foreach (const QDate & date, topLevelItems.keys()) {
                            if (date <= task.getDueDate()) {
                                parent = topLevelItems[date];
                            }
                        }
                    }
                    parent->addChild(tti);
                }
                tti->setTask(task);

                break;
            }
        }
        ++it;
    }
}

void TaskTree::slotItemDoubleClicked(QTreeWidgetItem * item, int column)
{
    TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(item);
    if (!tti) return;

    if (column == 0) {
        bool ok;
        QString newDescription = QInputDialog::getText(this, "ficken", "Update description", QLineEdit::Normal, tti->getTask().getDescription(),&ok);
        if (ok && !newDescription.isEmpty() && newDescription != tti->getTask().getDescription()) {
            Task newTask = tti->getTask();
            newTask.setDescription(newDescription);
            TaskModel::instance()->updateTask(newTask);
        }
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

    //	if (twi->type() == ChangeListItem::Type) {
    //		showChangeListContextMenu(dynamic_cast<ChangeListItem*>(twi), e->globalPos());
    //	} else if (twi->type() == SvnTreeWidgetItem::Type) {
    //		showSvnItemContextMenu(e);
    //	}
}


bool TaskTree::dropMimeData(QTreeWidgetItem *parent, int /*index*/, const QMimeData * data, Qt::DropAction /*action*/)
{
    QDate plannedDate;

    if (parent) {
        TopLevelItem  * tli = dynamic_cast<TopLevelItem*>(parent);
        if (tli) plannedDate = tli->getDate();
        else {
            TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(parent);
            if (tti && tti->getTask().getDueDate().isValid()) plannedDate = tti->getTask().getDueDate(); // fixme use planned here
            else return false;
        }
    }

	// Create a QByteArray from the mimedata associated with foo/bar
	QByteArray ba = data->data("myTasks/Task");
	QDataStream ds(&ba, QIODevice::ReadOnly);
	Task task;
	ds >> task;

	task.setDueDate(plannedDate); // fixme this should update the planned date
	TaskModel::instance()->updateTask(task);
	return true;
}

QMimeData * TaskTree::mimeData(const QList<QTreeWidgetItem *> items) const
{
	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);
	foreach (QTreeWidgetItem* itm, items) {
		TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(itm);
		if (tti) out << tti->getTask();
	}

	if (data.isEmpty()) return 0;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("myTasks/Task", data);
	return mimeData;
}

void TaskTree::dragEnterEvent(QDragEnterEvent * e)
{
	if (e->mimeData()->hasFormat("myTasks/Task")) {
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
			e->accept();
			return;
		}

		// ... there is a TaskTreeItem and it has a dueDate
		if (item->type() == TaskTreeItem::Type) {
			TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(item);
			if (tti->getTask().getDueDate().isValid()) {
				e->accept();
				return;
			}
		}
	}

	// otherwise ignore
	e->ignore();
}

Qt::DropActions TaskTree::supportedDropActions () const
{
    // returns what actions are supported when dropping
    return Qt::CopyAction;
}




//void TaskTree::showChangeListContextMenu(ChangeListItem * cli, const QPoint & globalPos)
//{
//	QMenu contextMenu;
//	QAction * renameAct = contextMenu.addAction("Rename Changelist");

//	QAction *act = contextMenu.exec( globalPos );
//	if (!act) return;

//	if (act == renameAct)
//	{
//		// request new name
//		bool ok;
//		QString changeListName = QInputDialog::getText(this, tr("New Changelist name"), tr("Name:"),
//													   QLineEdit::Normal, cli->getChangeList(), &ok);
//		if (!ok || changeListName.isEmpty()) return;

//		// get all files from the old changelist
//		QStringList filesInChangeList;
//		for (int i = 0; i < cli->childCount(); ++i) {
//			SvnTreeWidgetItem *twi = dynamic_cast<SvnTreeWidgetItem*>(cli->child(i));
//			if (twi) filesInChangeList << twi->getItem().filePath;
//		}

//		// add all those files to the new changelist
//		bool success = !filesInChangeList.isEmpty();
//		SvnApi::Client client;
//		success |= client.addToChangeList(filesInChangeList, changeListName);
//		if (success) emit contentChanged();
//	}
//}

//void TaskTree::showSvnItemContextMenu(QContextMenuEvent * e)
//{
//	// prepare lists
//	QList<SvnTreeWidgetItem*> itemsSelected;
//	QList<SvnTreeWidgetItem*> itemsWithChangeList;
//	QList<SvnTreeWidgetItem*> itemsNotVersioned;
//	QStringList selFiles;
//	foreach (QTreeWidgetItem * i, selectedItems()) {
//		SvnTreeWidgetItem* twi = dynamic_cast<SvnTreeWidgetItem*>(i);
//		if (!twi) continue;
//		itemsSelected << twi;
//		selFiles << twi->getItem().filePath;
//		if (twi->hasChangeList())        itemsWithChangeList << twi;
//		if (twi->getItem().notVersioned) itemsNotVersioned << twi;
//	}
//	SvnTreeWidgetItem* curItem = dynamic_cast<SvnTreeWidgetItem*>(itemAt(e->pos()));

//	if(itemsSelected.isEmpty() || !curItem) return;

//	QMenu contextMenu;

//// 	QAction* diffFile   = contextMenu.addAction(     "Diff" ); // TODO

//	QMenu*   changeListMenu = contextMenu.addMenu( QIcon(":/add_cl.png"), "Add to Changelist");
//	changeListMenu->setEnabled(itemsNotVersioned.isEmpty());
//	QAction* newChangeList  = changeListMenu->addAction( "New Changelist ..." );
//	QAction* removeFromCL   = contextMenu.addAction(     QIcon(":/removefromcl.png"), "Remove from Changelist" );
//	removeFromCL->setEnabled(!itemsWithChangeList.isEmpty());

//	changeListMenu->addSeparator();

//	QList<QAction*> changeListActions;
//	QStringList changeLists = getAllChangelists();
//	if (!changeLists.isEmpty()) {
//		foreach (const QString& cl, changeLists) {
//			changeListActions << changeListMenu->addAction(cl);
//		}
//	}

//	contextMenu.addSeparator();

//	QAction* commit = contextMenu.addAction( QIcon(":/commit.png"), "Commit ..." );
//	commit->setEnabled(itemsNotVersioned.isEmpty());

//	QAction* addFile = contextMenu.addAction( QIcon(":/add_file.png"), "Add Files / Folders" );
//	addFile->setEnabled(!itemsNotVersioned.isEmpty());

//	QAction* delFile = contextMenu.addAction( QIcon(":/del_file.png"), "Delete Files / Folders" );
//	delFile->setEnabled(!itemsNotVersioned.isEmpty());

//	QAction* ignoreFile = contextMenu.addAction( QIcon(":/ignore.png"), "Ignore Files / Folders" );
//	ignoreFile->setEnabled(!itemsNotVersioned.isEmpty());

//	contextMenu.addSeparator();

//	QAction* revertChanges   = contextMenu.addAction( QIcon(":/revert.png"), "Revert Changes" );
//	revertChanges->setEnabled(itemsNotVersioned.isEmpty());

//	QAction* resolveConflict = contextMenu.addAction( QIcon(":/resolved.png"), "Resolve Conflict" );
//	resolveConflict->setEnabled(itemsSelected.size() == 1 && curItem->getItem().isConflicted());

//	// then ... finally show the context menu ...
//	QAction *act = contextMenu.exec( e->globalPos() );
//	if (!act) return;

//	// ... and handle the choosen action
//	bool success = false;
//	if (act == newChangeList)
//	{
//		bool ok;
//		QString changeListName = QInputDialog::getText(this, tr("Add to new Changelist"), tr("Name:"), QLineEdit::Normal, "", &ok);
//		if (ok || !changeListName.isEmpty()) {
//			SvnApi::Client client;
//			success = client.addToChangeList(selFiles, changeListName);
//		}
//	}
//	else if (act == removeFromCL) {
//		SvnApi::Client client;
//		success = client.removeFromChangeList(selFiles);
//	}
//	else if (act == commit) {
//		foreach (SvnTreeWidgetItem * i, itemsSelected) {
//			i->setCheckState(0, Qt::Checked);
//		}
//		if (!itemsSelected.isEmpty()) {
//			emit showExtension(IqMainWidget::CommitView);
//		}
//	}
//	else if (act == addFile) {
//		SvnApi::Client client;
//		foreach (SvnTreeWidgetItem * i, itemsNotVersioned) {
//			success |= client.add(i->getItem().filePath);
//		}
//	}
//	else if (act == delFile) {
//		foreach (SvnTreeWidgetItem * i, itemsNotVersioned) {
//			success |= QFile::remove(i->getItem().filePath);
//		}
//	}
//	else if (act == revertChanges) {
//		SvnApi::Client client;
//		success = client.revert(selFiles);
//		if (success) {
//			// remove the reverted file also from its changelist
//			// (this is not done automatically!)
//			client.removeFromChangeList(selFiles);
//		}
//	}
//	else if (act == resolveConflict) {
//		SvnApi::Client client;
//		success = client.resolved(curItem->getItem().filePath);
//	}
//	else if (act == ignoreFile) {
//		SvnApi::Client client;

//		foreach (SvnTreeWidgetItem * i, itemsNotVersioned)
//		{
//			QStringList files = client.propGet(i->getItem().path(), "svn:ignore").split("\n", QString::SkipEmptyParts);
//			files << i->getItem().fileName();
//			success |= client.propSet(i->getItem().path(), "svn:ignore", files.join("\n"));
//		}
//	}
//	else if (changeListActions.contains(act))
//	{
//		QString changeListName = act->text();

//		SvnApi::Client client;
//		success = client.addToChangeList(selFiles, changeListName);
//	}

//	if (success) emit contentChanged();
//}
