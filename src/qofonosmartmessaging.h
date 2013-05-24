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

#ifndef QOFONOSMARTMESSAGING_H
#define QOFONOSMARTMESSAGING_H

#include <QObject>
#include <QDBusVariant>

#include "qofono_global.h"
//! This class is used to access ofono cell broadcast API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/smartmessaging-api.txt
 */

class QOfonoSmartMessagingPrivate;
class QOFONOSHARED_EXPORT QOfonoSmartMessaging : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)

public:
    explicit QOfonoSmartMessaging(QObject *parent = 0);
    ~QOfonoSmartMessaging();

    QString modemPath() const;
    void setModemPath(const QString &path);
    bool isValid() const;
public slots:

    QDBusObjectPath sendAppointment(const QString &toPhoneNumber, const QByteArray &appointment);
    QDBusObjectPath sendBusinessCard(const QString &toPhoneNumber, const QByteArray &card);

    void registerAgent(const QString &objectPath);
    void unregisterAgent(const QString &objectPath);

private:
    QOfonoSmartMessagingPrivate *d_ptr;
};

#endif // QOFONOSMARTMESSAGING_H
