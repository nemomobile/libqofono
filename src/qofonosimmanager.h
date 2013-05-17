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
class QOFONOSHARED_EXPORT QOfonoSimManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)
    Q_PROPERTY(bool present READ present NOTIFY presenceChanged)
    Q_PROPERTY(QString subscriberIdentity READ subscriberIdentity NOTIFY subscriberIdentityChanged)
    Q_PROPERTY(QString mobileCountryCode READ mobileCountryCode NOTIFY mobileCountryCodeChanged)
    Q_PROPERTY(QString mobileNetworkCode READ mobileNetworkCode NOTIFY mobileNetworkCodeChanged)
    Q_PROPERTY(QStringList subscriberNumbers READ subscriberNumbers WRITE setSubscriberNumbers NOTIFY subscriberNumbersChanged)
    Q_PROPERTY(QString pinRequired READ pinRequired NOTIFY pinRequiredChanged)
    Q_PROPERTY(QStringList lockedPins READ lockedPins NOTIFY lockedPinsChanged)
    Q_PROPERTY(QString cardIdentifier READ cardIdentifier NOTIFY cardIdentifierChanged)
    Q_PROPERTY(QStringList preferredLanguages READ preferredLanguages NOTIFY preferredLanguagesChanged)
    Q_PROPERTY(QVariantMap pinRetries READ pinRetries NOTIFY pinRetriesChanged)
    Q_PROPERTY(bool fixedDialing READ fixedDialing NOTIFY fixedDialingChanged)
    Q_PROPERTY(bool barredDialing READ barredDialing NOTIFY barredDialingChanged)

public:
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
      QString pinRequired() const;
      QStringList lockedPins() const;
      QString cardIdentifier() const;
      QStringList preferredLanguages() const;
      QVariantMap pinRetries() const; //
      bool fixedDialing() const;
      bool barredDialing() const;

Q_SIGNALS:
      void presenceChanged(bool ispresent);
      void subscriberIdentityChanged(const QString &imsi);
      void mobileCountryCodeChanged(const QString &mcc);
      void mobileNetworkCodeChanged(const QString &mnc);
      void subscriberNumbersChanged(const QStringList &msisdns);
      void serviceNumbersChanged(const QVariantMap &sdns);
      void pinRequiredChanged(const QString &pintype);
      void lockedPinsChanged(const QStringList &pins);
      void cardIdentifierChanged(const QString &iccid);
      void preferredLanguagesChanged(const QStringList &languages);
      void pinRetriesChanged(const QVariantMap &pinRetries);
      void fixedDialingChanged(bool fixedDialing);
      void barredDialingChanged(bool barredDialing);

      void enterPinComplete(bool success);
      void resetPinComplete(bool success);
      void changePinComplete(bool success);
      void lockPinComplete(bool success);
      void unlockPinComplete(bool success);

public slots:
      void changePin(const QString &pintype, const QString &oldpin, const QString &newpin);
      void enterPin(const QString &pintype, const QString &pin);
      void resetPin(const QString &pintype, const QString &puk, const QString &newpin);
      void lockPin(const QString &pintype, const QString &pin);
      void unlockPin(const QString &pintype, const QString &pin);
      QByteArray getIcon(quint8 id);

      void setSubscriberNumbers(const QStringList &numbers);
private:
    QOfonoSimManagerPrivate *d_ptr;
private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONOSimManager_H
