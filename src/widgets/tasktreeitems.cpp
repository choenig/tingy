#include "tasktreeitems.h"

#include <widgets/tasktree.h>

#include <QPainter>
#include <QDebug>

//
// TopLevelItem

TopLevelItem::TopLevelItem(QTreeWidget * treeWidget, QTreeWidgetItem * itmBefore, const QString & string, const QDate & date)
	: QTreeWidgetItem(treeWidget, itmBefore, Type), string_(string), date_(date)
{
	init(string);
}

TopLevelItem::TopLevelItem(QTreeWidget * treeWidget, const QString & string, const QDate & date)
	: QTreeWidgetItem(treeWidget, Type), string_(string), date_(date)
{
	init(string);
}

void TopLevelItem::init(const QString& changeList)
{
	setExpanded(true);
	setFirstColumnSpanned(true);
	setText(0, changeList);
	setSizeHint(0, QSize(0, 40));
}

bool TopLevelItem::operator<(const QTreeWidgetItem & rhs) const
{
	const TopLevelItem * tli = dynamic_cast<const TopLevelItem*>(&rhs);
	if (!tli) return false;

	if (date_.isValid() && tli->getDate().isValid()) {
		return date_ < tli->getDate();
	} else {
		return date_.isValid() ? true : false;
	}


}

//
// TaskTreeItem

TaskTreeItem::TaskTreeItem(TaskTree * tree, const Task & task)
	: QTreeWidgetItem(tree, 0, Type), task_(task)
{
	init();
}

TaskTreeItem::TaskTreeItem(TopLevelItem * topLevelItem, const Task & task)
	: QTreeWidgetItem(topLevelItem, Type), task_(task)
{
	init();
}

void TaskTreeItem::setTask(const Task & task)
{
	task_ = task;
	init();
}


void TaskTreeItem::init()
{
	setCheckState(0, task_.isDone() ? Qt::Checked : Qt::Unchecked);

	setText(1, task_.getDescription() + " [" + task_.getPlannedDate().toString(Qt::ISODate) + "]");
	setText(2, task_.getEffort().isValid()  ? task_.getEffort().toString("hh:mm")       : QString());
	setText(3, task_.getDueDate().isValid() ? task_.getDueDate().toString("dd.MM.yyyy") : QString());

	QColor color = Qt::black;
	if      (task_.getDueDate().isNull())                color = Qt::darkGreen;
	else if (task_.getDueDate() <  QDate::currentDate()) color = Qt::darkRed;
	else if (task_.getDueDate() == QDate::currentDate()) color = Qt::darkBlue;
	setForeground(0, color);
	setForeground(1, color);
	setForeground(2, color);
	setForeground(3, color);

	if (task_.getImportance() == Importance::High) {
		QFont f = font(0);
		f.setBold(true);
		setFont(0, f);
		setFont(1, f);
		setFont(2, f);
		setFont(3, f);
	}
}


bool TaskTreeItem::operator<(const QTreeWidgetItem & rhs) const
{
	const TaskTreeItem * tti = dynamic_cast<const TaskTreeItem*>(&rhs);
	if (!tti) return true;

	QDate lhsDate = task_.getEffectiveDate();
	QDate rhsDate = tti->getTask().getEffectiveDate();
	if (lhsDate != rhsDate) return lhsDate < rhsDate;

	lhsDate = task_.getDueDate();
	rhsDate = tti->getTask().getDueDate();
	if (lhsDate.isValid() && rhsDate.isValid()) {
		if (lhsDate != rhsDate) return lhsDate < rhsDate;
	} else {
		if (lhsDate != rhsDate) return !lhsDate.isNull();
	}

	return task_.getCreationTimestamp() < tti->getTask().getCreationTimestamp();
}

//
// TopLevelItemDelegate

TopLevelItemDelegate::TopLevelItemDelegate(QTreeWidget * parent)
	: QItemDelegate(parent)
{
}

void TopLevelItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
