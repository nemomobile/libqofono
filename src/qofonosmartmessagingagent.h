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

#ifndef QOFONOSmartMessagingAgent_H
#define QOFONOSmartMessagingAgent_H

#include "qofono_global.h"

//! This class is used to access ofono cell broadcast API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/smartmessaging-api.txt
 */
class QOfonoSmartMessagingAgentPrivate;
class QOFONOSHARED_EXPORT QOfonoSmartMessagingAgent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QOfonoSmartMessagingAgent)
    Q_PROPERTY(QString agentPath READ agentPath WRITE setAgentPath NOTIFY agentPathChanged)

public:
    explicit QOfonoSmartMessagingAgent(QObject *parent = 0);
    ~QOfonoSmartMessagingAgent();

    QString agentPath() const;
    void setAgentPath(const QString &path);

Q_SIGNALS:
    void agentPathChanged(const QString &path);
    void receiveAppointment(const QByteArray &appointment, const QVariantMap &info);
    void receiveBusinessCard(const QByteArray &card, const QVariantMap &info);
    void release();

private:
    QOfonoSmartMessagingAgentPrivate *d_ptr;
    QString smAgentPath;

private Q_SLOTS:
    void ReceiveAppointment(const QByteArray &appointment, const QVariantMap &info);
    void ReceiveBusinessCard(const QByteArray &card, const QVariantMap &info);
    void Release();
};

#endif // QOFONOSmartMessagingAgent_H
