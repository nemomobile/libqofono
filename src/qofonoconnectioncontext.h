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

#ifndef QOFONOCONNECTIONCONTEXT_H
#define QOFONOCONNECTIONCONTEXT_H

#include "qofonoobject.h"
#include "qofono_global.h"

//! This class is used to access ofono connman context API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob;f=doc/connman-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoConnectionContext : public QOfonoObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool provisioning READ provisioning NOTIFY provisioningChanged)
    Q_PROPERTY(QString accessPointName READ accessPointName WRITE setAccessPointName NOTIFY accessPointNameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString authMethod READ authMethod WRITE setAuthMethod NOTIFY authMethodChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString messageProxy READ messageProxy WRITE setMessageProxy NOTIFY messageProxyChanged)
    Q_PROPERTY(QString messageCenter READ messageCenter WRITE setMessageCenter NOTIFY messageCenterChanged)
    Q_PROPERTY(QVariantMap settings READ settings NOTIFY settingsChanged)
    Q_PROPERTY(QVariantMap IPv6Settings READ IPv6Settings NOTIFY IPv6SettingsChanged)

    Q_PROPERTY(QString contextPath READ contextPath WRITE setContextPath NOTIFY contextPathChanged)
    Q_PROPERTY(QString modemPath READ modemPath NOTIFY modemPathChanged)

public:
    explicit QOfonoConnectionContext(QObject *parent = 0);
    ~QOfonoConnectionContext();

    void setContextPath(const QString &idPath);
    QString contextPath() const;

    bool active() const;
    void setActive(bool);

    bool provisioning() const;

    QString accessPointName() const;
    void setAccessPointName(const QString &);

    QString type() const;
    void setType(const QString &);

    QString authMethod() const;
    void setAuthMethod(const QString &);

    QString username() const;
    void setUsername(const QString &);

    QString password() const;
    void setPassword(const QString &);

    QString protocol() const;
    void setProtocol(const QString &);

    QString name() const;
    void setName(const QString &);

    QString messageProxy() const;
    void setMessageProxy(const QString &);

    QString messageCenter() const;
    void setMessageCenter(const QString &);

    QVariantMap settings() const;
    QVariantMap IPv6Settings() const;

    bool isValid() const;
    QString modemPath() const;

    bool validateProvisioning(); //check provision against mbpi
    bool validateProvisioning(const QString &provider, const QString &mcc, const QString &mnc); //check provision against mbpi
    #if QT_VERSION < 0x050000
    void provision(const QString &provider, const QString &mcc, const QString &mnc, const QString &type="internet");
    #else
    void provision(const QString &provider, const QString &mcc, const QString &mnc, const QString &type=QStringLiteral("internet")); // provision context against mbpi
    #endif
    void provisionForCurrentNetwork(const QString &type);

    Q_INVOKABLE void disconnect();
    Q_INVOKABLE bool provision();

Q_SIGNALS:
    void disconnectRequested();
    void activeChanged(bool active);
    void provisioningChanged(bool provisioning);
    void accessPointNameChanged(const QString &apn);
    void nameChanged(const QString &name);
    void typeChanged(const QString &type);
    void authMethodChanged(const QString &method);
    void usernameChanged(const QString &uname);
    void passwordChanged(const QString &pw);
    void protocolChanged(const QString &proto);
    void messageProxyChanged(const QString &proxy);
    void messageCenterChanged(const QString &msc);
    void settingsChanged(const QVariantMap &settingsMap);
    void IPv6SettingsChanged(const QVariantMap &ipv6SettingsMap);
    void contextPathChanged(const QString &contextPath);
    void provisioningFinished(const QString &error);
    void modemPathChanged(const QString &path);

private Q_SLOTS:
    void onProvisionContextFinished(QDBusPendingCallWatcher *watch);
    void onManagerValidChanged(bool valid);

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    QVariant convertProperty(const QString &key, const QVariant &value);
    void propertyChanged(const QString &key, const QVariant &value);
    void objectPathChanged(const QString &path, const QVariantMap *properties);

private:
    class Private;
    Private *privateData() const;
};

#endif // QOFONOCONNECTIONCONTEXT_H
