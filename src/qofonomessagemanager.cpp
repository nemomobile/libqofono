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
    if (!d_ptr->messageManager) {
        if (path != modemPath()) {
            d_ptr->messageManager = new OfonoMessageManager("org.ofono", path, QDBusConnection::systemBus(),this);

            if (d_ptr->messageManager->isValid()) {
                d_ptr->modemPath = path;
                connect(d_ptr->messageManager,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                        this,SLOT(propertyChanged(QString,QDBusVariant)));
                connect(d_ptr->messageManager,SIGNAL(ImmediateMessage(QString,QVariantMap)),
                        this,SIGNAL(immediateMessage(QString,QVariantMap)));
                connect(d_ptr->messageManager,SIGNAL(IncomingMessage(QString,QVariantMap)),
                        this,SIGNAL(incomingMessage(QString,QVariantMap)));

                QDBusReply<QVariantMap> reply;
                reply = d_ptr->messageManager->GetProperties();
                d_ptr->properties = reply.value();

                ObjectPathPropertiesList messages;

                QDBusMessage request = QDBusMessage::createMethodCall("org.ofono",
                                                                      "org.ofono.MessageManager",
                                                                      path,
                                                                      "GetMessages");
                QDBusReply<ObjectPathPropertiesList> reply2 = QDBusConnection::systemBus().call(request);

                messages = reply2.value();
                foreach(ObjectPathProperties message, messages) {
                    d_ptr->messageList << message.path.path();
                }
                Q_EMIT modemPathChanged(path);
            }
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
    } else if (property == QLatin1String("AlphabetChanged")) {
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
    if (d_ptr->messageManager)
        d_ptr->messageManager->SetProperty("ServiceCenterAddress",QDBusVariant(address));

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
    if (d_ptr->messageManager)
        d_ptr->messageManager->SetProperty("UseDeliveryReports",QDBusVariant(useDeliveryReports));
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
    if (d_ptr->messageManager)
        d_ptr->messageManager->SetProperty("Bearer",QDBusVariant(bearer));
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
    if (d_ptr->messageManager)
        d_ptr->messageManager->SetProperty("Alphabet",QDBusVariant(alphabet));

}

void QOfonoMessageManager::sendMessage(const QString &numberTo, const QString &message)
{
    if (d_ptr->messageManager)
        d_ptr->messageManager->SendMessage(numberTo,message);
}

QStringList QOfonoMessageManager::messages()
{
    if (d_ptr->messageManager)
        return d_ptr->messageList;
    return QStringList();
}


void QOfonoMessageManager::onMessageAdded(const QString &message)
{
    if (d_ptr->messageManager) {
        if (!d_ptr->messageList.contains(message)) {
            d_ptr->messageList.append(message);
        }

    }
}

void QOfonoMessageManager::onMessageRemoved(const QString &message)
{
    if (d_ptr->messageManager) {
        if (d_ptr->messageList.contains(message)) {
            d_ptr->messageList.removeOne(message);
        }
    }
}

bool QOfonoMessageManager::isValid() const
{
    return d_ptr->messageManager->isValid();
}
