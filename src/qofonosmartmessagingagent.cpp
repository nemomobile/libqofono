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

#include "qofonosmartmessagingagent.h"
#include "dbus/ofonosmartmessagingagent.h"

class QOfonoSmartMessagingAgentPrivate
{
public:
    QOfonoSmartMessagingAgentPrivate();
    QString agentPath;
    QOfonoSmartMessagingAgentAdaptor *smartAdaptor;
};

QOfonoSmartMessagingAgentPrivate::QOfonoSmartMessagingAgentPrivate() :
    agentPath(QString())
  , smartAdaptor(0)
{
}

QOfonoSmartMessagingAgent::QOfonoSmartMessagingAgent(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoSmartMessagingAgentPrivate)
{
}

QOfonoSmartMessagingAgent::~QOfonoSmartMessagingAgent()
{
    delete d_ptr;
}

void QOfonoSmartMessagingAgent::setAgentPath(const QString &path)
{
    if (!d_ptr->smartAdaptor) {
        d_ptr->agentPath = path;
        d_ptr->smartAdaptor = new QOfonoSmartMessagingAgentAdaptor(this);
        if (d_ptr->smartAdaptor)
            smAgentPath = path;

        if (!QDBusConnection::systemBus().registerObject(smAgentPath, this)) {
            qDebug() << "object not registered";
        }
    }
}

QString QOfonoSmartMessagingAgent::agentPath() const
{
    return d_ptr->agentPath;
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

QOfonoSmartMessagingAgentAdaptor::QOfonoSmartMessagingAgentAdaptor(QOfonoSmartMessagingAgent *parent)
    : QDBusAbstractAdaptor(parent),
      smartAgent(parent)
{
}

QOfonoSmartMessagingAgentAdaptor::~QOfonoSmartMessagingAgentAdaptor()
{
}

void QOfonoSmartMessagingAgentAdaptor::ReceiveAppointment(const QByteArray &appointment, const QVariantMap &info)
{
    smartAgent->ReceiveAppointment(appointment, info);
}

void QOfonoSmartMessagingAgentAdaptor::ReceiveBusinessCard(const QByteArray &card, const QVariantMap &info)
{
    smartAgent->receiveAppointment(card, info);
}

void QOfonoSmartMessagingAgentAdaptor::Release()
{
    smartAgent->release();
}



