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

#ifndef QOFONOMessageManager_H
#define QOFONOMessageManager_H

#include <QObject>
#include <QDBusVariant>
#include <QStringList>
#include "qofono_global.h"
//! This class is used to access ofono message manager API
/*!
 * oFono message manager API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/messagemanager-api.txt
 */

class QOfonoMessageManagerPrivate;
class QOFONOSHARED_EXPORT QOfonoMessageManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(QString serviceCenterAddress READ serviceCenterAddress WRITE setServiceCenterAddress NOTIFY serviceCenterAddressChanged)
    Q_PROPERTY(bool useDeliveryReports READ useDeliveryReports WRITE setUseDeliveryReports NOTIFY useDeliveryReportsChanged)
    Q_PROPERTY(QString bearer READ bearer WRITE setBearer NOTIFY bearerChanged)
    Q_PROPERTY(QString alphabet READ alphabet WRITE setAlphabet NOTIFY alphabetChanged)

public:
    explicit QOfonoMessageManager(QObject *parent = 0);
    ~QOfonoMessageManager();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QString serviceCenterAddress();
    void setServiceCenterAddress(const QString &address);

    bool useDeliveryReports();
    void setUseDeliveryReports(bool useDeliveryReports);

    QString bearer();
    void setBearer(const QString &bearer);

    QString alphabet();
    void setAlphabet(const QString &alphabet);

    Q_INVOKABLE void sendMessage(const QString &numberTo, const QString &message);
    Q_INVOKABLE QStringList messages();

    bool isValid() const;
Q_SIGNALS:
    void serviceCenterAddressChanged(const QString &address);
    void useDeliveryReportsChanged(const bool &useDeliveryReports);
    void bearerChanged(const QString &bearer);
    void alphabetChanged(const QString &alphabet);

    void immediateMessage(const QString &message, const QVariantMap &info);
    void incomingMessage(const QString &message, const QVariantMap &info);

    void messageAdded(const QString &message);
    void messageRemoved(const QString &message);
    void modemPathChanged(const QString &path);

    void messagesFinished();

    void sendMessageComplete(bool success, const QString& path);
    void setServiceCenterAddressComplete(bool success);
    void setUseDeliveryReportsComplete(bool success);
    void setBearerComplete(bool success);
    void setAlphabetComplete(bool success);

private:
    QOfonoMessageManagerPrivate *d_ptr;
private slots:
    void onMessageAdded(const QDBusObjectPath &path, const QVariantMap &properties);
    void onMessageRemoved(const QDBusObjectPath &path);

    void propertyChanged(const QString &property,const QDBusVariant &value);
    void getMessagesFinished(const ObjectPathPropertiesList &list);
    void messagesError(const QDBusError &error);

    void sendMessageFinished(QDBusPendingCallWatcher *call);
    void setServiceCenterAddressFinished(QDBusPendingCallWatcher*);
    void setUseDeliveryReportsFinished(QDBusPendingCallWatcher*);
    void setBearerFinished(QDBusPendingCallWatcher*);
    void setAlphabetFinished(QDBusPendingCallWatcher*);
};

#endif // QOFONOMessageManager_H
