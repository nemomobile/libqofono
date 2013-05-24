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

#include "qofonomessagewaiting.h"
#include "dbus/ofonomessagewaiting.h"

class QOfonoMessageWaitingPrivate
{
public:
    QOfonoMessageWaitingPrivate();
    QString modemPath;
    OfonoMessageWaiting *messageWaiting;
    QVariantMap properties;

};

QOfonoMessageWaitingPrivate::QOfonoMessageWaitingPrivate() :
    modemPath(QString())
  , messageWaiting(0)
{
}

QOfonoMessageWaiting::QOfonoMessageWaiting(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoMessageWaitingPrivate)
{
}

QOfonoMessageWaiting::~QOfonoMessageWaiting()
{
    delete d_ptr;
}

void QOfonoMessageWaiting::setModemPath(const QString &path)
{
    if (!d_ptr->messageWaiting) {
        d_ptr->messageWaiting = new OfonoMessageWaiting("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->messageWaiting->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->messageWaiting,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->messageWaiting->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoMessageWaiting::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoMessageWaiting::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("VoicemailWaiting")) {
        Q_EMIT voicemailWaitingChanged(value.value<bool>());
    } else if (property == QLatin1String("VoicemailMessageCount")) {
        Q_EMIT voicemailMessageCountChanged(value.value<int>());
    } else if (property == QLatin1String("VoiceMailboxNumber")) {
        Q_EMIT voicemailMailboxNumberChanged(value.value<QString>());
    }
}


bool QOfonoMessageWaiting::voicemailWaiting() const
{
    if ( d_ptr->messageWaiting)
        return d_ptr->properties["VoicemailWaiting"].value<bool>();
    else
        return false;
}

int QOfonoMessageWaiting::voicemailMessageCount() const
{
    if ( d_ptr->messageWaiting)
        return d_ptr->properties["VoicemailMessageCount"].value<int>();
    else
        return false;
}

QString QOfonoMessageWaiting::voicemailMailboxNumber() const
{
    if (d_ptr->messageWaiting)
        return d_ptr->properties["VoiceMailboxNumber"].value<QString>();
    else
        return QString();
}

void QOfonoMessageWaiting::setVoicemailMailboxNumber(const QString &mailboxnumber)
{
    if (d_ptr->messageWaiting)
        d_ptr->messageWaiting->SetProperty(QLatin1String("VoiceMailboxNumber"),QDBusVariant(mailboxnumber));

}

bool QOfonoMessageWaiting::isValid() const
{
    return d_ptr->messageWaiting->isValid();
}
