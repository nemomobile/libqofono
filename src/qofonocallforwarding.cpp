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

#include "qofonocallforwarding.h"
#include "dbus/ofonocallforwarding.h"

class QOfonoCallForwardingPrivate
{
public:
    QOfonoCallForwardingPrivate();
    QString modemPath;
    OfonoCallForwarding *callForward;
    QVariantMap properties;
};

QOfonoCallForwardingPrivate::QOfonoCallForwardingPrivate() :
    modemPath(QString())
  , callForward(0)
{
}

QOfonoCallForwarding::QOfonoCallForwarding(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoCallForwardingPrivate)
{
}

QOfonoCallForwarding::~QOfonoCallForwarding()
{
    delete d_ptr;
}

void QOfonoCallForwarding::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        if (d_ptr->callForward) {
            delete d_ptr->callForward;
            d_ptr->callForward = 0;
            d_ptr->properties .clear();
        }
        d_ptr->callForward = new OfonoCallForwarding("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->callForward->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->callForward,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->callForward->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoCallForwarding::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoCallForwarding::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("VoiceUnconditional")) {
        Q_EMIT voiceUnconditionalChanged(value.value<QString>());
    } else if (property == QLatin1String("VoiceBusy")) {
        Q_EMIT voiceBusyChanged(value.value<QString>());
    } else if (property == QLatin1String("VoiceNoReply")) {
        Q_EMIT voiceNoReplyChanged(value.value<QString>());
    } else if (property == QLatin1String("VoiceNoReplyTimeout")) {
        Q_EMIT voiceNoReplyTimeoutChanged(value.value<quint16>());
    } else if (property == QLatin1String("VoiceNotReachable")) {
        Q_EMIT voiceNotReachableChanged(value.value<QString>());
    } else if (property == QLatin1String("ForwardingFlagOnSim")) {
        Q_EMIT forwardingFlagOnSimChanged(value.value<bool>());
    }
}

QString QOfonoCallForwarding::voiceUnconditional()
{
    if (d_ptr->callForward)
        return d_ptr->properties["VoiceUnconditional"].value<QString>();
    else
        return QString();
}

void QOfonoCallForwarding::setVoiceUnconditional(const QString &property)
{
    if (d_ptr->callForward)
        d_ptr->callForward->SetProperty("VoiceUnconditional",QDBusVariant(property));
}

QString QOfonoCallForwarding::voiceBusy()
{
    if (d_ptr->callForward)
        return d_ptr->properties["VoiceBusy"].value<QString>();
    else
        return QString();
}

void QOfonoCallForwarding::setVoiceBusy(const QString &property)
{
    if (d_ptr->callForward)
        d_ptr->callForward->SetProperty("VoiceBusy",QDBusVariant(property));
}

QString QOfonoCallForwarding::voiceNoReply()
{
    if (d_ptr->callForward)
        return d_ptr->properties["VoiceNoReply"].value<QString>();
    else
        return QString();
}

void QOfonoCallForwarding::setVoiceNoReply(const QString &property)
{
    if (d_ptr->callForward)
        d_ptr->callForward->SetProperty("VoiceNoReply",QDBusVariant(property));
}

quint16 QOfonoCallForwarding::voiceNoReplyTimeout()
{
    if (d_ptr->callForward)
        return d_ptr->properties["VoiceNoReplyTimeout"].value<quint16>();
    else
        return 0;
}

void QOfonoCallForwarding::setVoiceNoReplyTimeout(ushort timeout)
{
    if (d_ptr->callForward)
        d_ptr->callForward->SetProperty("VoiceNoReplyTimeout",QDBusVariant(timeout));
}


QString QOfonoCallForwarding::voiceNotReachable()
{
    if (d_ptr->callForward)
        return d_ptr->properties["VoiceNotReachable"].value<QString>();
    else
        return QString();
}

void QOfonoCallForwarding::setVoiceNotReachable(const QString &property)
{
    if (d_ptr->callForward)
        d_ptr->callForward->SetProperty("VoiceNotReachable",QDBusVariant(property));
}


bool QOfonoCallForwarding::forwardingFlagOnSim()
{
    if (d_ptr->callForward)
        return d_ptr->properties["ForwardingFlagOnSim"].value<bool>();
    else
        return false;
}


void QOfonoCallForwarding::disableAll(const QString &type)
{
    if (d_ptr->callForward)
        d_ptr->callForward->DisableAll(type);
}

bool QOfonoCallForwarding::isValid() const
{
    return d_ptr->callForward->isValid();
}
