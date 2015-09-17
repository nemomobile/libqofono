/***************************************************`*************************
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

#include "qofonovoicecallmanager.h"
#include "ofono_voicecallmanager_interface.h"

#define SUPER QOfonoModemInterface

class QOfonoVoiceCallManager::Private : public SUPER::ExtData
{
public:
    bool initialized;
    QStringList callList;
    QString errorMessage;

    Private() : initialized(false) {}

    class Watcher : public QDBusPendingCallWatcher {
    public:
        typedef void (QOfonoVoiceCallManager::*Signal)(bool);
        const char* name;
        Signal signal;
        Watcher(QOfonoVoiceCallManager* target, OfonoVoiceCallManager *parent,
            const char* callName, const QDBusPendingCall &call, Signal complete) :
            QDBusPendingCallWatcher(call, parent),
            name(callName), signal(complete)
        {
            connect(this, SIGNAL(finished(QDBusPendingCallWatcher*)),
                target, SLOT(onVoidCallFinished(QDBusPendingCallWatcher*)));
        }
    };

    class ObjectPathListWatcher : public QDBusPendingCallWatcher {
    public:
        typedef void (QOfonoVoiceCallManager::*Signal)(bool, const QStringList&);
        const char* name;
        Signal signal;
        ObjectPathListWatcher(QOfonoVoiceCallManager* target, OfonoVoiceCallManager *parent,
            const char* callName, const QDBusPendingCall &call, Signal complete) :
            QDBusPendingCallWatcher(call, parent),
            name(callName), signal(complete)
        {
            connect(this, SIGNAL(finished(QDBusPendingCallWatcher*)),
                target, SLOT(onObjectPathListCallFinished(QDBusPendingCallWatcher*)));
        }
    };
};

QOfonoVoiceCallManager::QOfonoVoiceCallManager(QObject *parent) :
    SUPER(OfonoVoiceCallManager::staticInterfaceName(), new Private, parent)
{
    QOfonoDbusTypes::registerObjectPathProperties();
}

QOfonoVoiceCallManager::~QOfonoVoiceCallManager()
{
}

bool QOfonoVoiceCallManager::isValid() const
{
    return privateData()->initialized && SUPER::isValid();
}

QDBusAbstractInterface *QOfonoVoiceCallManager::createDbusInterface(const QString &path)
{
    OfonoVoiceCallManager* iface = new OfonoVoiceCallManager("org.ofono", path, QDBusConnection::systemBus(), this);
    connect(new QDBusPendingCallWatcher(iface->GetCalls(), iface),
        SIGNAL(finished(QDBusPendingCallWatcher*)),
        SLOT(onGetCallsFinished(QDBusPendingCallWatcher*)));
    connect(iface,
        SIGNAL(CallAdded(QDBusObjectPath,QVariantMap)),
        SLOT(onCallAdded(QDBusObjectPath,QVariantMap)));
    connect(iface,
        SIGNAL(CallRemoved(QDBusObjectPath)),
        SLOT(onCallRemoved(QDBusObjectPath)));
    connect(iface, SIGNAL(BarringActive(QString)), SIGNAL(barringActive(QString)));
    connect(iface, SIGNAL(Forwarded(QString)), SIGNAL(forwarded(QString)));
    return iface;
}

void QOfonoVoiceCallManager::dbusInterfaceDropped()
{
    SUPER::dbusInterfaceDropped();

    Private *d_ptr = privateData();
    d_ptr->initialized = false;
    if (!d_ptr->callList.isEmpty()) {
        QStringList list = d_ptr->callList;
        d_ptr->callList.clear();
        for (int i=0; i<list.count(); i++) {
            Q_EMIT callRemoved(list[i]);
        }
    }
}

void QOfonoVoiceCallManager::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("EmergencyNumbers")) {
        Q_EMIT emergencyNumbersChanged(value.toStringList());
    }
}

QStringList QOfonoVoiceCallManager::emergencyNumbers() const
{
    return getStringList("EmergencyNumbers");
}

QStringList QOfonoVoiceCallManager::getCalls() const
{
    return privateData()->callList;
}

QString QOfonoVoiceCallManager::errorMessage() const
{
    return privateData()->errorMessage;
}

void QOfonoVoiceCallManager::dial(const QString &number, const QString &calleridHide)
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::Watcher(this, iface, "Dial",
            iface->Dial(number, calleridHide), &QOfonoVoiceCallManager::dialComplete);
    }
}

void QOfonoVoiceCallManager::hangupAll()
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
         new Private::Watcher(this, iface, "HangupAll",
            iface->HangupAll(), &QOfonoVoiceCallManager::hangupAllComplete);
    }
}

void QOfonoVoiceCallManager::sendTones(const QString &tonestring)
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::Watcher(this, iface, "SendTones",
            iface->SendTones(tonestring), &QOfonoVoiceCallManager::sendTonesComplete);
    }
}

void QOfonoVoiceCallManager::transfer()
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::Watcher(this, iface, "Transfer",
            iface->Transfer(), &QOfonoVoiceCallManager::transferComplete);
    }
}

void QOfonoVoiceCallManager::swapCalls()
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::Watcher(this, iface, "SwapCalls",
            iface->SwapCalls(), &QOfonoVoiceCallManager::swapCallsComplete);
    }
}

void QOfonoVoiceCallManager::releaseAndAnswer()
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::Watcher(this, iface, "ReleaseAndAnswer",
            iface->ReleaseAndAnswer(), &QOfonoVoiceCallManager::releaseAndAnswerComplete);
    }
}

void QOfonoVoiceCallManager::holdAndAnswer()
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::Watcher(this, iface, "HoldAndAnswer",
            iface->HoldAndAnswer(), &QOfonoVoiceCallManager::holdAndAnswerComplete);
    }
}

void QOfonoVoiceCallManager::createMultiparty()
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::ObjectPathListWatcher(this, iface, "CreateMultiparty",
            iface->CreateMultiparty(), &QOfonoVoiceCallManager::createMultipartyComplete);
    }
}

void QOfonoVoiceCallManager::hangupMultiparty()
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::Watcher(this, iface, "HangupMultiparty",
            iface->HangupMultiparty(), &QOfonoVoiceCallManager::hangupMultipartyComplete);
    }
}

void QOfonoVoiceCallManager::privateChat(const QString &path)
{
    OfonoVoiceCallManager *iface = (OfonoVoiceCallManager*)dbusInterface();
    if (iface) {
        new Private::ObjectPathListWatcher(this, iface, "PrivateChat",
            iface->PrivateChat(QDBusObjectPath(path)), &QOfonoVoiceCallManager::privateChatComplete);
    }
}

void QOfonoVoiceCallManager::onVoidCallFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    Private::Watcher* call = (Private::Watcher*)watch;
    QDBusPendingReply<> reply(*watch);
    bool ok = true;
    if (reply.isError()) {
        Private *d_ptr = privateData();
        d_ptr->errorMessage = QString("%1: %2 %3").arg(call->name).
            arg(reply.error().name()).arg(reply.error().message());
        qWarning() << d_ptr->errorMessage;
        ok = false;
    }
    (this->*call->signal)(ok);
}

void QOfonoVoiceCallManager::onObjectPathListCallFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<ObjectPathList> reply(*watch);
    Private::ObjectPathListWatcher* call = (Private::ObjectPathListWatcher*)watch;
    QStringList list;
    bool ok = true;
    if (reply.isError()) {
        Private *d_ptr = privateData();
        d_ptr->errorMessage = QString("%1: %2 %3").arg(call->name).
            arg(reply.error().name()).arg(reply.error().message());
        qWarning() << d_ptr->errorMessage;
        ok = false;
    } else {
        Q_FOREACH(const QDBusObjectPath &path, reply.value()) {
            list << path.path();
        }
    }
    (this->*call->signal)(ok, list);
}

void QOfonoVoiceCallManager::addCall(const QString &callPath)
{
    Private *d_ptr = privateData();
    if (!d_ptr->callList.contains(callPath)) {
        d_ptr->callList.append(callPath);
        Q_EMIT callAdded(callPath);
    }
}

void QOfonoVoiceCallManager::onGetCallsFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT reportError(reply.error().message());
    } else {
        ValidTracker valid(this);
        ObjectPathPropertiesList list = reply.value();
        privateData()->initialized = true;
        for (int i=0; i<list.count(); i++) {
            addCall(list[i].path.path());
        }
    }
}

void QOfonoVoiceCallManager::onCallRemoved(const QDBusObjectPath &path)
{
    QString callPath = path.path();
    if (privateData()->callList.removeOne(callPath)) {
        Q_EMIT callRemoved(callPath);
    }
}

void QOfonoVoiceCallManager::onCallAdded(const QDBusObjectPath &path, const QVariantMap &)
{
    addCall(path.path());
}

QString QOfonoVoiceCallManager::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoVoiceCallManager::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

QOfonoVoiceCallManager::Private *QOfonoVoiceCallManager::privateData() const
{
    return (QOfonoVoiceCallManager::Private*)SUPER::extData();
}
