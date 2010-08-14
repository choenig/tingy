#include "settings.h"

#include <QSettings>
#include <QDir>

namespace {
QString settingsPath = Settings::dataPath() + "tingy.conf";
}


namespace Settings
{

QString dataPath() {
    return QDir::homePath() + QDir::separator() + ".tingy" + QDir::separator();
}

//
// NetworkStorage

namespace NetworkStorage
{
    QString ns = "NetworkStorage/";

    bool Enabled() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Enabled", 0).toBool();
    }

    QString Hostname() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Hostname", "ftp.tingy.de").toString();
    }

    QString Username() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Username").toString();
    }

    QString Password() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Password").toString();
    }

    QString Taskdir() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Taskdir").toString();
    }
}

}
