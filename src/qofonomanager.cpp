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

#include "qofonomanager.h"
#include "ofono_manager_interface.h"

class QOfonoManager::Private
{
public:
    OfonoManager *ofonoManager;
    QStringList modems;
    bool available;

    Private() : ofonoManager(NULL), available(false) {}
};

QOfonoManager::QOfonoManager(QObject *parent) :
    QObject(parent),
    d_ptr(new Private)
{
    QOfonoDbusTypes::registerObjectPathProperties();
    QDBusConnection systemBus(QDBusConnection::systemBus());
    QDBusServiceWatcher *ofonoWatcher = new QDBusServiceWatcher("org.ofono", systemBus,
            QDBusServiceWatcher::WatchForRegistration |
            QDBusServiceWatcher::WatchForUnregistration, this);

    connect(ofonoWatcher, SIGNAL(serviceRegistered(QString)),
            this, SLOT(connectToOfono(QString)));
    connect(ofonoWatcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(ofonoUnregistered(QString)));

    if (systemBus.interface()->isServiceRegistered("org.ofono")) {
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

QString QOfonoManager::defaultModem()
{
    return d_ptr->modems.isEmpty() ? QString() : d_ptr->modems[0];
}

bool QOfonoManager::available() const
{
    return d_ptr->available;
}

bool QOfonoManager::isValid() const
{
    // isValid() is essentially the same as available(), keeping it around for
    // backward compatibility
    return d_ptr->available;
}

void QOfonoManager::onModemAdded(const QDBusObjectPath& path, const QVariantMap&)
{
    QString pathStr = path.path();
    if (!d_ptr->modems.contains(pathStr)) {
        QString prevDefault = defaultModem();
        d_ptr->modems.append(pathStr);
        qSort(d_ptr->modems);
        Q_EMIT modemAdded(pathStr);
        Q_EMIT modemsChanged(d_ptr->modems);
        QString newDefault = defaultModem();
        if (newDefault != prevDefault) {
            Q_EMIT defaultModemChanged(newDefault);
        }
    }
}

void QOfonoManager::onModemRemoved(const QDBusObjectPath& path)
{
    QString pathStr = path.path();
    QString prevDefault = defaultModem();
    if (d_ptr->modems.removeOne(pathStr)) {
        Q_EMIT modemRemoved(pathStr);
        Q_EMIT modemsChanged(d_ptr->modems);
        QString newDefault = defaultModem();
        if (newDefault != prevDefault) {
            Q_EMIT defaultModemChanged(newDefault);
        }
    }
}

void QOfonoManager::onGetModemsFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watcher);
    if (reply.isValid() && !reply.isError()) {
        QString prevDefault = defaultModem();
        QStringList newModems;
        Q_FOREACH(ObjectPathProperties modem, reply.value()) {
            newModems.append(modem.path.path());
        }
        qSort(newModems);
        if (d_ptr->modems != newModems) {
            d_ptr->modems = newModems;
            Q_EMIT modemsChanged(d_ptr->modems);
        }
        QString newDefault = defaultModem();
        if (newDefault != prevDefault) {
            Q_EMIT defaultModemChanged(newDefault);
        }
        d_ptr->available = true;
        Q_EMIT availableChanged(true);
    }
    watcher->deleteLater();
}

void QOfonoManager::connectToOfono(const QString &)
{
    if (!d_ptr->ofonoManager) {
        OfonoManager* mgr = new OfonoManager("org.ofono", "/", QDBusConnection::systemBus(), this);
        if (mgr->isValid()) {
            d_ptr->ofonoManager = mgr;
            connect(new QDBusPendingCallWatcher(mgr->GetModems(), mgr),
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(onGetModemsFinished(QDBusPendingCallWatcher*)));
            connect(mgr,
                SIGNAL(ModemAdded(QDBusObjectPath,QVariantMap)),
                SLOT(onModemAdded(QDBusObjectPath,QVariantMap)));
            connect(mgr,
                SIGNAL(ModemRemoved(QDBusObjectPath)),
                SLOT(onModemRemoved(QDBusObjectPath)));
        } else {
            delete mgr;
        }
    }
}

void QOfonoManager::ofonoUnregistered(const QString &)
{
    if (d_ptr->available) {
        d_ptr->available = false;
        Q_EMIT availableChanged(false);
    }
    if (d_ptr->ofonoManager) {
        delete d_ptr->ofonoManager;
        d_ptr->ofonoManager = NULL;
        if (!d_ptr->modems.isEmpty()) {
            Q_FOREACH(QString modem, d_ptr->modems) {
                Q_EMIT modemRemoved(modem);
            }
            d_ptr->modems.clear();
            Q_EMIT modemsChanged(d_ptr->modems);
            Q_EMIT defaultModemChanged(QString());
        }
    }
}

QSharedPointer<QOfonoManager> QOfonoManager::instance()
{
    static QWeakPointer<QOfonoManager> sharedInstance;
    QSharedPointer<QOfonoManager> mgr = sharedInstance;
    if (mgr.isNull()) {
        mgr = QSharedPointer<QOfonoManager>::create();
        sharedInstance = mgr;
    }
    return mgr;
}
