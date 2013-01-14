/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**

**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qofonomanager.h"
#include "dbus/ofonomanager.h"
#include <QVariant>
#include "dbustypes.h"

QDBusArgument &operator<<(QDBusArgument &argument, const OfonoPathProperties &modem)
{
    argument.beginStructure();
    argument << modem.path << modem.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, OfonoPathProperties &modem)
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
};

QOfonoManagerPrivate::QOfonoManagerPrivate() :
 ofonoManager(0)
, modems(QStringList())
{
    qDBusRegisterMetaType<OfonoPathProperties>();
    qDBusRegisterMetaType<QArrayOfPathProperties>();
}

QOfonoManager::QOfonoManager(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoManagerPrivate)
{
    qDebug() << Q_FUNC_INFO << d_ptr->modems;
    d_ptr->ofonoManager = new OfonoManager("org.ofono","/",QDBusConnection::systemBus(),this);
    if (d_ptr->ofonoManager->isValid()) {
        QDBusReply<QArrayOfPathProperties> reply = d_ptr->ofonoManager->GetModems();
        foreach(OfonoPathProperties modem, reply.value()) {
            d_ptr->modems << modem.path.path();
        }
    }
}

QOfonoManager::~QOfonoManager()
{
    delete d_ptr;
}

QStringList QOfonoManager::modems()
{
    qDebug() << Q_FUNC_INFO << d_ptr->modems;
    return d_ptr->modems;
}

void QOfonoManager::onModemAdded(const QDBusObjectPath& path, const QVariantMap& /*map*/)
{
    QString pathStr = path.path();
    if (!d_ptr->modems.contains(pathStr)) {
        d_ptr->modems.append(pathStr);
        Q_EMIT modemAdded(pathStr);
    }
}

void QOfonoManager::onModemRemoved(const QDBusObjectPath& path)
{
    QString pathStr = path.path();
    if (!d_ptr->modems.contains(pathStr)) {
        d_ptr->modems.removeOne(pathStr);
        Q_EMIT modemRemoved(pathStr);
    }
}
