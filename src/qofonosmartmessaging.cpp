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

#include "qofonosmartmessaging.h"
#include "dbus/ofonosmartmessaging.h"

class QOfonoSmartMessagingPrivate
{
public:
    QOfonoSmartMessagingPrivate();
    QString modemPath;
    OfonoSmartMessaging *smartMessaging;
};

QOfonoSmartMessagingPrivate::QOfonoSmartMessagingPrivate() :
    modemPath(QString())
  , smartMessaging(0)
{
}

QOfonoSmartMessaging::QOfonoSmartMessaging(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoSmartMessagingPrivate)
{
}

QOfonoSmartMessaging::~QOfonoSmartMessaging()
{
    delete d_ptr;
}

void QOfonoSmartMessaging::setModemPath(const QString &path)
{
    if (!d_ptr->smartMessaging) {
        d_ptr->smartMessaging = new OfonoSmartMessaging("org.ofono", path, QDBusConnection::systemBus(),this);
        if (d_ptr->smartMessaging->isValid())
            d_ptr->modemPath = path;

    }
}

QString QOfonoSmartMessaging::modemPath() const
{
    return d_ptr->modemPath;
}

QDBusObjectPath QOfonoSmartMessaging::sendAppointment(const QString &toPhoneNumber, const QByteArray &appointment)
{
    if (d_ptr->smartMessaging) {
         QDBusPendingReply<QDBusObjectPath> returnPath = d_ptr->smartMessaging->SendAppointment(toPhoneNumber, appointment);
    return returnPath;
    }
    return QDBusObjectPath();
}

QDBusObjectPath QOfonoSmartMessaging::sendBusinessCard(const QString &toPhoneNumber, const QByteArray &card)
{
    if (d_ptr->smartMessaging) {
        QDBusPendingReply<QDBusObjectPath> returnPath = d_ptr->smartMessaging->SendBusinessCard(toPhoneNumber,card);
        return returnPath;
    }
    return QDBusObjectPath();
}

void QOfonoSmartMessaging::registerAgent(const QString &objectPath)
{
    if (d_ptr->smartMessaging)
        d_ptr->smartMessaging->RegisterAgent(QDBusObjectPath(objectPath));
}

void QOfonoSmartMessaging::unregisterAgent(const QString &objectPath)
{
    if (d_ptr->smartMessaging)
        d_ptr->smartMessaging->UnregisterAgent(QDBusObjectPath(objectPath));
}

bool QOfonoSmartMessaging::isValid() const
{
    return d_ptr->smartMessaging->isValid();
}
