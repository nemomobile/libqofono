#-------------------------------------------------
#
# Project created by QtCreator 2013-05-21T10:46:43
#
#-------------------------------------------------

QT       += testlib dbus

QT       -= gui

TARGET = tst_locationtest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
INCLUDEPATH += ../../../src

LIBS +=  -lqofono
LIBS += -L../../../src

SOURCES += tst_locationtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
