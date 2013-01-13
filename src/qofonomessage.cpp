/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**
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
