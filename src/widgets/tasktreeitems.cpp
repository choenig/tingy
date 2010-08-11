#include "tasktreeitems.h"

#include <core/clock.h>
#include <widgets/tasktree.h>

#include <QPainter>
#include <QDebug>

TopLevelItem::TopLevelItem(QTreeWidget * treeWidget)
	: QTreeWidgetItem(treeWidget, Type)
{
}

void TopLevelItem::init()
{
	const QDate today = Clock::currentDate();

	setExpanded(true);
	setFirstColumnSpanned(true);
	setText(0, string_);
	setSizeHint(0, QSize(0, 40));

	QColor fgColor = "#96bb00";
	if      (date_.isNull()) fgColor = "#96bb00";
	else if (date_ <  today) fgColor = Qt::darkRed;
	else if (date_ == today) fgColor = Qt::darkBlue;
	setForeground(0, fgColor);

	QColor bgColor = Qt::transparent;
	if      (date_.isNull()) bgColor = Qt::transparent;
	else if (date_ <  today) bgColor = QColor("#ffe7e9");
	else if (date_ == today) bgColor = QColor("#e7e7ff");
	setBackground(0, bgColor);
}

bool TopLevelItem::operator<(const QTreeWidgetItem & rhs) const
{
	const TopLevelItem * tli = dynamic_cast<const TopLevelItem*>(&rhs);
	if (!tli) return false;

	if (date_.isValid() && tli->getDate().isValid()) {
		return date_ < tli->getDate();
	} else {
		return date_.isValid();
	}
}

//
// TaskTreeItem

TaskTreeItem::TaskTreeItem(TaskTree * tree, const Task & task)
	: QTreeWidgetItem(tree, 0, Type), task_(task)
{
	update();
}

TaskTreeItem::TaskTreeItem(TopLevelItem * topLevelItem, const Task & task)
	: QTreeWidgetItem(topLevelItem, Type), task_(task)
{
	update();
}

void TaskTreeItem::setTask(const Task & task)
{
	task_ = task;
	update();
}

void TaskTreeItem::update()
{
	const QDate today = Clock::currentDate();

	// done checkbox
	setCheckState(0, task_.isDone() ? Qt::Checked : Qt::Unchecked);

	// set priority icon
	if      (task_.getPriority() == Priority::High)   setIcon(1, QIcon(":images/highPriority.png"));
	else if (task_.getPriority() == Priority::Normal) setIcon(1, QIcon(":images/normalPriority.png"));
	else if (task_.getPriority() == Priority::Low)    setIcon(1, QIcon(":images/lowPriority.png"));

	// format additional infos
	QStringList infos;
	if (task_.getEffort().isValid())
		infos << QString::fromUtf8("\xe2\x86\xa5") + task_.getEffort().toString();
	if (!task_.isDone() && task_.getEffectiveDate().isValid() && task_.getEffectiveDate() != Clock::currentDate())
		infos << QString::fromUtf8("\xe2\x86\xa6%1d").arg(Clock::currentDate().daysTo(task_.getEffectiveDate()));
	if (task_.getPlannedDate().isValid())
		infos << QString::fromUtf8("\xe2\x86\xb4") + task_.getPlannedDate().toString("dd.MM.yyyy");
	if (task_.getDueDate().isValid())
		infos << QString::fromUtf8("\xe2\x8a\x9a") + task_.getDueDate().toString("dd.MM.yyyy");
	setText(1, task_.getDescription() + (!infos.isEmpty() ? " [" + infos.join(QString::fromUtf8(" \xe2\x80\xa2 ")) + "]" : QString()));

	// fgColor
	QColor fgColor = Qt::black;
	if      (task_.isDone())              fgColor = Qt::black;
	else if (task_.getDueDate().isNull()) fgColor = "#505050";
	else if (task_.getDueDate() <  today) fgColor = Qt::darkRed;
	else if (task_.getDueDate() == today) fgColor = Qt::darkBlue;
	for (int i = 0; i < 2 ; ++i) setForeground(i, fgColor);

	// bgColor
	QColor bgColor = Qt::transparent;
	if      (task_.isDone())                    bgColor = Qt::transparent;
	else if (task_.getEffectiveDate().isNull()) bgColor = Qt::transparent;
	else if (task_.getEffectiveDate() <  today) bgColor = QColor("#ffe7e9");
	else if (task_.getEffectiveDate() == today) bgColor = QColor("#e7e7ff");
	for (int i = 0; i < 2 ; ++i) setBackground(i, bgColor);

	QFont f = font(0);
	f.setBold(task_.getPriority() == Priority::High && !task_.isDone());
	for (int i = 0; i < 2 ; ++i) setFont(i, f);

	QString tooltip ;
	tooltip += "<table>";
	tooltip += "<tr><td><b>Priorität:</b></td><td>" + task_.getPriority().toTrString() + "</td></tr>";
	tooltip += "<tr><td><b>Beschreibung:</b></td><td>" + task_.getDescription() + "</td></tr>";
	tooltip += "<tr><td><b>Aufwand:</b></td><td>" + task_.getEffort().toString() + "</td></tr>";
	tooltip += "<tr><td><b>Due:</b></td><td>" + QLocale().toString(task_.getDueDate(), "dddd, dd.MM.yyyy") + "</td></tr>";
	tooltip += "<tr><td><b>Geplant:</b></td><td>" + QLocale().toString(task_.getPlannedDate(), "dddd, dd.MM.yyyy") + "</td></tr>";
	tooltip += "<tr><td><b>Erledigt:</b></td><td>" +
			QLocale().toString(task_.getDoneTimestamp(), "dddd, dd.MM.yyyy hh:mm:ss").replace(" ", "&nbsp;") +
			"</td></tr>";
	tooltip += "</table>";
	setToolTip(1, tooltip);
}

void TaskTreeItem::highlightDate(const QDate & date)
{
	if (task_.isDone()) return;

	QFont f = font(1);
	f.setUnderline(date.isValid() && task_.getEffectiveDate() == date);
	f.setOverline( date.isValid() && task_.getEffectiveDate() == date);
	setFont(1, f);
}

bool TaskTreeItem::operator<(const QTreeWidgetItem & other) const
{
	const TaskTreeItem * tti = dynamic_cast<const TaskTreeItem*>(&other);
	if (!tti) return true;

	// shortcuts
	const Task & lhs = task_;
	const Task   rhs = tti->getTask();

	// done tasks are ordered by doneTimestamp
	if (lhs.isDone() && rhs.isDone()) {
		return lhs.getDoneTimestamp() > rhs.getDoneTimestamp();
	}

	// check effective date
	QDate lhsDate = lhs.getEffectiveDate();
	QDate rhsDate = rhs.getEffectiveDate();
	if (lhsDate != rhsDate) return lhsDate < rhsDate;

	// check duedate
	lhsDate = lhs.getDueDate();
	rhsDate = rhs.getDueDate();
	if (lhsDate.isValid() && rhsDate.isValid()) {
		if (lhsDate != rhsDate) return lhsDate < rhsDate;
	} else {
		if (lhsDate != rhsDate) return !lhsDate.isNull();
	}

	// finally compare creation timestamp
	return lhs.getCreationTimestamp() < rhs.getCreationTimestamp();
}

//
// TopLevelItemDelegate

TopLevelItemDelegate::TopLevelItemDelegate(QTreeWidget * parent)
	: QItemDelegate(parent)
{
}

void TopLevelItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	TopLevelItem* tli = dynamic_cast<TopLevelItem*>(static_cast<QTreeWidgetItem*>(index.internalPointer()));
	if (!tli) {
		QItemDelegate::paint(painter, option, index);
		return;
	}

	painter->save();

	// draw background
	painter->fillRect(option.rect.adjusted(0, 10, 0, 0), tli->background(0));

	// draw text
	QFont f(painter->font());
	f.setBold(true);
	painter->setFont(f);
	painter->setPen(tli->foreground(0).color());
	painter->drawText(option.rect.adjusted(10,0,0,-5), Qt::AlignBottom, tli->getString());

	// draw line
	QPoint off(0,-4);
	QLinearGradient gradient(0, 0, 500, 0);
	gradient.setColorAt(0, tli->foreground(0).color());
	gradient.setColorAt(1, Qt::transparent);
	painter->fillRect(QRect(option.rect.bottomLeft()+off, option.rect.bottomRight()+off), QBrush(gradient));

	painter->restore();
}
