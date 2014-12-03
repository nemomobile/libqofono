/****************************************************************************
**
** Copyright (C) 2014 Jolla Ltd.
** Contact: slava.monich@jolla.com
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

#include "dbustypes.h"

QDBusArgument &operator<<(QDBusArgument &arg, const ObjectPathProperties &props)
{
    arg.beginStructure();
    arg << props.path << props.properties;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, ObjectPathProperties &props)
{
    arg.beginStructure();
    arg >> props.path >> props.properties;
    arg.endStructure();
    return arg;
}

void QOfonoDbusTypes::registerObjectPathProperties()
{
    qDBusRegisterMetaType<ObjectPathProperties>();
    qDBusRegisterMetaType<ObjectPathPropertiesList>();
    qRegisterMetaType<ObjectPathProperties>("ObjectPathProperties");
    qRegisterMetaType<ObjectPathPropertiesList>("ObjectPathPropertiesList");
}
