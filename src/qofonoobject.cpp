/****************************************************************************
**
** Copyright (C) 2014-2015 Jolla Ltd.
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
#include "qofonoutils_p.h"

class QOfonoObject::Private
{
public:
    QOfonoObject::ExtData *ext;
    QDBusAbstractInterface *interface;
    bool initialized;
    bool fixedPath;
    QString objectPath;
    QVariantMap properties;

    Private(QOfonoObject::ExtData *data) : ext(data),
        interface(NULL), initialized(false), fixedPath(false) {}
    ~Private() { delete ext; }

    QDBusPendingCall setProperty(const QString &key, const QVariant &value);

    class SetPropertyWatcher : public QDBusPendingCallWatcher {
    public:
        QString property;
        SetPropertyWatcher(QDBusAbstractInterface *parent, const QString &name,
            const QDBusPendingCall &call) : QDBusPendingCallWatcher(call, parent),
            property(name) {}
    };
};

QOfonoObject::ExtData::~ExtData()
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
    d_ptr(new QOfonoObject::Private(NULL))
{
}

QOfonoObject::QOfonoObject(QOfonoObject::ExtData *ext, QObject *parent) :
    QObject(parent),
    d_ptr(new QOfonoObject::Private(ext))
{
}

QOfonoObject::~QOfonoObject()
{
    delete d_ptr;
}

QOfonoObject::ExtData* QOfonoObject::extData() const
{
    return d_ptr->ext;
}

QString QOfonoObject::objectPath() const
{
    return d_ptr->objectPath;
}

void QOfonoObject::setObjectPath(const QString &path, const QVariantMap *properties)
{
    if (d_ptr->fixedPath) {
        if (d_ptr->objectPath != path) {
            qWarning() << "Attempt to change a fixed path";
        }
    } else if (d_ptr->objectPath != path) {
        d_ptr->objectPath = path;
        objectPathChanged(path, properties);
    }
}

void QOfonoObject::fixObjectPath(const QString &path)
{
    if (d_ptr->fixedPath) {
        qWarning() << "Attempt to fix object path more than once";
    } else if (d_ptr->objectPath != path) {
        d_ptr->objectPath = path;
        d_ptr->fixedPath = true;
        objectPathChanged(path, NULL);
    }
}

void QOfonoObject::objectPathChanged(const QString &, const QVariantMap *properties)
{
    resetDbusInterface(properties);
}

bool QOfonoObject::isValid() const
{
    return d_ptr->interface && d_ptr->interface->isValid() && d_ptr->initialized;
}

QDBusAbstractInterface *QOfonoObject::dbusInterface() const
{
    return d_ptr->interface;
}

void QOfonoObject::resetDbusInterface(const QVariantMap *properties)
{
    setDbusInterface(d_ptr->objectPath.isEmpty() ? NULL :
        createDbusInterface(d_ptr->objectPath), properties);
}

void QOfonoObject::setDbusInterface(QDBusAbstractInterface *iface, const QVariantMap *properties)
{
    bool wasValid = isValid();
    d_ptr->initialized = false;
    if (d_ptr->interface) {
        delete d_ptr->interface;
        d_ptr->interface = NULL;
        dbusInterfaceDropped();
    }
    if (iface) {
        d_ptr->interface = iface;
        if (properties) {
            d_ptr->initialized = true;
            // Ofono objects have fixed sets of properties, there's no need to check
            // for disappearance or reappearance of individual properties.
            for (QVariantMap::ConstIterator it = properties->constBegin();
                it != properties->constEnd(); ++it) {
                updateProperty(it.key(), it.value());
            }
        } else {
            d_ptr->initialized = false;
            connect(new QDBusPendingCallWatcher(
                iface->asyncCall("GetProperties"), iface),
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(onGetPropertiesFinished(QDBusPendingCallWatcher*)));
        }
        connect(iface,
            SIGNAL(PropertyChanged(QString,QDBusVariant)),
            SLOT(onPropertyChanged(QString,QDBusVariant)));
    }
    bool valid = isValid();
    if (valid != wasValid) {
        Q_EMIT validChanged(valid);
    }
}

void QOfonoObject::dbusInterfaceDropped()
{
    if (!d_ptr->properties.isEmpty()) {
        const QStringList keys = d_ptr->properties.keys();
        for (int i=0; i<keys.size(); i++) {
            updateProperty(keys.at(i), QVariant());
        }
    }
}

void QOfonoObject::onGetPropertiesFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<QVariantMap> reply(*watch);
    if (!reply.isError()) {
        getPropertiesFinished(reply.value(), NULL);
    } else {
        QDBusError error = reply.error();
        getPropertiesFinished(QVariantMap(), &error);
    }
}

void QOfonoObject::getPropertiesFinished(const QVariantMap &properties, const QDBusError *error)
{
    if (!error) {
        for (QVariantMap::ConstIterator it = properties.constBegin();
             it != properties.constEnd(); ++it) {
            updateProperty(it.key(), it.value());
        }
        d_ptr->initialized = true;
        if (isValid()) {
            Q_EMIT validChanged(true);
        }
    } else {
        qDebug() << *error;
        Q_EMIT reportError(error->message());
    }
}

void QOfonoObject::removeProperty(const QString &key)
{
    if (d_ptr->properties.remove(key) > 0) {
        propertyChanged(key, QVariant());
    }
}

QVariantMap QOfonoObject::getProperties() const
{
    return d_ptr->properties;
}

QVariant QOfonoObject::getProperty(const QString &key) const
{
    return d_ptr->properties.value(key);
}

QVariant QOfonoObject::convertProperty(const QString &key, const QVariant &value)
{
    return value;
}

void QOfonoObject::propertyChanged(const QString &key, const QVariant &value)
{
}

void QOfonoObject::updateProperty(const QString &key, const QVariant &value)
{
    QVariant oldValue = getProperty(key);
    QVariant newValue;
    if (value.isValid()) {
        newValue = convertProperty(key, value);
    }
    if (!qofono::safeVariantEq(oldValue, newValue)) {
        if (newValue.isValid()) {
            d_ptr->properties.insert(key, newValue);
        } else {
            d_ptr->properties.remove(key);
        }
        propertyChanged(key, newValue);
    } else if (!oldValue.isValid() && newValue.isValid()) {
        // Not a change, but we need to insert the new value to make isReady() work.
        d_ptr->properties.insert(key, newValue);
    }
}

void QOfonoObject::onPropertyChanged(const QString &key, const QDBusVariant &value)
{
    updateProperty(key, value.variant());
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
        connect(new Private::SetPropertyWatcher(d_ptr->interface,
            key, d_ptr->setProperty(key, value)),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onSetPropertyFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoObject::onSetPropertyFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<> reply(*watch);
    QDBusError dbusError;
    const QDBusError *error;
    if (!reply.isError()) {
        error = NULL;
    } else {
        dbusError = reply.error();
        error = &dbusError;
    }
    setPropertyFinished(((Private::SetPropertyWatcher*)watch)->property, error);
}

void QOfonoObject::setPropertyFinished(const QString &property, const QDBusError *error)
{
    if (error) {
        qDebug() << qPrintable(property) << ": " << *error;
        Q_EMIT reportError(error->message());
    }
    Q_EMIT setPropertyFinished();
}
