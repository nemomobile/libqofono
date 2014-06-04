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

#include "qofonoconnectionmanager.h"
#include "dbus/ofonoconnectionmanager.h"
#include "dbustypes.h"

class QOfonoConnectionManagerPrivate
{
public:
    QOfonoConnectionManagerPrivate();
    QString modemPath;
    OfonoConnectionManager *connman;
    QVariantMap properties;
    QStringList contexts;
    QHash<QString,QString> contextTypes;
    QString filter;

    void getContexts();
    void filterContexts();
};

QOfonoConnectionManagerPrivate::QOfonoConnectionManagerPrivate() :
    modemPath(QString())
   , connman(0)
  ,contexts(QStringList())
{
}

void QOfonoConnectionManagerPrivate::getContexts()
{
    contextTypes.clear();
    QDBusReply<ObjectPathPropertiesList> reply2 = connman->GetContexts();
    foreach(ObjectPathProperties context, reply2.value()) {
        contextTypes.insert(context.path.path(), context.properties.value("Type").toString());
    }
    filterContexts();
}

// FILTER = [!]NAMES
// NAMES = NAME [,NAMES]
// Spaces and tabs are ignored
void QOfonoConnectionManagerPrivate::filterContexts()
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
    QObject(parent),
    d_ptr(new QOfonoConnectionManagerPrivate)
{
}

QOfonoConnectionManager::~QOfonoConnectionManager()
{
    delete d_ptr;
}

void QOfonoConnectionManager::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    QStringList removedProperties = d_ptr->properties.keys();

    delete d_ptr->connman;
    d_ptr->connman = new OfonoConnectionManager("org.ofono", path, QDBusConnection::systemBus(),this);

    if (d_ptr->connman->isValid()) {
        d_ptr->modemPath = path;

        connect(d_ptr->connman,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));
        connect(d_ptr->connman,SIGNAL(ContextAdded(QDBusObjectPath,QVariantMap)),
                this,SLOT(onContextAdd(QDBusObjectPath,QVariantMap)));
        connect(d_ptr->connman,SIGNAL(ContextRemoved(QDBusObjectPath)),
                this,SLOT(onContextRemove(QDBusObjectPath)));

        QVariantMap properties = d_ptr->connman->GetProperties().value();
        for (QVariantMap::ConstIterator it = properties.constBegin();
             it != properties.constEnd(); ++it) {
            updateProperty(it.key(), it.value());
            removedProperties.removeOne(it.key());
        }

        d_ptr->getContexts();
        Q_EMIT modemPathChanged(path);
        Q_EMIT contextsChanged(d_ptr->contexts);
    }

    foreach (const QString &p, removedProperties)
        updateProperty(p, QVariant());
}

QString QOfonoConnectionManager::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoConnectionManager::deactivateAll()
{
    if (!d_ptr->connman)
        return;
    d_ptr->connman->DeactivateAll();
}

void QOfonoConnectionManager::addContext(const QString &type)
{
    if (!d_ptr->connman)
        return;

    QStringList allowedTypes;
    allowedTypes << "internet";
    allowedTypes << "mms";
    allowedTypes << "wap";
    allowedTypes << "ims";

    if(!allowedTypes.contains(type)) {
        Q_EMIT reportError("Type not allowed");
        return;
    }
    QDBusPendingReply<QDBusObjectPath> reply = d_ptr->connman->AddContext(type);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(addContextFinished(QDBusPendingCallWatcher*)));
}

void QOfonoConnectionManager::removeContext(const QString &path)
{
    if (!d_ptr->connman)
        return;
    QDBusPendingReply<> reply = d_ptr->connman->RemoveContext(QDBusObjectPath(path));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(removeContextFinished(QDBusPendingCallWatcher*)));
}

bool QOfonoConnectionManager::attached() const
{
    if (d_ptr->connman)
        return d_ptr->properties["Attached"].value<bool>();
    else
        return false;
}

QString QOfonoConnectionManager::bearer() const
{
    if (d_ptr->connman)
        return d_ptr->properties["Bearer"].value<QString>();
    else
        return QString();
}

bool QOfonoConnectionManager::suspended() const
{
    if (d_ptr->connman)
        return d_ptr->properties["Suspended"].value<bool>();
    else
        return false;
}


bool QOfonoConnectionManager::roamingAllowed() const
{
    if (d_ptr->connman)
        return d_ptr->properties["RoamingAllowed"].value<bool>();
    else
        return false;
}

void QOfonoConnectionManager::setRoamingAllowed(bool value)
{
    if (roamingAllowed() == value)
        return;

    QString str("RoamingAllowed");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

bool QOfonoConnectionManager::powered() const
{
    if (d_ptr->connman)
        return d_ptr->properties["Powered"].value<bool>();
    else
        return false;
}

void QOfonoConnectionManager::setPowered(bool value)
{
    if (powered() == value)
        return;

    QString str("Powered");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionManager::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    updateProperty(property, dbusvalue.variant());
}

void QOfonoConnectionManager::updateProperty(const QString &property, const QVariant &value)
{
    if (d_ptr->properties.value(property) == value)
        return;

    if (value.isValid())
        d_ptr->properties.insert(property, value);
    else
        d_ptr->properties.remove(property);

    if (property == QLatin1String("Attached")) {
        if (value.value<bool>()) {
            if (d_ptr->contextTypes.isEmpty()) {
                d_ptr->getContexts();
                Q_EMIT contextsChanged(d_ptr->contexts);
            }
        } else {
            d_ptr->contextTypes.clear();
            d_ptr->contexts.clear();
            Q_EMIT contextsChanged(d_ptr->contexts);
        }
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
    return d_ptr->contexts;
}

void QOfonoConnectionManager::onContextAdd(const QDBusObjectPath &path, const QVariantMap &propertyMap)
{
    Q_UNUSED(propertyMap);
    d_ptr->contextTypes.insert(path.path(), propertyMap.value("Type").toString());
    d_ptr->filterContexts();
    Q_EMIT contextAdded(path.path());
    Q_EMIT contextsChanged(d_ptr->contexts);
}

void QOfonoConnectionManager::onContextRemove(const QDBusObjectPath &path)
{
    d_ptr->contextTypes.remove(path.path());
    d_ptr->filterContexts();
    Q_EMIT contextRemoved(path.path());
    Q_EMIT contextsChanged(d_ptr->contexts);
}

QString QOfonoConnectionManager::filter() const
{
    return d_ptr->filter;
}

void QOfonoConnectionManager::setFilter(const QString &filter)
{
    if (d_ptr->filter != filter) {
        d_ptr->filter = filter;
        d_ptr->filterContexts();
        Q_EMIT filterChanged(filter);
        Q_EMIT contextsChanged(d_ptr->contexts);
    }
}

bool QOfonoConnectionManager::isValid() const
{
    return d_ptr->connman->isValid();
}

void QOfonoConnectionManager::setOneProperty(const QString &prop, const QDBusVariant &var)
{
    if (d_ptr->connman) {
        QDBusPendingReply <> reply = d_ptr->connman->SetProperty(prop,var);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setPropertyFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoConnectionManager::addContextFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watch;
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        Q_EMIT reportError(reply.error().message());
    }
}

void QOfonoConnectionManager::removeContextFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<> reply = *watch;
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        Q_EMIT reportError(reply.error().message());
    }
}

void QOfonoConnectionManager::setPropertyFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<> reply = *watch;
    if (reply.isError()) {
        qDebug() << Q_FUNC_INFO << reply.error();
        Q_EMIT reportError(reply.error().message());
    }
}

