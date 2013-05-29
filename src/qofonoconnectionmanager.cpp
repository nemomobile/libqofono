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

    QStringList getContexts();
};

QOfonoConnectionManagerPrivate::QOfonoConnectionManagerPrivate() :
    modemPath(QString())
   , connman(0)
  ,contexts(QStringList())
{
}

QStringList QOfonoConnectionManagerPrivate::getContexts()
{
    QStringList contextList;
    QDBusReply<ObjectPathPropertiesList> reply2 = connman->GetContexts();
    foreach(ObjectPathProperties context, reply2.value()) {
        contextList.append(context.path.path());
    }
    contexts = contextList;
    return contexts;
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

    if (path != modemPath()) {
        if (d_ptr->connman) {
            delete d_ptr->connman;
            d_ptr->connman = 0;
            d_ptr->properties.clear();
        }
        d_ptr->connman = new OfonoConnectionManager("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->connman->isValid()) {
            d_ptr->modemPath = path;

            connect(d_ptr->connman,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            connect(d_ptr->connman,SIGNAL(ContextAdded(QDBusObjectPath,QVariantMap)),
                    this,SLOT(onContextAdd(QDBusObjectPath,QVariantMap)));
            connect(d_ptr->connman,SIGNAL(ContextRemoved(QDBusObjectPath)),
                    this,SLOT(onContextRemove(QDBusObjectPath)));

            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->connman->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            d_ptr->getContexts();
            Q_EMIT modemPathChanged(path);
        }
    }
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
    QString str("Powered");
    QDBusVariant var(value);
    setOneProperty(str,var);
}

void QOfonoConnectionManager::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);
    if (property == QLatin1String("Attached")) {
        if (value.value<bool>()) {
            if (d_ptr->contexts.isEmpty()) {
                d_ptr->getContexts();
                Q_EMIT contextsChanged(d_ptr->contexts);
            }
        } else {
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
    if (!d_ptr->contexts.contains(path.path()))
        d_ptr->contexts.append(path.path());
    Q_EMIT contextAdded(path.path());
    Q_EMIT contextsChanged(d_ptr->contexts);
}

void QOfonoConnectionManager::onContextRemove(const QDBusObjectPath &path)
{
    if (d_ptr->contexts.contains(path.path()))
        d_ptr->contexts.removeOne(path.path());
    Q_EMIT contextRemoved(path.path());
    Q_EMIT contextsChanged(d_ptr->contexts);
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

