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

#include "qofonocallsettings.h"
#include "dbus/ofonocallsettings.h"

QOfonoCallSettings::QOfonoCallSettings(QObject *parent) :
    QOfonoModemInterface(OfonoCallSettings::staticInterfaceName(), parent)
{
}

QOfonoCallSettings::~QOfonoCallSettings()
{
}

void QOfonoCallSettings::connectOfono()
{
    resetDbusInterface();
}

QDBusAbstractInterface *QOfonoCallSettings::createDbusInterface(const QString &path)
{
    return new OfonoCallSettings("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoCallSettings::propertyChanged(const QString &property, const QVariant &value)
{
    QOfonoModemInterface::propertyChanged(property, value);
    if (property == QLatin1String("CallingLinePresentation")) {
        Q_EMIT callingLinePresentationChanged(value.value<QString>());
    } else if (property == QLatin1String("CalledLinePresentation")) {
        Q_EMIT calledLinePresentationChanged(value.value<QString>());
    } else if (property == QLatin1String("CallingNamePresentation")) {
        Q_EMIT callingNamePresentationChanged(value.value<QString>());
    } else if (property == QLatin1String("ConnectedLinePresentation")) {
        Q_EMIT connectedLinePresentationChanged(value.value<QString>());
    } else if (property == QLatin1String("ConnectedLineRestriction")) {
        Q_EMIT connectedLineRestrictionChanged(value.value<QString>());
    } else if (property == QLatin1String("CallingLineRestriction")) {
        Q_EMIT callingLineRestrictionChanged(value.value<QString>());
    } else if (property == QLatin1String("HideCallerId")) {
        Q_EMIT hideCallerIdChanged(value.value<QString>());
    } else if (property == QLatin1String("VoiceCallWaiting")) {
        Q_EMIT voiceCallWaitingChanged(value.value<QString>());
    }
}

QString QOfonoCallSettings::callingLinePresentation()
{
    return getString("CallingLinePresentation");
}

QString QOfonoCallSettings::calledLinePresentation()
{
    return getString("CalledLinePresentation");
}

QString QOfonoCallSettings::callingNamePresentation()
{
    return getString("CallingNamePresentation");
}

QString QOfonoCallSettings::connectedLinePresentation()
{
    return getString("ConnectedLinePresentation");
}

QString QOfonoCallSettings::connectedLineRestriction()
{
    return getString("ConnectedLineRestriction");
}

QString QOfonoCallSettings::callingLineRestriction()
{
    return getString("CallingLineRestriction");
}

QString QOfonoCallSettings::hideCallerId()
{
    return getString("HideCallerId");
}

void QOfonoCallSettings::setHideCallerId(const QString &setting)
{
    setProperty("HideCallerId", setting);
}

QString QOfonoCallSettings::voiceCallWaiting()
{
    return getString("VoiceCallWaiting");
}

void QOfonoCallSettings::setVoiceCallWaiting(const QString &setting)
{
    setProperty("VoiceCallWaiting", setting);
}

void QOfonoCallSettings::getPropertiesFinished(const QVariantMap &properties, const QDBusError *error)
{
    QOfonoModemInterface::getPropertiesFinished(properties, error);
    if (error) {
        Q_EMIT getPropertiesFailed();
    }
}

void QOfonoCallSettings::setPropertyFinished(const QString &property, const QDBusError *error)
{
    QOfonoModemInterface::setPropertyFinished(property, error);
    if (property == "HideCallerId") {
        Q_EMIT hideCallerIdComplete(!error);
    } else if (property == "VoiceCallWaiting") {
        Q_EMIT voiceCallWaitingComplete(!error);
    }
}
