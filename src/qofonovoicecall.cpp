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

#include "qofonovoicecall.h"
#include "dbus/ofonovoicecall.h"

class QOfonoVoiceCallWatcher : public QDBusPendingCallWatcher {
public:
    typedef void (QOfonoVoiceCall::*Signal)(QOfonoVoiceCall::Error,const QString&);
    const char* name;
    Signal signal;
    QOfonoVoiceCallWatcher(QOfonoVoiceCall* target, OfonoVoiceCall *parent,
        const char* callName, const QDBusPendingCall &call, Signal complete) :
        QDBusPendingCallWatcher(call, parent),
        name(callName), signal(complete)
    {
        connect(this, SIGNAL(finished(QDBusPendingCallWatcher*)),
            target, SLOT(onDbusCallFinished(QDBusPendingCallWatcher*)));
    }
};

QOfonoVoiceCall::QOfonoVoiceCall(QObject *parent) :
    QOfonoObject(parent)
{
}

QOfonoVoiceCall::~QOfonoVoiceCall()
{
}

void QOfonoVoiceCall::setVoiceCallPath(const QString &path)
{
    setObjectPath(path);
}

QString QOfonoVoiceCall::voiceCallPath() const
{
    return voiceCallPath();
}

void QOfonoVoiceCall::objectPathChanged(const QString &path, const QVariantMap *properties)
{
    QOfonoObject::objectPathChanged(path, properties);
    Q_EMIT voiceCallPathChanged(path);
}

QDBusAbstractInterface *QOfonoVoiceCall::createDbusInterface(const QString &path)
{
    return new OfonoVoiceCall("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoVoiceCall::propertyChanged(const QString &property, const QVariant &value)
{
    QOfonoObject::propertyChanged(property, value);
    if (property == QLatin1String("LineIdentification")) {
        Q_EMIT lineIdentificationChanged(value.toString());
    } else if (property == QLatin1String("DisconnectReason")) {
        Q_EMIT disconnectReason(value.toString());
    } else if (property == QLatin1String("Name")) {
        Q_EMIT nameChanged(value.toString());
    } else if (property == QLatin1String("State")) {
        Q_EMIT stateChanged(value.toString());
    } else if (property == QLatin1String("StartTime")) {
        Q_EMIT startTimeChanged(value.toString());
    } else if (property == QLatin1String("Information")) {
        Q_EMIT informationChanged(value.toString());
    } else if (property == QLatin1String("IncomingLine")) {
        Q_EMIT incomingLineChanged(value.toString());
    } else if (property == QLatin1String("Multiparty")) {
        Q_EMIT multipartyChanged(value.toBool());
    } else if (property == QLatin1String("Icon")) {
        Q_EMIT iconChanged(value.value<quint8>());
    } else if (property == QLatin1String("Emergency")) {
        Q_EMIT emergencyChanged(value.toBool());
    } else if (property == QLatin1String("RemoteHeld")) {
        Q_EMIT remoteHeldChanged(value.toBool());
    } else if (property == QLatin1String("RemoteMultiparty")) {
        Q_EMIT remoteMultipartyChanged(value.toBool());
    }
}

QString QOfonoVoiceCall::lineIdentification() const
{
    return getString("LineIdentification");
}

QString QOfonoVoiceCall::incomingLine() const
{
    return getString("IncomingLine");
}

QString QOfonoVoiceCall::name() const
{
    return getString("Name");
}

QString QOfonoVoiceCall::state() const
{
    return getString("State");
}

QString QOfonoVoiceCall::startTime() const
{
    return getString("StartTime");
}

QString QOfonoVoiceCall::information() const
{
    return getString("Information");
}

bool QOfonoVoiceCall::multiparty() const
{
    return getBool("Multiparty");
}

bool QOfonoVoiceCall::emergency() const
{
    return getBool("Emergency");
}

quint8 QOfonoVoiceCall::icon() const
{
    return getProperty("Icon").value<quint8>();
}

bool QOfonoVoiceCall::remoteHeld() const
{
    return getBool("RemoteHeld");
}

bool QOfonoVoiceCall::remoteMultiparty() const
{
    return getBool("RemoteMultiparty");
}

void QOfonoVoiceCall::answer()
{
    OfonoVoiceCall *iface = (OfonoVoiceCall*)dbusInterface();
    if (iface) {
        new QOfonoVoiceCallWatcher(this, iface, "Answer",
            iface->Answer(), &QOfonoVoiceCall::answerComplete);
    }
}

void QOfonoVoiceCall::hangup()
{
    OfonoVoiceCall *iface = (OfonoVoiceCall*)dbusInterface();
    if (iface) {
        new QOfonoVoiceCallWatcher(this, iface, "Hangup",
            iface->Hangup(), &QOfonoVoiceCall::hangupComplete);
    }
}

void QOfonoVoiceCall::deflect(const QString &number)
{
    OfonoVoiceCall *iface = (OfonoVoiceCall*)dbusInterface();
    if (iface) {
        new QOfonoVoiceCallWatcher(this, iface, "Deflect",
            iface->Deflect(number), &QOfonoVoiceCall::deflectComplete);
    }
}

void QOfonoVoiceCall::onDbusCallFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<> reply(*watch);
    QOfonoVoiceCallWatcher* call = (QOfonoVoiceCallWatcher*)watch;
    QOfonoVoiceCall::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << call->name << " failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }
    (this->*call->signal)(error, errorString);
}

QOfonoVoiceCall::Error QOfonoVoiceCall::errorNameToEnum(const QString &errorName)
{
    if (errorName == "")
        return NoError;
    else if (errorName == "org.ofono.Error.NotImplemented")
        return NotImplementedError;
    else if (errorName == "org.ofono.Error.InProgress")
        return InProgressError;
    else if (errorName == "org.ofono.Error.InvalidArguments")
        return InvalidArgumentsError;
    else if (errorName == "org.ofono.Error.InvalidFormat")
        return InvalidFormatError;
    else if (errorName == "org.ofono.Error.Failed")
        return FailedError;
    else
        return UnknownError;
}
