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

#ifndef QOFONOMessageWaiting_H
#define QOFONOMessageWaiting_H

#include <QObject>
#include <QDBusVariant>
#include "qofono_global.h"
//! This class is used to access ofono message waiting API
/*!
 * oFono message manager API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/message-waiting-api.txt
 */

class QOfonoMessageWaitingPrivate;
class QOFONOSHARED_EXPORT QOfonoMessageWaiting : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(bool voicemailWaiting READ voicemailWaiting NOTIFY voicemailWaitingChanged)
    Q_PROPERTY(int voicemailMessageCount READ voicemailMessageCount NOTIFY voicemailMessageCountChanged)
    Q_PROPERTY(QString voicemailMailboxNumber READ voicemailMailboxNumber WRITE setVoicemailMailboxNumber NOTIFY voicemailMailboxNumberChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)

public:
    explicit QOfonoMessageWaiting(QObject *parent = 0);
    ~QOfonoMessageWaiting();

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool voicemailWaiting() const;
    int voicemailMessageCount() const;
    QString voicemailMailboxNumber() const;
    void setVoicemailMailboxNumber(const QString &mailboxnumber);

    bool isValid() const;
    bool isReady() const;

    void connectOfono();

Q_SIGNALS:
    void voicemailWaitingChanged(bool waiting);
    void voicemailMessageCountChanged(int count);
    void voicemailMailboxNumberChanged(const QString &mailboxnumber);
    void modemPathChanged(const QString &path);

    void voicemailMailboxComplete(bool success);
    void getPropertiesFailed();
    void readyChanged();
    
private:
    QOfonoMessageWaitingPrivate *d_ptr;

private slots:
    void modemInterfacesChanged(const QStringList &interfaces);
    void propertyChanged(const QString &property,const QDBusVariant &value);
    void setVoicemailMailboxNumberComplete(QDBusPendingCallWatcher*);
};

#endif // QOFONOMessageWaiting_H
