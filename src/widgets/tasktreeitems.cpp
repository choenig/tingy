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

void TopLevelItem::init(const QString& string)
{
	const QDate today = QDate::currentDate();

	setExpanded(true);
	setFirstColumnSpanned(true);
	setText(0, string);
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

	setCheckState(0, task_.isDone() ? Qt::Checked : Qt::Unchecked);

	QStringList infos;
	if (task_.getEffort().isValid())  infos << task_.getEffort().toString();
	if (task_.getDueDate().isValid()) infos << task_.getDueDate().toString("dd.MM.yyyy");

	QString txt = task_.getDescription();
	if (!infos.isEmpty()) txt += " [" + infos.join(", ") + "]";

	setText(1, txt);

	QColor fgColor = Qt::black;
	if      (task_.getDueDate().isNull())                 fgColor = Qt::darkGreen;
	else if (task_.getDueDate() <  today)                 fgColor = Qt::darkRed;
	else if (task_.getDueDate() == today)                 fgColor = Qt::darkBlue;
	for (int i = 0; i < 2 ; ++i) setForeground(i, fgColor);

	QColor bgColor = Qt::transparent;
	if      (task_.getEffectiveDate().isNull()) bgColor = Qt::transparent;
	else if (task_.getEffectiveDate() <  today) bgColor = QColor("#ffe7e9");
	else if (task_.getEffectiveDate() == today) bgColor = QColor("#e7e7ff");
	for (int i = 0; i < 2 ; ++i) setBackground(i, bgColor);

	QFont f = font(0);
	f.setBold(false);
	for (int i = 0; i < 2 ; ++i) setFont(i, f);

	if (task_.getImportance() == Importance::High) {
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
