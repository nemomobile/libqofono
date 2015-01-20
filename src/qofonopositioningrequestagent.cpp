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

#include "qofonopositioningrequestagent.h"
#include "qofonoassistedsatellitenavigation.h"
#include "ofono_positioning_request_agent_adaptor.h"

class QOfonoPositioningRequestAgentPrivate
{
public:
    QOfonoPositioningRequestAgentPrivate();
    ~QOfonoPositioningRequestAgentPrivate();
    QString positioningAgentPath;
    bool registered;
};

QOfonoPositioningRequestAgentPrivate::QOfonoPositioningRequestAgentPrivate() :
    registered(false)
{
}

QOfonoPositioningRequestAgentPrivate::~QOfonoPositioningRequestAgentPrivate()
{
    if (registered)
        QDBusConnection::systemBus().unregisterObject(positioningAgentPath);
}

QOfonoPositioningRequestAgent::QOfonoPositioningRequestAgent(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoPositioningRequestAgentPrivate)
{
    new QOfonoPositioningRequestAgentAdaptor(this);
}

QOfonoPositioningRequestAgent::~QOfonoPositioningRequestAgent()
{
    delete d_ptr;
}

void QOfonoPositioningRequestAgent::setAgentPath(const QString &path)
{
    if (d_ptr->positioningAgentPath != path) {
        QDBusConnection dbus = QDBusConnection::systemBus();
        if (d_ptr->registered) {
            dbus.unregisterObject(d_ptr->positioningAgentPath);
            d_ptr->registered = false;
        }
        d_ptr->positioningAgentPath = path;
        if (!path.isEmpty()) {
            if (!dbus.registerObject(path, this)) {
                qWarning() << "Object registration failed:" << dbus.lastError();
            }
            d_ptr->registered = true;
        }
        Q_EMIT agentPathChanged(path);
    }
}

QString QOfonoPositioningRequestAgent::agentPath() const
{
    return d_ptr->positioningAgentPath;
}

void QOfonoPositioningRequestAgent::Release()
{
    Q_EMIT release();
}

void QOfonoPositioningRequestAgent::PositioningRequest()
{
    Q_EMIT positioningRequest();
}

void QOfonoPositioningRequestAgent::Request(const QString &xmlElement)
{
    Q_EMIT request(xmlElement);
}
bool QOfonoPositioningRequestAgent::isValid() const
{
    return d_ptr->registered;
}
