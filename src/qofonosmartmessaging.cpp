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

#include "qofonosmartmessaging.h"
#include "dbus/ofonosmartmessaging.h"

QOfonoSmartMessaging::QOfonoSmartMessaging(QObject *parent) :
    QOfonoModemInterface2(OfonoSmartMessaging::staticInterfaceName(), parent)
{
}

QOfonoSmartMessaging::~QOfonoSmartMessaging()
{
}

QDBusAbstractInterface *QOfonoSmartMessaging::createDbusInterface(const QString &path)
{
    return new OfonoSmartMessaging("org.ofono", path, QDBusConnection::systemBus(), this);
}

QDBusObjectPath QOfonoSmartMessaging::sendAppointment(const QString &toPhoneNumber, const QByteArray &appointment)
{
    OfonoSmartMessaging *iface = (OfonoSmartMessaging*)dbusInterface();
    if (iface) {
        // BLOCKING CALL!
        QDBusPendingReply<QDBusObjectPath> returnPath = iface->SendAppointment(toPhoneNumber, appointment);
        returnPath.waitForFinished();
        return returnPath.value();
    }
    return QDBusObjectPath();
}

QDBusObjectPath QOfonoSmartMessaging::sendBusinessCard(const QString &toPhoneNumber, const QByteArray &card)
{
    OfonoSmartMessaging *iface = (OfonoSmartMessaging*)dbusInterface();
    if (iface) {
        // BLOCKING CALL!
        QDBusPendingReply<QDBusObjectPath> returnPath = iface->SendBusinessCard(toPhoneNumber,card);
        returnPath.waitForFinished();
        return returnPath.value();
    }
    return QDBusObjectPath();
}

void QOfonoSmartMessaging::registerAgent(const QString &objectPath)
{
    OfonoSmartMessaging *iface = (OfonoSmartMessaging*)dbusInterface();
    if (iface) {
        iface->RegisterAgent(QDBusObjectPath(objectPath));
    }
}

void QOfonoSmartMessaging::unregisterAgent(const QString &objectPath)
{
    OfonoSmartMessaging *iface = (OfonoSmartMessaging*)dbusInterface();
    if (iface) {
        iface->UnregisterAgent(QDBusObjectPath(objectPath));
    }
}
