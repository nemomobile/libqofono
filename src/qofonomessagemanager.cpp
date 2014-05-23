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

#include "dbustypes.h"
#include "qofonomessagemanager.h"
#include "dbus/ofonomessagemanager.h"

class QOfonoMessageManagerPrivate
{
public:
    QOfonoMessageManagerPrivate();
    QString modemPath;
    OfonoMessageManager *messageManager;
    QVariantMap properties;
    QStringList messageList;
    QString errorMessage;
};

QOfonoMessageManagerPrivate::QOfonoMessageManagerPrivate() :
    modemPath(QString())
  , messageManager(0)
  , messageList(QStringList())
{
}

QOfonoMessageManager::QOfonoMessageManager(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoMessageManagerPrivate)
{
}

QOfonoMessageManager::~QOfonoMessageManager()
{
    delete d_ptr;
}

void QOfonoMessageManager::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

       if (path != modemPath()) {
           if (d_ptr->messageManager) {
            delete d_ptr->messageManager;
            d_ptr->messageManager = 0;
            d_ptr->properties.clear();
        }
        d_ptr->messageManager = new OfonoMessageManager("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->messageManager->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->messageManager,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            connect(d_ptr->messageManager,SIGNAL(ImmediateMessage(QString,QVariantMap)),
                    this,SIGNAL(immediateMessage(QString,QVariantMap)));
            connect(d_ptr->messageManager,SIGNAL(IncomingMessage(QString,QVariantMap)),
                    this,SIGNAL(incomingMessage(QString,QVariantMap)));
            connect(d_ptr->messageManager,SIGNAL(MessageAdded(QDBusObjectPath,QVariantMap)),
                    this,SLOT(onMessageAdded(QDBusObjectPath,QVariantMap)));
            connect(d_ptr->messageManager,SIGNAL(MessageRemoved(QDBusObjectPath)),
                    this,SLOT(onMessageRemoved(QDBusObjectPath)));

            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->messageManager->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();

            QDBusMessage request = QDBusMessage::createMethodCall("org.ofono",
                                                                  path,
                                                                  "org.ofono.MessageManager",
                                                                  "GetMessages");

            QDBusConnection::systemBus().callWithCallback(request,
                                                           this,
                                                           SLOT(getMessagesFinished(ObjectPathPropertiesList)),
                                                           SLOT(messagesError(QDBusError)));
        }
    }
}

QString QOfonoMessageManager::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoMessageManager::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("ServiceCenterAddress")) {
        Q_EMIT serviceCenterAddressChanged(value.value<QString>());
    } else if (property == QLatin1String("UseDeliveryReports")) {
        Q_EMIT useDeliveryReportsChanged(value.value<bool>());
    } else if (property == QLatin1String("Bearer")) {
        Q_EMIT bearerChanged(value.value<QString>());
    } else if (property == QLatin1String("Alphabet")) {
        Q_EMIT alphabetChanged(value.value<QString>());
    }
}

QString QOfonoMessageManager::serviceCenterAddress()
{
    if (d_ptr->messageManager)
        return d_ptr->properties["ServiceCenterAddress"].value<QString>();
    else
        return QString();
}

void QOfonoMessageManager::setServiceCenterAddress(const QString &address)
{
    if (d_ptr->messageManager) {
        QDBusPendingReply<> result = d_ptr->messageManager->SetProperty("ServiceCenterAddress",QDBusVariant(address));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setServiceCenterAddressFinished(QDBusPendingCallWatcher*)));
    }
}

bool QOfonoMessageManager::useDeliveryReports()
{
    if (d_ptr->messageManager)
        return d_ptr->properties["UseDeliveryReports"].value<bool>();
    else
        return false;
}

void QOfonoMessageManager::setUseDeliveryReports(bool useDeliveryReports)
{
    if (d_ptr->messageManager) {
        QDBusPendingReply<> result = d_ptr->messageManager->SetProperty("UseDeliveryReports",QDBusVariant(useDeliveryReports));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setUseDeliveryReportsFinished(QDBusPendingCallWatcher*)));
    }
}

QString QOfonoMessageManager::bearer()
{
    if (d_ptr->messageManager)
        return d_ptr->properties["Bearer"].value<QString>();
    else
        return QString();
}

void QOfonoMessageManager::setBearer(const QString &bearer)
{
    if (d_ptr->messageManager) {
        QDBusPendingReply<> result = d_ptr->messageManager->SetProperty("Bearer",QDBusVariant(bearer));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setBearerFinished(QDBusPendingCallWatcher*)));
    }
}

QString QOfonoMessageManager::alphabet()
{
    if (d_ptr->messageManager)
        return d_ptr->properties["Alphabet"].value<QString>();
    else
        return QString();
}

void QOfonoMessageManager::setAlphabet(const QString &alphabet)
{
    if (d_ptr->messageManager) {
        QDBusPendingReply<> result = d_ptr->messageManager->SetProperty("Alphabet",QDBusVariant(alphabet));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setAlphabetFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoMessageManager::sendMessage(const QString &numberTo, const QString &message)
{
    if (d_ptr->messageManager) {
        QDBusPendingReply<QDBusObjectPath> result = d_ptr->messageManager->SendMessage(numberTo,message);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(sendMessageFinished(QDBusPendingCallWatcher*)));
    }
}

QStringList QOfonoMessageManager::messages()
{
    if (d_ptr->messageManager)
        return d_ptr->messageList;
    return QStringList();
}


void QOfonoMessageManager::onMessageAdded(const QDBusObjectPath &path,
        const QVariantMap &properties)
{
    Q_UNUSED(properties);

    if (d_ptr->messageManager) {
        if (!d_ptr->messageList.contains(path.path())) {
            d_ptr->messageList.append(path.path());
            Q_EMIT messageAdded(path.path());
        }
    }
}

void QOfonoMessageManager::onMessageRemoved(const QDBusObjectPath &path)
{
    if (d_ptr->messageManager) {
        if (d_ptr->messageList.contains(path.path())) {
            d_ptr->messageList.removeOne(path.path());
            Q_EMIT messageRemoved(path.path());
        }
    }
}

bool QOfonoMessageManager::isValid() const
{
    return d_ptr->messageManager->isValid();
}

void QOfonoMessageManager::getMessagesFinished(const ObjectPathPropertiesList &list)
{
    qDebug() << Q_FUNC_INFO << list.count();
//    messages = reply2.value();
    foreach(ObjectPathProperties message, list) {
        d_ptr->messageList << message.path.path();
        Q_EMIT messageAdded(message.path.path());
    }
    Q_EMIT messagesFinished();
}

void QOfonoMessageManager::messagesError(const QDBusError &error)
{
    qDebug() << Q_FUNC_INFO << error.message();
}

void QOfonoMessageManager::sendMessageFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<QDBusObjectPath> reply = *call;
    bool ok = true;
    if (reply.isError()) {
        qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
         d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
         ok = false;
    }
    Q_EMIT sendMessageComplete(ok, reply.value().path());
}

void QOfonoMessageManager::setServiceCenterAddressFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT setServiceCenterAddressComplete(!reply.isError());
}

void QOfonoMessageManager::setUseDeliveryReportsFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT setUseDeliveryReportsComplete(!reply.isError());
}

void QOfonoMessageManager::setBearerFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT setBearerComplete(!reply.isError());
}

void QOfonoMessageManager::setAlphabetFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT setAlphabetComplete(!reply.isError());
}

