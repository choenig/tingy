#include "effort.h"

#include <QStringList>

Effort::Effort()
    : effortInMinutes_(0)
{
}

QString Effort::toString() const
{
    const int d = effortInMinutes_ / (60 * 24);
    const int h = effortInMinutes_ % (60 * 24) / 60;
    const int m = effortInMinutes_ %             60;

    QStringList retval;
    if (d > 0) retval << QString("%1d").arg(d);
    if (h > 0) retval << QString("%1h").arg(h);
    if (m > 0) retval << QString("%1m").arg(m);
    return retval.join(" ");
}

Effort Effort::fromMinutes(quint32 minutes)
{
    Effort retval;
    retval.effortInMinutes_ = minutes;
    return retval;
}