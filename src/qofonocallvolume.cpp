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

#include "qofonocallvolume.h"
#include "dbus/ofonocallvolume.h"

QOfonoCallVolume::QOfonoCallVolume(QObject *parent) :
    QOfonoModemInterface(OfonoCallVolume::staticInterfaceName(), parent)
{
}

QOfonoCallVolume::~QOfonoCallVolume()
{
}

QDBusAbstractInterface *QOfonoCallVolume::createDbusInterface(const QString &path)
{
    return new OfonoCallVolume("org.ofono", path, QDBusConnection::systemBus(),this);
}

void QOfonoCallVolume::propertyChanged(const QString &property, const QVariant &value)
{
    QOfonoModemInterface::propertyChanged(property, value);
    if (property == QLatin1String("Muted")) {
        Q_EMIT mutedChanged(value.value<bool>());
    } else if (property == QLatin1String("SpeakerVolume")) {
        Q_EMIT speakerVolumeChanged(value.value<quint8>());
    } else if (property == QLatin1String("MicrophoneVolume")) {
        Q_EMIT microphoneVolumeChanged(value.value<quint8>());
    }
}

bool QOfonoCallVolume::muted() const
{
    return getBool("Muted");
}

void QOfonoCallVolume::setMuted(bool mute)
{
    setProperty("Muted", mute);
}

quint8 QOfonoCallVolume::speakerVolume() const
{
    return getProperty("SpeakerVolume").value<quint8>();
}

// Passing one byte by reference is such a great idea!
void QOfonoCallVolume::setSpeakerVolume(const quint8 &volume)
{
    setProperty("SpeakerVolume", QVariant::fromValue(volume));
}

quint8 QOfonoCallVolume::microphoneVolume()const
{
    return getProperty("microphoneVolume").value<quint8>();
}

void QOfonoCallVolume::setMicrophoneVolume(const quint8 &volume)
{
    setProperty("MicrophoneVolume", QVariant::fromValue(volume));
}
