/****************************************************************************
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

#include "qofonosimmanager.h"
#include "ofono_simmanager_interface.h"

#define SUPER QOfonoModemInterface

namespace QOfonoSimManagerPrivate
{
    static QHash<QOfonoSimManager::PinType, QString> pinTypes()
    {
        QHash<QOfonoSimManager::PinType, QString> types;
        types[QOfonoSimManager::NoPin] = "none";
        types[QOfonoSimManager::SimPin] = "pin";
        types[QOfonoSimManager::SimPin2] = "pin2";
        types[QOfonoSimManager::PhoneToSimPin] = "phone";
        types[QOfonoSimManager::PhoneToFirstSimPin] = "firstphone";
        types[QOfonoSimManager::NetworkPersonalizationPin] = "network";
        types[QOfonoSimManager::NetworkSubsetPersonalizationPin] = "netsub";
        types[QOfonoSimManager::ServiceProviderPersonalizationPin] = "service";
        types[QOfonoSimManager::CorporatePersonalizationPin] = "corp";
        types[QOfonoSimManager::SimPuk] = "puk";
        types[QOfonoSimManager::SimPuk2] = "puk2";
        types[QOfonoSimManager::PhoneToFirstSimPuk] = "firstphonepuk";
        types[QOfonoSimManager::NetworkPersonalizationPuk] = "networkpuk";
        types[QOfonoSimManager::NetworkSubsetPersonalizationPuk] = "netsubpuk";
        types[QOfonoSimManager::CorporatePersonalizationPuk] = "corppuk";
        return types;
    }
    static QHash<QOfonoSimManager::PinType, QString> allPinTypes = pinTypes();
}

QOfonoSimManager::QOfonoSimManager(QObject *parent) :
    SUPER(OfonoSimManager::staticInterfaceName(), parent)
{
}

QOfonoSimManager::~QOfonoSimManager()
{
}

QDBusAbstractInterface *QOfonoSimManager::createDbusInterface(const QString &path)
{
    return new OfonoSimManager("org.ofono", path, QDBusConnection::systemBus(), this);
}

QVariant QOfonoSimManager::convertProperty(const QString &property, const QVariant &value)
{
    // Perform necessary type conversions
    // This serve two purposes:
    //  (1) Usability from QML side
    //  (2) Possibility to compare with QVariant::operator== (comparing variants
    //      of user type is NOT supported with Qt < 5.2 and with Qt >= 5.2 it
    //      relies on QVariant::registerComparisonOperators)
    if (property == QLatin1String("ServiceNumbers")) {
        QVariantMap convertedNumbers;
        if (value.userType() == qMetaTypeId<QDBusArgument>()) {
            QMap<QString, QString> numbers;
            value.value<QDBusArgument>() >> numbers;
            Q_FOREACH(const QString &key, numbers.keys()) {
                convertedNumbers.insert(key, numbers.value(key));
            }
        }
        return convertedNumbers;
    } else if (property == QLatin1String("LockedPins")) {
        QVariantList convertedPins;
        if (value.userType() == qMetaTypeId<QStringList>()) {
            QStringList pins = value.value<QStringList>();
            Q_FOREACH(QString type, pins) {
                convertedPins << (int)pinTypeFromString(type);
            }
        }
        return convertedPins;
    } else if (property == QLatin1String("PinRequired")) {
        // Use int instead of QString so that default value can be
        // default-constructed (NoPin corresponds to "none") (Also not
        // using PinType for above mentioned reason)
        return (int)pinTypeFromString(value.value<QString>());
    } else if (property == QLatin1String("Retries")) {
        QVariantMap convertedRetries;
        if (value.userType() == qMetaTypeId<QDBusArgument>()) {
            QMap<QString, unsigned char> retries;
            value.value<QDBusArgument>() >> retries;
            Q_FOREACH(const QString &type, retries.keys()) {
                QVariant retryCountVariant = retries[type];
                bool ok = false;
                int retryCount = retryCountVariant.toInt(&ok);
                if (ok) {
                    convertedRetries[QString::number(pinTypeFromString(type))] = retryCount;
                }
            }
        }
        return convertedRetries;
    } else {
        return SUPER::convertProperty(property, value);
    }
}

void QOfonoSimManager::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
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
        Q_EMIT serviceNumbersChanged(getVariantMap("ServiceNumbers"));
    } else if (property == QLatin1String("PinRequired")) {
        Q_EMIT pinRequiredChanged((PinType)getInt("PinRequired"));
    } else if (property == QLatin1String("LockedPins")) {
        Q_EMIT lockedPinsChanged(getVariantList("LockedPins"));
    } else if (property == QLatin1String("CardIdentifier")) {
        Q_EMIT cardIdentifierChanged(value.value<QString>());
    } else if (property == QLatin1String("PreferredLanguages")) {
        Q_EMIT preferredLanguagesChanged(value.value<QStringList>());
    } else if (property == QLatin1String("Retries")) {
        Q_EMIT pinRetriesChanged(getVariantMap("Retries"));
    } else if (property == QLatin1String("FixedDialing")) {
        Q_EMIT fixedDialingChanged(value.value<bool>());
    } else if (property == QLatin1String("BarredDialing")) {
        Q_EMIT barredDialingChanged(value.value<bool>());
    }
}

bool QOfonoSimManager::present() const
{
    return getBool("Present");
}

QString QOfonoSimManager::subscriberIdentity() const
{
    return getString("SubscriberIdentity");
}

QString QOfonoSimManager::mobileCountryCode() const
{
    return getString("MobileCountryCode");
}

QString QOfonoSimManager::mobileNetworkCode() const
{
    return getString("MobileNetworkCode");
}

QStringList QOfonoSimManager::subscriberNumbers() const
{
    return getStringList("SubscriberNumbers");
}

QVariantMap QOfonoSimManager::serviceNumbers() const //
{
    return getVariantMap("ServiceNumbers");
}

QOfonoSimManager::PinType QOfonoSimManager::pinRequired() const
{
    return (PinType)getInt("PinRequired");
}

QVariantList QOfonoSimManager::lockedPins() const
{
    return getVariantList("LockedPins");
}

QString QOfonoSimManager::cardIdentifier() const
{
    return getString("CardIdentifier");
}

QStringList QOfonoSimManager::preferredLanguages() const
{
    return getStringList("PreferredLanguages");
}

QVariantMap QOfonoSimManager::pinRetries() const
{
    return getVariantMap("Retries");
}

bool QOfonoSimManager::fixedDialing() const
{
    return getBool("FixedDialing");
}

bool QOfonoSimManager::barredDialing() const
{
    return getBool("BarredDialing");
}

void QOfonoSimManager::setSubscriberNumbers(const QStringList &numbers)
{
    setProperty("SubscriberNumbers", numbers);
}

void QOfonoSimManager::changePin(QOfonoSimManager::PinType pinType, const QString &oldpin, const QString &newpin)
{
    OfonoSimManager *iface = (OfonoSimManager*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->ChangePin(pinTypeToString(pinType), oldpin, newpin), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(changePinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::enterPin(QOfonoSimManager::PinType pinType, const QString &pin)
{
    OfonoSimManager *iface = (OfonoSimManager*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->EnterPin(pinTypeToString(pinType), pin), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(enterPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::resetPin(QOfonoSimManager::PinType pinType, const QString &puk, const QString &newpin)
{
    OfonoSimManager *iface = (OfonoSimManager*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->ResetPin(pinTypeToString(pinType), puk, newpin), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(resetPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::lockPin(QOfonoSimManager::PinType pinType, const QString &pin)
{
    OfonoSimManager *iface = (OfonoSimManager*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->LockPin(pinTypeToString(pinType), pin), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(lockPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::unlockPin(QOfonoSimManager::PinType pinType, const QString &pin)
{
    OfonoSimManager *iface = (OfonoSimManager*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->UnlockPin(pinTypeToString(pinType), pin), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(unlockPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

QByteArray QOfonoSimManager::getIcon(quint8 id)
{
    OfonoSimManager *iface = (OfonoSimManager*)dbusInterface();
    if (iface) {
        // BLOCKING CALL!
        QDBusPendingReply<QByteArray> reply = iface->GetIcon(id);
        reply.waitForFinished();
        if (!reply.isError()) {
            return reply.value();
        }
        qDebug() << reply.error().message();
    }
    return QByteArray();
}

void QOfonoSimManager::changePinCallFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::changePin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }

    emit changePinComplete(error, errorString);
}

void QOfonoSimManager::enterPinCallFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::enterPin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }

    emit enterPinComplete(error, errorString);
}

void QOfonoSimManager::resetPinCallFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::resetPin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }

    emit resetPinComplete(error, errorString);
}

void QOfonoSimManager::lockPinCallFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::lockPin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }

    emit lockPinComplete(error, errorString);
}

void QOfonoSimManager::unlockPinCallFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::unlockPin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }

    emit unlockPinComplete(error, errorString);
}

QOfonoSimManager::Error QOfonoSimManager::errorNameToEnum(const QString &errorName)
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

int QOfonoSimManager::minimumPinLength(QOfonoSimManager::PinType pinType)
{
    if (isPukType(pinType))
        return 8;

    switch (pinType) {
    case SimPin:
    case SimPin2:
    case PhoneToSimPin:
    case PhoneToFirstSimPin:
    case NetworkPersonalizationPin:
    case NetworkSubsetPersonalizationPin:
    case CorporatePersonalizationPin:
        return 4;
    default:
        return -1;
    }
}

int QOfonoSimManager::maximumPinLength(QOfonoSimManager::PinType pinType)
{
    if (isPukType(pinType))
        return 8;

    switch (pinType) {
    case SimPin:
    case SimPin2:
        return 8;
    case PhoneToSimPin:
    case PhoneToFirstSimPin:
    case NetworkPersonalizationPin:
    case NetworkSubsetPersonalizationPin:
    case CorporatePersonalizationPin:
        return 16;
    default:
        return -1;
    }
}

QString QOfonoSimManager::pinTypeToString(QOfonoSimManager::PinType pinType)
{
    return QOfonoSimManagerPrivate::allPinTypes.value(pinType);
}

int QOfonoSimManager::pinTypeFromString(const QString &s)
{
    return (int)QOfonoSimManagerPrivate::allPinTypes.key(s);
}

bool QOfonoSimManager::isPukType(QOfonoSimManager::PinType pinType)
{
    switch (pinType) {
    case SimPuk:
    case SimPuk2:
    case PhoneToFirstSimPuk:
    case NetworkPersonalizationPuk:
    case NetworkSubsetPersonalizationPuk:
    case CorporatePersonalizationPuk:
        return true;
    default:
        return false;
    }
}

int QOfonoSimManager::pukToPin(QOfonoSimManager::PinType puk)
{
    switch (puk) {
    case QOfonoSimManager::SimPuk:
        return (int)QOfonoSimManager::SimPin;
    case QOfonoSimManager::PhoneToFirstSimPuk:
        return (int)QOfonoSimManager::PhoneToFirstSimPin;
    case QOfonoSimManager::SimPuk2:
        return (int)QOfonoSimManager::SimPin2;
    case QOfonoSimManager::NetworkPersonalizationPuk:
        return (int)QOfonoSimManager::NetworkPersonalizationPin;
    case QOfonoSimManager::NetworkSubsetPersonalizationPuk:
        return (int)QOfonoSimManager::NetworkSubsetPersonalizationPin;
    case QOfonoSimManager::CorporatePersonalizationPuk:
        return (int)QOfonoSimManager::CorporatePersonalizationPin;
    default:
        return (int)QOfonoSimManager::NoPin;
    }
}

QString QOfonoSimManager::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoSimManager::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

bool QOfonoSimManager::isValid() const
{
    return SUPER::isValid();
}
