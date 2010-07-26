#pragma once

#include <QDataStream>
#include <QString>

//
// Effort

class Effort
{
public:
    explicit Effort(quint32 minutes = 0);

    bool isNull()  const { return effortInMinutes_ == 0; }
    bool isValid() const { return !isNull(); }

    QString toString() const;
    static Effort fromString(const QString & str);

    bool operator==(const Effort & rhs) const { return effortInMinutes_ == rhs.effortInMinutes_; }
    bool operator!=(const Effort & rhs) const { return !operator==(rhs); }

    quint32 toMinutes() const { return effortInMinutes_; }

private:
    quint32 effortInMinutes_;

    friend QDataStream & operator<<( QDataStream & out, const Effort & effort);
    friend QDataStream & operator>>( QDataStream & out, Effort & effort);
};

//
// stream operators

inline QDataStream & operator<<(QDataStream & out, const Effort & effort) {
    out << effort.effortInMinutes_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Effort & effort) {
    in >> effort.effortInMinutes_;
    return in;
}
