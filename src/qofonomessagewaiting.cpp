/****************************************************************************
**
** Copyright (C) 2013-2015 Jolla Ltd.
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
#include "ofono_messagewaiting_interface.h"

#define SUPER QOfonoModemInterface

QOfonoMessageWaiting::QOfonoMessageWaiting(QObject *parent) :
    SUPER(OfonoMessageWaiting::staticInterfaceName(), parent)
{
}

QOfonoMessageWaiting::~QOfonoMessageWaiting()
{
}

QDBusAbstractInterface *QOfonoMessageWaiting::createDbusInterface(const QString &path)
{
    return new OfonoMessageWaiting("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoMessageWaiting::connectOfono()
{
    resetDbusInterface();
}

void QOfonoMessageWaiting::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("VoicemailWaiting")) {
        Q_EMIT voicemailWaitingChanged(value.toBool());
    } else if (property == QLatin1String("VoicemailMessageCount")) {
        Q_EMIT voicemailMessageCountChanged(value.toInt());
    } else if (property == QLatin1String("VoicemailMailboxNumber")) {
        Q_EMIT voicemailMailboxNumberChanged(value.toString());
    }
}

bool QOfonoMessageWaiting::voicemailWaiting() const
{
    return getBool("VoicemailWaiting");
}

int QOfonoMessageWaiting::voicemailMessageCount() const
{
    return getInt("VoicemailMessageCount");
}

QString QOfonoMessageWaiting::voicemailMailboxNumber() const
{
    return getString("VoicemailMailboxNumber");
}

void QOfonoMessageWaiting::setVoicemailMailboxNumber(const QString &mailboxnumber)
{
    setProperty("VoicemailMailboxNumber", mailboxnumber);
}

void QOfonoMessageWaiting::getPropertiesFinished(const QVariantMap &properties, const QDBusError *error)
{
    SUPER::getPropertiesFinished(properties, error);
    if (error) {
        Q_EMIT getPropertiesFailed();
    }
}

void QOfonoMessageWaiting::setPropertyFinished(const QString &property, const QDBusError *error)
{
    SUPER::setPropertyFinished(property, error);
    if (property == "VoicemailMailboxNumber") {
        Q_EMIT voicemailMailboxComplete(!error);
    }
}

QString QOfonoMessageWaiting::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoMessageWaiting::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

bool QOfonoMessageWaiting::isValid() const
{
    return SUPER::isValid();
}

bool QOfonoMessageWaiting::isReady() const
{
    return SUPER::isReady();
}
