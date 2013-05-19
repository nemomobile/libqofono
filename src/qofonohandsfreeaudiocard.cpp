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

#include "qofonohandsfreeaudiocard.h"
#include "dbus/ofonohandsfreeaudiocard.h"

class QOfonoHandsfreeAudioCardPrivate
{
public:
    QOfonoHandsfreeAudioCardPrivate();
    QString modemPath;
    OfonoHandsfreeAudioCard *ofonoHandsfreeAudioCard;
    QVariantMap properties;

};

QOfonoHandsfreeAudioCardPrivate::QOfonoHandsfreeAudioCardPrivate() :
    modemPath(QString())
  , ofonoHandsfreeAudioCard(0)
{
}

QOfonoHandsfreeAudioCard::QOfonoHandsfreeAudioCard(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoHandsfreeAudioCardPrivate)
{
}

QOfonoHandsfreeAudioCard::~QOfonoHandsfreeAudioCard()
{
    delete d_ptr;
}

void QOfonoHandsfreeAudioCard::setModemPath(const QString &path)
{
    if (!d_ptr->ofonoHandsfreeAudioCard) {
        d_ptr->modemPath = path;
        d_ptr->ofonoHandsfreeAudioCard = new OfonoHandsfreeAudioCard("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->ofonoHandsfreeAudioCard) {
//            connect(d_ptr->ofonoHandsfreeAudioCard,SIGNAL(PropertyChanged(QString,QDBusVariant)),
//                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->ofonoHandsfreeAudioCard->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoHandsfreeAudioCard::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoHandsfreeAudioCard::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    // do these ever change?
//    if (property == QLatin1String("RemoteAddress")) {
//        Q_EMIT remoteAddressChanged(value.value<QString>());
//    } else if (property == QLatin1String("LocalAddress")) {
//        Q_EMIT localAddressChanged(value.value<QString>());
//    }
}

QString QOfonoHandsfreeAudioCard::remoteAddress() const
{
    if (d_ptr->ofonoHandsfreeAudioCard) {
        return d_ptr->properties["RemoteAddress"].value<QString>();
    }
    return QString();
}

QString QOfonoHandsfreeAudioCard::localAddress() const
{
    if (d_ptr->ofonoHandsfreeAudioCard) {
        return d_ptr->properties["LocalAddress"].value<QString>();
    }
    return QString();
}

void QOfonoHandsfreeAudioCard::connect() const
{
    if (d_ptr->ofonoHandsfreeAudioCard) {
        d_ptr->ofonoHandsfreeAudioCard->Connect();
    }
}
