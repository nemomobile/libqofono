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

#include "qofonocallbarring.h"
#include "dbus/ofonocallbarring.h"
#include "qofonomodem.h"

class QOfonoCallBarringPrivate
{
public:
    QOfonoCallBarringPrivate();
    QString modemPath;
    OfonoCallBarring *callBarring;
    QVariantMap properties;
    bool propertiesPending;
    QSharedPointer<QOfonoModem> modem;
};

QOfonoCallBarringPrivate::QOfonoCallBarringPrivate() :
    modemPath(QString())
  , callBarring(0)
  , propertiesPending(false)
{
}

QOfonoCallBarring::QOfonoCallBarring(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoCallBarringPrivate)
{
}

QOfonoCallBarring::~QOfonoCallBarring()
{
    delete d_ptr;
}

void QOfonoCallBarring::setModemPath(const QString &path)
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

void QOfonoCallBarring::modemInterfacesChanged(const QStringList &interfaces)
{
    bool haveIface = interfaces.contains("org.ofono.CallBarring");
    if (haveIface != (isValid() && (isReady() || d_ptr->propertiesPending)))
        connectOfono();
}

void QOfonoCallBarring::connectOfono()
{
    if (d_ptr->callBarring) {
        bool wasReady = isReady();
        delete d_ptr->callBarring;
        d_ptr->callBarring = 0;
        d_ptr->properties.clear();
        d_ptr->propertiesPending = false;
        if (wasReady != isReady())
            Q_EMIT readyChanged();
    }

    d_ptr->callBarring = new OfonoCallBarring("org.ofono", d_ptr->modemPath, QDBusConnection::systemBus(),this);

    if (d_ptr->callBarring->isValid()) {
        connect(d_ptr->callBarring,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));
        d_ptr->propertiesPending = true;
        QDBusPendingReply<QVariantMap> reply = d_ptr->callBarring->GetProperties();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(getPropertiesComplete(QDBusPendingCallWatcher*)));
    }
}

QString QOfonoCallBarring::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoCallBarring::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("VoiceIncoming")) {
        Q_EMIT voiceIncomingChanged(value.value<QString>());
    } else if (property == QLatin1String("VoiceOutgoing")) {
        Q_EMIT voiceOutgoingChanged(value.value<QString>());
    }
}

QString QOfonoCallBarring::voiceIncoming()
{
    if (d_ptr->callBarring)
        return d_ptr->properties.value("VoiceIncoming").value<QString>();
    else
        return QString();
}

void QOfonoCallBarring::setVoiceIncoming(const QString &barrings, const QString &password)
{
    if (d_ptr->callBarring) {
        QDBusPendingReply<> reply = d_ptr->callBarring->SetProperty("VoiceIncoming",QDBusVariant(barrings), password);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoiceIncomingComplete(QDBusPendingCallWatcher*)));
    }
}

QString QOfonoCallBarring::voiceOutgoing()
{
    if (d_ptr->callBarring)
        return d_ptr->properties.value("VoiceOutgoing").value<QString>();
    else
        return QString();
}

void QOfonoCallBarring::setVoiceOutgoing(const QString &barrings, const QString &password)
{

    if (d_ptr->callBarring) {
        QDBusPendingReply<> reply = d_ptr->callBarring->SetProperty("VoiceOutgoing",QDBusVariant(barrings),password);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoiceOutgoingComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::changePassword(const QString &oldPassword, const QString &newPassword)
{

    if (d_ptr->callBarring) {
        QDBusPendingReply<> reply = d_ptr->callBarring->ChangePassword(oldPassword, newPassword);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(changePasswordCallComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::disableAll(const QString &password)
{
    if (d_ptr->callBarring) {
        QDBusPendingReply<> reply = d_ptr->callBarring->DisableAll(password);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(disableAllCallComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::disableAllIncoming(const QString &password)
{
    if (d_ptr->callBarring) {
        QDBusPendingReply<> reply = d_ptr->callBarring->DisableAllIncoming(password);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(disableAllIncomingCallComplete(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallBarring::disableAllOutgoing(const QString &password)
{
    if (d_ptr->callBarring) {
        QDBusPendingReply<> reply = d_ptr->callBarring->DisableAllOutgoing(password);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(disableAllOutgoingCallComplete(QDBusPendingCallWatcher*)));
    }
}

bool QOfonoCallBarring::isValid() const
{
    return d_ptr->callBarring->isValid();
}

bool QOfonoCallBarring::isReady() const
{
    return !d_ptr->properties.isEmpty();
}

void QOfonoCallBarring::getPropertiesComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<QVariantMap> reply = *call;
    if (!reply.isError()) {
        d_ptr->properties = reply.value();
        Q_EMIT voiceIncomingChanged(voiceIncoming());
        Q_EMIT voiceOutgoingChanged(voiceOutgoing());
        Q_EMIT readyChanged();
    } else {
        Q_EMIT getPropertiesFailed();
    }
    d_ptr->propertiesPending = false;
}

void QOfonoCallBarring::setVoiceIncomingComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceIncomingComplete(!reply.isError());
}

void QOfonoCallBarring::setVoiceOutgoingComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceOutgoingComplete(!reply.isError());
}

void QOfonoCallBarring::changePasswordCallComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT changePasswordComplete(!reply.isError());
}

void QOfonoCallBarring::disableAllCallComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT disableAllComplete(!reply.isError());
    call->deleteLater();
}

void QOfonoCallBarring::disableAllIncomingCallComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT disableAllIncomingComplete(!reply.isError());
    call->deleteLater();
}

void QOfonoCallBarring::disableAllOutgoingCallComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT disableAllOutgoingComplete(!reply.isError());
    call->deleteLater();
}

