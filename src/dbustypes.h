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

#ifndef DBUSTYPES_H
#define DBUSTYPES_H

#include <QtDBus>
#include <QVariant>

struct ObjectPathProperties
{
    QDBusObjectPath path;
    QVariantMap properties;
};

typedef QList<QDBusObjectPath> ObjectPathList;
typedef QList<ObjectPathProperties> ObjectPathPropertiesList;
Q_DECLARE_METATYPE(ObjectPathProperties)
Q_DECLARE_METATYPE(ObjectPathPropertiesList)

QDBusArgument &operator<<(QDBusArgument &, const ObjectPathProperties &);
const QDBusArgument &operator>>(const QDBusArgument &, ObjectPathProperties &);

namespace QOfonoDbusTypes {
    void registerObjectPathProperties();
}

// Deprecated, left in for ABI compatibility
struct OfonoPathProps { QDBusObjectPath path; QVariantMap properties; };
typedef QList<OfonoPathProps> QArrayOfPathProps;
Q_DECLARE_METATYPE(OfonoPathProps)
Q_DECLARE_METATYPE (QArrayOfPathProps)
QDBusArgument &operator<<(QDBusArgument &, const OfonoPathProps &);
const QDBusArgument &operator>>(const QDBusArgument &, OfonoPathProps &);

#endif // DBUSTYPES_H
