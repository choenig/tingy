#include "priority.h"

QString Priority::toTrString() const
{
    switch (level_) {
        case High:   return "Hoch";
        case Normal: return "Normal";
        case Low:    return "Niedrig";
    }

    return QString();
}
