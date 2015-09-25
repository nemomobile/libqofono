#-------------------------------------------------
#
# Project created by QtCreator 2013-01-11T09:22:23
#
#-------------------------------------------------

QT += dbus xmlpatterns
QT -= gui
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-psabi

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

SOURCES += \
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
    qofonosimwatcher.cpp \
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
    qofonosimwatcher.h \
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

equals(QT_MAJOR_VERSION, 4): {
    headers.path = $$INSTALL_ROOT$$PREFIX/include/qofono
    qtconfig.path = $$[QT_INSTALL_PREFIX]/share/qt4/mkspecs/features
    qtconfig.files = qofono.prf
    pkgconfig.files = qofono.pc
    xmlfiles.path = $$INSTALL_ROOT$$PREFIX/include/qofono/dbus
}

equals(QT_MAJOR_VERSION, 5): {
    headers.path = $$INSTALL_ROOT$$PREFIX/include/qofono-qt5
    qtconfig.path = $$[QT_INSTALL_PREFIX]/share/qt5/mkspecs/features
    qtconfig.files = qofono-qt5.prf
    pkgconfig.files = qofono-qt5.pc
    xmlfiles.path = $$INSTALL_ROOT$$PREFIX/include/qofono-qt5/dbus
}

DBUS_INTERFACES += ofono_assisted_satellite_navigation
ofono_assisted_satellite_navigation.files = dbus/ofono_assisted_satellite_navigation.xml
ofono_assisted_satellite_navigation.header_flags = -N -c OfonoAssistedSatelliteNavigation
ofono_assisted_satellite_navigation.source_flags = -N -c OfonoAssistedSatelliteNavigation

DBUS_INTERFACES += ofono_call_barring
ofono_call_barring.files = dbus/ofono_call_barring.xml
ofono_call_barring.header_flags = -N -c OfonoCallBarring
ofono_call_barring.source_flags = -N -c OfonoCallBarring

DBUS_INTERFACES += ofono_call_forwarding
ofono_call_forwarding.files = dbus/ofono_call_forwarding.xml
ofono_call_forwarding.header_flags = -N -c OfonoCallForwarding
ofono_call_forwarding.source_flags = -N -c OfonoCallForwarding

DBUS_INTERFACES += ofono_callmeter
ofono_callmeter.files = dbus/ofono_callmeter.xml
ofono_callmeter.header_flags = -N -c OfonoCallMeter
ofono_callmeter.source_flags = -N -c OfonoCallMeter

DBUS_INTERFACES += ofono_call_settings
ofono_call_settings.files = dbus/ofono_call_settings.xml
ofono_call_settings.header_flags = -N -c OfonoCallSettings
ofono_call_settings.source_flags = -N -c OfonoCallSettings

DBUS_INTERFACES += ofono_callvolume
ofono_callvolume.files = dbus/ofono_callvolume.xml
ofono_callvolume.header_flags = -N -c OfonoCallVolume
ofono_callvolume.source_flags = -N -c OfonoCallVolume

DBUS_INTERFACES += ofono_cell_broadcast
ofono_cell_broadcast.files = dbus/ofono_cell_broadcast.xml
ofono_cell_broadcast.header_flags = -N -c OfonoCellBroadcast
ofono_cell_broadcast.source_flags = -N -c OfonoCellBroadcast

DBUS_INTERFACES += ofono_handsfree
ofono_handsfree.files = dbus/ofono_handsfree.xml
ofono_handsfree.header_flags = -N -c OfonoHandsfree
ofono_handsfree.source_flags = -N -c OfonoHandsfree

DBUS_INTERFACES += ofono_handsfree_audio_agent
ofono_handsfree_audio_agent.files = dbus/ofono_handsfree_audio_agent.xml
ofono_handsfree_audio_agent.header_flags = -N -c OfonoHandsfreeAudioAgent
ofono_handsfree_audio_agent.source_flags = -N -c OfonoHandsfreeAudioAgent

DBUS_INTERFACES += ofono_handsfree_audio_card
ofono_handsfree_audio_card.files = dbus/ofono_handsfree_audio_card.xml
ofono_handsfree_audio_card.header_flags = -N -c OfonoHandsfreeAudioCard
ofono_handsfree_audio_card.source_flags = -N -c OfonoHandsfreeAudioCard

DBUS_INTERFACES += ofono_handsfree_audio_manager
ofono_handsfree_audio_manager.files = dbus/ofono_handsfree_audio_manager.xml
ofono_handsfree_audio_manager.header_flags = -N -c OfonoHandsfreeAudioManager -i dbustypes.h
ofono_handsfree_audio_manager.source_flags = -N -c OfonoHandsfreeAudioManager

DBUS_INTERFACES += ofono_connection_context
ofono_connection_context.files = dbus/ofono_connection_context.xml
ofono_connection_context.header_flags = -N -c OfonoConnectionContext
ofono_connection_context.source_flags = -N -c OfonoConnectionContext

DBUS_INTERFACES += ofono_connection_manager
ofono_connection_manager.files = dbus/ofono_connection_manager.xml
ofono_connection_manager.header_flags = -N -c OfonoConnectionManager -i dbustypes.h
ofono_connection_manager.source_flags = -N -c OfonoConnectionManager

DBUS_INTERFACES += ofono_location_reporting
ofono_location_reporting.files = dbus/ofono_location_reporting.xml
ofono_location_reporting.header_flags = -N -c OfonoLocationReporting
ofono_location_reporting.source_flags = -N -c OfonoLocationReporting

DBUS_INTERFACES += ofono_manager
ofono_manager.files = dbus/ofono_manager.xml
ofono_manager.header_flags = -N -c OfonoManager -i dbustypes.h
ofono_manager.source_flags = -N -c OfonoManager

DBUS_INTERFACES += ofono_message
ofono_message.files = dbus/ofono_message.xml
ofono_message.header_flags = -N -c OfonoMessage
ofono_message.source_flags = -N -c OfonoMessage

DBUS_INTERFACES += ofono_message_manager
ofono_message_manager.files = dbus/ofono_message_manager.xml
ofono_message_manager.header_flags = -N -c OfonoMessageManager -i dbustypes.h
ofono_message_manager.source_flags = -N -c OfonoMessageManager

DBUS_INTERFACES += ofono_messagewaiting
ofono_messagewaiting.files = dbus/ofono_messagewaiting.xml
ofono_messagewaiting.header_flags = -N -c OfonoMessageWaiting
ofono_messagewaiting.source_flags = -N -c OfonoMessageWaiting

DBUS_INTERFACES += ofono_modem
ofono_modem.files = dbus/ofono_modem.xml
ofono_modem.header_flags = -N -c OfonoModem
ofono_modem.source_flags = -N -c OfonoModem

DBUS_INTERFACES += ofono_network_operator
ofono_network_operator.files = dbus/ofono_network_operator.xml
ofono_network_operator.header_flags = -N -c OfonoNetworkOperator
ofono_network_operator.source_flags = -N -c OfonoNetworkOperator

DBUS_INTERFACES += ofono_network_registration
ofono_network_registration.files = dbus/ofono_network_registration.xml
ofono_network_registration.header_flags = -N -c OfonoNetworkRegistration -i dbustypes.h
ofono_network_registration.source_flags = -N -c OfonoNetworkRegistration

DBUS_INTERFACES += ofono_phonebook
ofono_phonebook.files = dbus/ofono_phonebook.xml
ofono_phonebook.header_flags = -N -c OfonoPhonebook
ofono_phonebook.source_flags = -N -c OfonoPhonebook

DBUS_ADAPTORS += ofono_positioning_request_agent
ofono_positioning_request_agent.files = dbus/ofono_positioning_request_agent.xml
ofono_positioning_request_agent.header_flags = -N -c QOfonoPositioningRequestAgentAdaptor
ofono_positioning_request_agent.source_flags = -N -c QOfonoPositioningRequestAgentAdaptor

DBUS_INTERFACES += ofono_radio_settings
ofono_radio_settings.files = dbus/ofono_radio_settings.xml
ofono_radio_settings.header_flags = -N -c OfonoRadioSettings
ofono_radio_settings.source_flags = -N -c OfonoRadioSettings

DBUS_INTERFACES += ofono_simmanager
ofono_simmanager.files = dbus/ofono_simmanager.xml
ofono_simmanager.header_flags = -N -c OfonoSimManager
ofono_simmanager.source_flags = -N -c OfonoSimManager

DBUS_INTERFACES += ofono_smart_messaging
ofono_smart_messaging.files = dbus/ofono_smart_messaging.xml
ofono_smart_messaging.header_flags = -N -c OfonoSmartMessaging
ofono_smart_messaging.source_flags = -N -c OfonoSmartMessaging

DBUS_ADAPTORS += ofono_smart_messaging_agent
ofono_smart_messaging_agent.files = dbus/ofono_smart_messaging_agent.xml
ofono_smart_messaging_agent.header_flags = -N -c QOfonoSmartMessagingAgentAdaptor
ofono_smart_messaging_agent.source_flags = -N -c QOfonoSmartMessagingAgentAdaptor

DBUS_INTERFACES += ofono_supplementary_services
ofono_supplementary_services.files = dbus/ofono_supplementary_services.xml
ofono_supplementary_services.header_flags = -N -c OfonoSupplementaryServices
ofono_supplementary_services.source_flags = -N -c OfonoSupplementaryServices

DBUS_INTERFACES += ofono_text_telephony
ofono_text_telephony.files = dbus/ofono_text_telephony.xml
ofono_text_telephony.header_flags = -N -c OfonoTextTelephony
ofono_text_telephony.source_flags = -N -c OfonoTextTelephony

DBUS_INTERFACES += ofono_voice_call
ofono_voice_call.files = dbus/ofono_voice_call.xml
ofono_voice_call.header_flags = -N -c OfonoVoiceCall
ofono_voice_call.source_flags = -N -c OfonoVoiceCall

DBUS_INTERFACES += ofono_voicecallmanager
ofono_voicecallmanager.files = dbus/ofono_voicecallmanager.xml
ofono_voicecallmanager.header_flags = -N -c OfonoVoiceCallManager -i dbustypes.h
ofono_voicecallmanager.source_flags = -N -c OfonoVoiceCallManager

target.path = $$[QT_INSTALL_LIBS]
headers.files = $$PUBLIC_HEADERS

xmlfiles.files = $$XML_FILES

CONFIG += create_pc create_prl link_pkgconfig

QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_INCDIR = $$headers.path


INSTALLS += target headers qtconfig xmlfiles pkgconfig
