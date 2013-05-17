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

#include "qofonosimmanager.h"
#include "dbus/ofonosimmanager.h"

class QOfonoSimManagerPrivate
{
public:
    QOfonoSimManagerPrivate();
    QString modemPath;
    OfonoSimManager *simManager;
    QVariantMap properties;

};

QOfonoSimManagerPrivate::QOfonoSimManagerPrivate() :
    modemPath(QString())
  , simManager(0)
{
}

QOfonoSimManager::QOfonoSimManager(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoSimManagerPrivate)
{
}

QOfonoSimManager::~QOfonoSimManager()
{
    delete d_ptr;
}

void QOfonoSimManager::setModemPath(const QString &path)
{
    if (!d_ptr->simManager) {
        d_ptr->simManager = new OfonoSimManager("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->simManager->isValid()) {
            d_ptr->modemPath = path;

            connect(d_ptr->simManager,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->simManager->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoSimManager::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoSimManager::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("Present")) {
        Q_EMIT presenceChanged(value.value<bool>());
    } else if (property == QLatin1String("SubscriberIdentity")) {
        Q_EMIT subscriberIdentityChanged(value.value<QString>());
    } else if (property == QLatin1String("MobileCountryCode")) {
        Q_EMIT mobileCountryCodeChanged(value.value<QString>());
    } else if (property == QLatin1String("MobileNetworkCode")) {
        Q_EMIT mobileNetworkCodeChanged(value.value<QString>());
    } else if (property == QLatin1String("SubscriberNumbers")) {
        Q_EMIT subscriberNumbersChanged(value.value<QStringList>());
    } else if (property == QLatin1String("ServiceNumbers")) {
        QVariantMap map;
        value.value<QDBusArgument>() >> map;
        Q_EMIT serviceNumbersChanged(map);
    } else if (property == QLatin1String("PinRequired")) {
        Q_EMIT pinRequiredChanged(value.value<QString>());
    } else if (property == QLatin1String("LockedPins")) {
        Q_EMIT lockedPinsChanged(value.value<QStringList>());
    } else if (property == QLatin1String("CardIdentifier")) {
        Q_EMIT cardIdentifierChanged(value.value<QString>());
    } else if (property == QLatin1String("PreferredLanguages")) {
        Q_EMIT preferredLanguagesChanged(value.value<QStringList>());
    } else if (property == QLatin1String("Retries")) {
        QVariantMap retries;
        value.value<QDBusArgument>() >> retries;
        Q_EMIT pinRetriesChanged(retries);
    } else if (property == QLatin1String("FixedDialing")) {
        Q_EMIT fixedDialingChanged(value.value<bool>());
    } else if (property == QLatin1String("BarredDialing")) {
        Q_EMIT barredDialingChanged(value.value<bool>());
    }
}

bool QOfonoSimManager::present() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["Present"].value<bool>();
    else
        return false;
}

QString QOfonoSimManager::subscriberIdentity() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["SubscriberIdentity"].value<QString>();
    else
        return QString();
}

QString QOfonoSimManager::mobileCountryCode() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["MobileCountryCode"].value<QString>();
    else
        return QString();
}

QString QOfonoSimManager::mobileNetworkCode() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["MobileNetworkCode"].value<QString>();
    else
        return QString();
}

QStringList QOfonoSimManager::subscriberNumbers() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["SubscriberNumbers"].value<QStringList>();
    else
        return QStringList();
}

QVariantMap QOfonoSimManager::serviceNumbers() const //
{
    if (d_ptr->simManager)
        return d_ptr->properties["ServiceNumbers"].value<QVariantMap>();
    else
        return QVariantMap();
}

QString QOfonoSimManager::pinRequired() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["PinRequired"].value<QString>();
    else
        return QString();
}

QStringList QOfonoSimManager::lockedPins() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["LockedPins"].value<QStringList>();
    else
        return QStringList();
}

QString QOfonoSimManager::cardIdentifier() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["CardIdentifier"].value<QString>();
    else
        return QString();
}

QStringList QOfonoSimManager::preferredLanguages() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["PreferredLanguages"].value<QStringList>();
    else
        return QStringList();
}

QVariantMap QOfonoSimManager::pinRetries() const //
{
    if (d_ptr->simManager)
        return d_ptr->properties["Retries"].value<QVariantMap>();
    else
        return QVariantMap();
}

bool QOfonoSimManager::fixedDialing() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["FixedDialing"].value<bool>();
    else
        return false;
}

bool QOfonoSimManager::barredDialing() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["BarredDialing"].value<bool>();
    else
        return false;
}

void QOfonoSimManager::changePin(const QString &pintype, const QString &oldpin, const QString &newpin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> reply =d_ptr->simManager->ChangePin(pintype,oldpin,newpin);
        if (reply.isError()) {
            Q_EMIT changePinComplete(false);
            qWarning() << reply.error().name() << reply.error().message();
        } else {
            Q_EMIT changePinComplete(true);
        }
    }
}

void QOfonoSimManager::enterPin(const QString &pintype, const QString &pin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> reply =d_ptr->simManager->EnterPin(pintype,pin);
        if (reply.isError()) {
            Q_EMIT enterPinComplete(false);
            qWarning() << reply.error().name() << reply.error().message();
        } else {
            Q_EMIT enterPinComplete(true);
        }
    }
}

void QOfonoSimManager::resetPin(const QString &pintype, const QString &puk, const QString &newpin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> reply =d_ptr->simManager->ResetPin(pintype,puk,newpin);
        if (reply.isError()) {
            Q_EMIT resetPinComplete(false);
            qWarning() << reply.error().name() << reply.error().message();
        } else {
            Q_EMIT resetPinComplete(true);
        }
    }
}

void QOfonoSimManager::lockPin(const QString &pintype, const QString &pin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> reply =d_ptr->simManager->LockPin(pintype,pin);
        if (reply.isError()) {
            Q_EMIT unlockPinComplete(false);
            qWarning() << reply.error().name() << reply.error().message();
        } else {
            Q_EMIT unlockPinComplete(true);
        }
    }
}

void QOfonoSimManager::unlockPin(const QString &pintype, const QString &pin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> reply =d_ptr->simManager->UnlockPin(pintype,pin);
        if (reply.isError()) {
            Q_EMIT unlockPinComplete(false);
            qWarning() << reply.error().name() << reply.error().message();
        } else {
            Q_EMIT unlockPinComplete(true);
        }
    }
}

QByteArray QOfonoSimManager::getIcon(quint8 id)
{
    QDBusMessage request = QDBusMessage::createMethodCall("org.ofono",
                                                          "org.ofono.SimManager",
                                                          d_ptr->simManager->path(),
                                                          "GetIcon");

    QDBusReply<QByteArray> reply2 = QDBusConnection::systemBus().call(request);

    return reply2.value();
}


void QOfonoSimManager::setSubscriberNumbers(const QStringList &numbers)
{
    if (d_ptr->simManager)
        d_ptr->simManager->SetProperty("SubscriberNumbers",QDBusVariant(numbers));
}
