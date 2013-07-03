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

#include "qofonomanager.h"
#include "qofonomodem.h"
#include "dbus/ofonomanager.h"
#include <QVariant>
#include <QTimer>
#include "dbustypes.h"

QDBusArgument &operator<<(QDBusArgument &argument, const ObjectPathProperties &modem)
{
    argument.beginStructure();
    argument << modem.path << modem.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ObjectPathProperties &modem)
{
    argument.beginStructure();
    argument >> modem.path >> modem.properties;
    argument.endStructure();
    return argument;
}


class QOfonoManagerPrivate
{
public:
    QOfonoManagerPrivate();
    OfonoManager *ofonoManager;
    QStringList modems;
    bool available;
    QDBusServiceWatcher *ofonoWatcher;
};

QOfonoManagerPrivate::QOfonoManagerPrivate() :
 ofonoManager(0)
, modems(QStringList())
, available(0)
, ofonoWatcher(0)
{
    qDBusRegisterMetaType<ObjectPathProperties>();
    qDBusRegisterMetaType<ObjectPathPropertiesList>();
    qRegisterMetaType<ObjectPathProperties>("ObjectPathProperties");
    qRegisterMetaType<ObjectPathPropertiesList>("ObjectPathPropertiesList");
}

QOfonoManager::QOfonoManager(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoManagerPrivate)
{

    d_ptr->ofonoWatcher = new QDBusServiceWatcher("org.ofono",QDBusConnection::systemBus(),
            QDBusServiceWatcher::WatchForRegistration |
            QDBusServiceWatcher::WatchForUnregistration, this);

    connect(d_ptr->ofonoWatcher, SIGNAL(serviceRegistered(QString)),
            this, SLOT(connectToOfono(QString)));
    connect(d_ptr->ofonoWatcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(ofonoUnregistered(QString)));

    d_ptr->available = QDBusConnection::systemBus().interface()->isServiceRegistered("org.ofono");
    if(d_ptr->available) {
        connectToOfono(QString());
    }
}

QOfonoManager::~QOfonoManager()
{
    delete d_ptr;
}

QStringList QOfonoManager::modems()
{
    return d_ptr->modems;
}

void QOfonoManager::onModemAdd(const QDBusObjectPath& path, const QVariantMap& var)
{
    QString pathStr = path.path();
    if (!d_ptr->modems.contains(pathStr)) {
        d_ptr->modems.append(pathStr);
        Q_EMIT modemAdded(pathStr);
        Q_EMIT modemsChanged(d_ptr->modems);
    }
}

void QOfonoManager::onModemRemove(const QDBusObjectPath& path)
{
    QString pathStr = path.path();
    /* we need to send out modem removed signal, since we decided to turn modem off directly */
    Q_EMIT modemRemoved(pathStr);
    if (d_ptr->modems.contains(pathStr)) {
        d_ptr->modems.removeOne(pathStr);
        Q_EMIT modemsChanged(d_ptr->modems);
    }
}

bool QOfonoManager::available() const
{
    return d_ptr->available;
}

void QOfonoManager::connectToOfono(const QString &)
{
    d_ptr->ofonoManager = new OfonoManager("org.ofono","/",QDBusConnection::systemBus(),this);
    if (d_ptr->ofonoManager->isValid()) {
        QDBusPendingReply<ObjectPathPropertiesList> reply = d_ptr->ofonoManager->GetModems();
        reply.waitForFinished();
        // fugly I know... but we need sorted modems
        // with hardware listed first
        QOfonoModem oModem;
        foreach(ObjectPathProperties modem, reply.value()) {
            QString modemPath = modem.path.path();
            oModem.setModemPath(modemPath);
            if (oModem.type() == "hardware") {
                d_ptr->modems.prepend(modemPath);
            } else {
                d_ptr->modems.append(modemPath);
            }

            Q_EMIT modemAdded(modemPath);
        }
        Q_EMIT modemsChanged(d_ptr->modems);
    }
    Q_EMIT availableChanged(true);
    connect(d_ptr->ofonoManager, SIGNAL(ModemAdded(QDBusObjectPath,QVariantMap)), this, SLOT(onModemAdd(QDBusObjectPath,QVariantMap)));
    connect(d_ptr->ofonoManager, SIGNAL(ModemRemoved(QDBusObjectPath)), this, SLOT(onModemRemove(QDBusObjectPath)));
}

void QOfonoManager::ofonoUnregistered(const QString &)
{
    d_ptr->available = false;
    delete d_ptr->ofonoManager;
    Q_FOREACH(const QString &modem, d_ptr->modems) {
        Q_EMIT modemRemoved(modem);
    }
    d_ptr->modems.clear();
    Q_EMIT modemsChanged(d_ptr->modems);
    Q_EMIT availableChanged(false);
}


bool QOfonoManager::isValid() const
{
    return d_ptr->ofonoManager->isValid();
}
