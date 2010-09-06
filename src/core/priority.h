#pragma once

#include <QDataStream>

//
// Priority

class Priority
{
public:
    enum Level {
        High   =  1,
        Normal =  0,
        Low    = -1
    };

public:
    Priority(Level level = Normal) : level_(level) {}

    QString toTrString() const;

    int toInt() { return (int)level_; }

    bool operator==(const Priority & rhs) const { return level_ == rhs.level_; }
    bool operator!=(const Priority & rhs) const { return !operator==(rhs); }

    bool operator<(const Priority & rhs) const { return level_ < rhs.level_; }
    bool operator>(const Priority & rhs) const { return level_ > rhs.level_; }

private:
    Level level_;

    friend QDataStream & operator<<(QDataStream & out, const Priority & priority);
    friend QDataStream & operator>>(QDataStream & in, Priority & priority );
};

//
// stream operators

inline QDataStream & operator<<(QDataStream & out, const Priority & priority) {
    out << (qint32)priority.level_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Priority & priority ) {
    qint32 lvl;
    in >> lvl;
    priority.level_ = (Priority::Level)lvl;
    return in;
}
