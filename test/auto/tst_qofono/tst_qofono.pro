#-------------------------------------------------
#
# Project created by QtCreator 2013-01-15T03:55:39
#
#-------------------------------------------------

QT       += testlib dbus

QT       -= gui

TARGET = tst_qofonotest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS += -L ../../../src -lqofono
INCLUDEPATH += ../../../src

SOURCES += tst_qofonotest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
isEmpty(PREFIX) {
  PREFIX=/usr
}

target.path = $$INSTALL_ROOT$$PREFIX/lib/libqofono/tests
INSTALLS += target

