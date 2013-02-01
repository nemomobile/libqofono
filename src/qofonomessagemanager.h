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
