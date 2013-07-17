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

#ifndef QOFONOCALLBARRING_H
#define QOFONOCALLBARRING_H

#include <QObject>
#include <QDBusVariant>

#include "qofono_global.h"
//! This class is used to access ofono call barring API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/call-barring-api.txt
 */

class QOfonoCallBarringPrivate;
class QOFONOSHARED_EXPORT QOfonoCallBarring : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)

    Q_PROPERTY(QString voiceIncoming READ voiceIncoming NOTIFY voiceIncomingChanged)
    Q_PROPERTY(QString voiceOutgoing READ voiceOutgoing NOTIFY voiceOutgoingChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)

public:
    explicit QOfonoCallBarring(QObject *parent = 0);
    ~QOfonoCallBarring();

    QString modemPath() const;
    void setModemPath(const QString &path);

     QString voiceIncoming();
     Q_INVOKABLE void setVoiceIncoming(const QString &barrings, const QString &password);

     QString voiceOutgoing();
     Q_INVOKABLE void setVoiceOutgoing(const QString &barrings, const QString &password);

     bool isValid() const;
     bool isReady() const;

Q_SIGNALS:
     void voiceIncomingChanged(const QString &barrings);
     void voiceOutgoingChanged(const QString &barrings);
     void voiceIncomingComplete(bool success);
     void voiceOutgoingComplete(bool success);
     void modemPathChanged(const QString &path);
     void getPropertiesFailed();
     void readyChanged();

     void changePasswordComplete(bool success);
//     void disableAllComplete(bool success);
//     void disableAllIncomingComplete(bool success);
//     void disableAllOutgoingComplete(bool success);

public slots:
     void changePassword(const QString &oldPassword, const QString &newPassword);
     void disableAll(const QString &password);
     void disableAllIncoming(const QString &password);
     void disableAllOutgoing(const QString &password);

private:
    QOfonoCallBarringPrivate *d_ptr;
private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
    void getPropertiesComplete(QDBusPendingCallWatcher *);
    void setVoiceIncomingComplete(QDBusPendingCallWatcher *);
    void setVoiceOutgoingComplete(QDBusPendingCallWatcher *);
    void changePasswordCallComplete(QDBusPendingCallWatcher *);
};

#endif // QOFONOCALLBARRING_H
