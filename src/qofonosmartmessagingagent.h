/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
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

#ifndef QOFONOSmartMessagingAgent_H
#define QOFONOSmartMessagingAgent_H

#include <QObject>
#include <QDBusVariant>
#include <QDBusAbstractAdaptor>

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
    Q_PROPERTY(QString agentPath READ agentPath WRITE setAgentPath)

public:
    explicit QOfonoSmartMessagingAgent(QObject *parent = 0);
    ~QOfonoSmartMessagingAgent();

    QString agentPath() const;
    void setAgentPath(const QString &path);

Q_SIGNALS:
    void receiveAppointment(const QByteArray &appointment, const QVariantMap &info);
    void receiveBusinessCard(const QByteArray &card, const QVariantMap &info);
    void release();

private:
    QOfonoSmartMessagingAgentPrivate *d_ptr;
    friend class QOfonoSmartMessagingAgentAdaptor;
    QString smAgentPath;

private Q_SLOTS:
    void ReceiveAppointment(const QByteArray &appointment, const QVariantMap &info);
    void ReceiveBusinessCard(const QByteArray &card, const QVariantMap &info);
    void Release();
};

class QOfonoSmartMessagingAgentAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO ("D-Bus Interface", "org.ofono.SmartMessagingAgent")
public:
    explicit QOfonoSmartMessagingAgentAdaptor(QOfonoSmartMessagingAgent *parent = 0);
    virtual ~QOfonoSmartMessagingAgentAdaptor();

public Q_SLOTS:
     Q_NOREPLY void ReceiveAppointment(const QByteArray &appointment, const QVariantMap &info);
     Q_NOREPLY void ReceiveBusinessCard(const QByteArray &card, const QVariantMap &info);
     Q_NOREPLY void Release();

private:
   QOfonoSmartMessagingAgent* smartAgent;

};

#endif // QOFONOSmartMessagingAgent_H
