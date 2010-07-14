#pragma once

class QDate;
class QString;
class Effort;

QDate parseDate(const QString & string);
Effort parseEffort(const QString & string);
