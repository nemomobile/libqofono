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

#ifndef QOFONOSupplimentaryServices_H
#define QOFONOSupplimentaryServices_H

#include "qofonomodeminterface.h"
#include "qofono_global.h"

//! This class is used to access ofono supplementary services API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/supplementaryservices-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoSupplementaryServices : public QOfonoModemInterface
{
    Q_OBJECT
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)

public:
    explicit QOfonoSupplementaryServices(QObject *parent = 0);
    ~QOfonoSupplementaryServices();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QString state() const;

    bool isValid() const;

    Q_INVOKABLE void initiate(const QString &command);
    Q_INVOKABLE void respond(const QString &reply);
    Q_INVOKABLE void cancel();

Q_SIGNALS:
    void notificationReceived(const QString &message);
    void requestReceived(const QString &message);
    void ussdResponse(const QString &response);
    void callBarringResponse(const QString &ssOp, const QString &cbService, const QVariantMap &cbMap);
    void callForwardingResponse(const QString &ssOp, const QString &cfService, const QVariantMap &cfMap);
    void callWaitingResponse(const QString &ssOp, const QVariantMap &cwMap);
    void callingLinePresentationResponse(const QString &ssOp, const QString &status);
    void connectedLinePresentationResponse(const QString &ssOp, const QString &status);
    void callingLineRestrictionResponse(const QString &ssOp, const QString &status);
    void connectedLineRestrictionResponse(const QString &ssOp, const QString &status);
    void initiateFailed();
    void respondComplete(bool success, const QString &message);
    void cancelComplete(bool success);

    void stateChanged(const QString &state);
    void modemPathChanged(const QString &path);
    
protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void propertyChanged(const QString &key, const QVariant &value);

private slots:
    void initiateResponseReceived(QDBusPendingCallWatcher*);
    void respondResponseReceived(QDBusPendingCallWatcher*);
    void cancelResponseReceived(QDBusPendingCallWatcher*);
};

#endif // QOFONOSupplimentaryServices_H
