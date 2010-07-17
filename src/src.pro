#-------------------------------------------------
#
# Project created by QtCreator 2010-07-11T12:01:52
#
#-------------------------------------------------

QT       += core gui

TARGET = myTasks
TEMPLATE = app

INCLUDEPATH += . core widgets

SOURCES += main.cpp\
        widgets/mainwindow.cpp \
    widgets/filterlineedit.cpp \
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
    widgets/textpopup.cpp

HEADERS  += widgets/mainwindow.h \
    widgets/filterlineedit.h \
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
    widgets/textpopup.h

FORMS    += widgets/ui/mainwindow.ui \
    widgets/ui/taskeditwidget.ui \
    widgets/ui/calendarpopup.ui

RESOURCES += \
    mytasks.qrc
