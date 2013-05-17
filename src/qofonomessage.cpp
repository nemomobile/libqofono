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

#include "qofonomessage.h"
#include "dbus/ofonomessage.h"

class QOfonoMessagePrivate
{
public:
    QOfonoMessagePrivate();
    QString messagePath;
    OfonoMessage *oMessage;
    QVariantMap properties;

};

QOfonoMessagePrivate::QOfonoMessagePrivate() :
    messagePath(QString())
  , oMessage(0)
{
}

QOfonoMessage::QOfonoMessage(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoMessagePrivate)
{
}

QOfonoMessage::~QOfonoMessage()
{
    delete d_ptr;
}

void QOfonoMessage::setMessagePath(const QString &path)
{
    if (!d_ptr->oMessage) {
        d_ptr->messagePath = path;
        d_ptr->oMessage = new OfonoMessage("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->oMessage) {
            connect(d_ptr->oMessage,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->oMessage->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoMessage::messagePath() const
{
    return d_ptr->messagePath;
}

void QOfonoMessage::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("State")) {
        Q_EMIT stateChanged(value.value<QString>());
   }
}

QString QOfonoMessage::state() const
{
    if (d_ptr->oMessage)
        return d_ptr->properties["State"].value<QString>();
    else
        return QString();
}

void QOfonoMessage::cancel()
{
    if (d_ptr->oMessage)
        d_ptr->oMessage->Cancel();
}
