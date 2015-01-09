/****************************************************************************
**
** Copyright (C) 2013-2015 Jolla Ltd.
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

#include "qofonosupplementaryservices.h"
#include "ofono_supplementary_services_interface.h"

#define SUPER QOfonoModemInterface

QOfonoSupplementaryServices::QOfonoSupplementaryServices(QObject *parent) :
    SUPER(OfonoSupplementaryServices::staticInterfaceName(), parent)
{
}

QOfonoSupplementaryServices::~QOfonoSupplementaryServices()
{
}

QDBusAbstractInterface *QOfonoSupplementaryServices::createDbusInterface(const QString &path)
{
    return new OfonoSupplementaryServices("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoSupplementaryServices::initiate(const QString &command)
{
    OfonoSupplementaryServices *iface = (OfonoSupplementaryServices*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(iface->Initiate(command), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(initiateResponseReceived(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSupplementaryServices::respond(const QString &command)
{
    OfonoSupplementaryServices *iface = (OfonoSupplementaryServices*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(iface->Respond(command), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(respondResponseReceived(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSupplementaryServices::cancel()
{
    OfonoSupplementaryServices *iface = (OfonoSupplementaryServices*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(iface->Cancel(), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(cancelResponseReceived(QDBusPendingCallWatcher*)));
    }
}

void QOfonoSupplementaryServices::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("State")) {
        Q_EMIT stateChanged(value.value<QString>());
    }
}

QString QOfonoSupplementaryServices::state() const
{
    return getString("State");
}

void QOfonoSupplementaryServices::initiateResponseReceived(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<QString, QDBusVariant> reply = *call;
    if (reply.isError()) {
        emit initiateFailed();
        return;
    }

    QString type = reply.value();
    if (type == QLatin1String("USSD")) {
        QString resp = qvariant_cast<QDBusVariant>(reply.argumentAt(1)).variant().toString();
        emit ussdResponse(resp);
    } else {
        QVariant val = qvariant_cast<QDBusVariant>(reply.argumentAt(1)).variant();
        const QDBusArgument arg = qvariant_cast<QDBusArgument>(val);

        if (type == QLatin1String("CallBarring")) {
            QString ss_op;
            QString cb_service;
            QVariantMap cb_dict;
            arg.beginStructure();
            arg >> ss_op >> cb_service >> cb_dict;
            arg.endStructure();
            emit callBarringResponse(ss_op, cb_service, cb_dict);
        } else if (type == QLatin1String("CallForwarding")) {
            QString ss_op;
            QString cf_service;
            QVariantMap cf_dict;
            arg.beginStructure();
            arg >> ss_op >> cf_service >> cf_dict;
            arg.endStructure();
            emit callForwardingResponse(ss_op, cf_service, cf_dict);
        } else if (type == QLatin1String("CallWaiting")) {
            QString ss_op;
            QVariantMap cw_dict;
            arg.beginStructure();
            arg >> ss_op >> cw_dict;
            arg.endStructure();
            emit callWaitingResponse(ss_op, cw_dict);
        } else if (type == QLatin1String("CallingLinePresentation")) {
            QString ss_op;
            QString status;
            arg.beginStructure();
            arg >> ss_op >> status;
            arg.endStructure();
            emit callingLinePresentationResponse(ss_op, status);
        } else if (type == QLatin1String("ConnectedLinePresentation")) {
            QString ss_op;
            QString status;
            arg.beginStructure();
            arg >> ss_op >> status;
            arg.endStructure();
            emit connectedLinePresentationResponse(ss_op, status);
        } else if (type == QLatin1String("CallingLineRestriction")) {
            QString ss_op;
            QString status;
            arg.beginStructure();
            arg >> ss_op >> status;
            arg.endStructure();
            emit callingLineRestrictionResponse(ss_op, status);
        } else if (type == QLatin1String("ConnectedLineRestriction")) {
            QString ss_op;
            QString status;
            arg.beginStructure();
            arg >> ss_op >> status;
            arg.endStructure();
            emit connectedLineRestrictionResponse(ss_op, status);
        } else {
            emit initiateFailed();
        }
    }
}

void QOfonoSupplementaryServices::respondResponseReceived(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<QString> reply = *call;
    Q_EMIT respondComplete(!reply.isError(), reply.value());
}

void QOfonoSupplementaryServices::cancelResponseReceived(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT cancelComplete(!reply.isError());
}

QString QOfonoSupplementaryServices::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoSupplementaryServices::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

bool QOfonoSupplementaryServices::isValid() const
{
    return SUPER::isValid();
}
