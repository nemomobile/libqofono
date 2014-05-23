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
#include "qofonomodem.h"

class QOfonoCallForwardingPrivate
{
public:
    QOfonoCallForwardingPrivate();
    QString modemPath;
    OfonoCallForwarding *callForward;
    QVariantMap properties;
    bool propertiesPending;
    QSharedPointer<QOfonoModem> modem;
};

QOfonoCallForwardingPrivate::QOfonoCallForwardingPrivate() :
    modemPath(QString())
  , callForward(0)
  , propertiesPending(false)
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
    if (path == d_ptr->modemPath || path.isEmpty())
        return;

    if (!d_ptr->modem.isNull())
        disconnect(d_ptr->modem.data(), SIGNAL(interfacesChanged(QStringList)),
                   this, SLOT(modemInterfacesChanged(QStringList)));

    d_ptr->modemPath = path;
    connectOfono();

    d_ptr->modem = QOfonoModem::instance(path);
    connect(d_ptr->modem.data(), SIGNAL(interfacesChanged(QStringList)),
            this, SLOT(modemInterfacesChanged(QStringList)));

    Q_EMIT modemPathChanged(path);
}

QString QOfonoCallForwarding::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoCallForwarding::modemInterfacesChanged(const QStringList &interfaces)
{
    bool haveIface = interfaces.contains("org.ofono.CallForwarding");
    if (haveIface != (isValid() && (isReady() || d_ptr->propertiesPending)))
        connectOfono();
}

void QOfonoCallForwarding::connectOfono()
{
    if (d_ptr->callForward) {
        bool wasReady = isReady();
        delete d_ptr->callForward;
        d_ptr->callForward = 0;
        d_ptr->properties.clear();
        d_ptr->propertiesPending = false;
        if (wasReady != isReady())
            Q_EMIT readyChanged();
    }
    d_ptr->callForward = new OfonoCallForwarding("org.ofono", d_ptr->modemPath, QDBusConnection::systemBus(),this);

    if (d_ptr->callForward->isValid()) {
        connect(d_ptr->callForward,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));
        d_ptr->propertiesPending = true;
        QDBusPendingReply<QVariantMap> reply = d_ptr->callForward->GetProperties();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(getPropertiesComplete(QDBusPendingCallWatcher*)));
    }
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
        return d_ptr->properties.value("VoiceUnconditional").value<QString>();
    else
        return QString();
}

void QOfonoCallForwarding::setVoiceUnconditional(const QString &property)
{
    if (d_ptr->callForward) {
        QDBusPendingReply<> reply = d_ptr->callForward->SetProperty("VoiceUnconditional",QDBusVariant(property));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoiceUnconditionalComplete(QDBusPendingCallWatcher*)));
    }
}

QString QOfonoCallForwarding::voiceBusy()
{
    if (d_ptr->callForward)
        return d_ptr->properties.value("VoiceBusy").value<QString>();
    else
        return QString();
}

void QOfonoCallForwarding::setVoiceBusy(const QString &property)
{
    if (d_ptr->callForward) {
        QDBusPendingReply<> reply = d_ptr->callForward->SetProperty("VoiceBusy",QDBusVariant(property));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoiceBusyComplete(QDBusPendingCallWatcher*)));
    }
}

QString QOfonoCallForwarding::voiceNoReply()
{
    if (d_ptr->callForward)
        return d_ptr->properties.value("VoiceNoReply").value<QString>();
    else
        return QString();
}

void QOfonoCallForwarding::setVoiceNoReply(const QString &property)
{
    if (d_ptr->callForward) {
        QDBusPendingReply<> reply = d_ptr->callForward->SetProperty("VoiceNoReply",QDBusVariant(property));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoiceNoReplyComplete(QDBusPendingCallWatcher*)));
    }
}

quint16 QOfonoCallForwarding::voiceNoReplyTimeout()
{
    if (d_ptr->callForward)
        return d_ptr->properties.value("VoiceNoReplyTimeout").value<quint16>();
    else
        return 0;
}

void QOfonoCallForwarding::setVoiceNoReplyTimeout(ushort timeout)
{
    if (d_ptr->callForward) {
        QDBusPendingReply<> reply = d_ptr->callForward->SetProperty("VoiceNoReplyTimeout",QDBusVariant(QVariant::fromValue((quint16)timeout)));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoiceNoReplyTimeoutComplete(QDBusPendingCallWatcher*)));
    }
}


QString QOfonoCallForwarding::voiceNotReachable()
{
    if (d_ptr->callForward)
        return d_ptr->properties.value("VoiceNotReachable").value<QString>();
    else
        return QString();
}

void QOfonoCallForwarding::setVoiceNotReachable(const QString &property)
{
    if (d_ptr->callForward) {
        QDBusPendingReply<> reply = d_ptr->callForward->SetProperty("VoiceNotReachable",QDBusVariant(property));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoiceNotReachableComplete(QDBusPendingCallWatcher*)));
    }
}


bool QOfonoCallForwarding::forwardingFlagOnSim()
{
    if (d_ptr->callForward)
        return d_ptr->properties.value("ForwardingFlagOnSim").value<bool>();
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

bool QOfonoCallForwarding::isReady() const
{
    return !d_ptr->properties.isEmpty();
}

void QOfonoCallForwarding::getPropertiesComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantMap> reply = *call;
    if (!reply.isError()) {
        d_ptr->properties = reply.value();
        Q_EMIT voiceUnconditionalChanged(voiceUnconditional());
        Q_EMIT voiceBusyChanged(voiceBusy());
        Q_EMIT voiceNoReplyChanged(voiceNoReply());
        Q_EMIT voiceNoReplyTimeoutChanged(voiceNoReplyTimeout());
        Q_EMIT voiceNotReachableChanged(voiceNotReachable());
        Q_EMIT forwardingFlagOnSimChanged(forwardingFlagOnSim());
        Q_EMIT readyChanged();
    } else {
        Q_EMIT getPropertiesFailed();
    }
    d_ptr->propertiesPending = false;
    call->deleteLater();
}

void QOfonoCallForwarding::setVoiceUnconditionalComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceUnconditionalComplete(!reply.isError());
    call->deleteLater();
}

void QOfonoCallForwarding::setVoiceBusyComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceBusyComplete(!reply.isError());
    call->deleteLater();
}

void QOfonoCallForwarding::setVoiceNoReplyComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceNoReplyComplete(!reply.isError());
    call->deleteLater();
}

void QOfonoCallForwarding::setVoiceNoReplyTimeoutComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceNoReplyTimeoutComplete(!reply.isError());
    call->deleteLater();
}

void QOfonoCallForwarding::setVoiceNotReachableComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceNotReachableComplete(!reply.isError());
    call->deleteLater();
}
