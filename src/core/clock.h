#pragma once

#include <QObject>

class QDate;
class QDateTime;
class QTime;
class QTimer;

class Clock : public QObject
{
    Q_OBJECT
public:
    Clock();
    Clock(const QDateTime & now);
    ~Clock();

    static Clock * instance() { return instance_; }

public:
    static QDateTime currentDateTime();
    static QDate currentDate();
    static QTime currentTime();

signals:
    void dateChanged(const QDate & date);

private slots:
    void handleDateChange();

private:
    qint64 msecsTillTomorrow() const;
    void init();

private:
    qint64 offsetInMSecs_;
    QTimer * dateChangeTimer_;

private:
    static Clock * instance_;
};
