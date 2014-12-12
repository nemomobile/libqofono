TARGET=QOfonoQtDeclarative
TEMPLATE = lib
QT += dbus
CONFIG += plugin

QT_VERSION=$$[QT_VERSION]

equals(QT_MAJOR_VERSION, 4):{
    QT += declarative
    PLUGIN_TYPE = declarative
    LIBS += -L../src -lqofono
}

equals(QT_MAJOR_VERSION, 5): {
    QT += qml
    QT -= gui
    LIBS += -L../src -lqofono-qt5
}

SOURCES =  \ 
    qofonodeclarativeplugin.cpp \
    qofononetworkoperatorlistmodel.cpp

HEADERS = \
    qofonodeclarativeplugin.h \
    qofononetworkoperatorlistmodel.h

INCLUDEPATH += ../src

OTHER_FILES += \
    plugin.json qmldir
#plugins.qmltypes

equals(QT_MAJOR_VERSION, 4): {
    qmldir.path = $$[QT_INSTALL_IMPORTS]/MeeGo/QOfono
    target.path = $$[QT_INSTALL_IMPORTS]/MeeGo/QOfono
}

equals(QT_MAJOR_VERSION, 5): {
    qmldir.path = $$[QT_INSTALL_QML]/MeeGo/QOfono
    target.path = $$[QT_INSTALL_QML]/MeeGo/QOfono
}

qmldir.files += qmldir


INSTALLS += target qmldir
