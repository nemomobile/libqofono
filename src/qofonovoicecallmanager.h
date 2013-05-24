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

#ifndef QOFONOVoiceCallManager_H
#define QOFONOVoiceCallManager_H

#include <QObject>
#include <QDBusVariant>
#include <QStringList>
#include "qofono_global.h"
//! This class is used to access ofono voice call manager API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/voicecallmanager-api.txt
 */

class QOfonoVoiceCallManagerPrivate;
class QOFONOSHARED_EXPORT QOfonoVoiceCallManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)
    Q_PROPERTY(QStringList emergencyNumbers READ emergencyNumbers)

public:
    explicit QOfonoVoiceCallManager(QObject *parent = 0);
    ~QOfonoVoiceCallManager();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QStringList emergencyNumbers() const;

    Q_INVOKABLE QStringList getCalls() const;

    bool isValid() const;
Q_SIGNALS:
    void emergencyNumbersChanged(const QStringList &numbers);

    void callAdded(const QString &call, const QVariantMap &map);
    void callRemoved(const QString &call, const QVariantMap &map);

    void dialComplete(bool status);
    void hangupAllComplete(bool status);
    void sendTonesComplete(bool status);
    void transferComplete(bool status);
    void swapCallsComplete(bool status);
    void releaseAndAnswerComplete(bool status);
    void holdAndAnswerComplete(bool status);
    void privateChatComplete(bool status, const QStringList& calls);
    void createMultipartyComplete(bool status, const QStringList& calls);
    void hangupMultipartyComplete(bool status);

    void barringActive(const QString &type);
    void forwarded(const QString &type);

public slots:
    void dial(const QString &number, const QString &calleridHide);
     void hangupAll();
     void sendTones(const QString &tonestring);
     void transfer();
     void swapCalls();
     void releaseAndAnswer();
     void holdAndAnswer();
     void privateChat(const QString &path);
     void createMultiparty();
     void hangupMultiparty();

private:
    QOfonoVoiceCallManagerPrivate *d_ptr;
    void connectSignals();

private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONOVoiceCallManager_H
