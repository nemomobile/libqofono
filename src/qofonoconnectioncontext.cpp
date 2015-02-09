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

#include <QtXmlPatterns/QXmlQuery>

#include "qofonoconnectioncontext.h"
#include "qofononetworkregistration.h"
#include "ofono_connection_context_interface.h"

#define SUPER QOfonoObject

class QOfonoConnectionContext::Private : public SUPER::ExtData
{
public:
    bool provisioning;
    Private() : provisioning(false) {}
};

QOfonoConnectionContext::QOfonoConnectionContext(QObject *parent) :
    SUPER(new Private, parent)
{
}

QOfonoConnectionContext::~QOfonoConnectionContext()
{
}

QDBusAbstractInterface *QOfonoConnectionContext::createDbusInterface(const QString &path)
{
    return new OfonoConnectionContext("org.ofono", path, QDBusConnection::systemBus(),this);
}

void QOfonoConnectionContext::objectPathChanged(const QString &path, const QVariantMap *properties)
{
    SUPER::objectPathChanged(path, properties);
    Q_EMIT contextPathChanged(path);
}

QString QOfonoConnectionContext::contextPath() const
{
    return objectPath();
}

QString QOfonoConnectionContext::modemPath() const
{
    QString path = objectPath();
    int lastSlash = path.lastIndexOf('/');
    return (lastSlash > 0) ? path.left(lastSlash) : QString();
}

void QOfonoConnectionContext::setContextPath(const QString &path)
{
    if (path != objectPath()) {
        // Modem path is redundant but supported for historical reasons
        QString oldModemPath(modemPath());
        setObjectPath(path);
        QString newModemPath(modemPath());
        if (oldModemPath != newModemPath) {
            Q_EMIT modemPathChanged(newModemPath);
        }
    }
}

QVariant QOfonoConnectionContext::convertProperty(const QString &key, const QVariant &value)
{
    if (key == QLatin1String("Settings") ||
        key == QLatin1String("IPv6.Settings")) {
        QVariantMap map;
        value.value<QDBusArgument>() >> map;
        return map;
    } else {
        return SUPER::convertProperty(key, value);
    }
}

void QOfonoConnectionContext::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("Active")) {
        Q_EMIT activeChanged(value.value<bool>());
    } else if (property == QLatin1String("Name")) {
        Q_EMIT nameChanged(value.value<QString>());
    } else if (property == QLatin1String("AccessPointName")) {
        Q_EMIT accessPointNameChanged(value.value<QString>());
    } else if (property == QLatin1String("Type")) {
        Q_EMIT typeChanged(value.value<QString>());
    } else if (property == QLatin1String("Username")) {
        Q_EMIT usernameChanged(value.value<QString>());
    } else if (property == QLatin1String("Password")) {
        Q_EMIT passwordChanged(value.value<QString>());
    } else if (property == QLatin1String("Protocol")) {
        Q_EMIT protocolChanged(value.value<QString>());
    } else if (property == QLatin1String("MessageProxy")) {
        Q_EMIT messageProxyChanged(value.value<QString>());
    } else if (property == QLatin1String("MessageCenter")) {
        Q_EMIT messageCenterChanged(value.value<QString>());
    } else if (property == QLatin1String("Settings")) {
        Q_EMIT settingsChanged(getVariantMap("Settings"));
    } else if (property == QLatin1String("IPv6.Settings")) {
        Q_EMIT IPv6SettingsChanged(getVariantMap("IPv6.Settings"));
    }
}

bool QOfonoConnectionContext::active() const
{
    return getBool("Active");
}

QString QOfonoConnectionContext::accessPointName() const
{
    return getString("AccessPointName");
}

QString QOfonoConnectionContext::type() const
{
    return getString("Type");
}

QString QOfonoConnectionContext::username() const
{
    return getString("Username");
}

QString QOfonoConnectionContext::password() const
{
    return getString("Password");
}

QString QOfonoConnectionContext::protocol() const
{
    return getString("Protocol");
}

QString QOfonoConnectionContext::name() const
{
    return getString("Name");
}

QString QOfonoConnectionContext::messageProxy() const
{
    return getString("MessageProxy");
}

QString QOfonoConnectionContext::messageCenter() const
{
    return getString("MessageCenter");
}

QVariantMap QOfonoConnectionContext::settings() const
{
    return getVariantMap("Settings");
}

QVariantMap QOfonoConnectionContext::IPv6Settings() const
{
    return getVariantMap("IPv6.Settings");
}

void QOfonoConnectionContext::setActive(const bool value)
{
    // need someway to tell ui that someone wants to disconnect
    if (!value) {
        Q_EMIT disconnectRequested();
    }
    setProperty("Active", value);
}

void QOfonoConnectionContext::setAccessPointName(const QString &value)
{
    setProperty("AccessPointName", value);
}

void QOfonoConnectionContext::setType(const QString &value)
{
    setProperty("Type", value);
}

void QOfonoConnectionContext::setUsername(const QString &value)
{
    setProperty("Username", value);
}

void QOfonoConnectionContext::setPassword(const QString &value)
{
    setProperty("Password", value);
}

void QOfonoConnectionContext::setProtocol(const QString &value)
{
    setProperty("Protocol", value);
}

void QOfonoConnectionContext::setName(const QString &value)
{
    setProperty("Name", value);
}

void QOfonoConnectionContext::setMessageProxy(const QString &value)
{
    setProperty("MessageProxy", value);
}

void QOfonoConnectionContext::setMessageCenter(const QString &value)
{
    setProperty("MessageCenter", value);
}

void QOfonoConnectionContext::disconnect()
{
    Q_EMIT disconnectRequested();
    setPropertySync("Active", false);
}

bool QOfonoConnectionContext::isValid() const
{
    return SUPER::isValid();
}

bool QOfonoConnectionContext::provisioning() const
{
    return privateData()->provisioning;
}

QOfonoConnectionContext::Private *QOfonoConnectionContext::privateData() const
{
    return (QOfonoConnectionContext::Private*)SUPER::extData();
}

bool QOfonoConnectionContext::provision()
{
    Private *priv = privateData();
    if (!priv->provisioning) {
        OfonoConnectionContext *iface = (OfonoConnectionContext*)dbusInterface();
        if (iface) {
            priv->provisioning = true;
            Q_EMIT provisioningChanged(true);
            connect(new QDBusPendingCallWatcher(iface->ProvisionContext(), iface),
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(onProvisionContextFinished(QDBusPendingCallWatcher*)));
            return true;
        }
    }
    return false;
}

void QOfonoConnectionContext::onProvisionContextFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<> reply(*watch);
    QString error;
    if (reply.isError()) {
        qWarning() << "Provisioning failed:" << reply.error();
        error = reply.error().name();
    }
    privateData()->provisioning = false;
    Q_EMIT provisioningChanged(false);
    Q_EMIT provisioningFinished(error);
}

/*
 * These provisioning functions use the mobile broadband provider info database available from this url:
 * https://git.gnome.org/browse/mobile-broadband-provider-info/
 *
 **/

/*
 * Tries to validate the context against the current registered network
 **/
//check provision against mbpi
bool QOfonoConnectionContext::validateProvisioning()
{
    QString modem(modemPath());
    qDebug() << modem;
    if (modem.isEmpty())
        return false;

    QOfonoNetworkRegistration netReg;
    netReg.setModemPath(modem);
    // This won't work because ofono queries are asynchronous:
    if (netReg.status() == "registered")
        return validateProvisioning(netReg.networkOperators().at(0),netReg.mcc(),netReg.mnc());
    return false;
}

/*
 * Tries to validate the context using the provider, mcc and mnc arguments
 **/
//check provision against mbpi
bool QOfonoConnectionContext::validateProvisioning(const QString &providerString, const QString &mcc, const QString &mnc)
{
    qDebug() << providerString;
    QXmlQuery query;
    QString provider = providerString;

    query.setFocus(QUrl("/usr/share/mobile-broadband-provider-info/serviceproviders.xml"));

    if (provider.contains("\'")) {
        provider = provider.replace("\'", "&apos;");
    }

    //provider
    query.setQuery("/serviceproviders/country/provider[ name =  '"+provider+"']/string()");
    QString providerName;
    query.evaluateTo(&providerName);
    providerName = providerName.simplified();

    if (providerName.isEmpty() && provider.at(1).isLower() ) {
        //try with uppercase
        provider[0] = provider.at(0).toUpper();

        query.setQuery("/serviceproviders/country/provider[ name =  '"+provider+"']/string()");
        query.evaluateTo(&providerName);
        if (providerName.isEmpty()) {
            qDebug() << "provider not found";
            return false;
        }
    }

    // apn
    query.setQuery("/serviceproviders/country/provider[ name =  '"+provider+"']/gsm[ network-id [ @mcc = '"+mcc+"' and @mnc = '"+mnc+"' ] ]/apn/@value/string()");
    QStringList accessPointNameList;
    query.evaluateTo(&accessPointNameList);

    if (accessPointNameList.isEmpty()) {
        qDebug() << "APN not found";
        return false;
    }

    QString apn = accessPointName();
    if (!accessPointNameList.contains(apn)) {
        qDebug() << "APN not valid"<< accessPointName();
        return false;
    }


    QString queryString("/serviceproviders/country/provider[ name =  '"+provider+"']//gsm[ network-id[@mcc = '"+mcc+"' and @mnc = '"+mnc+"']]/apn [ @value = '"+apn+ "']/");

    //type
    query.setQuery(queryString+"usage/@type/string()");
    QString typeStr;
    query.evaluateTo(&typeStr);
    typeStr = typeStr.simplified();
    if (!typeStr.isEmpty() && !typeStr.contains(type())) {
        qDebug() << "type is not the same"
                 << typeStr << type();

        return false;
    }

    //dns
    // ofono dns properties are read only

    //username
    query.setQuery(queryString+"username/string()");
    QString usernameStr;
    query.evaluateTo(&usernameStr);
    if (!usernameStr.contains(username())) {
        qDebug() << "username is not the same";
        return false;
    }

    //password
    query.setQuery(queryString+"password/string()");
    QString passwordStr;
    query.evaluateTo(&passwordStr);
    if (!passwordStr.contains(password())) {
        qDebug() << "password is not the same";
        return false;
    }

    //we got here, must be ok
    return true;
}

void QOfonoConnectionContext::provisionForCurrentNetwork(const QString &type)
{
    QString modem(modemPath());
    if (modem.isEmpty())
        return;

    QOfonoNetworkRegistration netReg;
    netReg.setModemPath(modem);

    if (netReg.status() == "registered")
        provision(netReg.name(), netReg.mcc(),netReg.mnc(), type);
}

/*
 * Tries to provision the context using the provider, mcc and mnc given.
 *
 * In the case of multiple apn's for given provider and type, this will provision the context
 * using the first available apn.
 *
 * The only way to see if this is a working context is to try to activate the context.
 *
 **/
// provision context against mbpi
void QOfonoConnectionContext::provision(const QString &provider, const QString &mcc, const QString &mnc, const QString &type)
{
    QXmlQuery query;
    query.setFocus(QUrl("/usr/share/mobile-broadband-provider-info/serviceproviders.xml"));

    QString providerStr = provider;
    if (providerStr.contains("\'")) {
        providerStr.replace("\'", "&apos;");
    }

    // provider
    query.setQuery("/serviceproviders/country/provider[ name =  '"+providerStr+"']/string()");
    QString providerName;
    query.evaluateTo(&providerName);
    providerName = providerName.simplified();

    if (providerName.isEmpty() && providerStr.at(0).isLower() ) {
        //try with uppercase first letter
        providerStr[0] = providerStr.at(0).toUpper();

        query.setQuery("/serviceproviders/country/provider  [ name =  '"+providerStr+"']/string()");
        query.evaluateTo(&providerName);
        if (providerName.isEmpty()) {
            Q_EMIT reportError("Provider not found");
            return;
        }
    }

    // apn
    query.setQuery("/serviceproviders/country/provider[ name =  '"+providerStr+"']/gsm[ network-id [ @mcc = '"+mcc+"' and @mnc = '"+mnc+"' ] ]/apn/@value/string()");
    QStringList accessPointNameList;
    query.evaluateTo(&accessPointNameList);

    if (accessPointNameList.isEmpty()) {
        Q_EMIT reportError("APN not found");
        return;
    }

    Q_FOREACH( const QString &apn, accessPointNameList) {

        QString queryString("/serviceproviders/country/provider[ name =  '"+providerStr+"']//gsm[ network-id[@mcc = '"+mcc+"' and @mnc = '"+mnc+"']]/apn [ @value = '"+apn+ "']/");

        // type
        query.setQuery(queryString+"usage/@type/string()");
        QString typeStr;
        query.evaluateTo(&typeStr);
        typeStr = typeStr.simplified();
        if (typeStr.isEmpty()) {
            typeStr = "internet";
        }
        if (typeStr != type) {
            qDebug() << typeStr <<  "continue";
            continue;
        }

        // name
        query.setQuery(queryString+"name/string()");
        QString nameStr;
        query.evaluateTo(&nameStr);
        nameStr = nameStr.simplified();
        if (nameStr.isEmpty())
            nameStr = providerStr;

        // username
        query.setQuery(queryString+"username/string()");
        QString usernameStr;
        query.evaluateTo(&usernameStr);
        usernameStr = usernameStr.simplified();

        // password
        query.setQuery(queryString+"password/string()");
        QString passwordStr;
        query.evaluateTo(&passwordStr);

        passwordStr = passwordStr.simplified();

        if (!nameStr.isEmpty()) {
             setPropertySync("Name", nameStr);
        }

        if (!type.isEmpty()) {
            setPropertySync("Type", type);
        }

        if (!apn.isEmpty()) {
            setPropertySync("AccessPointName", apn);
        }

        if (!passwordStr.isEmpty()) {
            setPropertySync("Password", passwordStr);
        }

        if (!usernameStr.isEmpty()) {
            setPropertySync("Username", usernameStr);
        }
        break;
    }
}
