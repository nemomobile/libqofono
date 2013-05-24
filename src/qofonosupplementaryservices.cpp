/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
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

#include "qofonosupplementaryservices.h"
#include "dbus/ofonosupplementaryservices.h"

class QOfonoSupplementaryServicesPrivate
{
public:
    QOfonoSupplementaryServicesPrivate();
    QString modemPath;
    OfonoSupplementaryServices *supplementaryServices;
    QVariantMap properties;

};

QOfonoSupplementaryServicesPrivate::QOfonoSupplementaryServicesPrivate() :
    modemPath(QString())
  , supplementaryServices(0)
{
}

QOfonoSupplementaryServices::QOfonoSupplementaryServices(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoSupplementaryServicesPrivate)
{
}

QOfonoSupplementaryServices::~QOfonoSupplementaryServices()
{
    delete d_ptr;
}

void QOfonoSupplementaryServices::setModemPath(const QString &path)
{
    if (!d_ptr->supplementaryServices) {
        if (path != modemPath()) {
            d_ptr->supplementaryServices = new OfonoSupplementaryServices("org.ofono", path, QDBusConnection::systemBus(),this);

            if (d_ptr->supplementaryServices->isValid()) {
                d_ptr->modemPath = path;

                connect(d_ptr->supplementaryServices,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                        this,SLOT(propertyChanged(QString,QDBusVariant)));

                connect(d_ptr->supplementaryServices,SIGNAL(NotificationReceived(QString)),
                        this,SIGNAL(notificationReceived(QString)));
                connect(d_ptr->supplementaryServices,SIGNAL(RequestReceived(QString)),
                        this,SIGNAL(requestReceived(QString)));
                QDBusReply<QVariantMap> reply;
                reply = d_ptr->supplementaryServices->GetProperties();
                d_ptr->properties = reply.value();
                Q_EMIT modemPathChanged(path);
            }
        }
    }
}

QString QOfonoSupplementaryServices::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoSupplementaryServices::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("State")) {
        Q_EMIT stateChanged(value.value<QString>());
    }
}


QString QOfonoSupplementaryServices::state() const
{
    if (d_ptr->supplementaryServices)
        return d_ptr->properties["State"].value<QString>();
    else
        return QString();
}

bool QOfonoSupplementaryServices::isValid() const
{
    return d_ptr->supplementaryServices->isValid();
}
