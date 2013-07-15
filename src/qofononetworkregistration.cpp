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

#include "qofononetworkregistration.h"
#include "dbus/ofononetworkregistration.h"


QDBusArgument &operator<<(QDBusArgument &argument, const OfonoPathProps &op)
{
    argument.beginStructure();
    argument << op.path << op.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, OfonoPathProps &op)
{
    argument.beginStructure();
    argument >> op.path >> op.properties;
    argument.endStructure();
    return argument;
}



class QOfonoNetworkRegistrationPrivate
{
public:
    QOfonoNetworkRegistrationPrivate();
    OfonoNetworkRegistration *networkRegistration;
    QString modemPath;
    QVariantMap properties;
    QStringList networkOperators;
    QArrayOfPathProps operatorArray;
};

QOfonoNetworkRegistrationPrivate::QOfonoNetworkRegistrationPrivate() :
    networkRegistration(0)
  , modemPath(QString())
  ,  networkOperators(QStringList())
{
    qDBusRegisterMetaType<OfonoPathProps>();
    qDBusRegisterMetaType<QArrayOfPathProps>();
}

QOfonoNetworkRegistration::QOfonoNetworkRegistration(QObject *parent) :
    QObject(parent),
    d_ptr(new QOfonoNetworkRegistrationPrivate)
{
}

QOfonoNetworkRegistration::~QOfonoNetworkRegistration()
{
    delete d_ptr;
}

void QOfonoNetworkRegistration::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    QStringList removedProperties = d_ptr->properties.keys();

    delete d_ptr->networkRegistration;
    d_ptr->networkRegistration = new OfonoNetworkRegistration("org.ofono", path, QDBusConnection::systemBus(),this);

    if (d_ptr->networkRegistration->isValid()) {
        d_ptr->modemPath = path;
        d_ptr->networkRegistration->setTimeout(1000 * 120); //increase dbus timeout as scanning can take a long time

        connect(d_ptr->networkRegistration,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));

        QVariantMap properties = d_ptr->networkRegistration->GetProperties().value();
        for (QVariantMap::ConstIterator it = properties.constBegin();
             it != properties.constEnd(); ++it) {
            updateProperty(it.key(), it.value());
            removedProperties.removeOne(it.key());
        }

        Q_EMIT modemPathChanged(path);
    }

    foreach (const QString &p, removedProperties)
        updateProperty(p, QVariant());
}

QString QOfonoNetworkRegistration::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoNetworkRegistration::registration()
{
    if (d_ptr->networkRegistration)
        d_ptr->networkRegistration->Register();
}

QStringList QOfonoNetworkRegistration::networkOperators()
{
    if (d_ptr->networkRegistration) {
        QDBusPendingReply<QArrayOfPathProps> pending = d_ptr->networkRegistration->GetOperators();
        pending.waitForFinished();

        if (!pending.isError()) {
            scanFinish(pending.value());
        } else {
            qDebug() << Q_FUNC_INFO << pending.error().message();
        }
    }
    return d_ptr->networkOperators;
}

void QOfonoNetworkRegistration::scan()
{
    if (d_ptr->networkRegistration) {
        QList<QVariant> arguments;
        if (!d_ptr->networkRegistration->callWithCallback(QLatin1String("Scan"),
                                                          arguments,
                                                          this,
                                                          SLOT(scanFinish(QArrayOfPathProps)),
                                                          SLOT(scanError(const QDBusError&)))) {
            qDebug() << "Failed to queue scan call";
        }
    }
}

QString QOfonoNetworkRegistration::mode() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["Mode"].value<QString>();
    else
        return QString();
}

QString QOfonoNetworkRegistration::status() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["Status"].value<QString>();
    else
        return QString();
}

uint QOfonoNetworkRegistration::locationAreaCode() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["LocalAreaCode"].value<uint>();
    else
        return 0;
}

uint QOfonoNetworkRegistration::cellId() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["CellId"].value<uint>();
    else
        return 0;
}

QString QOfonoNetworkRegistration::mcc() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["MobileCountryCode"].value<QString>();
    else
        return QString();

}

QString QOfonoNetworkRegistration::mnc() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["MobileNetworkCode"].value<QString>();
    else
        return QString();

}

QString QOfonoNetworkRegistration::technology() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["Technology"].value<QString>();
    else
        return QString();
}

QString QOfonoNetworkRegistration::name() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["Name"].value<QString>();
    else
        return QString();
}

uint QOfonoNetworkRegistration::strength() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["Strength"].value<uint>();
    else
        return 0;
}

QString QOfonoNetworkRegistration::baseStation() const
{
    if ( d_ptr->networkRegistration)
        return d_ptr->properties["BaseStation"].value<QString>();
    else
        return QString();
}

void QOfonoNetworkRegistration::propertyChanged(const QString &property, const QDBusVariant &dbusvalue)
{
    updateProperty(property, dbusvalue.variant());
}

void QOfonoNetworkRegistration::updateProperty(const QString &property, const QVariant &value)
{
    if (d_ptr->properties.value(property) == value)
        return;

    if (value.isValid())
        d_ptr->properties.insert(property, value);
    else
        d_ptr->properties.remove(property);

    if (property == QLatin1String("Mode")) {
        Q_EMIT modeChanged(value.value<QString>());
    } else if (property == QLatin1String("Status")) {
        Q_EMIT statusChanged(value.value<QString>());
    } else if (property == QLatin1String("LocationAreaCode")) {
        Q_EMIT locationAreaCodeChanged(value.value<uint>());
    } else if (property == QLatin1String("CellId")) {
        Q_EMIT cellIdChanged(value.value<uint>());
    } else if (property == QLatin1String("MobileCountryCode")) {
        Q_EMIT mccChanged(value.value<QString>());
    } else if (property == QLatin1String("MobileNetworkCode")) {
        Q_EMIT mncChanged(value.value<QString>());
    } else if (property == QLatin1String("Technology")) {
        Q_EMIT technologyChanged(value.value<QString>());
    } else if (property == QLatin1String("Name")) {
        Q_EMIT nameChanged(value.value<QString>());
    } else if (property == QLatin1String("Strength")) {
        Q_EMIT strengthChanged(value.value<uint>());
    } else if (property == QLatin1String("BaseStation")) {
        Q_EMIT baseStationChanged(value.value<QString>());
    }
}

void QOfonoNetworkRegistration::scanFinish(const QArrayOfPathProps &list)
{
    bool changed = false;
    d_ptr->operatorArray = list;
    QString current;
    foreach(OfonoPathProps netop, list) {
        // don't add forbidden operators
      //  if (netop.properties["Status"].toString() != QLatin1String("forbidden"))
            if (!d_ptr->networkOperators.contains(netop.path.path())) {
                d_ptr->networkOperators.append(netop.path.path());
                 if (netop.properties["Status"].toString() == QLatin1String("current")) {
                 current == netop.path.path();
                 }
                changed = true;
            }
    }
    if (changed) {
        Q_EMIT networkOperatorsChanged(d_ptr->networkOperators);
        Q_EMIT currentOperatorPathChanged(current);
    }
    Q_EMIT scanFinished();
}

void QOfonoNetworkRegistration::scanError(QDBusError error)
{
    Q_EMIT scanError(error.message());
}

QString QOfonoNetworkRegistration::currentOperatorPath()
{
    foreach(OfonoPathProps netop, d_ptr->operatorArray) {
        if (netop.properties["Status"].toString() == QLatin1String("current")) {
            return netop.path.path();
        }
    }
    return QString();
}

bool QOfonoNetworkRegistration::isValid() const
{
    return d_ptr->networkRegistration->isValid();
}
