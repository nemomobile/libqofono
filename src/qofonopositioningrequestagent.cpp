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

#include "qofonopositioningrequestagent.h"
#include "dbus/ofonopositioningrequestagent.h"

#include "qofonoassistedsatellitenavigation.h"

class QOfonoPositioningRequestAgentPrivate
{
public:
    QOfonoPositioningRequestAgentPrivate();
    QString positioningAgentPath;
    OfonoPositioningRequestAgent *ofonoPositioningRequestAgent;
    QOfonoAssistedSatelliteNavigation * satNav;

};

QOfonoPositioningRequestAgentPrivate::QOfonoPositioningRequestAgentPrivate() :
    positioningAgentPath(QString()),
    ofonoPositioningRequestAgent(0)
{
}

QOfonoPositioningRequestAgent::QOfonoPositioningRequestAgent(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoPositioningRequestAgentPrivate)
{
}

QOfonoPositioningRequestAgent::~QOfonoPositioningRequestAgent()
{
    delete d_ptr;
}

void QOfonoPositioningRequestAgent::setAgentPath(const QString &path)
{
//    if (!d_ptr->ofonoPositioningRequestAgent) {

//        d_ptr->positioningAgentPath = path;

//        d_ptr->ofonoPositioningRequestAgent = new OfonoPositioningRequestAgent("org.ofono", path, QDBusConnection::systemBus(),this);

        new QOfonoPositioningRequestAgentAdaptor(this);
        d_ptr->positioningAgentPath = path;
//        QDBusConnection::systemBus().registerObject(agentPath, this);

//        if (m_manager->isAvailable()) {
//            m_manager->registerAgent(QString(agentPath));
//        }
   // }
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
    return d_ptr->ofonoPositioningRequestAgent->isValid();
}

QOfonoPositioningRequestAgentAdaptor::QOfonoPositioningRequestAgentAdaptor(QOfonoPositioningRequestAgent *parent)
    : QDBusAbstractAdaptor(parent),
      positioningAgent(parent)
{
}

QOfonoPositioningRequestAgentAdaptor::~QOfonoPositioningRequestAgentAdaptor()
{
}

void QOfonoPositioningRequestAgentAdaptor::Release()
{
    positioningAgent->Release();
}

void QOfonoPositioningRequestAgentAdaptor::PositioningRequest()
{
    positioningAgent->PositioningRequest();
}

void QOfonoPositioningRequestAgentAdaptor::Request(const QString &xmlElement)
{
    positioningAgent->Request(xmlElement);
}

