#include "tasktreeitems.h"

#include <widgets/tasktree.h>

#include <QPainter>
#include <QDebug>

//
// TopLevelItem

//TopLevelItem::TopLevelItem(QTreeWidget * treeWidget, QTreeWidgetItem * itmBefore)
//	: QTreeWidgetItem(treeWidget, itmBefore, Type)
//{
//	init(string);
//}

TopLevelItem::TopLevelItem(QTreeWidget * treeWidget)
	: QTreeWidgetItem(treeWidget, Type)
{
}

void TopLevelItem::init()
{
	const QDate today = QDate::currentDate();

	setExpanded(true);
	setFirstColumnSpanned(true);
	setText(0, string_);
	setSizeHint(0, QSize(0, 40));

	QColor bgColor = Qt::transparent;
	if      (date_.isNull()) bgColor = Qt::transparent;
	else if (date_ <  today) bgColor = QColor("#ffe7e9");
	else if (date_ == today) bgColor = QColor("#e7e7ff");
	setBackgroundColor(0, bgColor);
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
	const QDate today = QDate::currentDate();

	if        (task_.getPriority() == Priority::High) {
		setIcon(1, QIcon(":images/highPriority.png"));
	} else if (task_.getPriority() == Priority::Normal) {
		setIcon(1, QIcon(":images/normalPriority.png"));
	} else if (task_.getPriority() == Priority::Low) {
		setIcon(1, QIcon(":images/lowPriority.png"));
	}

	setCheckState(0, task_.isDone() ? Qt::Checked : Qt::Unchecked);

	QStringList infos;
	if (task_.getEffort().isValid())
		infos << QString::fromUtf8("\xe2\x86\xa5") + task_.getEffort().toString();
	if (task_.getEffectiveDate().isValid() && task_.getEffectiveDate() != QDate::currentDate())
		infos << QString::fromUtf8("\xe2\x86\xa6%1d").arg(QDate::currentDate().daysTo(task_.getEffectiveDate()));
	if (task_.getPlannedDate().isValid())
		infos << QString::fromUtf8("\xe2\x86\xb4") + task_.getPlannedDate().toString("dd.MM.yyyy");
	if (task_.getDueDate().isValid())
		infos << QString::fromUtf8("\xe2\x8a\x9a") + task_.getDueDate().toString("dd.MM.yyyy");

	QString txt = task_.getDescription();
	if (!infos.isEmpty()) txt += " [" + infos.join(", ") + "]";

	setText(1, txt);

	QColor fgColor = Qt::black;
	if      (task_.getDueDate().isNull()) fgColor = Qt::darkGreen;
	else if (task_.getDueDate() <  today) fgColor = Qt::darkRed;
	else if (task_.getDueDate() == today) fgColor = Qt::darkBlue;
	for (int i = 0; i < 2 ; ++i) setForeground(i, fgColor);

	QColor bgColor = Qt::transparent;
	if      (task_.getEffectiveDate().isNull()) bgColor = Qt::transparent;
	else if (task_.getEffectiveDate() <  today) bgColor = QColor("#ffe7e9");
	else if (task_.getEffectiveDate() == today) bgColor = QColor("#e7e7ff");
	for (int i = 0; i < 2 ; ++i) setBackground(i, bgColor);

	QFont f = font(0);
	f.setBold(false);
	for (int i = 0; i < 2 ; ++i) setFont(i, f);

	if (task_.getPriority() == Priority::High) {
		f.setBold(true);
		for (int i = 0; i < 2 ; ++i) setFont(i, f);
	}

	if (task_.getDueDate() < task_.getPlannedDate()) {
		setForeground(3, Qt::red);
		f.setBold(true);
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

	painter->fillRect(option.rect.adjusted(0, 10, 0, 0), tli->backgroundColor(0));

	QFont f(painter->font());
	f.setBold(true);
	painter->setFont(f);
	painter->drawText(option.rect.adjusted(10,0,0,-5), Qt::AlignBottom, tli->getString());
	{
		QPoint off(0,-6);
		QLinearGradient gradient(0, 0, 300, 0);
		gradient.setColorAt(0, Qt::blue);
		gradient.setColorAt(1, Qt::transparent);
		painter->fillRect(QRect(option.rect.bottomLeft()+off, option.rect.bottomRight()+off), QBrush(gradient));
	}
	painter->restore();
}
