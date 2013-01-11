/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@gmail.com
**

**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qofonoconnectioncontext.h"
#include "dbus/ofonoconnectioncontext.h"

class QOfonoConnectionContextPrivate
{
public:
    QOfonoConnectionContextPrivate();
    OfonoConnectionContext *context;
    QString contextPath;
    QVariantMap properties;

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
}

QOfonoConnectionContext::~QOfonoConnectionContext()
{
    delete d_ptr;
}


QString QOfonoConnectionContext::contextPath() const
{
    return d_ptr->contextPath;
}

void QOfonoConnectionContext::setContextPath(const QString &idPath)
{
    if (!d_ptr) {
        d_ptr->contextPath = idPath;
        d_ptr->context = new OfonoConnectionContext("org.ofono", idPath, QDBusConnection::systemBus(),this);

        if (d_ptr->context) {
            connect(d_ptr->context,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->context->GetProperties();
            d_ptr->properties = reply.value();
        }
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
        return d_ptr->properties["QDBusVariant"].value<QString>();
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
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("Active"),QDBusVariant(value));
}

void QOfonoConnectionContext::setAccessPointName(const QString &value)
{
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("AccessPointName"),QDBusVariant(value));
}

void QOfonoConnectionContext::setType(const QString &value)
{
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("Type"),QDBusVariant(value));
}

void QOfonoConnectionContext::setUsername(const QString &value)
{
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("Username"),QDBusVariant(value));
}

void QOfonoConnectionContext::setPassword(const QString &value)
{
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("Password"),QDBusVariant(value));
}

void QOfonoConnectionContext::setProtocol(const QString &value)
{
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("Protocol"),QDBusVariant(value));
}

void QOfonoConnectionContext::setName(const QString &value)
{
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("Name"),QDBusVariant(value));
}

void QOfonoConnectionContext::setMessageProxy(const QString &value)
{
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("MessageProxy"),QDBusVariant(value));
}

void QOfonoConnectionContext::setMessageCenter(const QString &value)
{
    if (d_ptr->context)
        d_ptr->context->SetProperty(QLatin1String("MessageCenter"),QDBusVariant(value));
}

