/****************************************************************************
**
** Copyright (C) 2013-2015 Jolla Ltd.
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
#include "ofono_smart_messaging_interface.h"

#define SUPER QOfonoModemInterface2

class QOfonoSmartMessagingCallWatcher : public QDBusPendingCallWatcher {
public:
    typedef void (QOfonoSmartMessaging::*SignalSuccess)(const QString&);
    typedef void (QOfonoSmartMessaging::*SignalError)(const QString&, const QString&);
    const char* name;
    QString objectPath;
    SignalSuccess success;
    SignalError error;
    QOfonoSmartMessagingCallWatcher(QOfonoSmartMessaging* target,
        OfonoSmartMessaging *parent, const char* callName,
        const QString &path, const QDBusPendingCall &call,
        SignalSuccess ok, SignalError err) :
        QDBusPendingCallWatcher(call, parent),
        name(callName), objectPath(path), success(ok), error(err)
    {
        connect(this, SIGNAL(finished(QDBusPendingCallWatcher*)),
            target, SLOT(onDbusCallFinished(QDBusPendingCallWatcher*)));
    }
};

QOfonoSmartMessaging::QOfonoSmartMessaging(QObject *parent) :
    SUPER(OfonoSmartMessaging::staticInterfaceName(), parent)
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
        new QOfonoSmartMessagingCallWatcher(this, iface, "RegisterAgent",
            objectPath, iface->RegisterAgent(QDBusObjectPath(objectPath)),
            &QOfonoSmartMessaging::registered,
            &QOfonoSmartMessaging::registerFailed);
    }
}

void QOfonoSmartMessaging::unregisterAgent(const QString &objectPath)
{
    OfonoSmartMessaging *iface = (OfonoSmartMessaging*)dbusInterface();
    if (iface) {
        new QOfonoSmartMessagingCallWatcher(this, iface, "UnregisterAgent",
            objectPath, iface->UnregisterAgent(QDBusObjectPath(objectPath)),
            &QOfonoSmartMessaging::unregistered,
            &QOfonoSmartMessaging::unregisterFailed);
    }
}

QString QOfonoSmartMessaging::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoSmartMessaging::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

bool QOfonoSmartMessaging::isValid() const
{
    return SUPER::isValid();
}

void QOfonoSmartMessaging::onDbusCallFinished(QDBusPendingCallWatcher *watch)
{
    QOfonoSmartMessagingCallWatcher* call = (QOfonoSmartMessagingCallWatcher*)watch;
    QDBusPendingReply<> reply(*call);
    if (reply.isError()) {
        qWarning() << call->name << "failed:" << reply.error();
        (this->*call->error)(call->objectPath, reply.error().name());
    } else {
        (this->*call->success)(call->objectPath);
    }
    call->deleteLater();
}
