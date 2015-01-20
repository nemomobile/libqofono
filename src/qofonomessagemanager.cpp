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

#include "qofonomessagemanager.h"
#include "ofono_message_manager_interface.h"

#define SUPER QOfonoModemInterface

class QOfonoMessageManager::Private : public QOfonoObject::ExtData
{
public:
    bool initialized;
    QStringList messageList;
    Private() : initialized(false) {}
};

QOfonoMessageManager::QOfonoMessageManager(QObject *parent) :
    SUPER(OfonoMessageManager::staticInterfaceName(), new Private, parent)
{
    QOfonoDbusTypes::registerObjectPathProperties();
}

QOfonoMessageManager::~QOfonoMessageManager()
{
}

bool QOfonoMessageManager::isValid() const
{
    return privateData()->initialized && SUPER::isValid();
}

QDBusAbstractInterface *QOfonoMessageManager::createDbusInterface(const QString &path)
{
    OfonoMessageManager* iface = new OfonoMessageManager("org.ofono", path, QDBusConnection::systemBus(), this);
    connect(new QDBusPendingCallWatcher(iface->GetMessages(), iface),
        SIGNAL(finished(QDBusPendingCallWatcher*)),
        SLOT(onGetMessagesFinished(QDBusPendingCallWatcher*)));
    connect(iface,
        SIGNAL(ImmediateMessage(QString,QVariantMap)),
        SIGNAL(immediateMessage(QString,QVariantMap)));
    connect(iface,
        SIGNAL(IncomingMessage(QString,QVariantMap)),
        SIGNAL(incomingMessage(QString,QVariantMap)));
    connect(iface,
        SIGNAL(MessageAdded(QDBusObjectPath,QVariantMap)),
        SLOT(onMessageAdded(QDBusObjectPath,QVariantMap)));
    connect(iface,
        SIGNAL(MessageRemoved(QDBusObjectPath)),
        SLOT(onMessageRemoved(QDBusObjectPath)));
    return iface;
}

void QOfonoMessageManager::dbusInterfaceDropped()
{
    SUPER::dbusInterfaceDropped();
    Private *d_ptr = privateData();
    d_ptr->initialized = false;
    if (!d_ptr->messageList.isEmpty()) {
        QStringList list = d_ptr->messageList;
        d_ptr->messageList.clear();
        for (int i=0; i<list.count(); i++) {
            Q_EMIT messageRemoved(list[i]);
        }
    }
}

void QOfonoMessageManager::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("ServiceCenterAddress")) {
        Q_EMIT serviceCenterAddressChanged(value.toString());
    } else if (property == QLatin1String("UseDeliveryReports")) {
        Q_EMIT useDeliveryReportsChanged(value.toBool());
    } else if (property == QLatin1String("Bearer")) {
        Q_EMIT bearerChanged(value.toString());
    } else if (property == QLatin1String("Alphabet")) {
        Q_EMIT alphabetChanged(value.toString());
    }
}

QString QOfonoMessageManager::serviceCenterAddress()
{
    return getString("ServiceCenterAddress");
}

void QOfonoMessageManager::setServiceCenterAddress(const QString &address)
{
    setProperty("ServiceCenterAddress", address);
}

bool QOfonoMessageManager::useDeliveryReports()
{
    return getBool("UseDeliveryReports");
}

void QOfonoMessageManager::setUseDeliveryReports(bool useDeliveryReports)
{
    setProperty("UseDeliveryReports", useDeliveryReports);
}

QString QOfonoMessageManager::bearer()
{
    return getString("Bearer");
}

void QOfonoMessageManager::setBearer(const QString &bearer)
{
    setProperty("Bearer", bearer);
}

QString QOfonoMessageManager::alphabet()
{
    return getString("Alphabet");
}

void QOfonoMessageManager::setAlphabet(const QString &alphabet)
{
    setProperty("Alphabet", alphabet);
}

void QOfonoMessageManager::sendMessage(const QString &numberTo, const QString &message)
{
    OfonoMessageManager *iface = (OfonoMessageManager*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(iface->SendMessage(numberTo, message), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onSendMessageFinished(QDBusPendingCallWatcher*)));
    }
}

QStringList QOfonoMessageManager::messages()
{
    return privateData()->messageList;
}

void QOfonoMessageManager::onMessageAdded(const QDBusObjectPath &path, const QVariantMap &)
{
    addMessage(path.path());
}

void QOfonoMessageManager::onMessageRemoved(const QDBusObjectPath &path)
{
    QString messagePath = path.path();
    if (privateData()->messageList.removeOne(messagePath)) {
        Q_EMIT messageRemoved(messagePath);
    }
}

void QOfonoMessageManager::onGetMessagesFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT reportError(reply.error().message());
    } else {
        ObjectPathPropertiesList list = reply.value();
        privateData()->initialized = true;
        for (int i=0; i<list.count(); i++) {
            addMessage(list[i].path.path());
        }
        Q_EMIT messagesFinished();
        if (isValid()) validChanged(true);
    }
}

void QOfonoMessageManager::addMessage(const QString &messagePath)
{
    Private *d_ptr = privateData();
    if (!d_ptr->messageList.contains(messagePath)) {
        d_ptr->messageList.append(messagePath);
        Q_EMIT messageAdded(messagePath);
    }
}

void QOfonoMessageManager::onSendMessageFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<QDBusObjectPath> reply = *watch;
    if (reply.isError()) {
        qWarning() << reply.error();
        Q_EMIT sendMessageComplete(false, QString());
    } else {
        Q_EMIT sendMessageComplete(true, reply.value().path());
    }
}

void QOfonoMessageManager::setPropertyFinished(const QString &property, const QDBusError *error)
{
    SUPER::setPropertyFinished(property, error);
    if (property == "ServiceCenterAddress") {
        Q_EMIT setServiceCenterAddressComplete(!error);
    } else if (property == "UseDeliveryReports") {
        Q_EMIT setUseDeliveryReportsComplete(!error);
    } else if (property == "Bearer") {
        Q_EMIT setBearerComplete(!error);
    } else if (property == "Alphabet") {
        Q_EMIT setAlphabetComplete(!error);
    }
}

QString QOfonoMessageManager::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoMessageManager::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

QOfonoMessageManager::Private *QOfonoMessageManager::privateData() const
{
    return (QOfonoMessageManager::Private*)SUPER::extData();
}
