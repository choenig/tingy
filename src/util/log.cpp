#include "log.h"

#include <core/clock.h>

#include <QDebug>
#include <QMap>
#include <QMutableMapIterator>

//
// Logger

class Logger
{
public:
    void add(const QDateTime & now, Log::Severity severity, const QString & file, int line, const QString & msg, bool print = false)
    {
        QString logStr = "| " + QString::number((int)severity)
                + " | " + file.leftJustified(24, ' ', true) + " " + QString::number(line).rightJustified(4)
                + " |"  + msg;
        logBuffer[now] = logStr;

        if (print) {
            qDebug() << now.toString("dd.MM.yyyy hh:mm:ss.zzz") << logStr;
        }

        // clean up: remove all entries that are older then two days
        const QDateTime past = now.addDays(-2);
        QMutableMapIterator<QDateTime, QString> it(logBuffer);
        while (it.hasNext() && it.next().key() < past) it.remove();
    }

    void dumpLog()
    {
        QTextStream out(stdout);
        out << "================================================================================" << endl;
        out << "================================  DUMPING LOG  =================================" << endl;
        out << "================================================================================" << endl;
        QMapIterator<QDateTime, QString> it(logBuffer);
        while (it.hasNext()) {
            it.next();
            out << it.key().toString("dd.MM.yyyy hh:mm:ss.zzz") << " " << it.value() << endl;
        }
    }
    QMap<QDateTime, QString> logBuffer;
};
Q_GLOBAL_STATIC(Logger, logger);

//
// Log::dumpLog

namespace Log {
    void dumpLog() {
        logger()->dumpLog();
    }
}

//
// LogLine

LogLine::LogLine(const char * file, int line)
    : timestamp_(Clock::currentDateTime()), severity_(Log::Debug), file_(file), line_(line)
{
}

LogLine::~LogLine()
{
    logger()->add(timestamp_, severity_, file_, line_, string_, true);
}

LogLine & LogLine::operator()(Log::Severity severity)
{
    severity_ = severity;
    return *this;
}

LogLine & LogLine::operator<<(const QString & str)
{
    string_ += " \"" + str + "\"";
    return *this;
}

LogLine & LogLine::operator<<(const char * str)
{
    string_ += " \"" + QString::fromAscii(str) + "\"";
    return *this;
}

LogLine & LogLine::operator<<(int i)
{
    string_ += " " + QString::number(i);
    return *this;
}

LogLine & LogLine::operator<<(bool b)
{
    return operator<<(b ? "true" : "false");
}
