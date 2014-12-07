/****************************************************************************
**
** Copyright (C) 2013-2014 Jolla Ltd.
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

#ifndef QOFONOCallForwarding_H
#define QOFONOCallForwarding_H

#include "qofonomodeminterface.h"
#include "qofono_global.h"

//! This class is used to access ofono call forwarding API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/call-forwarding-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoCallForwarding : public QOfonoModemInterface
{
    Q_OBJECT
    Q_PROPERTY(QString voiceUnconditional READ voiceUnconditional WRITE setVoiceUnconditional NOTIFY voiceUnconditionalChanged)
    Q_PROPERTY(QString voiceBusy READ voiceBusy WRITE setVoiceBusy NOTIFY voiceBusyChanged)
    Q_PROPERTY(QString voiceNoReply READ voiceNoReply WRITE setVoiceNoReply NOTIFY voiceNoReplyChanged)
    Q_PROPERTY(quint16 voiceNoReplyTimeout READ voiceNoReplyTimeout WRITE setVoiceNoReplyTimeout NOTIFY voiceNoReplyTimeoutChanged)
    Q_PROPERTY(QString voiceNotReachable READ voiceNotReachable WRITE setVoiceNotReachable NOTIFY voiceNotReachableChanged)
    Q_PROPERTY(bool forwardingFlagOnSim READ forwardingFlagOnSim NOTIFY forwardingFlagOnSimChanged)

public:
    explicit QOfonoCallForwarding(QObject *parent = 0);
    ~QOfonoCallForwarding();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QString voiceUnconditional();
    void setVoiceUnconditional(const QString &property);

    QString voiceBusy();
    void setVoiceBusy(const QString &property);

    QString voiceNoReply();
    void setVoiceNoReply(const QString &property);

    quint16 voiceNoReplyTimeout();
    // TODO: Fix argument type - should be quint16
    void setVoiceNoReplyTimeout(ushort timeout);

    QString voiceNotReachable();
    void setVoiceNotReachable(const QString &property);

    bool forwardingFlagOnSim();

    Q_INVOKABLE void disableAll(const QString &type);

    bool isValid() const;
    bool isReady() const;

    void connectOfono();

Q_SIGNALS:
    void voiceUnconditionalChanged(const QString &property);
    void voiceBusyChanged(const QString &property);
    void voiceNoReplyChanged(const QString &property);
    // TODO: Fix argument type - should be quint16
    void voiceNoReplyTimeoutChanged(ushort timeout);
    void voiceNotReachableChanged(const QString &property);
    void forwardingFlagOnSimChanged(bool property);
    void getPropertiesFailed();
    void voiceUnconditionalComplete(bool success);
    void voiceBusyComplete(bool success);
    void voiceNoReplyComplete(bool success);
    void voiceNoReplyTimeoutComplete(bool success);
    void voiceNotReachableComplete(bool success);

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void propertyChanged(const QString &property, const QVariant &value);
    void getPropertiesFinished(const QVariantMap &properties, const QDBusError *error);
    void setPropertyFinished(const QString &property, const QDBusError *error);
};

#endif // QOFONOCallForwarding_H
