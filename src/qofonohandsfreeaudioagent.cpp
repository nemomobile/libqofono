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

#include "qofonohandsfreeaudioagent.h"
#include "ofono_handsfree_audio_agent_interface.h"

class QOfonoHandsfreeAudioAgentPrivate
{
public:
    QOfonoHandsfreeAudioAgentPrivate();
    QString audioAgentPath;
    OfonoHandsfreeAudioAgent *ofonoHandsfreeAudioAgent;

};

QOfonoHandsfreeAudioAgentPrivate::QOfonoHandsfreeAudioAgentPrivate() :
    ofonoHandsfreeAudioAgent(0)
{
}

QOfonoHandsfreeAudioAgent::QOfonoHandsfreeAudioAgent(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoHandsfreeAudioAgentPrivate)
{
}

QOfonoHandsfreeAudioAgent::~QOfonoHandsfreeAudioAgent()
{
    delete d_ptr;
}

void QOfonoHandsfreeAudioAgent::setAgentPath(const QString &path)
{
    if (!d_ptr->ofonoHandsfreeAudioAgent) {
        d_ptr->audioAgentPath = path;
        d_ptr->ofonoHandsfreeAudioAgent = new OfonoHandsfreeAudioAgent("org.ofono", path, QDBusConnection::systemBus(),this);

    }
}

QString QOfonoHandsfreeAudioAgent::agentPath() const
{
    return d_ptr->audioAgentPath;
}

void QOfonoHandsfreeAudioAgent::NewConnection(const QString &card, QDBusUnixFileDescriptor sco, const QByteArray &codec)
{
    Q_EMIT newConnection(card, sco, codec);
}

void QOfonoHandsfreeAudioAgent::Release()
{
    Q_EMIT release();
}

bool QOfonoHandsfreeAudioAgent::isValid() const
{
    return d_ptr->ofonoHandsfreeAudioAgent->isValid();
}

QOfonoHandsfreeAudioAgentAdaptor::QOfonoHandsfreeAudioAgentAdaptor(QOfonoHandsfreeAudioAgent *parent)
    : QDBusAbstractAdaptor(parent),
      audioAgent(parent)
{
}

QOfonoHandsfreeAudioAgentAdaptor::~QOfonoHandsfreeAudioAgentAdaptor()
{
}

void QOfonoHandsfreeAudioAgentAdaptor::NewConnection(const QString &card, QDBusUnixFileDescriptor sco, const QByteArray & codec)
{
    audioAgent->NewConnection(card, sco, codec);
}

void QOfonoHandsfreeAudioAgentAdaptor::Release()
{
    audioAgent->Release();
}
