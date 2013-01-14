TARGET=QOfonoQtDeclarative
TEMPLATE = lib
QT += declarative dbus
CONFIG += plugin
PLUGIN_TYPE = declarative

SOURCES =  \ 
    qofonodeclarativeplugin.cpp 

HEADERS = \
    qofonodeclarativeplugin.h 

INCLUDEPATH += ../src
LIBS += -L../src -lqofono

target.path = $$[QT_INSTALL_IMPORTS]/MeeGo/QOfono
qmldir.files += qmldir
qmldir.path = $$[QT_INSTALL_IMPORTS]/MeeGo/QOfono

INSTALLS += target qmldir
