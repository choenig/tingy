#pragma once

#include <QtGlobal>

class QDate;
class QString;

QDate parseDate(const QString & string);
qint32 parseYWDHMTime(const QString & string);

