/****************************************************************************
**
** Copyright (C) 2013-2014 Jolla Ltd.
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

#include "qofononetworkoperator.h"
#include "dbus/ofononetworkoperator.h"

class QOfonoNetworkOperator::Private
{
public:
    bool registering;
    Private() : registering(false) {}
};

QOfonoNetworkOperator::QOfonoNetworkOperator(QObject *parent) :
    QOfonoObject(parent),
    d_ptr(new Private)
{
}

// Constructs the object with known set of properties, saves a roundtrip
// via D-Bus and makes the object valid immediately
QOfonoNetworkOperator::QOfonoNetworkOperator(const QString &path,
    const QVariantMap &properties, QObject *parent) :
    QOfonoObject(parent),
    d_ptr(new Private)
{
    setObjectPath(path, &properties);
}

QOfonoNetworkOperator::~QOfonoNetworkOperator()
{
    delete d_ptr;
}

QString QOfonoNetworkOperator::operatorPath() const
{
    return objectPath();
}

void QOfonoNetworkOperator::setOperatorPath(const QString &path)
{
    setObjectPath(path);
}

void QOfonoNetworkOperator::objectPathChanged(const QString &path, const QVariantMap *properties)
{
    QOfonoObject::objectPathChanged(path, properties);
    Q_EMIT operatorPathChanged(path);
}

QDBusAbstractInterface *QOfonoNetworkOperator::createDbusInterface(const QString &path)
{
    OfonoNetworkOperator *iface = new OfonoNetworkOperator("org.ofono", path, QDBusConnection::systemBus(), this);
    iface->setTimeout(120*1000); //increase dbus timeout as registration can take a long time
    return iface;
}

void QOfonoNetworkOperator::dbusInterfaceDropped()
{
    QOfonoObject::dbusInterfaceDropped();
    if (d_ptr->registering) {
        d_ptr->registering = false;
        Q_EMIT registeringChanged(d_ptr->registering);
    }
}

void QOfonoNetworkOperator::registerOperator()
{
    if (!d_ptr->registering) {
        OfonoNetworkOperator *iface = (OfonoNetworkOperator*)dbusInterface();
        if (iface) {
            d_ptr->registering = true;
            Q_EMIT registeringChanged(d_ptr->registering);
            connect(new QDBusPendingCallWatcher(iface->Register(), iface),
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(onRegisterFinished(QDBusPendingCallWatcher*)));
        }
    }
}

void QOfonoNetworkOperator::onRegisterFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<> reply(*watch);
    QOfonoNetworkOperator::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoNetworkOperator::registerOperator() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }
    d_ptr->registering = false;
    Q_EMIT registerComplete(error, errorString);
    Q_EMIT registeringChanged(d_ptr->registering);
}

bool QOfonoNetworkOperator::registering() const
{
    return d_ptr->registering;
}

QString QOfonoNetworkOperator::name() const
{
    return getString("Name");
}

QString QOfonoNetworkOperator::status() const
{
    return getString("Status");
}

QString QOfonoNetworkOperator::mcc() const
{
    return getString("MobileCountryCode");
}

QString QOfonoNetworkOperator::mnc() const
{
    return getString("MobileNetworkCode");
}

QStringList QOfonoNetworkOperator::technologies() const
{
    return getStringList("Technologies");
}

QString QOfonoNetworkOperator::additionalInfo() const
{
    return getString("AdditionalInformation");
}

void QOfonoNetworkOperator::propertyChanged(const QString &property, const QVariant &value)
{
    QOfonoObject::propertyChanged(property, value);
    if (property == QLatin1String("Name")) {
        Q_EMIT nameChanged(value.value<QString>());
    } else if (property == QLatin1String("Status")) {
        Q_EMIT statusChanged(value.value<QString>());
    } else if (property == QLatin1String("MobileCountryCode")) {
        Q_EMIT mccChanged(value.value<QString>());
    } else if (property == QLatin1String("MobileNetworkCode")) {
        Q_EMIT mncChanged(value.value<QString>());
    } else if (property == QLatin1String("Technologies")) {
        Q_EMIT technologiesChanged(value.value<QStringList>());
    } else if (property == QLatin1String("AdditionalInformation")) {
        Q_EMIT additionalInfoChanged(value.value<QString>());
    }
}

QOfonoNetworkOperator::Error QOfonoNetworkOperator::errorNameToEnum(const QString &errorName)
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
