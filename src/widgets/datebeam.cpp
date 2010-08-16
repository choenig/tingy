#include "datebeam.h"

#include <core/clock.h>
#include <core/task.h>
#include <core/taskmodel.h>

#include <QDateEdit>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

DateBeam::DateBeam(QWidget *parent) :
    QWidget(parent), cellWidth_(30), hoveredCell_(0), mouseIn_(false)
{
    setMouseTracking(true);
    setAcceptDrops(true);

    // update on date change
    connect(Clock::instance(), SIGNAL(dateChanged(QDate)), this, SLOT(update()));
    // fixme cleanup tasks from the past in datesWithTasks_

    TaskModel * tm = TaskModel::instance();
    connect(tm, SIGNAL(taskAdded(Task)),        this, SLOT(addTask(Task)));
    connect(tm, SIGNAL(taskUpdated(Task,bool)), this, SLOT(updateTask(Task,bool)));
    connect(tm, SIGNAL(taskRemoved(TaskId)),    this, SLOT(removeTask(TaskId)));
}

QSize DateBeam::sizeHint() const
{
    return QSize(0, 25);
}

void DateBeam::enterEvent(QEvent *)
{
    mouseIn_ = true;
    update();
}

void DateBeam::mouseMoveEvent(QMouseEvent * event)
{
    hoveredCell_ = event->pos().x() / cellWidth_;
    update();

    emit dateHovered(Clock::currentDate().addDays(hoveredCell_));
}

void DateBeam::leaveEvent(QEvent *)
{
    mouseIn_ = false;
    update();

    emit dateHovered(QDate());
}

void DateBeam::paintEvent(QPaintEvent * event)
{
    const QRect cellRect(0, 0, cellWidth_, height());

    QPainter p(this);
    QFont f = p.font();

    // draw background
    p.fillRect(event->rect(), "#f8faec");
    p.setPen("#b7d24d");
    p.drawRect(event->rect().adjusted(0,0,-1,-1));
    p.setPen(Qt::black);

    const QDate today = Clock::currentDate();
    for (int i = 0; i < width()/cellWidth_ ; ++i)
    {
        const QDate curDate = today.addDays(i);

        // fill background
        if (mouseIn_ && i == hoveredCell_) p.fillRect( cellRect.translated(i*cellWidth_,0), "#9ebc20" );
        else if (curDate.dayOfWeek() >= 6) p.fillRect( cellRect.translated(i*cellWidth_,0), "#b7d24d" );

        // set pen
        if      (i == 0) p.setPen(Qt::darkBlue); // today
        else if (i == 1) p.setPen(Qt::black);    // other dates

        // set font
        f.setBold(i == 0);
        const bool hasTasks = !datesWithTasks_[curDate].isEmpty();
        f.setUnderline(hasTasks);
        f.setOverline(hasTasks);
        p.setFont(f);

        // draw text
        p.drawText(cellRect.translated(QPoint(i * cellWidth_, 0)), curDate.toString("d"),
                   QTextOption(Qt::AlignCenter));
    }

    // draw selection rect
    if (mouseIn_) {
        p.setPen(Qt::black);
        p.drawRect( cellRect.translated(hoveredCell_*cellWidth_,0).adjusted(0,0,0,-1) );
    }
}

void DateBeam::dragEnterEvent(QDragEnterEvent * event)
{
	if (!event->mimeData()->hasFormat("tingy/Task")) {
		event->ignore();
		return;
	}

	event->accept();

	mouseIn_ = true;
	update();
}

void DateBeam::dragMoveEvent(QDragMoveEvent * event)
{
    hoveredCell_ = event->pos().x() / cellWidth_;
    update();

    emit dateHovered(Clock::currentDate().addDays(hoveredCell_));
}

void DateBeam::dragLeaveEvent(QDragLeaveEvent *)
{
    mouseIn_ = false;
    update();

    emit dateHovered(QDate());
}

void DateBeam::dropEvent(QDropEvent * event)
{
	QDate plannedDate = Clock::currentDate().addDays(hoveredCell_);

	// Create a QByteArray from the mimedata associated with foo/bar
	QByteArray ba = event->mimeData()->data("tingy/Task");
	QDataStream ds(&ba, QIODevice::ReadOnly);
	Task task;
	ds >> task;

	// reset the planned date if it equals the due date
	if (task.getDueDate() == plannedDate) task.setPlannedDate(QDate());
	else                                  task.setPlannedDate(plannedDate);

	if (task.isValid()) TaskModel::instance()->updateTask(task);
}

void DateBeam::addTask(const Task & task)
{
	const QDate effDate = task.getEffectiveDate();
	if (!effDate.isValid()) return;

	datesWithTasks_[effDate] << task.getId();
	update();
}

void DateBeam::updateTask(const Task & task, bool doneChanged)
{
	Q_UNUSED(doneChanged);

	QDate oldEffDate;
	foreach (const QDate & d, datesWithTasks_.keys()) {
		if (datesWithTasks_[d].contains(task.getId())) {
			oldEffDate = d;
			break;
		}
	}

	const QDate effDate = task.getEffectiveDate();
	if (oldEffDate == effDate) return;


	datesWithTasks_[oldEffDate].remove(task.getId());
	if (effDate.isValid()) {
		datesWithTasks_[effDate] << task.getId();
	}
	update();
}

void DateBeam::removeTask(const TaskId & taskId)
{
	foreach (const QDate & d, datesWithTasks_.keys()) {
		if (datesWithTasks_[d].contains(taskId)) {
			datesWithTasks_[d].remove(taskId);
			update();
			return;
		}
	}
}
