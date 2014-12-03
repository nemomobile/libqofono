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

#include "qofonocallforwarding.h"
#include "dbus/ofonocallforwarding.h"

QOfonoCallForwarding::QOfonoCallForwarding(QObject *parent) :
    QOfonoModemInterface(OfonoCallForwarding::staticInterfaceName(), parent)
{
}

QOfonoCallForwarding::~QOfonoCallForwarding()
{
}

QDBusAbstractInterface *QOfonoCallForwarding::createDbusInterface(const QString &path)
{
    return new OfonoCallForwarding("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoCallForwarding::connectOfono()
{
    resetDbusInterface();
}

void QOfonoCallForwarding::propertyChanged(const QString &property, const QVariant &value)
{
    QOfonoModemInterface::propertyChanged(property, value);
    if (property == QLatin1String("VoiceUnconditional")) {
        Q_EMIT voiceUnconditionalChanged(value.toString());
    } else if (property == QLatin1String("VoiceBusy")) {
        Q_EMIT voiceBusyChanged(value.toString());
    } else if (property == QLatin1String("VoiceNoReply")) {
        Q_EMIT voiceNoReplyChanged(value.toString());
    } else if (property == QLatin1String("VoiceNoReplyTimeout")) {
        Q_EMIT voiceNoReplyTimeoutChanged(value.value<quint16>());
    } else if (property == QLatin1String("VoiceNotReachable")) {
        Q_EMIT voiceNotReachableChanged(value.toString());
    } else if (property == QLatin1String("ForwardingFlagOnSim")) {
        Q_EMIT forwardingFlagOnSimChanged(value.toBool());
    }
}

QString QOfonoCallForwarding::voiceUnconditional()
{
    return getString("VoiceUnconditional");
}

void QOfonoCallForwarding::setVoiceUnconditional(const QString &property)
{
    setProperty("VoiceUnconditional", property);
}

QString QOfonoCallForwarding::voiceBusy()
{
    return getString("VoiceBusy");
}

void QOfonoCallForwarding::setVoiceBusy(const QString &property)
{
    setProperty("VoiceBusy", property);
}

QString QOfonoCallForwarding::voiceNoReply()
{
    return getString("VoiceNoReply");
}

void QOfonoCallForwarding::setVoiceNoReply(const QString &property)
{
    setProperty("VoiceNoReply", property);
}

quint16 QOfonoCallForwarding::voiceNoReplyTimeout()
{
    return getProperty("VoiceNoReplyTimeout").value<quint16>();
}

void QOfonoCallForwarding::setVoiceNoReplyTimeout(ushort timeout)
{
    setProperty("VoiceNoReplyTimeout", QVariant::fromValue((quint16)timeout));
}

QString QOfonoCallForwarding::voiceNotReachable()
{
    return getString("VoiceNotReachable");
}

void QOfonoCallForwarding::setVoiceNotReachable(const QString &property)
{
    setProperty("VoiceNotReachable", property);
}

bool QOfonoCallForwarding::forwardingFlagOnSim()
{
    return getBool("ForwardingFlagOnSim");
}

void QOfonoCallForwarding::disableAll(const QString &type)
{
    OfonoCallForwarding *iface = (OfonoCallForwarding*)dbusInterface();
    if (iface) {
        iface->DisableAll(type);
    }
}

void QOfonoCallForwarding::getPropertiesFinished(const QVariantMap &properties, const QDBusError *error)
{
    QOfonoModemInterface::getPropertiesFinished(properties, error);
    if (error) {
        Q_EMIT getPropertiesFailed();
    }
}

void QOfonoCallForwarding::setPropertyFinished(const QString &property, const QDBusError *error)
{
    QOfonoModemInterface::setPropertyFinished(property, error);
    if (property == "VoiceUnconditional") {
        Q_EMIT voiceUnconditionalComplete(!error);
    } else if (property == "VoiceBusy") {
        Q_EMIT voiceBusyComplete(!error);
    } else if (property == "VoiceNoReply") {
        Q_EMIT voiceNoReplyComplete(!error);
    } else if (property == "VoiceNoReplyTimeout") {
        Q_EMIT voiceNoReplyTimeoutComplete(!error);
    } else if (property == "VoiceNotReachable") {
        Q_EMIT voiceNotReachableComplete(!error);
    }
}
