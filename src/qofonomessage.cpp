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

#include "qofonomessage.h"
#include "dbus/ofonomessage.h"

QOfonoMessage::QOfonoMessage(QObject *parent) :
    QOfonoObject(parent)
{
}

QOfonoMessage::~QOfonoMessage()
{
}

QDBusAbstractInterface* QOfonoMessage::createDbusInterface(const QString &path)
{
    return new OfonoMessage("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoMessage::objectPathChanged(const QString &path)
{
    Q_EMIT messagePathChanged(path);
}

void QOfonoMessage::setMessagePath(const QString &path)
{
    setObjectPath(path);
}

QString QOfonoMessage::messagePath() const
{
    return objectPath();
}

void QOfonoMessage::propertyChanged(const QString &property, const QVariant &value)
{
    if (property == QLatin1String("State")) {
        Q_EMIT stateChanged(value.value<QString>());
    }
    QOfonoObject::propertyChanged(property, value);
}

QString QOfonoMessage::state() const
{
    return getString("State");
}

void QOfonoMessage::cancel()
{
    QDBusAbstractInterface* dbus = dbusInterface();
    if (dbus) {
        ((OfonoMessage*)dbus)->Cancel();
    }
}

bool QOfonoMessage::isValid() const
{
    return QOfonoObject::isValid();
}
