#pragma once

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

    bool operator==(const Importance & rhs) const { return level_ == rhs.level_; }
    bool operator!=(const Importance & rhs) const { return !operator==(rhs); }

private:
        Level level_;

};
