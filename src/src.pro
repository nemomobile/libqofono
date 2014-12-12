#-------------------------------------------------
#
# Project created by QtCreator 2013-01-11T09:22:23
#
#-------------------------------------------------

QT += dbus xmlpatterns
QMAKE_CXXFLAGS += -Wno-unused-parameter

include(version.pri)

equals(QT_MAJOR_VERSION, 4): {
    TARGET = qofono
    pkgconfig.path = $$[QT_INSTALL_LIBS]/pkgconfig
}

equals(QT_MAJOR_VERSION, 5): {
    TARGET = qofono-qt5
    pkgconfig.path = $$[QT_INSTALL_LIBS]/pkgconfig-qt5
}

TEMPLATE = lib

DEFINES += QOFONO_LIBRARY

include(dbus/dbus.pro)

isEmpty(PREFIX) {
  PREFIX=/usr
}

XML_FILES += \
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
    dbus/ofono_handsfree.xml \
    dbus/ofono_handsfree_audio_agent.xml \
    dbus/ofono_handsfree_audio_card.xml \
    dbus/ofono_handsfree_audio_manager.xml \
    dbus/ofono_manager.xml \
    dbus/ofono_message.xml \
    dbus/ofono_message_manager.xml \
    dbus/ofono_messagewaiting.xml \
    dbus/ofono_modem.xml \
    dbus/ofono_network_operator.xml \
    dbus/ofono_network_registration.xml \
    dbus/ofono_phonebook.xml \
    dbus/ofono_push_notification.xml \
    dbus/ofono_radio_settings.xml \
    dbus/ofono_simmanager.xml \
    dbus/ofono_smart_messaging.xml \
    dbus/ofono_smart_messaging_agent.xml \
    dbus/ofono_supplementary_services.xml \
    dbus/ofono_text_telephony.xml \
    dbus/ofono_voice_call.xml \
    dbus/ofono_voicecallmanager.xml


OTHER_FILES += $$XML_FILES\
    version.pri \
    qofono.prf \
    qofono-qt5.prf

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
    dbus/ofonohandsfree\
    dbus/ofonohandsfreeaudioagent\
    dbus/ofonohandsfreeaudiocard\
    dbus/ofonohandsfreeaudiomanager\
    dbus/ofonolocationreporting\
    dbus/ofonomanager\
    dbus/ofonomessage \
    dbus/ofonomessagemanager\
    dbus/ofonomessagewaiting\
    dbus/ofonomodem\
    dbus/ofononetworkoperator\
    dbus/ofononetworkregistration\
    dbus/ofonophonebook\
    dbus/ofonopositioningrequestagent\
    dbus/ofonopushnotification\
    dbus/ofonoradiosettings\
    dbus/ofonosimmanager\
    dbus/ofonosmartmessaging\
    dbus/ofonosmartmessagingagent\
    dbus/ofonosupplementaryservices\
    dbus/ofonotexttelephony\
    dbus/ofonovoicecall\
    dbus/ofonovoicecallmanager

for(s,DBUS_FILES) {
    DBUS_HEADERS += $${s}.h
    DBUS_SOURCES += $${s}.cpp
}

SOURCES += $$DBUS_SOURCES\
    qofono.cpp \
    qofonodbustypes.cpp \
    qofonoobject.cpp \
    qofonomanager.cpp \
    qofonomodem.cpp \
    qofonomodeminterface.cpp \
    qofonomodeminterface2.cpp \
    qofonoassistedsatellitenavigation.cpp \
    qofonoconnectionmanager.cpp \
    qofonoconnectioncontext.cpp \
    qofononetworkregistration.cpp \
    qofononetworkoperator.cpp \
    qofonocellbroadcast.cpp \
    qofonocallbarring.cpp \
    qofonocallforwarding.cpp \
    qofonocallmeter.cpp \
    qofonocallsettings.cpp \
    qofonocallvolume.cpp \
    qofonohandsfree.cpp \
    qofonohandsfreeaudioagent.cpp \
    qofonohandsfreeaudiocard.cpp \
    qofonohandsfreeaudiomanager.cpp\
    qofonolocationreporting.cpp \
    qofonomessage.cpp \
    qofonomessagemanager.cpp \
    qofonomessagewaiting.cpp \
    qofonosimmanager.cpp \
    qofonophonebook.cpp \
    qofonopositioningrequestagent.cpp \
    qofonosmartmessaging.cpp \
    qofonosmartmessagingagent.cpp \
    qofonovoicecall.cpp \
    qofonovoicecallmanager.cpp \
    qofonoradiosettings.cpp \
    qofonosupplementaryservices.cpp \
    qofonotexttelephony.cpp

PUBLIC_HEADERS += \
    qofono.h \
    qofono_global.h \
    qofonoobject.h \
    qofonomanager.h \
    dbustypes.h \
    qofonomodem.h \
    qofonomodeminterface.h \
    qofonomodeminterface2.h \
    qofonoassistedsatellitenavigation.h \
    qofonoconnectionmanager.h \
    qofonoconnectioncontext.h \
    qofononetworkregistration.h \
    qofononetworkoperator.h \
    qofonocellbroadcast.h \
    qofonocallbarring.h \
    qofonocallforwarding.h \
    qofonolocationreporting.h \
    qofonocallmeter.h \
    qofonocallsettings.h \
    qofonocallvolume.h \
    qofonohandsfree.h \
    qofonohandsfreeaudioagent.h \
    qofonohandsfreeaudiocard.h \
    qofonohandsfreeaudiomanager.h\
    qofonomessage.h \
    qofonomessagemanager.h \
    qofonomessagewaiting.h \
    qofonosimmanager.h \
    qofonophonebook.h \
    qofonopositioningrequestagent.h \
    qofonosmartmessaging.h \
    qofonosmartmessagingagent.h \
    qofonovoicecall.h \
    qofonovoicecallmanager.h \
    qofonoradiosettings.h \
    qofonosupplementaryservices.h \
    qofonotexttelephony.h

HEADERS += \
    $$DBUS_HEADERS \
    $$PUBLIC_HEADERS \
    qofonoutils_p.h

    QT -= gui
equals(QT_MAJOR_VERSION, 4): {
    headers.path = $$INSTALL_ROOT$$PREFIX/include/qofono
    qtconfig.path = $$[QT_INSTALL_PREFIX]/share/qt4/mkspecs/features
    qtconfig.files = qofono.prf
    pkgconfig.files = qofono.pc
    dbusheaders.path = $$INSTALL_ROOT$$PREFIX/include/qofono/dbus
    xmlfiles.path = $$INSTALL_ROOT$$PREFIX/include/qofono/dbus
}

equals(QT_MAJOR_VERSION, 5): {
    headers.path = $$INSTALL_ROOT$$PREFIX/include/qofono-qt5
    qtconfig.path = $$[QT_INSTALL_PREFIX]/share/qt5/mkspecs/features
    qtconfig.files = qofono-qt5.prf
    pkgconfig.files = qofono-qt5.pc
    dbusheaders.path = $$INSTALL_ROOT$$PREFIX/include/qofono-qt5/dbus
    xmlfiles.path = $$INSTALL_ROOT$$PREFIX/include/qofono-qt5/dbus
}

target.path = $$[QT_INSTALL_LIBS]
headers.files = $$PUBLIC_HEADERS

dbusheaders.files = $$DBUS_HEADERS

xmlfiles.files = $$XML_FILES

CONFIG += create_pc create_prl link_pkgconfig

QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_INCDIR = $$headers.path


INSTALLS += target headers qtconfig dbusheaders xmlfiles pkgconfig


