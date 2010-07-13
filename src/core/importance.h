#pragma once

#include <QDataStream>

class Importance
{
public:
    enum Level {
        Low = -1,
        Normal = 0,
        High = 1
    };

public:
    Importance(Level level = Normal) : level_(level) {}

    operator int() { return (int)level_; }

    bool operator==(const Importance & rhs) const { return level_ == rhs.level_; }
    bool operator!=(const Importance & rhs) const { return !operator==(rhs); }

private:
    Level level_;

    friend QDataStream & operator<<(QDataStream & out, const Importance & importance);
    friend QDataStream & operator>>(QDataStream & in, Importance & importance );
};

inline QDataStream & operator<<(QDataStream & out, const Importance & importance) {
    out << (qint32)importance.level_;
    return out;
}
inline QDataStream & operator>>(QDataStream & in, Importance & importance ) {
    qint32 lvl;
    in >> lvl;
    importance.level_ = (Importance::Level)lvl;
    return in;
}
