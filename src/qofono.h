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

#ifndef QOFONO_H
#define QOFONO_H

#include "qofono_global.h"

#include "dbus/ofonoassistedsatellitenavigation.h"
#include "dbus/ofonoaudiosettings.h"
#include "dbus/ofonocallbarring.h"
#include "dbus/ofonocallforwarding.h"
#include "dbus/ofonocallmeter.h"
#include "dbus/ofonocallsettings.h"
#include "dbus/ofonocallvolume.h"
#include "dbus/ofonocellbroadcast.h"
#include "dbus/ofonoconnectionmanager.h"
#include "dbus/ofonomanager.h"
#include "dbus/ofonomessagemanager.h"
#include "dbus/ofonomodem.h"
#include "dbus/ofononetworkoperator.h"
#include "dbus/ofononetworkregistration.h"
#include "dbus/ofonophonebook.h"
#include "dbus/ofonopushnotification.h"
#include "dbus/ofonosimmanager.h"
#include "dbus/ofonosmartmessaging.h"
#include "dbus/ofonosupplementaryservices.h"
#include "dbus/ofonovoicecall.h"
#include "dbus/ofonovoicecallmanager.h"

class QOFONOSHARED_EXPORT QOfono
{
public:
    QOfono();

    static QString mobileCountryCodeToAlpha2CountryCode(int mcc);
};

#endif // QOFONO_H
