/****************************************************************************
**
** Copyright (C) 2013 Lorn Potter
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSensors module of the Qt Toolkit.
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

//#include "qofonocallbarring.h"
//#include "qofonocallforwarding.h"
//#include "qofonocallmeter.h"
//#include "qofonocallsettings.h"
//#include "qofonocallvolume.h"

//#include "qofonocellbroadcast.h"
//#include "qofonomessage.h"
//#include "qofonomessagemanager.h"
//#include "qofonomessagewaiting.h"
//#include "qofonomodeminterface.h"
//#include "qofononetworkoperator.h"
//#include "qofonophonebook.h"
//#include "qofonoradiosettings.h"
//#include "qofonosimmanager.h"
//#include "qofonosmartmessaging.h"
//#include "qofonosupplementaryservices.h"
//#include "qofonovoicecall.h"
//#include "qofonovoicecallmanager.h"


void QOfonoDeclarativePlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("MeeGo.QOfono"));

    qmlRegisterType<QOfonoManager>(uri,0,2,"ModemManager");
    qmlRegisterType<QOfonoModem>(uri,0,2,"Modem");

    qmlRegisterType<QOfonoConnectionManager>(uri,0,2,"OfonoConnMan");
    qmlRegisterType<QOfonoConnectionContext>(uri,0,2,"ContextConnection");

    qmlRegisterType<QOfonoNetworkRegistration>(uri,0,2,"NetworkRegistration");

 //   qmlRegisterType<OfonoCellBroadcast>(uri,0,2,"OfonoCellBroadcast");

    //    qmlRegisterType<OfonoMessage>(uri,0,2,"Message");
    //    qmlRegisterType<OfonoMessageManager>(uri,0,2,"MessageManager");
    //    qmlRegisterType<OfonoMessageWaiting>(uri,0,2,"MessageWaiting");

    //    qmlRegisterType<OfonoCallSettings>(uri,0,2,"OfonoCallSettings");

//  qmlRegisterType<OfonoCallBarring>(uri,0,2,"OfonoCallBarring");
 //   qmlRegisterType<OfonoCallForwarding>(uri,0,2,"OfonoCallForwarding");
 //   qmlRegisterType<OfonoCallMeter>(uri,0,2,"OfonoCallMeter");



    //    qmlRegisterType<OfonoNetworkOperator>(uri,0,2,"NetworkOperator");


//    qmlRegisterType<OfonoPhonebook>(uri,0,2,"Phonebook"); //TODO

//    qmlRegisterType<OfonoSmartMessaging>(uri,0,2,"SmartMessaging");
//    qmlRegisterType<OfonoSmartMessagingAgent>(uri,0,2,"SmartMessagingAgent");

//    qmlRegisterType<OfonoSupplementaryServices>(uri,0,2,"SupplementaryServices");

//    qmlRegisterType<OfonoVoiceCall>(uri,0,2,"VoiceCall");
//    qmlRegisterType<OfonoVoiceCallManager>(uri,0,2,"VoiceCallManager");

}

void QOfonoDeclarativePlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
    Q_UNUSED(engine);
}

//Q_EXPORT_PLUGIN(QOfonoDeclarativePlugin)
Q_EXPORT_PLUGIN2(ofonomodule, QT_PREPEND_NAMESPACE(QOfonoDeclarativePlugin));
