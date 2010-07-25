#pragma once

#include <core/taskid.h>

#include <QDate>
#include <QMap>
#include <QSet>
#include <QWidget>

class Task;

class DateBeam : public QWidget
{
    Q_OBJECT
public:
    DateBeam(QWidget *parent = 0);

public:
    virtual QSize sizeHint() const;

signals:
    void dateHovered(const QDate & date);

protected:
    virtual void enterEvent(QEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void paintEvent(QPaintEvent * event);

    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void dropEvent(QDropEvent *);

private slots:
    void addTask(const Task & task);
    void updateTask(const Task & task);
    void removeTask(const TaskId & taskId);

private:
    int cellWidth_;
    int hoveredCell_;
    bool mouseIn_;
    QMap<QDate, QSet<TaskId> > datesWithTasks_;
};
