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

#include "qofonomodeminterface.h"
#include "qofonomodem.h"

#define SUPER QOfonoObject

class QOfonoModemInterface::Private : public QOfonoObject::ExtData
{
public:
    QString interfaceName;
    QSharedPointer<QOfonoModem> modem;
    QOfonoModem::ExtData *ext;

    Private(const QString &iface, QOfonoModem::ExtData *data) :
        interfaceName(iface), ext(data) {}
    ~Private() { delete ext; }
};

QOfonoModemInterface::QOfonoModemInterface(const QString &iface, ExtData *ext, QObject *parent) :
    SUPER(new Private(iface, ext), parent)
{
}

QOfonoModemInterface::QOfonoModemInterface(const QString &iface, QObject *parent) :
    SUPER(new Private(iface, NULL), parent)
{
}

QOfonoModemInterface::~QOfonoModemInterface()
{
}

QOfonoModemInterface::Private* QOfonoModemInterface::privateData() const
{
    return (Private*)SUPER::extData();
}

QOfonoObject::ExtData* QOfonoModemInterface::extData() const
{
    return privateData()->ext;
}

QString QOfonoModemInterface::modemPath() const
{
    return objectPath();
}

void QOfonoModemInterface::setModemPath(const QString &path)
{
    setObjectPath(path);
}

void QOfonoModemInterface::objectPathChanged(const QString &path, const QVariantMap *)
{
    // The base implementation would immediately create the D-Bus interface
    // object. However we need to check if our interface is actually available
    // and postpone creation of the D-Bus interface object if our interface
    // isn't there (see onModemInterfacesChanged below)
    bool wasReady = isReady();

    Private *d_ptr = privateData();
    if (!d_ptr->modem.isNull()) {
        disconnect(d_ptr->modem.data(), SIGNAL(interfacesChanged(QStringList)),
            this, SLOT(onModemInterfacesChanged(QStringList)));
        d_ptr->modem.reset();
    }

    setDbusInterface(NULL, NULL);

    d_ptr->modem = QOfonoModem::instance(objectPath());
    connect(d_ptr->modem.data(), SIGNAL(interfacesChanged(QStringList)),
        this, SLOT(onModemInterfacesChanged(QStringList)));

    Q_EMIT modemPathChanged(path);
    onModemInterfacesChanged(d_ptr->modem->interfaces());
    if (wasReady != isReady()) {
        Q_EMIT readyChanged();
    }
}

void QOfonoModemInterface::onModemInterfacesChanged(const QStringList &interfaces)
{
    if (interfaces.contains(privateData()->interfaceName)) {
        Q_ASSERT(!objectPath().isEmpty());
        if (!dbusInterface()) {
            setDbusInterface(createDbusInterface(objectPath()), NULL);
        }
    } else {
        setDbusInterface(NULL, NULL);
    }
}

// The usefullness if the 'ready' property is questionable but it has to exist
// for backward compatibility.
bool QOfonoModemInterface::isReady() const
{
    return isValid() && !getProperties().isEmpty();
}

void QOfonoModemInterface::updateProperty(const QString &key, const QVariant &value)
{
    bool wasReady = isReady();
    SUPER::updateProperty(key, value);
    if (wasReady != isReady()) {
        Q_EMIT readyChanged();
    }
}

void QOfonoModemInterface::getPropertiesFinished(const QVariantMap &properties, const QDBusError *error)
{
    bool wasReady = isReady();
    SUPER::getPropertiesFinished(properties, error);
    if (wasReady != isReady()) {
        Q_EMIT readyChanged();
    }
}
