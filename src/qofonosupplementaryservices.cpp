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

#include "qofonosupplementaryservices.h"
#include "dbus/ofonosupplementaryservices.h"

class QOfonoSupplementaryServicesPrivate
{
public:
    QOfonoSupplementaryServicesPrivate();
    QString modemPath;
    OfonoSupplementaryServices *supplementaryServices;
    QVariantMap properties;

};

QOfonoSupplementaryServicesPrivate::QOfonoSupplementaryServicesPrivate() :
    modemPath(QString())
  , supplementaryServices(0)
{
}

QOfonoSupplementaryServices::QOfonoSupplementaryServices(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoSupplementaryServicesPrivate)
{
}

QOfonoSupplementaryServices::~QOfonoSupplementaryServices()
{
    delete d_ptr;
}

void QOfonoSupplementaryServices::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        if (d_ptr->supplementaryServices) {
            delete d_ptr->supplementaryServices;
            d_ptr->supplementaryServices = 0;
            d_ptr->properties.clear();
        }
        d_ptr->supplementaryServices = new OfonoSupplementaryServices("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->supplementaryServices->isValid()) {
            d_ptr->modemPath = path;

            connect(d_ptr->supplementaryServices,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            connect(d_ptr->supplementaryServices,SIGNAL(NotificationReceived(QString)),
                    this,SIGNAL(notificationReceived(QString)));
            connect(d_ptr->supplementaryServices,SIGNAL(RequestReceived(QString)),
                    this,SIGNAL(requestReceived(QString)));
            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->supplementaryServices->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoSupplementaryServices::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoSupplementaryServices::initiate(const QString &command)
{
    QDBusPendingReply<QString, QDBusVariant> result = d_ptr->supplementaryServices->Initiate(command);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(initiateResponseReceived(QDBusPendingCallWatcher*)));
}

void QOfonoSupplementaryServices::respond(const QString &command)
{
    QDBusPendingReply<QString> result = d_ptr->supplementaryServices->Respond(command);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(respondResponseReceived(QDBusPendingCallWatcher*)));
}

void QOfonoSupplementaryServices::cancel()
{
    QDBusPendingReply<> result = d_ptr->supplementaryServices->Cancel();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(cancelResponseReceived(QDBusPendingCallWatcher*)));
}

void QOfonoSupplementaryServices::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("State")) {
        Q_EMIT stateChanged(value.value<QString>());
    }
}


QString QOfonoSupplementaryServices::state() const
{
    if (d_ptr->supplementaryServices)
        return d_ptr->properties["State"].value<QString>();
    else
        return QString();
}

bool QOfonoSupplementaryServices::isValid() const
{
    return d_ptr->supplementaryServices->isValid();
}

void QOfonoSupplementaryServices::initiateResponseReceived(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString, QDBusVariant> reply = *call;
    if (reply.isError()) {
        call->deleteLater();
        emit initiateFailed();
        return;
    }

    QString type = reply.value();
    if (type == QLatin1String("USSD")) {
        QString resp = reply.argumentAt(1).toString();
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
    call->deleteLater();
}

void QOfonoSupplementaryServices::respondResponseReceived(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString> reply = *call;
    Q_EMIT respondComplete(!reply.isError(), reply.value());
    call->deleteLater();
}

void QOfonoSupplementaryServices::cancelResponseReceived(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    Q_EMIT cancelComplete(!reply.isError());
    call->deleteLater();
}
