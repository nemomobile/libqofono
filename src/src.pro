#-------------------------------------------------
#
# Project created by QtCreator 2013-01-11T09:22:23
#
#-------------------------------------------------

QT       += dbus

include(version.pri)

QT       -= gui

TARGET = qofono
TEMPLATE = lib

DEFINES += QOFONO_LIBRARY

include(dbus/dbus.pro)

isEmpty(PREFIX) {
  PREFIX=/usr
}

OTHER_FILES += \
    dbus/ofono_assisted_satellite_navigation.xml \
    dbus/ofono_audio_settings.xml \
    dbus/ofono_call_barring.xml \
    dbus/ofono_call_forwarding.xml \
    dbus/ofono_callmeter.xml \
    dbus/ofono_call_settings.xml \
    dbus/ofono_callvolume.xml \
    dbus/ofono_cell_broadcast.xml \
    dbus/ofono_connection_context.xml \
    dbus/ofono_connection_manager.xml \
    dbus/ofono_manager.xml \
    dbus/ofono_message_manager.xml \
    dbus/ofono_messagewaiting.xml \
    dbus/ofono_modem.xml \
    dbus/ofono_network_operator.xml \
    dbus/ofono_network_registration.xml \
    dbus/ofono_phonebook.xml \
    dbus/ofono_push_notification.xml \
    dbus/ofono_simmanager.xml \
    dbus/ofono_simtoolkit.xml \
    dbus/ofono_smart_messaging.xml \
    dbus/ofono_supplementary_services.xml \
    dbus/ofono_voice_call.xml \
    dbus/ofono_voicecallmanager.xml \
    version.pri \
    qofono.prf




INSTALLS += target

DBUS_FILES +=\
    dbus/ofonoassistedsatellitenavigation\
    dbus/ofonoaudiosettings\
    dbus/ofonocallbarring\
    dbus/ofonocallforwarding\
    dbus/ofonocallmeter\
    dbus/ofonocallsettings\
    dbus/ofonocallvolume\
    dbus/ofonocellbroadcast\
    dbus/ofonoconnectioncontext\
    dbus/ofonoconnectionmanager\
    dbus/ofonomanager\
    dbus/ofonomesagemanager\
    dbus/ofonomodem\
    dbus/ofononetworkoperator\
    dbus/ofononetworkregistration\
    dbus/ofonophonebook\
    dbus/ofonopushnotification\
    dbus/ofonosimmanager\
    dbus/ofonosimtoolkit\
    dbus/ofonosmartmessaging\
    dbus/ofonosupplimentaryservices\
    dbus/ofonovoicecall\
    dbus/ofonovoicecallmanager

for(s,DBUS_FILES) {
    DBUS_HEADERS += $${s}.h
    DBUS_SOURCES += $${s}.cpp
}

SOURCES += $$DBUS_SOURCES\
        qofono.cpp \
    qofonomanager.cpp \
    qofonomodem.cpp \
    qofonoconnectionmanager.cpp \
    qofonoconnectioncontext.cpp \
    qofononetworkregistration.cpp

PUBLIC_HEADERS += $$DBUS_HEADERS\
        qofono.h\
        qofono_global.h

HEADERS += $$PUBLIC_HEADERS \
    qofonomanager.h \
    dbustypes.h \
    qofonomodem.h \
    qofonoconnectionmanager.h \
    qofonoconnectioncontext.h \
    qofononetworkregistration.h

target.path = $$INSTALL_ROOT$$PREFIX/lib
headers.files = $$PUBLIC_HEADERS
headers.path = $$INSTALL_ROOT$$PREFIX/include/qofono

CONFIG += create_pc create_prl

QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_INCDIR = $$headers.path

qtconfig.path = $$[QT_INSTALL_PREFIX]/share/qt4/mkspecs/features
qtconfig.files = qofono.prf

INSTALLS += target headers qtconfig


