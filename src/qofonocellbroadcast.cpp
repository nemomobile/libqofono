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

#include "qofonocellbroadcast.h"
#include "dbus/ofonocellbroadcast.h"

class QOfonoCellBroadcastPrivate
{
public:
    QOfonoCellBroadcastPrivate();
    QString modemPath;
    OfonoCellBroadcast *cellBroadcast;
    QVariantMap properties;

};

QOfonoCellBroadcastPrivate::QOfonoCellBroadcastPrivate() :
    modemPath(QString())
  , cellBroadcast(0)
{
}

QOfonoCellBroadcast::QOfonoCellBroadcast(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoCellBroadcastPrivate)
{
}

QOfonoCellBroadcast::~QOfonoCellBroadcast()
{
    delete d_ptr;
}

void QOfonoCellBroadcast::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        if (d_ptr->cellBroadcast) {
            delete d_ptr->cellBroadcast;
            d_ptr->cellBroadcast = 0;
            d_ptr->properties.clear();
        }
        d_ptr->cellBroadcast = new OfonoCellBroadcast("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->cellBroadcast->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->cellBroadcast,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            connect(d_ptr->cellBroadcast,SIGNAL(IncomingBroadcast(QString,quint16)),
                    this,SIGNAL(incomingBroadcast(QString,quint16)));
            connect(d_ptr->cellBroadcast,SIGNAL(EmergencyBroadcast(QString,QVariantMap)),
                    this,SIGNAL(emergencyBroadcast(QString,QVariantMap)));

            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->cellBroadcast->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoCellBroadcast::modemPath() const
{
    return d_ptr->modemPath;
}

bool QOfonoCellBroadcast::enabled() const
{
    if ( d_ptr->cellBroadcast)
        return d_ptr->properties["Powered"].value<bool>();
    else
        return false;
}

void QOfonoCellBroadcast::setEnabled(bool b)
{
    if ( d_ptr->cellBroadcast)
        d_ptr->cellBroadcast->SetProperty("Powered",QDBusVariant(b));
}

QString QOfonoCellBroadcast::topics() const
{
    if ( d_ptr->cellBroadcast)
        return d_ptr->properties["Topics"].value<QString>();
    else
        return QString();
}

void QOfonoCellBroadcast::setTopics(const QString &topics) const
{
    if ( d_ptr->cellBroadcast)
        d_ptr->cellBroadcast->SetProperty("Topics",QDBusVariant(topics));
}

void QOfonoCellBroadcast::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("Powered")) {
        Q_EMIT enabledChanged(value.value<bool>());
    } else if (property == QLatin1String("Topics")) {
        Q_EMIT topicsChanged(value.value<QString>());
    }
}

bool QOfonoCellBroadcast::isValid() const
{
    return d_ptr->cellBroadcast->isValid();
}
