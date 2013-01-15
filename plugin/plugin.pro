TARGET=QOfonoQtDeclarative
TEMPLATE = lib
QT += dbus
CONFIG += plugin

QT_VERSION=$$[QT_VERSION]

contains( QT_VERSION, "^4.*" ) {
    DEFINES += QT_VERSION_4
    QT += declarative
    PLUGIN_TYPE = declarative
} else {
    QT += quick
}
SOURCES =  \ 
    qofonodeclarativeplugin.cpp 

HEADERS = \
    qofonodeclarativeplugin.h 

INCLUDEPATH += ../src
LIBS += -L../src -lqofono

OTHER_FILES += \
    plugin.json qmldir
#plugins.qmltypes

target.path = $$[QT_INSTALL_IMPORTS]/MeeGo/QOfono
qmldir.files += qmldir
qmldir.path = $$[QT_INSTALL_IMPORTS]/MeeGo/QOfono

INSTALLS += target qmldir
