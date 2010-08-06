#pragma once

#include <QDateTime>
#include <QMap>

class LogLine
{
public:
    LogLine(const char * file, int line);
    ~LogLine();

    LogLine & operator<<(const QString & str);
    LogLine & operator<<(const char    * str);
    LogLine & operator<<(int i);

private:
    QString  file_;
    int       line_;
    QDateTime timestamp_;
    QString   string_;
};

#define log LogLine(__FILE__, __LINE__)

namespace Log {
    void dumpLog();
}
