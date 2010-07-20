#-------------------------------------------------
#
# Project created by QtCreator 2010-07-11T12:01:52
#
#-------------------------------------------------

QT += core gui

TARGET = ../tingy
TEMPLATE = app

INCLUDEPATH += . core widgets util

SOURCES += main.cpp \
        widgets/mainwindow.cpp \
    core/task.cpp \
    core/taskid.cpp \
    core/taskmodel.cpp \
    core/priority.cpp \
    widgets/tasktree.cpp \
    util/parsetimestamp.cpp \
    core/filestorage.cpp \
    widgets/tasktreeitems.cpp \
    core/effort.cpp \
    widgets/taskeditwidget.cpp \
    widgets/autocompletelineedit.cpp \
    widgets/calendarpopup.cpp \
    widgets/textpopup.cpp \
    core/clock.cpp \
util/qxtglobalshortcut.cpp \
util/qxtglobalshortcut_x11.cpp \
    widgets/quickadddialog.cpp

HEADERS  += widgets/mainwindow.h \
    core/task.h \
    core/taskid.h \
    core/taskmodel.h \
    core/priority.h \
    widgets/tasktree.h \
    util/parsetimestamp.h \
    core/filestorage.h \
    widgets/tasktreeitems.h \
    core/effort.h \
    widgets/taskeditwidget.h \
    widgets/autocompletelineedit.h \
    widgets/calendarpopup.h \
    widgets/textpopup.h \
    core/clock.h \
util/qxtglobalshortcut.h \
util/qxtglobalshortcut_p.h \
    util/qxtglobalstuff.h \
    widgets/quickadddialog.h

FORMS    += widgets/ui/mainwindow.ui \
    widgets/ui/taskeditwidget.ui \
    widgets/ui/calendarpopup.ui \
    widgets/ui/quickadddialog.ui

RESOURCES += \
    tingy.qrc
