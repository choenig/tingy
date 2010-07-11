#pragma once

class QDate;
class QString;
class QTime;

QDate parseDate(const QString & string);
QTime parseTime(const QString & string);
