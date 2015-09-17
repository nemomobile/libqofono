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

#ifndef QOFONOMODEM_H
#define QOFONOMODEM_H

#include "qofonoobject.h"
#include "qofono_global.h"

//! This class is used to access an oFono modem object and its properties
/*!
 * oFono modem properties are documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/modem-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoModem : public QOfonoObject
{
    Q_OBJECT

    Q_PROPERTY(bool powered READ powered WRITE setPowered NOTIFY poweredChanged)
    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged) //flightmode
    Q_PROPERTY(bool lockdown READ lockdown WRITE setLockdown NOTIFY lockdownChanged)
    Q_PROPERTY(bool emergency READ emergency NOTIFY emergencyChanged)

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString manufacturer READ manufacturer NOTIFY manufacturerChanged)
    Q_PROPERTY(QString model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString revision READ revision NOTIFY revisionChanged)
    Q_PROPERTY(QString serial READ serial NOTIFY serialChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)

    Q_PROPERTY(QStringList features READ features NOTIFY featuresChanged)
    Q_PROPERTY(QStringList interfaces READ interfaces NOTIFY interfacesChanged)

    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)

public:
    explicit QOfonoModem(QObject *parent = 0);
    ~QOfonoModem();

    QString modemPath() const;
    void setModemPath(const QString &path);
    
    bool powered() const;
    void setPowered(bool powered);

    bool online() const;
    void setOnline(bool online);

    bool lockdown() const;
    void setLockdown(bool lockdown);

    bool emergency() const;

    QString name() const;
    QString manufacturer() const;
    QString model() const;
    QString revision() const;
    QString serial() const;
    QString type() const;

    QStringList features() const;
    QStringList interfaces() const;

    bool isValid() const;

    // If you use this, remember to keep a QSharedPointer to it, otherwise it may be destroyed.
    static QSharedPointer<QOfonoModem> instance(const QString &modemPath);

Q_SIGNALS:
    void poweredChanged(bool powered);
    void onlineChanged(bool online);
    void lockdownChanged(bool lockdown);
    void emergencyChanged(bool emergency);
    void nameChanged(const QString &name);
    void manufacturerChanged(const QString &manufacturer);
    void modelChanged(const QString &model);
    void revisionChanged(const QString &revision);
    void serialChanged(const QString &serial);
    void typeChanged(const QString &type);
    void featuresChanged(const QStringList &features);
    void interfacesChanged(const QStringList &interfaces);
    void modemPathChanged(const QString &path);

private Q_SLOTS:
    bool checkModemPathValidity();

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void propertyChanged(const QString &key, const QVariant &value);
    void objectPathChanged(const QString &path, const QVariantMap *properties);

private:
    class Private;
    Private* privateData() const;
};

#endif // QOFONOMODEM_H
