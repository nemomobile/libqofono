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

#include "qofonoassistedsatellitenavigation.h"
#include "dbus/ofonoassistedsatellitenavigation.h"

class QOfonoAssistedSatelliteNavigationPrivate
{
public:
    QOfonoAssistedSatelliteNavigationPrivate();
    QString modemPath;
    OfonoAssistedSatelliteNavigation *ofonoAssistedSatelliteNav;

};

QOfonoAssistedSatelliteNavigationPrivate::QOfonoAssistedSatelliteNavigationPrivate() :
    modemPath(QString())
  , ofonoAssistedSatelliteNav(0)
{
}

QOfonoAssistedSatelliteNavigation::QOfonoAssistedSatelliteNavigation(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoAssistedSatelliteNavigationPrivate)
{
}

QOfonoAssistedSatelliteNavigation::~QOfonoAssistedSatelliteNavigation()
{
    delete d_ptr;
}

void QOfonoAssistedSatelliteNavigation::setModemPath(const QString &path)
{
    if (!d_ptr->ofonoAssistedSatelliteNav) {
        d_ptr->modemPath = path;
        d_ptr->ofonoAssistedSatelliteNav = new OfonoAssistedSatelliteNavigation("org.ofono", path, QDBusConnection::systemBus(),this);
    }
}

QString QOfonoAssistedSatelliteNavigation::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoAssistedSatelliteNavigation::registerPositioningRequestAgent(const QString &path)
{
    if (d_ptr->ofonoAssistedSatelliteNav) {
        QDBusPendingReply <> reply = d_ptr->ofonoAssistedSatelliteNav->RegisterPositioningRequestAgent(QDBusObjectPath(path));
        if (reply.isError())
          qDebug() << reply.error().message();
    }
}

void QOfonoAssistedSatelliteNavigation::sendPositioningElement(const QString &xmlElement)
{
    if (d_ptr->ofonoAssistedSatelliteNav)
        d_ptr->ofonoAssistedSatelliteNav->SendPositioningElement(xmlElement);
}

void QOfonoAssistedSatelliteNavigation::unregisterPositioningRequestAgent(const QString &path)
{
    if (d_ptr->ofonoAssistedSatelliteNav)
        d_ptr->ofonoAssistedSatelliteNav->UnregisterPositioningRequestAgent(QDBusObjectPath(path));
}
