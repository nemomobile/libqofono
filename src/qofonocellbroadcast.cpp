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

#include "qofonocellbroadcast.h"
#include "dbus/ofonocellbroadcast.h"

QOfonoCellBroadcast::QOfonoCellBroadcast(QObject *parent) :
    QOfonoObject(parent)
{
}

QOfonoCellBroadcast::~QOfonoCellBroadcast()
{
}

QDBusAbstractInterface *QOfonoCellBroadcast::createDbusInterface(const QString &path)
{
    OfonoCellBroadcast *iface = new OfonoCellBroadcast("org.ofono", path, QDBusConnection::systemBus(), this);
    connect(iface,
        SIGNAL(IncomingBroadcast(QString,quint16)),
        SIGNAL(incomingBroadcast(QString,quint16)));
    connect(iface,
        SIGNAL(EmergencyBroadcast(QString,QVariantMap)),
        SIGNAL(emergencyBroadcast(QString,QVariantMap)));
    return iface;
}

void QOfonoCellBroadcast::objectPathChanged(const QString &path, const QVariantMap *properties)
{
    QOfonoObject::objectPathChanged(path, properties);
    Q_EMIT modemPathChanged(path);
}

void QOfonoCellBroadcast::setModemPath(const QString &path)
{
    setObjectPath(path);
}

QString QOfonoCellBroadcast::modemPath() const
{
    return objectPath();
}

void QOfonoCellBroadcast::propertyChanged(const QString &property, const QVariant &value)
{
    QOfonoObject::propertyChanged(property, value);
    if (property == QLatin1String("Powered")) {
        Q_EMIT enabledChanged(value.toBool());
    } else if (property == QLatin1String("Topics")) {
        Q_EMIT topicsChanged(value.toString());
    }
}

bool QOfonoCellBroadcast::enabled() const
{
    return getBool("Powered");
}

void QOfonoCellBroadcast::setEnabled(bool b)
{
    setProperty("Powered", b);
}

QString QOfonoCellBroadcast::topics() const
{
    return getString("Topics");
}

void QOfonoCellBroadcast::setTopics(const QString &topics)
{
    setProperty("Topics", topics);
}
