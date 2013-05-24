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

#include "qofonovoicecall.h"
#include "dbus/ofonovoicecall.h"

class QOfonoVoiceCallPrivate
{
public:
    QOfonoVoiceCallPrivate();
    QString voiceCallPath;
    OfonoVoiceCall *voiceCall;
    QVariantMap properties;

};

QOfonoVoiceCallPrivate::QOfonoVoiceCallPrivate() :
    voiceCallPath(QString())
  , voiceCall(0)
{
}

QOfonoVoiceCall::QOfonoVoiceCall(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoVoiceCallPrivate)
{
}

QOfonoVoiceCall::~QOfonoVoiceCall()
{
    delete d_ptr;
}

void QOfonoVoiceCall::setVoiceCallPath(const QString &path)
{
    if (!d_ptr->voiceCall) {
        d_ptr->voiceCall = new OfonoVoiceCall("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->voiceCall->isValid()) {
            d_ptr->voiceCallPath = path;

            connect(d_ptr->voiceCall,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->voiceCall->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoVoiceCall::voiceCallPath() const
{
    return d_ptr->voiceCallPath;
}

void QOfonoVoiceCall::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("LineIdentification")) {
        Q_EMIT lineIdentificationChanged(value.value<QString>());

    } else if (property == QLatin1String("DisconnectReason")) {
        Q_EMIT disconnectReason(value.value<QString>());


    } else if (property == QLatin1String("Name")) {
        Q_EMIT nameChanged(value.value<QString>());

    } else if (property == QLatin1String("State")) {
        Q_EMIT stateChanged(value.value<QString>());

    } else if (property == QLatin1String("StartTime")) {
        Q_EMIT startTimeChanged(value.value<QString>());

    } else if (property == QLatin1String("Information")) {
        Q_EMIT informationChanged(value.value<QString>());

    } else if (property == QLatin1String("IncomingLine")) {
        Q_EMIT incomingLineChanged(value.value<QString>());

    } else if (property == QLatin1String("Multiparty")) {
        Q_EMIT multipartyChanged(value.value<bool>());

    } else if (property == QLatin1String("Icon")) {
        Q_EMIT iconChanged(value.value<quint8>());

    } else if (property == QLatin1String("Emergency")) {
        Q_EMIT emergencyChanged(value.value<bool>());

    } else if (property == QLatin1String("RemoteHeld")) {
        Q_EMIT remoteHeldChanged(value.value<bool>());

    } else if (property == QLatin1String("RemoteMultiparty")) {
        Q_EMIT remoteMultipartyChanged(value.value<bool>());
    }
}

QString QOfonoVoiceCall::lineIdentification() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["LineIdentification"].value<QString>();
    else
        return QString();
}

QString QOfonoVoiceCall::incomingLine() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["IncomingLine"].value<QString>();
    else
        return QString();
}

QString QOfonoVoiceCall::name() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["Name"].value<QString>();
    else
        return QString();
}

QString QOfonoVoiceCall::state() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["State"].value<QString>();
    else
        return QString();
}

QString QOfonoVoiceCall::startTime() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["StartTime"].value<QString>();
    else
        return QString();
}

QString QOfonoVoiceCall::information() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["Information"].value<QString>();
    else
        return QString();
}

bool QOfonoVoiceCall::multiparty() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["Multiparty"].value<bool>();
    else
        return false;
}

bool QOfonoVoiceCall::emergency() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["Emergency"].value<bool>();
    else
        return false;
}

quint8 QOfonoVoiceCall::icon() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["Icon"].value<quint8>();
    else
        return 0;
}

bool QOfonoVoiceCall::remoteHeld() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["RemoteHeld"].value<bool>();
    else
        return false;
}

bool QOfonoVoiceCall::remoteMultiparty() const
{
    if (d_ptr->voiceCall)
        return d_ptr->properties["RemoteMultiparty"].value<bool>();
    else
        return false;
}

void QOfonoVoiceCall::answer()
{
    if (d_ptr->voiceCall)
        d_ptr->voiceCall->Answer();
}

void QOfonoVoiceCall::hangup()
{
    if (d_ptr->voiceCall)
        d_ptr->voiceCall->Hangup();
}

void QOfonoVoiceCall::deflect(const QString &number)
{
    if (d_ptr->voiceCall)
        d_ptr->voiceCall->Deflect(number);
}

bool QOfonoVoiceCall::isValid() const
{
    return d_ptr->voiceCall->isValid();
}
