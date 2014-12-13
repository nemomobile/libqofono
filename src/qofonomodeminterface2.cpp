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

#include "qofonomodeminterface2.h"
#include "qofonomodem.h"

class QOfonoModemInterface2::Private
{
public:
    QString modemPath;
    QString interfaceName;
    QSharedPointer<QOfonoModem> modem;
    QDBusAbstractInterface *interface;
    QOfonoModemInterface2::ExtData *ext;

    Private(const QString &iname, QOfonoModemInterface2::ExtData *data) :
        interfaceName(iname), interface(NULL), ext(data) {}
    ~Private() { delete ext; }
};

QOfonoModemInterface2::ExtData::~ExtData()
{
}

QOfonoModemInterface2::QOfonoModemInterface2(const QString &iname, ExtData *ext, QObject *parent) :
    QObject(parent),
    d_ptr(new Private(iname, ext))
{
}

QOfonoModemInterface2::QOfonoModemInterface2(const QString &iname, QObject *parent) :
    QObject(parent),
    d_ptr(new Private(iname, NULL))
{
}

QOfonoModemInterface2::~QOfonoModemInterface2()
{
    delete d_ptr;
}

QOfonoModemInterface2::ExtData* QOfonoModemInterface2::extData() const
{
    return d_ptr->ext;
}

QString QOfonoModemInterface2::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoModemInterface2::setModemPath(const QString &path)
{
    if (d_ptr->modemPath != path) {
        bool wasValid = isValid();
        d_ptr->modemPath = path;

        if (!d_ptr->modem.isNull()) {
            disconnect(d_ptr->modem.data(), SIGNAL(interfacesChanged(QStringList)),
                this, SLOT(onModemInterfacesChanged(QStringList)));
            d_ptr->modem.reset();
        }
        if (d_ptr->interface) {
            delete d_ptr->interface;
            d_ptr->interface = NULL;
            dbusInterfaceDropped();
        }

        d_ptr->modem = QOfonoModem::instance(d_ptr->modemPath);
        connect(d_ptr->modem.data(), SIGNAL(interfacesChanged(QStringList)),
            this, SLOT(onModemInterfacesChanged(QStringList)));

        QStringList interfaces = d_ptr->modem->interfaces();
        if (interfaces.contains(d_ptr->interfaceName)) {
            d_ptr->interface = createDbusInterface(d_ptr->modemPath);
        }

        Q_EMIT modemPathChanged(d_ptr->modemPath);

        bool valid = isValid();
        if (valid != wasValid) {
            Q_EMIT validChanged(valid);
        }
    }
}

void QOfonoModemInterface2::dbusInterfaceDropped()
{
}

void QOfonoModemInterface2::onModemInterfacesChanged(const QStringList &interfaces)
{
    bool wasValid = isValid();
    if (interfaces.contains(d_ptr->interfaceName)) {
        if (!d_ptr->interface) {
            d_ptr->interface = createDbusInterface(d_ptr->modemPath);
        }
    } else if (d_ptr->interface) {
        delete d_ptr->interface;
        d_ptr->interface = NULL;
        dbusInterfaceDropped();
    }
    bool valid = isValid();
    if (valid != wasValid) {
        Q_EMIT validChanged(valid);
    }
}

bool QOfonoModemInterface2::isValid() const
{
    return d_ptr->interface && d_ptr->interface->isValid();
}

QDBusAbstractInterface *QOfonoModemInterface2::dbusInterface() const
{
    return d_ptr->interface;
}
