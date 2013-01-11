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
#include "dbus/ofonomesagemanager.h"
#include "dbus/ofonomodem.h"
#include "dbus/ofononetworkoperator.h"
#include "dbus/ofononetworkregistration.h"
#include "dbus/ofonophonebook.h"
#include "dbus/ofonopushnotification.h"
#include "dbus/ofonosimmanager.h"
#include "dbus/ofonosimtoolkit.h"
#include "dbus/ofonosmartmessaging.h"
#include "dbus/ofonosupplimentaryservices.h"
#include "dbus/ofonovoicecall.h"
#include "dbus/ofonovoicecallmanager.h"

class QOFONOSHARED_EXPORT Qofono
{
public:
    Qofono();
};

#endif // QOFONO_H
