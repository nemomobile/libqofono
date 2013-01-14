/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**
**
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

#include "qofonocallvolume.h"
#include "dbus/ofonocallvolume.h"

class QOfonoCallVolumePrivate
{
public:
    QOfonoCallVolumePrivate();
    QString modemPath;
    OfonoCallVolume *callVolume;
    QVariantMap properties;

};

QOfonoCallVolumePrivate::QOfonoCallVolumePrivate() :
    modemPath(QString())
  , callVolume(0)
{
}

QOfonoCallVolume::QOfonoCallVolume(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoCallVolumePrivate)
{
}

QOfonoCallVolume::~QOfonoCallVolume()
{
    delete d_ptr;
}

void QOfonoCallVolume::setModemPath(const QString &path)
{
    if (!d_ptr->callVolume) {
        d_ptr->callVolume = new OfonoCallVolume("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->callVolume->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->callVolume,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->callVolume->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoCallVolume::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoCallVolume::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

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
    if ( d_ptr->callVolume)
        return d_ptr->properties["Muted"].value<bool>();
    else
        return false;
}

void QOfonoCallVolume::setMuted(bool mute)
{
    if ( d_ptr->callVolume)
        d_ptr->callVolume->SetProperty("Muted",QDBusVariant(mute));

}

quint8 QOfonoCallVolume::speakerVolume() const
{
    if (d_ptr->callVolume)
        return d_ptr->properties["SpeakerVolume"].value<quint8>();
    else
        return 0;
}

void QOfonoCallVolume::setSpeakerVolume(const quint8 &volume)
{
    if ( d_ptr->callVolume)
        d_ptr->callVolume->SetProperty("SpeakerVolume",QDBusVariant(volume));
}

quint8 QOfonoCallVolume::microphoneVolume()const
{
    if ( d_ptr->callVolume)
        return d_ptr->properties["MicrophoneVolume"].value<quint8>();
    else
        return 0;
}

void QOfonoCallVolume::setMicrophoneVolume(const quint8 &volume)
{
    if ( d_ptr->callVolume)
        d_ptr->callVolume->SetProperty("MicrophoneVolume",QDBusVariant(volume));
}

