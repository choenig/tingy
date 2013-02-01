#-------------------------------------------------
#
# Project created by QtCreator 2010-07-11T12:01:52
#
#-------------------------------------------------

include(../../config.pri)

TARGET = qtextlib
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += . qtextlib

SOURCES += \
    qxtglobalshortcut.cpp

unix:SOURCES  += qxtglobalshortcut_x11.cpp
win32:SOURCES += qxtglobalshortcut_win.cpp

HEADERS  += \
    qxtglobal.h \
    qxtglobalshortcut.h \
    qxtglobalshortcut_p.h
