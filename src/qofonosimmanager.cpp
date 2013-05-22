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

#include <mgconfitem.h>
#include <QDBusPendingCallWatcher>

#include "qofonosimmanager.h"
#include "dbus/ofonosimmanager.h"

static const int qofonosimmanager_pinRetries = 3;
static const int qofonosimmanager_pukRetries = 10;

static QHash<QOfonoSimManager::PinType, QString> qofonosimmanager_pinTypes()
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

class QOfonoSimManagerPrivate
{
public:
    enum PinOperation {
        ChangePin,
        EnterPin,
        ResetPin,
        LockPin,
        UnlockPin
    };

    QOfonoSimManagerPrivate();
    QString modemPath;
    OfonoSimManager *simManager;
    QVariantMap properties;
    QHash<PinOperation, QOfonoSimManager::PinType> pendingOpPinType;

    static QHash<QOfonoSimManager::PinType, QString> allPinTypes;
};

QHash<QOfonoSimManager::PinType, QString> QOfonoSimManagerPrivate::allPinTypes = qofonosimmanager_pinTypes();

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
    if (path == d_ptr->modemPath)
        return;

    d_ptr->properties.clear();
    d_ptr->pendingOpPinType.clear();

    if (!d_ptr->simManager) {
        d_ptr->simManager = new OfonoSimManager("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->simManager->isValid()) {
            d_ptr->modemPath = path;

            connect(d_ptr->simManager,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->simManager->GetProperties();

            QVariantMap properties = reply.value();
            Q_FOREACH(QString property, properties.keys())
                updateProperty(property, properties[property]);

            if (properties.value("Retries").toMap().isEmpty()
                    && !d_ptr->properties.value("CardIdentifier").toString().isEmpty()) {
                QVariantMap retries;
                Q_FOREACH(PinType type, QOfonoSimManagerPrivate::allPinTypes.keys()) {
                    MGConfItem retryCountConf(pinRetryConfPath(type));
                    if (!retryCountConf.key().isEmpty()) {
                        QVariant value = retryCountConf.value();
                        if (!value.isNull())
                            retries[QString::number(type)] = value;
                    }
                }
                if (!retries.isEmpty()) {
                    d_ptr->properties["Retries"] = retries;
                    Q_EMIT pinRetriesChanged(retries);
                }
            }

            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoSimManager::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoSimManager::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    updateProperty(property, dbusvalue.variant());
}

void QOfonoSimManager::updateProperty(const QString& property, const QVariant& value)
{
    if (d_ptr->properties.value(property) == value)
        return;

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
        PinType pinType = (PinType)pinTypeFromString(value.value<QString>());
        updateSavedPinRetryCount(pinType, false);
        Q_EMIT pinRequiredChanged(pinType);
    } else if (property == QLatin1String("LockedPins")) {
        QStringList pins = value.value<QStringList>();
        QVariantList convertedPins;
        Q_FOREACH(QString type, pins) {
            convertedPins << (PinType)pinTypeFromString(type);
        }
        d_ptr->properties["LockedPins"] = convertedPins;
        Q_EMIT lockedPinsChanged(convertedPins);
    } else if (property == QLatin1String("CardIdentifier")) {
        Q_EMIT cardIdentifierChanged(value.value<QString>());
    } else if (property == QLatin1String("PreferredLanguages")) {
        Q_EMIT preferredLanguagesChanged(value.value<QStringList>());
    } else if (property == QLatin1String("Retries")) {
        QVariantMap retries;
        value.value<QDBusArgument>() >> retries;
        QVariantMap convertedRetries;
        Q_FOREACH(QString type, retries.keys()) {
            convertedRetries[QString::number((PinType)pinTypeFromString(type))] = retries[type];
        }
        d_ptr->properties["Retries"] = convertedRetries;
        Q_EMIT pinRetriesChanged(convertedRetries);
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

QOfonoSimManager::PinType QOfonoSimManager::pinRequired() const
{
    if (d_ptr->simManager)
        return (PinType)pinTypeFromString(d_ptr->properties["PinRequired"].value<QString>());
    else
        return QOfonoSimManager::NoPin;
}

QVariantList QOfonoSimManager::lockedPins() const
{
    if (d_ptr->simManager)
        return d_ptr->properties["LockedPins"].value<QVariantList>();
    else
        return QVariantList();
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

QVariantMap QOfonoSimManager::pinRetries() const
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

void QOfonoSimManager::changePin(PinType pinType, const QString &oldpin, const QString &newpin)
{
    if (d_ptr->simManager) {
        d_ptr->pendingOpPinType[QOfonoSimManagerPrivate::ChangePin] = pinType;
        QDBusPendingReply<> result = d_ptr->simManager->ChangePin(pinTypeToString(pinType), oldpin, newpin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(changePinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::enterPin(PinType pinType, const QString &pin)
{
    if (d_ptr->simManager) {
        d_ptr->pendingOpPinType[QOfonoSimManagerPrivate::EnterPin] = pinType;
        QDBusPendingReply<> result = d_ptr->simManager->EnterPin(pinTypeToString(pinType), pin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(enterPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::resetPin(PinType pinType, const QString &puk, const QString &newpin)
{
    if (d_ptr->simManager) {
        d_ptr->pendingOpPinType[QOfonoSimManagerPrivate::ResetPin] = pinType;
        QDBusPendingReply<> result = d_ptr->simManager->ResetPin(pinTypeToString(pinType), puk, newpin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(resetPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::lockPin(PinType pinType, const QString &pin)
{
    if (d_ptr->simManager) {
        d_ptr->pendingOpPinType[QOfonoSimManagerPrivate::LockPin] = pinType;
        QDBusPendingReply<> result = d_ptr->simManager->LockPin(pinTypeToString(pinType), pin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(lockPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::unlockPin(PinType pinType, const QString &pin)
{
    if (d_ptr->simManager) {
        d_ptr->pendingOpPinType[QOfonoSimManagerPrivate::UnlockPin] = pinType;
        QDBusPendingReply<> result = d_ptr->simManager->UnlockPin(pinTypeToString(pinType), pin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(unlockPinCallFinished(QDBusPendingCallWatcher*)));
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

void QOfonoSimManager::changePinCallFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::changePin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }
    processPinOperationReply(error, QOfonoSimManagerPrivate::ChangePin);

    emit changePinComplete(error, errorString);
    call->deleteLater();
}

void QOfonoSimManager::enterPinCallFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::enterPin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }
    processPinOperationReply(error, QOfonoSimManagerPrivate::EnterPin);

    emit enterPinComplete(error, errorString);
    call->deleteLater();
}

void QOfonoSimManager::resetPinCallFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::resetPin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }
    processPinOperationReply(error, QOfonoSimManagerPrivate::ResetPin);

    emit resetPinComplete(error, errorString);
    call->deleteLater();
}

void QOfonoSimManager::lockPinCallFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::lockPin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }
    processPinOperationReply(error, QOfonoSimManagerPrivate::LockPin);

    emit lockPinComplete(error, errorString);
    call->deleteLater();
}

void QOfonoSimManager::unlockPinCallFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    QOfonoSimManager::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoSimManager::unlockPin() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }
    processPinOperationReply(error, QOfonoSimManagerPrivate::UnlockPin);

    emit unlockPinComplete(error, errorString);
    call->deleteLater();
}

QString QOfonoSimManager::pinRetryConfPath(PinType pinType) const
{
    QString s = pinTypeToString(pinType);
    if (s.isEmpty())
        return "";
    return "/nemo/qofono/retries/" + d_ptr->properties["CardIdentifier"].value<QString>() + "/" + s;
}

void QOfonoSimManager::updateSavedPinRetryCount(PinType pinType, bool hadWrongAttempt)
{
    // If the SIM's PIN retry count is not available, we keep our own count using gconf.

    if ((pinType != SimPin && pinType != SimPin2 && pinType != SimPuk && pinType == SimPuk2)
            || d_ptr->properties.value("Retries").value<QVariantMap>().contains(pinTypeToString(pinType))
            || d_ptr->properties.value("CardIdentifier").toString().isEmpty()) {
        return;
    }

    MGConfItem retryCountConf(pinRetryConfPath(pinType));
    if (!retryCountConf.key().isEmpty()) {
        QVariant retryCountVariant = retryCountConf.value();
        int retryCount;
        if (!retryCountVariant.isNull()) {
            retryCount = retryCountVariant.toInt();
        } else {
            retryCount = (isPukType(pinType) ? qofonosimmanager_pukRetries : qofonosimmanager_pinRetries);
        }
        if (hadWrongAttempt) {
            retryCount = qMax(0, retryCount - 1);
            retryCountConf.set(retryCount);
        }
        QVariantMap currentRetries = d_ptr->properties["Retries"].toMap();
        currentRetries[QString::number(pinType)] = retryCount;
        d_ptr->properties["Retries"] = currentRetries;
        Q_EMIT pinRetriesChanged(currentRetries);
    }
}

void QOfonoSimManager::processPinOperationReply(Error error, int opType)
{
    QOfonoSimManager::PinType pinType = d_ptr->pendingOpPinType[(QOfonoSimManagerPrivate::PinOperation)opType];

    if (error == NoError) {
        MGConfItem retryCountConf(pinRetryConfPath(pinType));
        retryCountConf.unset();

        QVariantMap currentRetries = d_ptr->properties["Retries"].toMap();
        currentRetries.remove(QString::number(pinType));

        // If PUK was successfully entered, we can reset the retry count for the corresponding PIN
        if (isPukType(pinType)) {
            PinType matchingPin = (PinType)pukToPin(pinType);
            if (matchingPin != NoPin) {
                MGConfItem matchingPinRetryConf(pinRetryConfPath(matchingPin));
                matchingPinRetryConf.unset();
                currentRetries.remove(QString::number(matchingPin));
            }
        }
        d_ptr->properties["Retries"] = currentRetries;
        Q_EMIT pinRetriesChanged(currentRetries);

    } else if (error == FailedError) { // other errors do not trigger change in retry count
        updateSavedPinRetryCount(pinType, true);
    }
    d_ptr->pendingOpPinType.remove((QOfonoSimManagerPrivate::PinOperation)opType);
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

int QOfonoSimManager::minimumPinLength(PinType pinType)
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

int QOfonoSimManager::maximumPinLength(PinType pinType)
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

QString QOfonoSimManager::pinTypeToString(PinType pinType)
{
    return QOfonoSimManagerPrivate::allPinTypes.value(pinType);
}

int QOfonoSimManager::pinTypeFromString(const QString &s)
{
    return (int)QOfonoSimManagerPrivate::allPinTypes.key(s);
}

bool QOfonoSimManager::isPukType(PinType pinType)
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

int QOfonoSimManager::pukToPin(PinType puk)
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
