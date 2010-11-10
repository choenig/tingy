#-------------------------------------------------
#
# Project created by QtCreator 2010-07-11T12:01:52
#
#-------------------------------------------------

include(../config.pri)

QT += core gui network

TARGET = ../tingy
TEMPLATE = app

INCLUDEPATH += . core widgets util qtextlib

SOURCES += \
    core/clock.cpp \
    core/effort.cpp \
    core/ftp.cpp \
    core/priority.cpp \
    core/settings.cpp \
    core/task.cpp \
    core/taskid.cpp \
    core/taskmodel.cpp \
    main.cpp \
    storage/filestorage.cpp \
    storage/networkstorage.cpp \
    storage/storageengine.cpp \
    util/log.cpp \
    util/parsetimestamp.cpp \
    widgets/autocompletelineedit.cpp \
    widgets/calendarpopup.cpp \
    widgets/datebeam.cpp \
    widgets/lineedit.cpp \
    widgets/mainwindow.cpp \
    widgets/quickadddialog.cpp \
    widgets/taskeditwidget.cpp \
    widgets/tasktree.cpp \
    widgets/tasktreeitems.cpp \
    widgets/textpopup.cpp \
    storage/caldavstorage.cpp

HEADERS  += \
    core/clock.h \
    core/effort.h \
    core/ftp.h \
    core/priority.h \
    core/settings.h \
    core/task.h \
    core/taskid.h \
    core/taskmodel.h \
    storage/filestorage.h \
    storage/networkstorage.h \
    storage/storageengine.h \
    util/log.h \
    util/parsetimestamp.h \
    util/util.h \
    widgets/autocompletelineedit.h \
    widgets/calendarpopup.h \
    widgets/datebeam.h \
    widgets/lineedit.h \
    widgets/mainwindow.h \
    widgets/quickadddialog.h \
    widgets/taskeditwidget.h \
    widgets/tasktree.h \
    widgets/tasktreeitems.h \
    widgets/textpopup.h \
    storage/caldavstorage.h

FORMS += \
widgets/ui/calendarpopup.ui \
widgets/ui/mainwindow.ui \
widgets/ui/quickadddialog.ui \
widgets/ui/taskeditwidget.ui

RESOURCES += \
    tingy.qrc

QMAKE_LIBDIR += $$PWD/qtextlib
LIBS += -lqtextlib
win32:RC_FILE = images/tingy.rc
