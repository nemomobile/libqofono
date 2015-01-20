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

#include "qofonocallbarring.h"
#include "ofono_call_barring_interface.h"

#define SUPER QOfonoModemInterface

QOfonoCallBarring::QOfonoCallBarring(QObject *parent) :
    SUPER(OfonoCallBarring::staticInterfaceName(), parent)
{
}

QOfonoCallBarring::~QOfonoCallBarring()
{
}

QDBusAbstractInterface *QOfonoCallBarring::createDbusInterface(const QString &path)
{
    return new OfonoCallBarring("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoCallBarring::connectOfono()
{
    resetDbusInterface();
}

void QOfonoCallBarring::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("VoiceIncoming")) {
        Q_EMIT voiceIncomingChanged(value.toString());
    } else if (property == QLatin1String("VoiceOutgoing")) {
        Q_EMIT voiceOutgoingChanged(value.toString());
    }
}

QString QOfonoCallBarring::voiceIncoming()
{
    return getString("VoiceIncoming");
}

void QOfonoCallBarring::setVoiceIncoming(const QString &barrings, const QString &password)
{
    OfonoCallBarring *iface = (OfonoCallBarring*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->SetProperty("VoiceIncoming", QDBusVariant(barrings), password), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(setVoiceIncomingComplete(QDBusPendingCallWatcher*)));
    }
}

QString QOfonoCallBarring::voiceOutgoing()
{
    return getString("VoiceOutgoing");
}

void QOfonoCallBarring::setVoiceOutgoing(const QString &barrings, const QString &password)
{
    OfonoCallBarring *iface = (OfonoCallBarring*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->SetProperty("VoiceOutgoing", QDBusVariant(barrings), password), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(setVoiceOutgoingComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::changePassword(const QString &oldPassword, const QString &newPassword)
{
    OfonoCallBarring *iface = (OfonoCallBarring*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->ChangePassword(oldPassword, newPassword), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(changePasswordCallComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::disableAll(const QString &password)
{
    OfonoCallBarring *iface = (OfonoCallBarring*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->DisableAll(password), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(disableAllCallComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::disableAllIncoming(const QString &password)
{
    OfonoCallBarring *iface = (OfonoCallBarring*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->DisableAllIncoming(password), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(disableAllIncomingCallComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::disableAllOutgoing(const QString &password)
{
    OfonoCallBarring *iface = (OfonoCallBarring*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->DisableAllOutgoing(password), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(disableAllOutgoingCallComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::getPropertiesFinished(const QVariantMap &properties, const QDBusError *error)
{
    SUPER::getPropertiesFinished(properties, error);
    if (error) {
        Q_EMIT getPropertiesFailed();
    }
}

void QOfonoCallBarring::setVoiceIncomingComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceIncomingComplete(!reply.isError());
}

void QOfonoCallBarring::setVoiceOutgoingComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceOutgoingComplete(!reply.isError());
}

void QOfonoCallBarring::changePasswordCallComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT changePasswordComplete(!reply.isError());
}

void QOfonoCallBarring::disableAllCallComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT disableAllComplete(!reply.isError());
}

void QOfonoCallBarring::disableAllIncomingCallComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT disableAllIncomingComplete(!reply.isError());
}

void QOfonoCallBarring::disableAllOutgoingCallComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT disableAllOutgoingComplete(!reply.isError());
}

QString QOfonoCallBarring::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoCallBarring::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

bool QOfonoCallBarring::isValid() const
{
    return SUPER::isValid();
}

bool QOfonoCallBarring::isReady() const
{
    return SUPER::isReady();
}
