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

#ifndef QOFONOSimManager_H
#define QOFONOSimManager_H

#include <QObject>
#include <QDBusVariant>
#include <QStringList>

#include "qofono_global.h"
//! This class is used to access ofono SIM API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/sim-api.txt
 */

class QOfonoSimManagerPrivate;
class QDBusPendingCallWatcher;

class QOFONOSHARED_EXPORT QOfonoSimManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(Error)
    Q_ENUMS(PinType)
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(bool present READ present NOTIFY presenceChanged)
    Q_PROPERTY(QString subscriberIdentity READ subscriberIdentity NOTIFY subscriberIdentityChanged)
    Q_PROPERTY(QString mobileCountryCode READ mobileCountryCode NOTIFY mobileCountryCodeChanged)
    Q_PROPERTY(QString mobileNetworkCode READ mobileNetworkCode NOTIFY mobileNetworkCodeChanged)
    Q_PROPERTY(QStringList subscriberNumbers READ subscriberNumbers WRITE setSubscriberNumbers NOTIFY subscriberNumbersChanged)
    Q_PROPERTY(PinType pinRequired READ pinRequired NOTIFY pinRequiredChanged)
    Q_PROPERTY(QVariantList lockedPins READ lockedPins NOTIFY lockedPinsChanged)
    Q_PROPERTY(QString cardIdentifier READ cardIdentifier NOTIFY cardIdentifierChanged)
    Q_PROPERTY(QStringList preferredLanguages READ preferredLanguages NOTIFY preferredLanguagesChanged)
    Q_PROPERTY(QVariantMap pinRetries READ pinRetries NOTIFY pinRetriesChanged)
    Q_PROPERTY(bool fixedDialing READ fixedDialing NOTIFY fixedDialingChanged)
    Q_PROPERTY(bool barredDialing READ barredDialing NOTIFY barredDialingChanged)

public:
    enum Error {
        NoError,
        NotImplementedError,
        InProgressError,
        InvalidArgumentsError,
        InvalidFormatError,
        FailedError,
        UnknownError
    };

    enum PinType {
        NoPin,
        SimPin,
        SimPin2,
        PhoneToSimPin,
        PhoneToFirstSimPin,
        NetworkPersonalizationPin,
        NetworkSubsetPersonalizationPin,
        ServiceProviderPersonalizationPin,
        CorporatePersonalizationPin,
        SimPuk,
        SimPuk2,
        PhoneToFirstSimPuk,
        NetworkPersonalizationPuk,
        NetworkSubsetPersonalizationPuk,
        CorporatePersonalizationPuk
    };

    explicit QOfonoSimManager(QObject *parent = 0);
    ~QOfonoSimManager();

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool present() const;
      QString subscriberIdentity() const;
      QString mobileCountryCode() const;
      QString mobileNetworkCode() const;
      QStringList subscriberNumbers() const;
      QVariantMap serviceNumbers() const; //
      PinType pinRequired() const;
      QVariantList lockedPins() const;
      QString cardIdentifier() const;
      QStringList preferredLanguages() const;
      QVariantMap pinRetries() const; //
      bool fixedDialing() const;
      bool barredDialing() const;

Q_SIGNALS:
      void modemPathChanged(const QString &modemPath);
      void presenceChanged(bool ispresent);
      void subscriberIdentityChanged(const QString &imsi);
      void mobileCountryCodeChanged(const QString &mcc);
      void mobileNetworkCodeChanged(const QString &mnc);
      void subscriberNumbersChanged(const QStringList &msisdns);
      void serviceNumbersChanged(const QVariantMap &sdns);
      void pinRequiredChanged(int pinType);
      void lockedPinsChanged(const QVariantList &pins);
      void cardIdentifierChanged(const QString &iccid);
      void preferredLanguagesChanged(const QStringList &languages);
      void pinRetriesChanged(const QVariantMap &pinRetries);
      void fixedDialingChanged(bool fixedDialing);
      void barredDialingChanged(bool barredDialing);

      void enterPinComplete(Error error, const QString &errorString);
      void resetPinComplete(Error error, const QString &errorString);
      void changePinComplete(Error error, const QString &errorString);
      void lockPinComplete(Error error, const QString &errorString);
      void unlockPinComplete(Error error, const QString &errorString);

public slots:
      void changePin(PinType pinType, const QString &oldpin, const QString &newpin);
      void enterPin(PinType pinType, const QString &pin);
      void resetPin(PinType pinType, const QString &puk, const QString &newpin);
      void lockPin(PinType pinType, const QString &pin);
      void unlockPin(PinType pinType, const QString &pin);
      QByteArray getIcon(quint8 id);

      void setSubscriberNumbers(const QStringList &numbers);

      static int minimumPinLength(PinType pinType);
      static int maximumPinLength(PinType pinType);
      static QString pinTypeToString(PinType pinType);
      static int pinTypeFromString(const QString &s);
      static bool isPukType(PinType pinType);
      static int pukToPin(PinType puk);

private:
      void updateProperty(const QString& property, const QVariant& value);
      QString pinRetryConfPath(PinType pinType) const;
      void updateSavedPinRetryCount(PinType pinType, bool hadWrongAttempt);
      void processPinOperationReply(Error error, int opType);
      Error errorNameToEnum(const QString &errorName);

    QOfonoSimManagerPrivate *d_ptr;

private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
    void changePinCallFinished(QDBusPendingCallWatcher *call);
    void enterPinCallFinished(QDBusPendingCallWatcher *call);
    void resetPinCallFinished(QDBusPendingCallWatcher *call);
    void lockPinCallFinished(QDBusPendingCallWatcher *call);
    void unlockPinCallFinished(QDBusPendingCallWatcher *call);
};

#endif // QOFONOSimManager_H
