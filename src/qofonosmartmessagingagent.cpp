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
    qDebug() << Q_FUNC_INFO << appointment << info;
    smartAgent->ReceiveAppointment(appointment, info);
}

void QOfonoSmartMessagingAgentAdaptor::ReceiveBusinessCard(const QByteArray &card, const QVariantMap &info)
{
    qDebug() << Q_FUNC_INFO << card << info;
    smartAgent->receiveAppointment(card, info);
}

void QOfonoSmartMessagingAgentAdaptor::Release()
{
    smartAgent->release();
}



