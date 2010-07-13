#include "tasktreeitems.h"

#include <widgets/tasktree.h>

#include <QPainter>

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

void TaskTreeItem::init()
{
	setCheckState(0, task_.isDone() ? Qt::Checked : Qt::Unchecked);

	setText(1, task_.getDescription());
	setText(2, task_.getEffort().isValid()  ? task_.getEffort().toString("hh:mm")       : QString());
	setText(3, task_.getDueDate().isValid() ? task_.getDueDate().toString("dd.MM.yyyy") : QString());


	QColor color = Qt::black;
	if      (task_.getDueDate().isNull())               color = Qt::darkBlue;
	else if (task_.getDueDate() <  QDate::currentDate()) color = Qt::darkRed;
	else if (task_.getDueDate() == QDate::currentDate()) color = Qt::darkGreen;
	setForeground(0, color);
	setForeground(1, color);
	setForeground(2, color);
	setForeground(3, color);
}


TopLevelItemDelegate::TopLevelItemDelegate(QTreeWidget * parent)
	: QItemDelegate(parent), parent_(parent)
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
