/****************************************************************************
**
** Copyright (C) 2013-2014 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "qofonodeclarativeplugin.h"

#include "qofonomanager.h"
#include "qofonomodem.h"
#include "qofonoconnectionmanager.h"
#include "qofonoconnectioncontext.h"
#include "qofononetworkregistration.h"

#include "qofononetworkoperator.h"
#include "qofonocellbroadcast.h"

#include "qofonocallbarring.h"
#include "qofonocallforwarding.h"

#include "qofonocallmeter.h"
#include "qofonocallsettings.h"
#include "qofonocallvolume.h"

#include "qofonomessage.h"
#include "qofonomessagemanager.h"
#include "qofonomessagewaiting.h"

#include "qofonosimmanager.h"

#include "qofonophonebook.h"

#include "qofonosmartmessaging.h"
#include "qofonosmartmessagingagent.h"

#include "qofonovoicecall.h"
#include "qofonovoicecallmanager.h"

#include "qofonoradiosettings.h"

#include "qofonosupplementaryservices.h"
#include "qofonophonebook.h"

#include "qofonotexttelephony.h"
#include "qofonohandsfree.h"
#include "qofonohandsfreeaudioagent.h"
#include "qofonohandsfreeaudiocard.h"
#include "qofonohandsfreeaudiomanager.h"

#include "qofonoassistedsatellitenavigation.h"
#include "qofonopositioningrequestagent.h"
#include "qofonolocationreporting.h"

#include "qofononetworkoperatorlistmodel.h"

void QOfonoDeclarativePlugin::registerTypes(const char *uri, int major, int minor)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("MeeGo.QOfono"));

    qmlRegisterType<QOfonoManager>(uri,major,minor,"OfonoManager");
    qmlRegisterType<QOfonoModem>(uri,major,minor,"OfonoModem");

    qmlRegisterType<QOfonoConnectionManager>(uri,major,minor,"OfonoConnMan");
    qmlRegisterType<QOfonoConnectionContext>(uri,major,minor,"OfonoContextConnection");

    qmlRegisterType<QOfonoNetworkRegistration>(uri,major,minor,"OfonoNetworkRegistration");
    qmlRegisterType<QOfonoNetworkOperator>(uri,major,minor,"OfonoNetworkOperator");

    qmlRegisterType<QOfonoCellBroadcast>(uri,major,minor,"OfonoCellBroadcast");

    qmlRegisterType<QOfonoCallBarring>(uri,major,minor,"OfonoCallBarring");
    qmlRegisterType<QOfonoCallForwarding>(uri,major,minor,"OfonoCallForwarding");

    qmlRegisterType<QOfonoCallSettings>(uri,major,minor,"OfonoCallSettings");
    qmlRegisterType<QOfonoCallMeter>(uri,major,minor,"OfonoCallMeter");

    qmlRegisterType<QOfonoMessage>(uri,major,minor,"OfonoMessage");
    qmlRegisterType<QOfonoMessageManager>(uri,major,minor,"OfonoMessageManager");
    qmlRegisterType<QOfonoMessageWaiting>(uri,major,minor,"OfonoMessageWaiting");

    qmlRegisterType<QOfonoSimManager>(uri,major,minor,"OfonoSimManager");

    qmlRegisterType<QOfonoSmartMessaging>(uri,major,minor,"OfonoSmartMessaging");
    qmlRegisterType<QOfonoSmartMessagingAgent>(uri,major,minor,"OfonoSmartMessagingAgent");

    qmlRegisterType<QOfonoVoiceCall>(uri,major,minor,"OfonoVoiceCall");
    qmlRegisterType<QOfonoVoiceCallManager>(uri,major,minor,"OfonoVoiceCallManager");

    qmlRegisterType<QOfonoRadioSettings>(uri,major,minor,"OfonoRadioSettings");
    qmlRegisterType<QOfonoSupplementaryServices>(uri,major,minor,"OfonoSupplementaryServices");

    qmlRegisterType<QOfonoPhonebook>(uri,major,minor,"OfonoPhonebook");
    qmlRegisterType<QOfonoTextTelephony>(uri,major,minor,"OfonoTextTelephony");

    qmlRegisterType<QOfonoHandsfree>(uri,major,minor,"OfonoHandsfree");
    qmlRegisterType<QOfonoHandsfreeAudioAgent>(uri,major,minor,"OfonoHandsfreeAudioAgent");
    qmlRegisterType<QOfonoHandsfreeAudioCard>(uri,major,minor,"OfonoHandsfreeAudioCard");
    qmlRegisterType<QOfonoHandsfreeAudioManager>(uri,major,minor,"OfonoHandsfreeAudioManager");

    qmlRegisterType<QOfonoAssistedSatelliteNavigation>(uri,major,minor,"OfonoAssistedSatelliteNavigation");
    qmlRegisterType<QOfonoPositioningRequestAgent>(uri,major,minor,"OfonoPositioningRequestAgent");
    qmlRegisterType<QOfonoLocationReporting>(uri,major,minor,"OfonoLocationReporting");

    qmlRegisterType<QOfonoNetworkOperatorListModel>(uri,major,minor,"OfonoNetworkOperatorListModel");
}

void QOfonoDeclarativePlugin::registerTypes(const char *uri)
{
    registerTypes(uri, 0, 2);
}

#if QT_VERSION < 0x050000
void QOfonoDeclarativePlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
    Q_UNUSED(engine);
}
Q_EXPORT_PLUGIN2(ofonomodule, QT_PREPEND_NAMESPACE(QOfonoDeclarativePlugin))
#endif
