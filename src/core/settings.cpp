#include "settings.h"

#include <QSettings>

namespace {
QString org = "tingy";
QString app = "tingy";
}


namespace Settings
{

//
// NetworkStorage

namespace NetworkStorage
{
    QString ns = "NetworkStorage/";

    QString Hostname() {
        QSettings settings(org, app);
        return settings.value(ns + "Hostname", "ftp.tingy.de").toString();
    }

    QString Username() {
        QSettings settings(org, app);
        return settings.value(ns + "Username", "tingy").toString();
    }

    QString Password() {
        QSettings settings(org, app);
        return settings.value(ns + "Password").toString(); // "Ym9VblJpcDc="
    }

    QString Taskdir() {
        QSettings settings(org, app);
        return settings.value(ns + "Taskdir", "/home/tingy/tasks").toString();
    }
}

}
