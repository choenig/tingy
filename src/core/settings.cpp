/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
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

QString QuickTingyShortcut() {
    QSettings settings(settingsPath, QSettings::IniFormat);
    return settings.value("QuickTingyShortcut", "Ctrl+Alt+Home").toString();
}

namespace FileStorage
{
    QString ns = "FileStorage/";

    bool Enabled() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Enabled", 1).toBool();
    }
}

//
// NetworkStorage

namespace NetworkStorage
{
    QString ns = "NetworkStorage/";

    bool Enabled() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Enabled", false).toBool();
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
        return QByteArray::fromBase64( settings.value(ns + "Password").toByteArray() );
    }

    QString Taskdir() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Taskdir").toString();
    }
}

namespace CalDavStorage
{
    QString ns = "CalDavStorage/";

    bool Enabled() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Enabled", false).toBool();
    }

    bool UseSSL() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "UseSSL", true).toBool();
    }

    QString Hostname() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Hostname").toString();
    }

    int Port() {
           QSettings settings(settingsPath, QSettings::IniFormat);
           return settings.value(ns + "Port").toInt();
    }

    QString Username() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Username").toString();
    }

    QString Password() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return QByteArray::fromBase64( settings.value(ns + "Password").toByteArray() );
    }

    QString Taskdir() {
        QSettings settings(settingsPath, QSettings::IniFormat);
        return settings.value(ns + "Taskdir").toString();
    }
}

}
