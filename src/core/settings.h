#pragma once

class QString;

namespace Settings
{
    QString dataPath();

    namespace FileStorage
    {
        bool Enabled();
    }

    namespace NetworkStorage
    {
        bool    Enabled();
        QString Hostname();
        QString Username();
        QString Password();
        QString Taskdir();
    }

    namespace CalDavStorage
    {
        bool    Enabled();
        bool    UseSSL();
        QString Hostname();
        int     Port();
        QString Username();
        QString Password();
        QString Taskdir();
    }
}
