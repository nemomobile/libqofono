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
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)
    Q_PROPERTY(QString serviceCenterAddress READ serviceCenterAddress)
    Q_PROPERTY(bool useDeliveryReports READ useDeliveryReports)
    Q_PROPERTY(QString bearer READ bearer)
    Q_PROPERTY(QString alphabet READ alphabet)

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

public slots:
    void onMessageAdded(const QString &message);
    void onMessageRemoved(const QString &message);

private:
    QOfonoMessageManagerPrivate *d_ptr;
private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONOMessageManager_H
