#-------------------------------------------------
#
# Project created by QtCreator 2013-01-15T03:55:39
#
#-------------------------------------------------

QT       += testlib dbus
QT -= gui

equals(QT_MAJOR_VERSION, 4): {
    LIBS +=  -lqofono
    TARGET = tst_qofonotest
    target.path = $$INSTALL_ROOT$$PREFIX/lib/libqofono/tests
}
equals(QT_MAJOR_VERSION, 5): {
    LIBS +=  -lqofono-qt5
    TARGET = tst_qofonotest-qt5
    target.path = $$INSTALL_ROOT$$PREFIX/lib/libqofono-qt5/tests
}

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS += -L../../../src
INCLUDEPATH += ../../../src

SOURCES += tst_qofonotest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
isEmpty(PREFIX) {
  PREFIX=/usr
}

INSTALLS += target

