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

#include <QDBusPendingCallWatcher>

#include "qofonosimmanager.h"
#include "qofonomodem.h"
#include "qofonoutils_p.h"
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
    QOfonoSimManagerPrivate();
    QString modemPath;
    OfonoSimManager *simManager;
    QVariantMap properties;
    QOfonoModem *oModem;
    static QHash<QOfonoSimManager::PinType, QString> allPinTypes;
};

QHash<QOfonoSimManager::PinType, QString> QOfonoSimManagerPrivate::allPinTypes = qofonosimmanager_pinTypes();

QOfonoSimManagerPrivate::QOfonoSimManagerPrivate() :
    modemPath(QString()),
    simManager(0),
    oModem(0)
{
}

QOfonoSimManager::QOfonoSimManager(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoSimManagerPrivate)
{
    d_ptr->oModem = new QOfonoModem(this);
}

QOfonoSimManager::~QOfonoSimManager()
{
    delete d_ptr;
}

void QOfonoSimManager::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;
    d_ptr->modemPath = path;
    Q_EMIT modemPathChanged(path);

    d_ptr->oModem->setModemPath(path);
    if (d_ptr->oModem->interfaces().contains(QLatin1String("org.ofono.SimManager")))
        initialize();
    else
        connect(d_ptr->oModem,SIGNAL(interfacesChanged(QStringList)),this,SLOT(modemInterfacesChanged(QStringList)));
}

void QOfonoSimManager::initialize()
{
    if (d_ptr->modemPath.isEmpty())
        return;

    delete d_ptr->simManager;
    d_ptr->simManager = new OfonoSimManager("org.ofono", d_ptr->modemPath, QDBusConnection::systemBus(), this);

    if (d_ptr->simManager->isValid()) {

        connect(d_ptr->simManager,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));

        getAllProperties();
    }
}

void QOfonoSimManager::modemInterfacesChanged(const QStringList &list)
{
    if (list.contains(QLatin1String("org.ofono.SimManager"))) {
        disconnect(d_ptr->oModem,SIGNAL(interfacesChanged(QStringList)),this,SLOT(modemInterfacesChanged(QStringList)));
        initialize();
    }
}

void QOfonoSimManager::getAllProperties()
{
    QStringList removedProperties = d_ptr->properties.keys();

    QDBusPendingReply<QVariantMap> reply = d_ptr->simManager->GetProperties();
    reply.waitForFinished();
    QVariantMap properties = reply.value();

    for (QVariantMap::ConstIterator it = properties.constBegin();
         it != properties.constEnd(); ++it) {
        updateProperty(it.key(), it.value());
        removedProperties.removeOne(it.key());
    }
    foreach (const QString &p, removedProperties)
        updateProperty(p, QVariant());

}

QString QOfonoSimManager::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoSimManager::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    updateProperty(property, dbusvalue.variant());
}

void QOfonoSimManager::updateProperty(const QString& property, const QVariant& value_)
{
    // Perform necessary type conversions
    // This serve two purposes:
    //  (1) Usability from QML side
    //  (2) Possibility to compare with QVariant::operator== (comparing variants
    //      of user type is NOT supported with Qt < 5.2 and with Qt >= 5.2 it
    //      relies on QVariant::registerComparisonOperators)
    QVariant value = value_;
    if (value.isValid()) {
        if (property == QLatin1String("ServiceNumbers")) {
            QVariantMap convertedNumbers;
            if (value.userType() == qMetaTypeId<QDBusArgument>()) {
                QMap<QString, QString> numbers;
                value.value<QDBusArgument>() >> numbers;
                Q_FOREACH(const QString &key, numbers.keys()) {
                    convertedNumbers.insert(key, numbers.value(key));
                }
            }
            value = convertedNumbers;
        } else if (property == QLatin1String("LockedPins")) {
            QVariantList convertedPins;
            if (value.userType() == qMetaTypeId<QStringList>()) {
                QStringList pins = value.value<QStringList>();
                Q_FOREACH(QString type, pins) {
                    convertedPins << (int)pinTypeFromString(type);
                }
            }
            value = convertedPins;
        } else if (property == QLatin1String("PinRequired")) {
            // Use int instead of QString so that default value can be
            // default-constructed (NoPin corresponds to "none") (Also not
            // using PinType for above mentioned reason)
            value = (int)pinTypeFromString(value.value<QString>());
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
            value = convertedRetries;
        }
    }

    QVariant old = d_ptr->properties.value(property);

    if (value.isValid())
        d_ptr->properties.insert(property, value);
    else
        d_ptr->properties.remove(property);

    if (qofono::safeVariantEq(old, value))
        return;

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
        Q_EMIT serviceNumbersChanged(value.value<QVariantMap>());
    } else if (property == QLatin1String("PinRequired")) {
        Q_EMIT pinRequiredChanged((PinType)value.value<int>());
    } else if (property == QLatin1String("LockedPins")) {
        Q_EMIT lockedPinsChanged(value.value<QVariantList>());
    } else if (property == QLatin1String("CardIdentifier")) {
        Q_EMIT cardIdentifierChanged(value.value<QString>());
    } else if (property == QLatin1String("PreferredLanguages")) {
        Q_EMIT preferredLanguagesChanged(value.value<QStringList>());
    } else if (property == QLatin1String("Retries")) {
        Q_EMIT pinRetriesChanged(value.value<QVariantMap>());
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
        return (PinType)d_ptr->properties["PinRequired"].value<int>();
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

void QOfonoSimManager::changePin(QOfonoSimManager::PinType pinType, const QString &oldpin, const QString &newpin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> result = d_ptr->simManager->ChangePin(pinTypeToString(pinType), oldpin, newpin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(changePinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::enterPin(QOfonoSimManager::PinType pinType, const QString &pin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> result = d_ptr->simManager->EnterPin(pinTypeToString(pinType), pin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(enterPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::resetPin(QOfonoSimManager::PinType pinType, const QString &puk, const QString &newpin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> result = d_ptr->simManager->ResetPin(pinTypeToString(pinType), puk, newpin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(resetPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::lockPin(QOfonoSimManager::PinType pinType, const QString &pin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> result = d_ptr->simManager->LockPin(pinTypeToString(pinType), pin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(lockPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSimManager::unlockPin(QOfonoSimManager::PinType pinType, const QString &pin)
{
    if (d_ptr->simManager) {
        QDBusPendingReply<> result = d_ptr->simManager->UnlockPin(pinTypeToString(pinType), pin);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(unlockPinCallFinished(QDBusPendingCallWatcher*)));
    }
}

QByteArray QOfonoSimManager::getIcon(quint8 id)
{
    QDBusMessage request = QDBusMessage::createMethodCall("org.ofono",
                                                          d_ptr->simManager->path(),
                                                          "org.ofono.SimManager",
                                                          "GetIcon");
    request.setArguments(QList<QVariant>() << QVariant::fromValue(id));

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

    emit unlockPinComplete(error, errorString);
    call->deleteLater();
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

bool QOfonoSimManager::isValid() const
{
    return d_ptr->simManager->isValid();
}
