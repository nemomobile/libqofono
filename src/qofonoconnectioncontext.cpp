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

#include <QtXmlPatterns/QXmlQuery>

#include "qofonoconnectioncontext.h"
#include "dbus/ofonoconnectioncontext.h"

#include "qofonomanager.h"
#include "qofonoconnectionmanager.h"
#include "qofononetworkregistration.h"

class QOfonoConnectionContextPrivate
{
public:
    QOfonoConnectionContextPrivate();
    OfonoConnectionContext *context;
    QString contextPath;
    QVariantMap properties;
    QString modemPath;

};

QOfonoConnectionContextPrivate::QOfonoConnectionContextPrivate() :
    context(0)
  , contextPath(QString())
{
}

QOfonoConnectionContext::QOfonoConnectionContext(QObject *parent) :
    QObject(parent),
    d_ptr(new QOfonoConnectionContextPrivate)
{
    qDebug() << Q_FUNC_INFO;
}

QOfonoConnectionContext::~QOfonoConnectionContext()
{
    delete d_ptr;
}


QString QOfonoConnectionContext::contextPath() const
{
    return d_ptr->contextPath;
}

QString QOfonoConnectionContext::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoConnectionContext::setContextPath(const QString &idPath)
{

    if (idPath != contextPath()) {
        if(d_ptr->context) {
            delete d_ptr->context;
            d_ptr->context = 0;
            d_ptr->properties.clear();
        }

qDebug() << Q_FUNC_INFO;
        d_ptr->context = new OfonoConnectionContext("org.ofono", idPath, QDBusConnection::systemBus(),this);
        if (d_ptr->context->isValid()) {
            d_ptr->contextPath = idPath;
            connect(d_ptr->context,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->context->GetProperties();
            reply.waitForFinished();
            if (reply.isError())
                Q_EMIT reportError(reply.error().message());

            d_ptr->properties = reply.value();
           Q_EMIT contextPathChanged(idPath);

        } else {
            Q_EMIT reportError("Context is not valid");
            qDebug() << Q_FUNC_INFO << "error Context is not valid";
        }
        QOfonoManager manager;
        if (manager.modems().count() > 0) {
            QOfonoConnectionManager connManager;
            Q_FOREACH (const QString &path, manager.modems()) {
                connManager.setModemPath(path);
                if (connManager.contexts().contains(idPath)) {
                    d_ptr->modemPath = path;
                    Q_EMIT modemPathChanged(path);
                    break;
                }
            }
        }
/*        if (!validateProvisioning()) {
            provisionForCurrentNetwork(this->type());
        } else {
            Q_EMIT reportError("Context provision is not valid");
            qDebug() << Q_FUNC_INFO << "error Context provision is not valid";
        }
*/
    }
}

void QOfonoConnectionContext::propertyChanged(const QString &property, const QDBusVariant &dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property, value);
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
        QVariantMap map;
        value.value<QDBusArgument>()>>map;
        Q_EMIT settingsChanged(map);
    } else if (property == QLatin1String("IPv6.Settings")) {
        QVariantMap map;
        value.value<QDBusArgument>()>>map;
        Q_EMIT IPv6SettingsChanged(map);
    }
}

bool QOfonoConnectionContext::active() const
{
    if (d_ptr->context)
        return d_ptr->properties["Active"].value<bool>();
    else
        return false;
}

QString QOfonoConnectionContext::accessPointName() const
{
    if (d_ptr->context)
        return d_ptr->properties["AccessPointName"].value<QString>();
    else
        return QString();
}

QString QOfonoConnectionContext::type() const
{
    if (d_ptr->context)
        return d_ptr->properties["Type"].value<QString>();
    else
        return QString();
}

QString QOfonoConnectionContext::username() const
{
    if (d_ptr->context)
        return d_ptr->properties["Username"].value<QString>();
    else
        return QString();
}

QString QOfonoConnectionContext::password() const
{
    if (d_ptr->context)
        return d_ptr->properties["Password"].value<QString>();
    else
        return QString();
}

QString QOfonoConnectionContext::protocol() const
{
    if (d_ptr->context)
        return d_ptr->properties["Protocol"].value<QString>();
    else
        return QString();
}

QString QOfonoConnectionContext::name() const
{
    if (d_ptr->context)
        return d_ptr->properties["Name"].value<QString>();
    else
        return QString();
}

QString QOfonoConnectionContext::messageProxy() const
{
    if (d_ptr->context)
        return d_ptr->properties["MessageProxy"].value<QString>();
    else
        return QString();
}

QString QOfonoConnectionContext::messageCenter() const
{
    if  (d_ptr->context)
        return d_ptr->properties["MessageCenter"].value<QString>();
    else
        return QString();
}

QVariantMap QOfonoConnectionContext::settings() const
{
    QVariantMap map;
    if (d_ptr->context)
        d_ptr->properties["Settings"].value<QDBusArgument>()>>map;

    return map;
}

QVariantMap QOfonoConnectionContext::IPv6Settings() const
{
    QVariantMap map;
    if (d_ptr->context)
        d_ptr->properties["IPv6Settings"].value<QDBusArgument>()>>map;

    return map;
}

void QOfonoConnectionContext::setActive(const bool value)
{
    QString str("Active");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionContext::setAccessPointName(const QString &value)
{
    if (!active()) {
        QString str("AccessPointName");
        QDBusVariant var(value);
        setOneProperty(str,var);
    } else {
     qDebug() << Q_FUNC_INFO  << "is active cannot set APN";
     Q_EMIT reportError("Context active");
    }
}

void QOfonoConnectionContext::setType(const QString &value)
{
    QString str("Type");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionContext::setUsername(const QString &value)
{
    QString str("Username");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionContext::setPassword(const QString &value)
{
    QString str("Password");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionContext::setProtocol(const QString &value)
{
    QString str("Protocol");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionContext::setName(const QString &value)
{
    QString str("Name");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionContext::setMessageProxy(const QString &value)
{
    QString str("MessageProxy");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionContext::setMessageCenter(const QString &value)
{
    QString str("MessageCenter");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

bool QOfonoConnectionContext::isValid() const
{
    return d_ptr->context->isValid();
}

void QOfonoConnectionContext::setOneProperty(const QString &prop, const QDBusVariant &var)
{
    if (d_ptr->context) {
        QDBusPendingReply <> reply = d_ptr->context->SetProperty(prop,var);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setPropertyFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoConnectionContext::setPropertyFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<> reply = *watch;

    if(reply.isError()) {
        qDebug() << Q_FUNC_INFO  << reply.error().message();
        Q_EMIT reportError(reply.error().message());
    }
    Q_EMIT setPropertyFinished();

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
    qDebug() << Q_FUNC_INFO << d_ptr->modemPath;
    if (d_ptr->modemPath.isEmpty())
        return false;

    QString provider;
    QString mcc;
    QString mnc;

    QOfonoNetworkRegistration netReg;
    netReg.setModemPath(d_ptr->modemPath);
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
    qDebug() << Q_FUNC_INFO << providerString;
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
    if (d_ptr->modemPath.isEmpty())
        return;

    QOfonoNetworkRegistration netReg;
    netReg.setModemPath(d_ptr->modemPath);

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

    //provider
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

//        qDebug() << "For APN" << apn;

        QString queryString("/serviceproviders/country/provider[ name =  '"+providerStr+"']//gsm[ network-id[@mcc = '"+mcc+"' and @mnc = '"+mnc+"']]/apn [ @value = '"+apn+ "']/");

        //type
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

        //name
            query.setQuery(queryString+"name/string()");
        QString nameStr;
        query.evaluateTo(&nameStr);
        nameStr = nameStr.simplified();
        if (nameStr.isEmpty())
            nameStr = providerStr;

        //username
        query.setQuery(queryString+"username/string()");
        QString usernameStr;
        query.evaluateTo(&usernameStr);
        usernameStr = usernameStr.simplified();

        //password
        query.setQuery(queryString+"password/string()");
        QString passwordStr;
        query.evaluateTo(&passwordStr);

        passwordStr = passwordStr.simplified();

        if (!nameStr.isEmpty()) {
             QDBusPendingReply <> reply = d_ptr->context->SetProperty("Name",QDBusVariant(nameStr));
             reply.waitForFinished();
             d_ptr->properties["Name"] = nameStr;
        }

        if (!type.isEmpty()) {
            QDBusPendingReply <> reply = d_ptr->context->SetProperty("Type",QDBusVariant(type));
            reply.waitForFinished();
            d_ptr->properties["Type"] = type;
        }

        if (!apn.isEmpty()) {
            QDBusPendingReply <> reply = d_ptr->context->SetProperty("AccessPointName",QDBusVariant(apn));
            reply.waitForFinished();
            d_ptr->properties["AccessPointName"] = apn;
        }

        if (!passwordStr.isEmpty()) {
            QDBusPendingReply <> reply = d_ptr->context->SetProperty("Password",QDBusVariant(passwordStr));
            reply.waitForFinished();
            d_ptr->properties["Password"] = passwordStr;
        }
        if (!usernameStr.isEmpty()) {
            QDBusPendingReply <> reply = d_ptr->context->SetProperty("Username",QDBusVariant(usernameStr));
            reply.waitForFinished();
            d_ptr->properties["Username"] = usernameStr;
        }
        break;
    }
    Q_EMIT setPropertyFinished();
    Q_EMIT provisioningFinished();
}

