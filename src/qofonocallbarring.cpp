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

#include "qofonocallbarring.h"
#include "dbus/ofonocallbarring.h"

class QOfonoCallBarringPrivate
{
public:
    QOfonoCallBarringPrivate();
    QString modemPath;
    OfonoCallBarring *callBarring;
    QVariantMap properties;

};

QOfonoCallBarringPrivate::QOfonoCallBarringPrivate() :
    modemPath(QString())
  , callBarring(0)
{
}

QOfonoCallBarring::QOfonoCallBarring(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoCallBarringPrivate)
{
}

QOfonoCallBarring::~QOfonoCallBarring()
{
    delete d_ptr;
}

void QOfonoCallBarring::setModemPath(const QString &path)
{
    if (!d_ptr->callBarring) {
        d_ptr->callBarring = new OfonoCallBarring("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->callBarring->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->callBarring,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->callBarring->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoCallBarring::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoCallBarring::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("VoiceIncoming")) {
        Q_EMIT voiceIncomingChanged(value.value<QString>());
    } else if (property == QLatin1String("VoiceOutgoing")) {
        Q_EMIT voiceOutgoingChanged(value.value<QString>());
    }
}

QString QOfonoCallBarring::voiceIncoming()
{
    if (d_ptr->callBarring)
        return d_ptr->properties["VoiceIncoming"].value<QString>();
    else
        return QString();
}

void QOfonoCallBarring::setVoiceIncoming(const QString &barrings, const QString &password)
{
    if (d_ptr->callBarring) {
        QVariantList arguments;
        arguments << QVariant(barrings);
        if (!password.isNull())
            arguments << QVariant(password);
        d_ptr->callBarring->SetProperty("VoiceIncoming",QDBusVariant(arguments), password);

    }
}

QString QOfonoCallBarring::voiceOutgoing()
{
    if (d_ptr->callBarring)
        return d_ptr->properties["VoiceOutgoing"].value<QString>();
    else
        return QString();
}

void QOfonoCallBarring::setVoiceOutgoing(const QString &barrings, const QString &password)
{

    if (d_ptr->callBarring) {
        QVariantList arguments;
        arguments << QVariant(barrings);
        if (!password.isNull())
            arguments << QVariant(password);
        d_ptr->callBarring->SetProperty("VoiceOutgoing",QDBusVariant(arguments),password);

    }
}

void QOfonoCallBarring::changePassword(const QString &oldPassword, const QString &newPassword)
{

    if (d_ptr->callBarring)
        d_ptr->callBarring->ChangePassword(oldPassword, newPassword);
}

void QOfonoCallBarring::disableAll(const QString &password)
{
    if (d_ptr->callBarring)
        d_ptr->callBarring->DisableAll(password);
}

void QOfonoCallBarring::disableAllIncoming(const QString &password)
{
    if (d_ptr->callBarring)
        d_ptr->callBarring->DisableAllIncoming(password);
}

void QOfonoCallBarring::disableAllOutgoing(const QString &password)
{
    if (d_ptr->callBarring)
        d_ptr->callBarring->DisableAllOutgoing(password);
}
