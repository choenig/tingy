#pragma once

class QString;

namespace Settings
{
    QString dataPath();

    namespace NetworkStorage
    {
        bool Enabled();
        QString Hostname();
        QString Username();
        QString Password();
        QString Taskdir();
    }
}
