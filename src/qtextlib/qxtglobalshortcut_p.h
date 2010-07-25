#pragma once

#include "qxtglobalshortcut.h"
#include <QAbstractEventDispatcher>
#include <QKeySequence>
#include <QHash>


class QxtGlobalShortcutPrivate : public QxtPrivate<QxtGlobalShortcut>
{
public:
//    Q_DECLARE_PUBLIC(QxtGlobalShortcut)
    QxtGlobalShortcutPrivate();
    ~QxtGlobalShortcutPrivate();

    bool enabled;
    Qt::Key key;
    Qt::KeyboardModifiers mods;

    bool setShortcut(const QKeySequence& shortcut);
    bool unsetShortcut();

    static bool error;
    static int ref;
    static QAbstractEventDispatcher::EventFilter prevEventFilter;
    static bool eventFilter(void* message);

private:
    static quint32 nativeKeycode(Qt::Key keycode);
    static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);

    static bool registerShortcut(quint32 nativeKey, quint32 nativeMods);
    static bool unregisterShortcut(quint32 nativeKey, quint32 nativeMods);
    static void activateShortcut(quint32 nativeKey, quint32 nativeMods);

    static QHash<QPair<quint32, quint32>, QxtGlobalShortcut*> shortcuts;
};
