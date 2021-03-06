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
#include "tasktreeitems.h"

#include <core/clock.h>
#include <core/taskmodel.h>
#include <util/util.h>
#include <widgets/taskeditwidget.h>
#include <widgets/tasktree.h>

#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextTable>

namespace {

QTextCharFormat textCharFormat(int fontSize, bool bold = false)
{
    QFont font;
    font.setPixelSize(fontSize);
    font.setBold(bold);

    QTextCharFormat retval;
    retval.setFont(font);
    return retval;
}

QTextBlockFormat textBlockFormat(Qt::Alignment alignment)
{
    QTextBlockFormat retval;
    retval.setAlignment(alignment);
    return retval;
}

QTextTableFormat textTableFormat(int border, Qt::Alignment alignment, const QVector<QTextLength> & widthCostraints)
{
    QTextTableFormat retval;
    retval.setBorder(border);
    retval.setAlignment(alignment);
    retval.setColumnWidthConstraints(widthCostraints);
    return retval;
}

}

TopLevelItem::TopLevelItem(QTreeWidget * treeWidget)
	: QTreeWidgetItem(treeWidget, Type)
{
}

void TopLevelItem::invokeContextMenu(const QPoint & pos)
{
    QMenu contextMenu;
    QAction * printAct = contextMenu.addAction(QIcon(":/images/print.png"), "Tasks ausdrucken");

    QAction *act = contextMenu.exec(pos);
    if (act == printAct)
    {
        // gather tasks of current TopLevelItem
        QList<Task> tasks;
        for (int i = 0 ; i < childCount() ; ++i) {
            tasks << static_cast<TaskTreeItem*>(child(i))->getTask();
        }
        qSort(tasks);

        QTextDocument doc;
        QTextCursor cursor(&doc);

        {   // tingy header
            QVector<QTextLength> headerWidthConstraints;
            headerWidthConstraints << QTextLength(QTextLength::PercentageLength, 50);
            headerWidthConstraints << QTextLength(QTextLength::PercentageLength, 50);

            QTextTable * headerTable = cursor.insertTable(1, 2, textTableFormat(0, Qt::AlignCenter, headerWidthConstraints));

            headerTable->cellAt(0,0).firstCursorPosition().setBlockFormat(textBlockFormat(Qt::AlignLeft));
            headerTable->cellAt(0,0).firstCursorPosition().insertText("tingy");

            headerTable->cellAt(0,1).firstCursorPosition().setBlockFormat(textBlockFormat(Qt::AlignRight));
            headerTable->cellAt(0,1).firstCursorPosition().insertHtml(dot(Clock::currentDateTime().toString("dd.MM.yyyy' * 'hh:mm:ss' Uhr'")));
            cursor.movePosition(QTextCursor::Down);
        }

        cursor.insertBlock();
        cursor.insertBlock();

        {   // date header
            QTextFrameFormat headerFrameFormat;
            headerFrameFormat.setBackground(QColor("#d4d4d4"));
            cursor.insertFrame(headerFrameFormat);

            cursor.insertText(text(0), textCharFormat(16, true));
            cursor.movePosition(QTextCursor::Down);
        }

        QVector<QTextLength> dataWidthConstraints;
        dataWidthConstraints << QTextLength(QTextLength::PercentageLength,  3);
        dataWidthConstraints << QTextLength(QTextLength::PercentageLength, 74);
        dataWidthConstraints << QTextLength(QTextLength::PercentageLength, 10);
        dataWidthConstraints << QTextLength(QTextLength::PercentageLength, 13);

        QTextTable * dataTable = cursor.insertTable(tasks.size()*2+2, 4, textTableFormat(0, Qt::AlignCenter, dataWidthConstraints));

        int idx = 0;
        {   // empty small line
            dataTable->mergeCells(0,0,1,4);
            dataTable->cellAt(0,0).setFormat(textCharFormat(4));
            ++idx;
        }

        { // table header
            QTextCharFormat charFormat = textCharFormat(10);
            dataTable->cellAt(1,1).firstCursorPosition().insertText("Titel",   charFormat);
            dataTable->cellAt(1,2).firstCursorPosition().insertText("Aufwand", charFormat);
            dataTable->cellAt(1,3).firstCursorPosition().insertText("Due",     charFormat);
            ++idx;
        }

        foreach (const Task & task, tasks)
        {
            // checkbox
            QString txt = task.isDone() ? QString::fromUtf8("\xe2\x98\x91") : QString::fromUtf8("\xe2\x98\x90");
            dataTable->cellAt(idx,0).firstCursorPosition().setBlockFormat(textBlockFormat(Qt::AlignHCenter));
            dataTable->cellAt(idx,0).firstCursorPosition().insertHtml(txt);

            // title + description
            txt = task.getTitle();
            if (task.getPriority() == Priority::High) txt.prepend("<b>").append("</b>");
            if (!task.getDescription().isEmpty()) txt += "<br><i>" + task.getDescription() + "</i>";
            dataTable->cellAt(idx,1).firstCursorPosition().insertHtml(txt);

            // effort
            txt = task.getEffort().toString();
            if (task.getPriority() == Priority::High) txt.prepend("<b>").append("</b>");
            dataTable->cellAt(idx,2).firstCursorPosition().insertHtml(txt);

            // due date
            txt = task.getDueDate().toString("dd.MM.yyyy");
            if (task.getPriority() == Priority::High) txt.prepend("<b>").append("</b>");
            dataTable->cellAt(idx,3).firstCursorPosition().insertHtml(txt);
            ++idx;

            // empty small line
            dataTable->mergeCells(idx,0,1,4);
            dataTable->cellAt(idx,0).setFormat(textCharFormat(4));
            ++idx;
        }

        // for debugging
        //QTextEdit te;
        //te.setDocument(&doc);
        //te.resize(800, 600);
        //te.show();

        // finally print the document
        QPrinter printer;
        QPrintDialog printDialog(&printer, this->treeWidget());
        if (printDialog.exec() == QDialog::Accepted) {
            doc.print(&printer);
        }
    }
}

void TopLevelItem::update(const QDate & date, const QString & string)
{
	date_ = date;
	string_ = string;
	init();
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

void TaskTreeItem::invokeContextMenu(const QPoint & pos)
{
    QMenu contextMenu;
    // reset planned status
    QAction * resetPlannedAct = contextMenu.addAction(QIcon(":/images/reset.png"), "�Geplant�-Status zur�cksetzen");
    resetPlannedAct->setEnabled(task_.getPlannedDate().isValid());

    // remove task
    QAction * removeTaskAct = contextMenu.addAction(QIcon(":/images/trash.png"), "Task l�schen");

    // edit task
    contextMenu.addSeparator();
    QAction * editTaskAct = contextMenu.addAction(QIcon(":/images/properties.png"), "Eigenschaften");
    QFont f = editTaskAct->font();
    f.setBold(true);
    editTaskAct->setFont(f);

    QAction *act = contextMenu.exec(pos);
    if (act == removeTaskAct)
    {
        int result = QMessageBox::question(0,"Task l�schen?",
                                           QString("Sind Sie sicher, dass Sie den Task '%1' l�schen m�chten?").arg(task_.getTitle()),
                                           QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            TaskModel::instance()->removeTask(task_.getId());
        }
    }
    else if (act == resetPlannedAct)
    {
        task_.resetPlannedDate();
        TaskModel::instance()->updateTask(task_);
    }
    else if (act == editTaskAct)
    {
        TaskEditWidget * tew = new TaskEditWidget;
        Task newTask = tew->exec(task_);
        if (newTask.isValid()) {
            TaskModel::instance()->updateTask(newTask);
        }
    }
}

void TaskTreeItem::update()
{
	const QDate today = Clock::currentDate();

	// done checkbox
	setCheckState(0, task_.isDone() ? Qt::Checked : Qt::Unchecked);

	// set priority icon
	if      (task_.getPriority() == Priority::High)   setIcon(1, QIcon(":/images/highPriority.png"));
	else if (task_.getPriority() == Priority::Normal) setIcon(1, QIcon(":/images/normalPriority.png"));
	else if (task_.getPriority() == Priority::Low)    setIcon(1, QIcon(":/images/lowPriority.png"));

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
	setText(1, task_.getTitle() + (!infos.isEmpty() ? " [" + infos.join(QString::fromUtf8(" \xe2\x80\xa2 ")) + "]" : QString()));

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

	QLocale lc;

	QString tooltip ;
	tooltip += "<table>";
	tooltip += "<tr><td><b>Priorit�t:</b></td><td>" + task_.getPriority().toTrString() + "</td></tr>";
	tooltip += "<tr><td><b>Title:</b></td><td>" + task_.getTitle() + "</td></tr>";
	tooltip += "<tr><td><b>Beschreibung:</b></td><td>" + task_.getDescription() + "</td></tr>";
	tooltip += "<tr><td><b>Aufwand:</b></td><td>" + task_.getEffort().toString() + "</td></tr>";
	tooltip += "<tr><td><b>Geplant:</b></td><td>" + lc.toString(task_.getPlannedDate(), "dddd, dd.MM.yyyy") + "</td></tr>";
	tooltip += "<tr><td><b>Due:</b></td><td>" + lc.toString(task_.getDueDate(), "dddd, dd.MM.yyyy") + "</td></tr>";
	tooltip += "<tr><td><b>Erledigt:</b></td><td>" +
			lc.toString(task_.getDoneTimestamp(), "dddd,'&npsp;'dd.MM.yyyy'&nbsp;'hh:mm:ss") + "</td></tr>";
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

void TaskTreeItem::filterYourself(const QString & filterText)
{
	filterHits.clear();

	if (!filterText.isEmpty()) {
		const int len = filterText.size();
		const QString txt = text(1);
		int idx = 0;
		while ((idx = txt.indexOf(filterText, idx, Qt::CaseInsensitive)) != -1) {
			filterHits << QPoint(idx, len);
			idx += len;
		}
	}

	setHidden(filterHits.isEmpty() && !filterText.isEmpty());
}

bool TaskTreeItem::operator<(const QTreeWidgetItem & other) const
{
	const TaskTreeItem * tti = dynamic_cast<const TaskTreeItem*>(&other);
	if (!tti) return true;

	return task_ < tti->getTask();
}

//
// TopLevelItemDelegate

void TopLevelItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QTreeWidgetItem * twi = static_cast<QTreeWidgetItem*>(index.internalPointer());
	if (twi->type() != TopLevelItem::Type) {
		QItemDelegate::paint(painter, option, index);
		return;
	}

	painter->save();

	// draw background
	painter->fillRect(option.rect.adjusted(0, 10, 0, 0), index.data(Qt::BackgroundRole).value<QBrush>().color());

	// draw text
	QFont f(painter->font());
	f.setBold(true);
	painter->setFont(f);
	painter->setPen(index.data(Qt::ForegroundRole).value<QBrush>().color());
	painter->drawText(option.rect.adjusted(10,0,0,-5), Qt::AlignBottom, index.data().toString());

	// draw line
	QPoint off(0,-4);
	QLinearGradient gradient(0, 0, 500, 0);
	gradient.setColorAt(0, index.data(Qt::ForegroundRole).value<QBrush>().color());
	gradient.setColorAt(1, Qt::transparent);
	painter->fillRect(QRect(option.rect.bottomLeft()+off, option.rect.bottomRight()+off), QBrush(gradient));

	painter->restore();
}


//
// TaskTreeItemDelegate


void TaskTreeItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	TaskTreeItem * tti = dynamic_cast<TaskTreeItem*>(static_cast<QTreeWidgetItem*>(index.internalPointer()));

	if (tti) filters = tti->getFilterHits();
	else     filters.clear();

	QItemDelegate::paint(painter, option, index);
}


void TaskTreeItemDelegate::drawDisplay(QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text) const
{
	if (!filters.isEmpty())
	{
		painter->save();

		QFontMetrics fm(option.fontMetrics);
		foreach (const QPoint & f, filters) {
			int x = fm.size(Qt::TextSingleLine, text.left(f.x()))        .width()     + 3; // margin
			int w = fm.size(Qt::TextSingleLine, text.left(f.x() + f.y())).width() - x + 1;
			painter->fillRect(rect.x() + x, rect.y(), w, rect.height(), Qt::yellow);
		}

		painter->restore();
	}

	QItemDelegate::drawDisplay(painter, option, rect, text);
}
