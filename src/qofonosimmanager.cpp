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

static const QString kPresent(QLatin1String("Present"));
static const QString kSubscriberIdentity(QLatin1String("SubscriberIdentity"));
static const QString kMobileCountryCode(QLatin1String("MobileCountryCode"));
static const QString kMobileNetworkCode(QLatin1String("MobileNetworkCode"));
static const QString kSubscriberNumbers(QLatin1String("SubscriberNumbers"));
static const QString kServiceNumbers(QLatin1String("ServiceNumbers"));
static const QString kPinRequired(QLatin1String("PinRequired"));
static const QString kLockedPins(QLatin1String("LockedPins"));
static const QString kCardIdentifier(QLatin1String("CardIdentifier"));
static const QString kPreferredLanguages(QLatin1String("PreferredLanguages"));
static const QString kRetries(QLatin1String("Retries"));
static const QString kFixedDialing(QLatin1String("FixedDialing"));
static const QString kBarredDialing(QLatin1String("BarredDialing"));

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
    if (property == kServiceNumbers) {
        QVariantMap convertedNumbers;
        if (value.userType() == qMetaTypeId<QDBusArgument>()) {
            QMap<QString, QString> numbers;
            value.value<QDBusArgument>() >> numbers;
            Q_FOREACH(const QString &key, numbers.keys()) {
                convertedNumbers.insert(key, numbers.value(key));
            }
        }
        return convertedNumbers;
    } else if (property == kLockedPins) {
        QVariantList convertedPins;
        if (value.userType() == qMetaTypeId<QStringList>()) {
            QStringList pins = value.value<QStringList>();
            Q_FOREACH(QString type, pins) {
                convertedPins << (int)pinTypeFromString(type);
            }
        }
        return convertedPins;
    } else if (property == kPinRequired) {
        // Use int instead of QString so that default value can be
        // default-constructed (NoPin corresponds to "none") (Also not
        // using PinType for above mentioned reason)
        return (int)pinTypeFromString(value.value<QString>());
    } else if (property == kRetries) {
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
    if (property == kPresent) {
        Q_EMIT presenceChanged(value.toBool());
    } else if (property == kSubscriberIdentity) {
        Q_EMIT subscriberIdentityChanged(value.value<QString>());
    } else if (property == kMobileCountryCode) {
        Q_EMIT mobileCountryCodeChanged(value.value<QString>());
    } else if (property == kMobileNetworkCode) {
        Q_EMIT mobileNetworkCodeChanged(value.value<QString>());
    } else if (property == kSubscriberNumbers) {
        Q_EMIT subscriberNumbersChanged(value.value<QStringList>());
    } else if (property == kServiceNumbers) {
        Q_EMIT serviceNumbersChanged(getVariantMap(kServiceNumbers));
    } else if (property == kPinRequired) {
        Q_EMIT pinRequiredChanged((PinType)getInt(kPinRequired));
    } else if (property == kLockedPins) {
        Q_EMIT lockedPinsChanged(getVariantList(kLockedPins));
    } else if (property == kCardIdentifier) {
        Q_EMIT cardIdentifierChanged(value.value<QString>());
    } else if (property == kPreferredLanguages) {
        Q_EMIT preferredLanguagesChanged(value.value<QStringList>());
    } else if (property == kRetries) {
        Q_EMIT pinRetriesChanged(getVariantMap(kRetries));
    } else if (property == kFixedDialing) {
        Q_EMIT fixedDialingChanged(value.value<bool>());
    } else if (property == kBarredDialing) {
        Q_EMIT barredDialingChanged(value.value<bool>());
    }
}

bool QOfonoSimManager::present() const
{
    return getBool(kPresent);
}

QString QOfonoSimManager::subscriberIdentity() const
{
    return getString(kSubscriberIdentity);
}

QString QOfonoSimManager::mobileCountryCode() const
{
    return getString(kMobileCountryCode);
}

QString QOfonoSimManager::mobileNetworkCode() const
{
    return getString(kMobileNetworkCode);
}

QStringList QOfonoSimManager::subscriberNumbers() const
{
    return getStringList(kSubscriberNumbers);
}

QVariantMap QOfonoSimManager::serviceNumbers() const //
{
    return getVariantMap(kServiceNumbers);
}

QOfonoSimManager::PinType QOfonoSimManager::pinRequired() const
{
    return (PinType)getInt(kPinRequired);
}

QVariantList QOfonoSimManager::lockedPins() const
{
    return getVariantList(kLockedPins);
}

QString QOfonoSimManager::cardIdentifier() const
{
    return getString(kCardIdentifier);
}

QStringList QOfonoSimManager::preferredLanguages() const
{
    return getStringList(kPreferredLanguages);
}

QVariantMap QOfonoSimManager::pinRetries() const
{
    return getVariantMap(kRetries);
}

bool QOfonoSimManager::fixedDialing() const
{
    return getBool(kFixedDialing);
}

bool QOfonoSimManager::barredDialing() const
{
    return getBool(kBarredDialing);
}

void QOfonoSimManager::setSubscriberNumbers(const QStringList &numbers)
{
    setProperty(kSubscriberNumbers, numbers);
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
