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

#include "qofononetworkregistration.h"
#include "qofononetworkoperator.h"
#include "dbus/ofononetworkregistration.h"

class QOfonoNetworkRegistration::Private
{
public:
    bool initialized;
    bool scanning;
    QOfonoNetworkOperator* currentOperator;
    QHash<QString,QOfonoNetworkOperator*> networkOperators;
    QStringList operatorPaths;

    Private() : initialized(false), scanning(false), currentOperator(NULL) {}
};

QOfonoNetworkRegistration::QOfonoNetworkRegistration(QObject *parent) :
    QOfonoModemInterface(OfonoNetworkRegistration::staticInterfaceName(), parent),
    d_ptr(new Private)
{
    QOfonoDbusTypes::registerObjectPathProperties();
}

QOfonoNetworkRegistration::~QOfonoNetworkRegistration()
{
    delete d_ptr;
}

bool QOfonoNetworkRegistration::isValid() const
{
    return d_ptr->initialized && QOfonoModemInterface::isValid();
}

bool QOfonoNetworkRegistration::scanning() const
{
    return d_ptr->scanning;
}

QDBusAbstractInterface *QOfonoNetworkRegistration::createDbusInterface(const QString &path)
{
    OfonoNetworkRegistration* iface = new OfonoNetworkRegistration("org.ofono", path, QDBusConnection::systemBus(), this);
    iface->setTimeout(120*1000); //increase dbus timeout as scanning can take a long time
    connect(new QDBusPendingCallWatcher(iface->GetOperators(), iface),
        SIGNAL(finished(QDBusPendingCallWatcher*)),
        SLOT(onGetOperatorsFinished(QDBusPendingCallWatcher*)));
    return iface;
}

void QOfonoNetworkRegistration::dbusInterfaceDropped()
{
    QOfonoModemInterface::dbusInterfaceDropped();
    d_ptr->initialized = false;
    if (d_ptr->scanning) {
        d_ptr->scanning = false;
        scanningChanged(false);
    }
    if (!d_ptr->networkOperators.isEmpty()) {
        qDeleteAll(d_ptr->networkOperators.values());
        d_ptr->operatorPaths.clear();
        d_ptr->networkOperators.clear();
        d_ptr->currentOperator = NULL;
        Q_EMIT networkOperatorsChanged(d_ptr->operatorPaths);
    }
}

void QOfonoNetworkRegistration::registration()
{
    OfonoNetworkRegistration *iface = (OfonoNetworkRegistration*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(iface->Register(), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onRegistrationFinished(QDBusPendingCallWatcher*)));
    } else {
        Q_EMIT registrationError(QStringLiteral("Error.ServiceUnknown"));
    }
}

void QOfonoNetworkRegistration::onRegistrationFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<> reply(*watch);
    if (!reply.isError()) {
        Q_EMIT registrationFinished();
    } else {
        Q_EMIT registrationError(reply.error().message());
    }
}

QStringList QOfonoNetworkRegistration::networkOperators() const
{
    return d_ptr->operatorPaths;
}

QOfonoNetworkOperator* QOfonoNetworkRegistration::networkOperator(const QString &path) const
{
    return d_ptr->networkOperators[path];
}

QString QOfonoNetworkRegistration::currentOperatorPath()
{
    return d_ptr->currentOperator ? d_ptr->currentOperator->operatorPath() : QString();
}

void QOfonoNetworkRegistration::scan()
{
    if (!d_ptr->scanning) {
        OfonoNetworkRegistration *iface = (OfonoNetworkRegistration*)dbusInterface();
        if (iface) {
            d_ptr->scanning = true;
            scanningChanged(true);
            connect(new QDBusPendingCallWatcher(iface->Scan(), iface),
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(onScanFinished(QDBusPendingCallWatcher*)));
        }
    }
}

QString QOfonoNetworkRegistration::mode() const
{
    return getString("Mode");
}

QString QOfonoNetworkRegistration::status() const
{
    return getString("Status");
}

uint QOfonoNetworkRegistration::locationAreaCode() const
{
    return getUInt("LocalAreaCode");
}

uint QOfonoNetworkRegistration::cellId() const
{
    return getUInt("CellId");
}

QString QOfonoNetworkRegistration::mcc() const
{
    return getString("MobileCountryCode");
}

QString QOfonoNetworkRegistration::mnc() const
{
    return getString("MobileNetworkCode");
}

QString QOfonoNetworkRegistration::technology() const
{
    return getString("Technology");
}

QString QOfonoNetworkRegistration::name() const
{
    return getString("Name");
}

uint QOfonoNetworkRegistration::strength() const
{
    return getUInt("Strength");
}

QString QOfonoNetworkRegistration::baseStation() const
{
    return getString("BaseStation");
}

void QOfonoNetworkRegistration::propertyChanged(const QString &property, const QVariant &value)
{
    QOfonoModemInterface::propertyChanged(property, value);
    if (property == QLatin1String("Mode")) {
        Q_EMIT modeChanged(value.toString());
    } else if (property == QLatin1String("Status")) {
        Q_EMIT statusChanged(value.toString());
    } else if (property == QLatin1String("LocationAreaCode")) {
        Q_EMIT locationAreaCodeChanged(value.toUInt());
    } else if (property == QLatin1String("CellId")) {
        Q_EMIT cellIdChanged(value.toUInt());
    } else if (property == QLatin1String("MobileCountryCode")) {
        Q_EMIT mccChanged(value.toString());
    } else if (property == QLatin1String("MobileNetworkCode")) {
        Q_EMIT mncChanged(value.toString());
    } else if (property == QLatin1String("Technology")) {
        Q_EMIT technologyChanged(value.toString());
    } else if (property == QLatin1String("Name")) {
        Q_EMIT nameChanged(value.toString());
    } else if (property == QLatin1String("Strength")) {
        Q_EMIT strengthChanged(value.toUInt());
    } else if (property == QLatin1String("BaseStation")) {
        Q_EMIT baseStationChanged(value.toString());
    }
}

void QOfonoNetworkRegistration::setOperators(const ObjectPathPropertiesList &list)
{
    QString oldPath = currentOperatorPath();

    QStringList paths;
    QList<QOfonoNetworkOperator*> newOperators;
    QOfonoNetworkOperator* currentOperator = NULL;
    bool listChanged = false;
    int i;

    // Find new operators
    for (i=0; i<list.count(); i++) {
        QString path = list[i].path.path();
        paths.append(path);
        if (!d_ptr->networkOperators.contains(path)) {
            // No need to query the properties as we already have the.
            // The object becomes valid immediately.
            QOfonoNetworkOperator* op = new QOfonoNetworkOperator(path,
                list[i].properties, this);
            newOperators.append(op);
            connect(op, SIGNAL(statusChanged(QString)),
                SLOT(onOperatorStatusChanged(QString)));
            if (op->status() == "current") {
                currentOperator = op;
            }
            listChanged = true;
        }
    }

    // Remove operators that are no longer on the list
    for (i=d_ptr->operatorPaths.count()-1; i>=0; i--) {
        QString path = d_ptr->operatorPaths[i];
        if (!paths.contains(path)) {
            QOfonoNetworkOperator* op = d_ptr->networkOperators[path];
            if (op == d_ptr->currentOperator) d_ptr->currentOperator = NULL;
            d_ptr->operatorPaths.removeAt(i);
            d_ptr->networkOperators.remove(path);
            delete op;
            listChanged = true;
        }
    }

    // Append new operators to the end of the list
    for (i=0; i<newOperators.count(); i++) {
        QOfonoNetworkOperator* op = newOperators[i];
        d_ptr->operatorPaths.append(op->operatorPath());
        d_ptr->networkOperators.insert(op->operatorPath(), op);
    }

    // Replace the current operator if it has changed
    if (currentOperator) d_ptr->currentOperator = currentOperator;

    // Fire necessary events
    if (listChanged) {
        Q_EMIT networkOperatorsChanged(d_ptr->operatorPaths);
    }
    QString currentPath = currentOperatorPath();
    if (currentPath != oldPath) {
        Q_EMIT currentOperatorPathChanged(currentPath);
    }
}

void QOfonoNetworkRegistration::onGetOperatorsFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT reportError(reply.error().message());
    } else {
        d_ptr->initialized = true;
        setOperators(reply.value());
        if (isValid()) validChanged(true);
    }
}

void QOfonoNetworkRegistration::onScanFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT scanError(reply.error().message());
    } else {
        setOperators(reply.value());
        Q_EMIT scanFinished();
    }
    d_ptr->scanning = false;
    scanningChanged(false);
}

void QOfonoNetworkRegistration::onOperatorStatusChanged(const QString &status)
{
    QString oldPath = currentOperatorPath();
    QOfonoNetworkOperator *op = (QOfonoNetworkOperator*)sender();
    if (status == "current") {
        d_ptr->currentOperator = op;
    } else if (d_ptr->currentOperator == op) {
        d_ptr->currentOperator = NULL;
    }
    QString currentPath = currentOperatorPath();
    if (currentPath != oldPath) {
        Q_EMIT currentOperatorPathChanged(currentPath);
    }
}
