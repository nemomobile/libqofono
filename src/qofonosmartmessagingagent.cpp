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

#include "qofonosmartmessagingagent.h"
#include "ofono_smart_messaging_agent_adaptor.h"

class QOfonoSmartMessagingAgentPrivate
{
public:
    QOfonoSmartMessagingAgentPrivate();
    bool registered;
};

QOfonoSmartMessagingAgentPrivate::QOfonoSmartMessagingAgentPrivate() :
    registered(false)
{
}

QOfonoSmartMessagingAgent::QOfonoSmartMessagingAgent(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoSmartMessagingAgentPrivate)
{
    new QOfonoSmartMessagingAgentAdaptor(this);
}

QOfonoSmartMessagingAgent::~QOfonoSmartMessagingAgent()
{
    if (d_ptr->registered)
        QDBusConnection::systemBus().unregisterObject(smAgentPath);
    delete d_ptr;
}

void QOfonoSmartMessagingAgent::setAgentPath(const QString &path)
{
    if (smAgentPath != path) {
        QDBusConnection dbus = QDBusConnection::systemBus();
        if (d_ptr->registered) {
            dbus.unregisterObject(smAgentPath);
            d_ptr->registered = false;
        }
        smAgentPath = path;
        if (!path.isEmpty()) {
            if (!dbus.registerObject(path, this)) {
                qWarning() << "Object registration failed:" << dbus.lastError();
            }
            d_ptr->registered = true;
        }
        Q_EMIT agentPathChanged(path);
    }
}

QString QOfonoSmartMessagingAgent::agentPath() const
{
    return smAgentPath;
}

void QOfonoSmartMessagingAgent::ReceiveAppointment(const QByteArray &appointment, const QVariantMap &info)
{
    Q_EMIT receiveAppointment(appointment, info);
}

void QOfonoSmartMessagingAgent::ReceiveBusinessCard(const QByteArray &card, const QVariantMap &info)
{
    Q_EMIT receiveBusinessCard(card, info);
}

void QOfonoSmartMessagingAgent::Release()
{
    Q_EMIT release();
}
