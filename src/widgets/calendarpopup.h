#pragma once

#include <QWidget>
#include <QDate>

class QLabel;


namespace Ui { class CalendarPopup; }

class CalendarPopup : public QWidget
{
    Q_OBJECT

public:
    CalendarPopup(QWidget *parent = 0);
    ~CalendarPopup();

    void reset();

signals:
    void dateSelected(const QDate & date);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void keyPressEvent(QKeyEvent * keyEvent);

private slots:
    void updateSelectedDate(const QDate & selectedDate);

private:
    void updateVisibleDates(const QDate & dateInFirstRow);
    void moveSelectedIndex(int days);
    QLabel * getDateLabel(QPoint pos);
    QLabel * getKWLable(QPoint pos);
    QLabel * getDayOfWeekLable(QPoint pos);

private:
    QPoint selectedIdx_;
    QDate  selectedDate_;
    QDate currentStartDate_;

private:
    Ui::CalendarPopup *ui;
};
