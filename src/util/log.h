#pragma once

#include <QDateTime>
#include <QMap>

namespace Log {
enum Severity {
    Trace   = 0,
    Debug   = 2,
    Info    = 5,
    Warning = 7,
    Error   = 8,
    Fatal   = 9
};
}

class LogLine
{
public:
    LogLine(const char * file, int line);
    ~LogLine();

    LogLine & operator()(Log::Severity severity);

    template <class T>
    LogLine & operator<<(const T & t) { return operator <<((QString)t.toString()); }

    LogLine & operator<<(const QString & str);
    LogLine & operator<<(const char    * str);
    LogLine & operator<<(int i);
    LogLine & operator<<(bool b);

private:
    QDateTime timestamp_;
    Log::Severity severity_;
    QString  file_;
    int       line_;
    QString   string_;
};

#define log LogLine(__FILE__, __LINE__)

namespace Log {
    void dumpLog();
}
