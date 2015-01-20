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

#include "qofonoconnectionmanager.h"
#include "ofono_connection_manager_interface.h"

#define SUPER QOfonoModemInterface

class QOfonoConnectionManager::Private : public QOfonoObject::ExtData
{
public:
    bool initialized;
    QStringList contexts;
    QHash<QString,QString> contextTypes;
    QString filter;

    Private() : initialized(false) {}
    void filterContexts();
};

// FILTER = [!]NAMES
// NAMES = NAME [,NAMES]
// Spaces and tabs are ignored
void QOfonoConnectionManager::Private::filterContexts()
{
    if (contextTypes.isEmpty()) {
        contexts.clear();
    } else {
        QStringList contextList = contextTypes.keys();
        if (filter.isEmpty()) {
            contexts = contextList;
        } else {
            contexts.clear();
            QString f(filter);
            f.remove(' ').remove('\t');
            if (f[0] == '!') {
                QStringList blackList = f.remove(0,1).split(',', QString::SkipEmptyParts);
                foreach (QString path, contextList) {
                    if (!blackList.contains(contextTypes.value(path)))
                        contexts.append(path);
                }
            } else {
                QStringList whiteList = f.split(',', QString::SkipEmptyParts);
                foreach (QString path, contextList) {
                    if (whiteList.contains(contextTypes.value(path)))
                        contexts.append(path);
                }
            }
        }
    }
}

QOfonoConnectionManager::QOfonoConnectionManager(QObject *parent) :
    SUPER(OfonoConnectionManager::staticInterfaceName(), new Private, parent)
{
    QOfonoDbusTypes::registerObjectPathProperties();
}

QOfonoConnectionManager::~QOfonoConnectionManager()
{
}

bool QOfonoConnectionManager::isValid() const
{
    return privateData()->initialized && SUPER::isValid();
}

QDBusAbstractInterface *QOfonoConnectionManager::createDbusInterface(const QString &path)
{
    OfonoConnectionManager* iface = new OfonoConnectionManager("org.ofono", path, QDBusConnection::systemBus(), this);
    connect(new QDBusPendingCallWatcher(iface->GetContexts(), iface),
        SIGNAL(finished(QDBusPendingCallWatcher*)),
        SLOT(onGetContextsFinished(QDBusPendingCallWatcher*)));
    connect(iface,
        SIGNAL(ContextAdded(QDBusObjectPath,QVariantMap)),
        SLOT(onContextAdded(QDBusObjectPath,QVariantMap)));
    connect(iface,
        SIGNAL(ContextRemoved(QDBusObjectPath)),
        SLOT(onContextRemoved(QDBusObjectPath)));
    return iface;
}

void QOfonoConnectionManager::dbusInterfaceDropped()
{
    QOfonoModemInterface::dbusInterfaceDropped();
    Private *d_ptr = privateData();
    d_ptr->initialized = false;
    if (!d_ptr->contexts.isEmpty()) {
        QStringList list = d_ptr->contexts;
        d_ptr->contexts.clear();
        d_ptr->contextTypes.clear();
        for (int i=0; i<list.count(); i++) {
            Q_EMIT contextRemoved(list[i]);
        }
    }
}

void QOfonoConnectionManager::deactivateAll()
{
    OfonoConnectionManager *iface = (OfonoConnectionManager*)dbusInterface();
    if (iface) {
        iface->DeactivateAll();
    }
}

void QOfonoConnectionManager::addContext(const QString &type)
{
    OfonoConnectionManager *iface = (OfonoConnectionManager*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->AddContext(type), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onAddContextFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoConnectionManager::removeContext(const QString &path)
{
    OfonoConnectionManager *iface = (OfonoConnectionManager*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(
            iface->RemoveContext(QDBusObjectPath(path)), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onRemoveContextFinished(QDBusPendingCallWatcher*)));
    }
}

bool QOfonoConnectionManager::attached() const
{
    return getBool("Attached");
}

QString QOfonoConnectionManager::bearer() const
{
    return getString("Bearer");
}

bool QOfonoConnectionManager::suspended() const
{
    return getBool("Suspended");
}


bool QOfonoConnectionManager::roamingAllowed() const
{
    return getBool("RoamingAllowed");
}

void QOfonoConnectionManager::setRoamingAllowed(bool value)
{
    setProperty("RoamingAllowed", value);
}

bool QOfonoConnectionManager::powered() const
{
    return getBool("Powered");
}

void QOfonoConnectionManager::setPowered(bool value)
{
    setProperty("Powered", value);
}

void QOfonoConnectionManager::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("Attached")) {
        Q_EMIT attachedChanged(value.value<bool>());
    } else if (property == QLatin1String("Bearer")) {
        Q_EMIT bearerChanged(value.value<QString>());
    } else if (property == QLatin1String("Suspended")) {
        Q_EMIT suspendedChanged(value.value<bool>());
    } else if (property == QLatin1String("RoamingAllowed")) {
        Q_EMIT roamingAllowedChanged(value.value<bool>());
    } else if (property == QLatin1String("Powered")) {
        Q_EMIT poweredChanged(value.value<bool>());
    }
}

QStringList QOfonoConnectionManager::contexts()
{
    return privateData()->contexts;
}

void QOfonoConnectionManager::onContextAdded(const QDBusObjectPath &path, const QVariantMap &properties)
{
    QString contextPath(path.path());
    Private *d_ptr = privateData();
    d_ptr->contextTypes.insert(contextPath, properties.value("Type").toString());
    d_ptr->filterContexts();
    if (d_ptr->contexts.contains(contextPath)) {
        Q_EMIT contextAdded(contextPath);
        Q_EMIT contextsChanged(d_ptr->contexts);
    }
}

void QOfonoConnectionManager::onContextRemoved(const QDBusObjectPath &path)
{
    Private *d_ptr = privateData();
    QString contextPath(path.path());
    d_ptr->contextTypes.remove(contextPath);
    if (d_ptr->contexts.removeOne(contextPath)) {
        Q_EMIT contextRemoved(contextPath);
        Q_EMIT contextsChanged(d_ptr->contexts);
    }
}

QString QOfonoConnectionManager::filter() const
{
    return privateData()->filter;
}

void QOfonoConnectionManager::setFilter(const QString &filter)
{
    Private *d_ptr = privateData();
    if (d_ptr->filter != filter) {
        d_ptr->filter = filter;
        d_ptr->filterContexts();
        Q_EMIT filterChanged(filter);
        Q_EMIT contextsChanged(d_ptr->contexts);
    }
}

void QOfonoConnectionManager::onGetContextsFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT reportError(reply.error().message());
    } else {
        Private *d_ptr = privateData();
        QStringList old = d_ptr->contexts;
        d_ptr->contextTypes.clear();
        foreach (ObjectPathProperties context, reply.value()) {
            QString contextPath(context.path.path());
            bool contextWasThere = d_ptr->contexts.contains(contextPath);
            d_ptr->contextTypes.insert(contextPath, context.properties.value("Type").toString());
            d_ptr->filterContexts();
            bool contextIsThere = d_ptr->contexts.contains(contextPath);
            if (contextWasThere && !contextIsThere) {
                Q_EMIT contextRemoved(contextPath);
            } else if (!contextWasThere && contextIsThere) {
                Q_EMIT contextAdded(contextPath);
            }
        }
        d_ptr->initialized = true;
        if (d_ptr->contexts != old) {
            Q_EMIT contextsChanged(d_ptr->contexts);
        }
    }
}

void QOfonoConnectionManager::onAddContextFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<QDBusObjectPath> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT reportError(reply.error().message());
    }
}

void QOfonoConnectionManager::onRemoveContextFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT reportError(reply.error().message());
    }
}

QString QOfonoConnectionManager::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoConnectionManager::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

QOfonoConnectionManager::Private *QOfonoConnectionManager::privateData() const
{
    return (QOfonoConnectionManager::Private*)SUPER::extData();
}
