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

#include "qofonophonebook.h"
#include "dbus/ofonophonebook.h"

#define SUPER QOfonoModemInterface2

class QOfonoPhonebook::Private : public SUPER::ExtData
{
public:
    bool importing;
    Private() : importing(false) {}
};

QOfonoPhonebook::QOfonoPhonebook(QObject *parent) :
    SUPER(OfonoPhonebook::staticInterfaceName(), new Private, parent)
{
}

QOfonoPhonebook::~QOfonoPhonebook()
{
}

QDBusAbstractInterface *QOfonoPhonebook::createDbusInterface(const QString &path)
{
    return new OfonoPhonebook("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoPhonebook::dbusInterfaceDropped()
{
    SUPER::dbusInterfaceDropped();
    Private *d_ptr = privateData();
    if (d_ptr->importing) {
        d_ptr->importing = false;
        Q_EMIT importingChanged();
    }
}

bool QOfonoPhonebook::importing() const
{
    return privateData()->importing;
}

void QOfonoPhonebook::beginImport()
{
    Private *d_ptr = privateData();
    if (!d_ptr->importing) {
        OfonoPhonebook *iface = (OfonoPhonebook*)dbusInterface();
        if (iface) {
            connect(new QDBusPendingCallWatcher(iface->Import(), iface),
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(onImportFinished(QDBusPendingCallWatcher*)));
            d_ptr->importing = true;
            Q_EMIT importingChanged();
        }
    }
}

void QOfonoPhonebook::onImportFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<QString> reply(*watch);
    if (!reply.isError()) {
        Q_EMIT importReady(reply.value());
    } else {
        Q_EMIT importFailed();
    }
    privateData()->importing = false;
    Q_EMIT importingChanged();
}

QString QOfonoPhonebook::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoPhonebook::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

bool QOfonoPhonebook::isValid() const
{
    return SUPER::isValid();
}

QOfonoPhonebook::Private *QOfonoPhonebook::privateData() const
{
    return (QOfonoPhonebook::Private*)SUPER::extData();
}
