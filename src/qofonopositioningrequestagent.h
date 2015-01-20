/****************************************************************************
**
** Copyright (C) 2013-2014 Jolla Ltd.
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

#ifndef QOFONOQOFONOPOSITIONINGREQUESTAGENT_H
#define QOFONOQOFONOPOSITIONINGREQUESTAGENT_H

#include "qofono_global.h"

class QOfonoPositioningRequestAgentPrivate;
class QOfonoPositioningRequestAgent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QOfonoPositioningRequestAgent)
    Q_PROPERTY(QString agentPath READ agentPath WRITE setAgentPath NOTIFY agentPathChanged)

public:
    explicit QOfonoPositioningRequestAgent(QObject *parent = 0);
    ~QOfonoPositioningRequestAgent();

    QString agentPath() const;
    void setAgentPath(const QString &path);
    bool isValid() const;

Q_SIGNALS:
    void agentPathChanged(const QString &path);
    void request(const QString &xmlElement);
    void positioningRequest();
    void release();

private:
    QOfonoPositioningRequestAgentPrivate *d_ptr;

private Q_SLOTS:
    void Request(const QString &xmlElement);
    void PositioningRequest();
    void Release();
};

#endif // QOFONOQOFONOPOSITIONINGREQUESTAGENT_H
