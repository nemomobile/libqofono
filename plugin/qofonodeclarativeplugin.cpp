/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**

**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
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

//#include "qofonocallbarring.h"
//#include "qofonocallforwarding.h"
//#include "qofonocallmeter.h"
//#include "qofonocallsettings.h"
//#include "qofonocallvolume.h"

//#include "qofonomessage.h"
//#include "qofonomessagemanager.h"
//#include "qofonomessagewaiting.h"
//#include "qofonophonebook.h"
//#include "qofonoradiosettings.h"
//#include "qofonosimmanager.h"
//#include "qofonosmartmessaging.h"
//#include "qofonosupplementaryservices.h"
//#include "qofonotexttelephony.h"
//#include "qofonovoicecall.h"
//#include "qofonovoicecallmanager.h"


void QOfonoDeclarativePlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("MeeGo.QOfono"));

    qmlRegisterType<QOfonoManager>(uri,0,2,"Manager");
    qmlRegisterType<QOfonoModem>(uri,0,2,"Modem");

    qmlRegisterType<QOfonoConnectionManager>(uri,0,2,"OfonoConnMan");
    qmlRegisterType<QOfonoConnectionContext>(uri,0,2,"ContextConnection");

    qmlRegisterType<QOfonoNetworkRegistration>(uri,0,2,"NetworkRegistration");
    qmlRegisterType<QOfonoNetworkOperator>(uri,0,2,"NetworkOperator");

    qmlRegisterType<QOfonoCellBroadcast>(uri,0,2,"CellBroadcast");

    //    qmlRegisterTypeQ<OfonoCallSettings>(uri,0,2,"CallSettings");
    //  qmlRegisterType<QOfonoCallBarring>(uri,0,2,"CallBarring");
    //   qmlRegisterType<QOfonoCallForwarding>(uri,0,2,"CallForwarding");
    //   qmlRegisterType<QOfonoCallMeter>(uri,0,2,"CallMeter");

    //    qmlRegisterType<QOfonoVoiceCall>(uri,0,2,"VoiceCall");
    //    qmlRegisterType<QOfonoVoiceCallManager>(uri,0,2,"VoiceCallManager");

    //    qmlRegisterType<QOfonoRadioSettings>(uri,0,2,"RadioSettings");
    //    qmlRegisterType<QOfonoSimManager>(uri,0,2,"SimManager");
    //    qmlRegisterType<QOfonoSimToolkit>(uri,0,2,"SimToolkit");
    //    qmlRegisterType<QOfonoTextTelephony>(uri,0,2,"TextTelephony");

    //    qmlRegisterType<QOfonoMessage>(uri,0,2,"Message");
    //    qmlRegisterType<QOfonoMessageManager>(uri,0,2,"MessageManager");
    //    qmlRegisterType<QOfonoMessageWaiting>(uri,0,2,"MessageWaiting");


//    qmlRegisterType<QOfonoPhonebook>(uri,0,2,"Phonebook"); //TODO

//    qmlRegisterType<QOfonoSmartMessaging>(uri,0,2,"SmartMessaging");
//    qmlRegisterType<QOfonoSmartMessagingAgent>(uri,0,2,"SmartMessagingAgent");

//    qmlRegisterType<QOfonoSupplementaryServices>(uri,0,2,"SupplementaryServices");


}

void QOfonoDeclarativePlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
    Q_UNUSED(engine);
}

//Q_EXPORT_PLUGIN(QOfonoDeclarativePlugin)
Q_EXPORT_PLUGIN2(ofonomodule, QT_PREPEND_NAMESPACE(QOfonoDeclarativePlugin));
