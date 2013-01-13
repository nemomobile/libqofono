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
    if (!d_ptr->callForward) {
        d_ptr->modemPath = path;
        d_ptr->callForward = new OfonoCallForwarding("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->callForward) {
            connect(d_ptr->callForward,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->callForward->GetProperties();
            d_ptr->properties = reply.value();
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

