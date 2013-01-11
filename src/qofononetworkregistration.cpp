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

#include "qofononetworkregistration.h"
#include "dbus/ofononetworkregistration.h"

class QOfonoNetworkRegistrationPrivate
{
public:
    QOfonoNetworkRegistrationPrivate();
    OfonoNetworkRegistration *networkRegistration;
    QString modemPath;
    QVariantMap properties;
};

QOfonoNetworkRegistrationPrivate::QOfonoNetworkRegistrationPrivate() :
    networkRegistration(0)
  , modemPath(QString())
{
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
    if (!d_ptr->networkRegistration) {
        d_ptr->modemPath = path;
        d_ptr->networkRegistration = new OfonoNetworkRegistration("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->networkRegistration) {
            connect(d_ptr->networkRegistration,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->networkRegistration->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
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

void QOfonoNetworkRegistration::getOperators()
{
    if (d_ptr->networkRegistration)
        QDBusPendingReply<QArrayOfPathProperties> pending = d_ptr->networkRegistration->GetOperators();
}

void QOfonoNetworkRegistration::scan()
{
    if (d_ptr->networkRegistration)
        d_ptr->networkRegistration->Scan();
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

