/****************************************************************************
**
** Copyright (C) 2014 Jolla Ltd.
** Contact: slava.monich@jolla.com
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

#include "qofonoobject.h"
#include <QDBusPendingCallWatcher>

class QOfonoObject::Private
{
public:
    Private();
    QDBusPendingCall setProperty(const QString &key, const QVariant &value);

public:
    QDBusAbstractInterface* interface;
    bool initialized;
    QString objectPath;
    QVariantMap properties;
};

QOfonoObject::Private::Private() :
    interface(NULL),
    initialized(false)
{
}

QDBusPendingCall QOfonoObject::Private::setProperty(const QString &key, const QVariant &value)
{
    // Caller checks interface for NULL
    QVariantList args;
    args << QVariant(key) << QVariant::fromValue(QDBusVariant(value));
    return interface->asyncCallWithArgumentList("SetProperty", args);
}

QOfonoObject::QOfonoObject(QObject *parent) :
    QObject(parent),
    d_ptr(new QOfonoObject::Private)
{
}

QOfonoObject::~QOfonoObject()
{
    delete d_ptr;
}

QString QOfonoObject::objectPath() const
{
    return d_ptr->objectPath;
}

void QOfonoObject::setObjectPath(const QString &path)
{
    if (d_ptr->objectPath != path) {
        d_ptr->objectPath = path;
        setDbusInterface(path.isEmpty() ? NULL : createDbusInterface(path));
        objectPathChanged(d_ptr->objectPath);
    }
}

bool QOfonoObject::isValid() const
{
    return d_ptr->interface && d_ptr->interface->isValid() && d_ptr->initialized;
}

QDBusAbstractInterface* QOfonoObject::dbusInterface() const
{
    return d_ptr->interface;
}

void QOfonoObject::setDbusInterface(QDBusAbstractInterface* interface)
{
    bool wasValid = isValid();
    d_ptr->initialized = false;
    if (d_ptr->interface) {
        delete d_ptr->interface;
        d_ptr->interface = NULL;
        if (!d_ptr->properties.isEmpty()) {
            QStringList keys = d_ptr->properties.keys();
            d_ptr->properties.clear();
            for (int i=0; i<keys.size(); i++) {
                propertyChanged(keys[i], QVariant());
            }
        }
    }
    if (interface) {
        if (interface->isValid()) {
            d_ptr->interface = interface;
            connect(new QDBusPendingCallWatcher(interface->asyncCall("GetProperties"), interface),
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(onGetPropertiesFinished(QDBusPendingCallWatcher*)));
            connect(interface,
                SIGNAL(PropertyChanged(QString,QDBusVariant)),
                SLOT(onPropertyChanged(QString,QDBusVariant)));
        } else {
            delete interface;
        }
    }
    bool valid = isValid();
    if (valid != wasValid) {
        Q_EMIT validChanged(valid);
    }
}

void QOfonoObject::resetDbusInterface()
{
    setDbusInterface(d_ptr->objectPath.isEmpty() ? NULL : createDbusInterface(d_ptr->objectPath));
}

void QOfonoObject::onGetPropertiesFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QVariantMap> reply(*watcher);
    watcher->deleteLater();
    if (!reply.isError()) {
        // Ofono objects have fixed sets of properties, there's no need to check
        // for disappearance or reappearance of individual properties.
        d_ptr->properties = reply.value();
        QVariantMap properties = reply.value();
        for (QVariantMap::ConstIterator it = properties.constBegin();
             it != properties.constEnd(); ++it) {
            QVariant value = convertProperty(it.key(), it.value());
            d_ptr->properties.insert(it.key(), value);
            propertyChanged(it.key(), value);
        }
        d_ptr->initialized = true;
        if (isValid()) {
            Q_EMIT validChanged(true);
        }
    } else {
        QString message(reply.error().message());
        qDebug() << message;
        Q_EMIT reportError(message);
    }
}

QVariant QOfonoObject::getProperty(const QString &key) const
{
    return d_ptr->interface ? d_ptr->properties[key] : QVariant();
}

QVariant QOfonoObject::convertProperty(const QString &key, const QVariant &value)
{
    return value;
}

void QOfonoObject::onPropertyChanged(const QString &key, const QDBusVariant &value)
{
    QVariant variant = convertProperty(key, value.variant());
    if (variant.isValid()) {
        d_ptr->properties.insert(key, variant);
    } else {
        d_ptr->properties.remove(key);
    }
    propertyChanged(key, variant);
}

bool QOfonoObject::setPropertySync(const QString &key, const QVariant &value)
{
    if (d_ptr->interface) {
        QDBusPendingReply<> reply = d_ptr->setProperty(key, value);
        reply.waitForFinished();
        return reply.isValid() && !reply.isError();
    } else {
        return false;
    }
}

void QOfonoObject::setProperty(const QString &key, const QVariant &value)
{
    if (d_ptr->interface) {
        connect(new QDBusPendingCallWatcher(d_ptr->setProperty(key, value),
            d_ptr->interface), SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onSetPropertyFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoObject::onSetPropertyFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<> reply(*watch);
    watch->deleteLater();
    if (reply.isError()) {
        QString message(reply.error().message());
        qDebug() << message;
        Q_EMIT reportError(message);
    }
    Q_EMIT setPropertyFinished();
}

void QOfonoObject::propertyChanged(const QString &key, const QVariant &value)
{
}
