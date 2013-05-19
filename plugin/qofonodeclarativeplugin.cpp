/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
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


void QOfonoDeclarativePlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("MeeGo.QOfono"));

    qmlRegisterType<QOfonoManager>(uri,0,2,"OfonoManager");
    qmlRegisterType<QOfonoModem>(uri,0,2,"OfonoModem");

    qmlRegisterType<QOfonoConnectionManager>(uri,0,2,"OfonoConnMan");
    qmlRegisterType<QOfonoConnectionContext>(uri,0,2,"OfonoContextConnection");

    qmlRegisterType<QOfonoNetworkRegistration>(uri,0,2,"OfonoNetworkRegistration");
    qmlRegisterType<QOfonoNetworkOperator>(uri,0,2,"OfonoNetworkOperator");

    qmlRegisterType<QOfonoCellBroadcast>(uri,0,2,"OfonoCellBroadcast");

    qmlRegisterType<QOfonoCallBarring>(uri,0,2,"OfonoCallBarring");
    qmlRegisterType<QOfonoCallForwarding>(uri,0,2,"OfonoCallForwarding");

    qmlRegisterType<QOfonoCallSettings>(uri,0,2,"OfonoCallSettings");
    qmlRegisterType<QOfonoCallMeter>(uri,0,2,"OfonoCallMeter");

    qmlRegisterType<QOfonoMessage>(uri,0,2,"OfonoMessage");
    qmlRegisterType<QOfonoMessageManager>(uri,0,2,"OfonoMessageManager");
    qmlRegisterType<QOfonoMessageWaiting>(uri,0,2,"OfonoMessageWaiting");

    qmlRegisterType<QOfonoSimManager>(uri,0,2,"OfonoSimManager");

    qmlRegisterType<QOfonoSmartMessaging>(uri,0,2,"OfonoSmartMessaging");
    qmlRegisterType<QOfonoSmartMessagingAgent>(uri,0,2,"OfonoSmartMessagingAgent");

    qmlRegisterType<QOfonoVoiceCall>(uri,0,2,"OfonoVoiceCall");
    qmlRegisterType<QOfonoVoiceCallManager>(uri,0,2,"OfonoVoiceCallManager");

    qmlRegisterType<QOfonoRadioSettings>(uri,0,2,"OfonoRadioSettings");
    qmlRegisterType<QOfonoSupplementaryServices>(uri,0,2,"OfonoSupplementaryServices");

    qmlRegisterType<QOfonoPhonebook>(uri,0,2,"OfonoPhonebook");
    qmlRegisterType<QOfonoTextTelephony>(uri,0,2,"OfonoTextTelephony");

    qmlRegisterType<QOfonoHandsfree>(uri,0,2,"OfonoHandsfree");
    qmlRegisterType<QOfonoHandsfreeAudioAgent>(uri,0,2,"OfonoHandsfreeAudioAgent");
    qmlRegisterType<QOfonoHandsfreeAudioCard>(uri,0,2,"OfonoHandsfreeAudioCard");
    qmlRegisterType<QOfonoHandsfreeAudioManager>(uri,0,2,"OfonoHandsfreeAudioManager");

}

#ifdef QT_VERSION_4
void QOfonoDeclarativePlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
    Q_UNUSED(engine);
}
Q_EXPORT_PLUGIN2(ofonomodule, QT_PREPEND_NAMESPACE(QOfonoDeclarativePlugin))
#endif
