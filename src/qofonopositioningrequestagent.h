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

#ifndef QOFONOQOFONOPOSITIONINGREQUESTAGENT_H
#define QOFONOQOFONOPOSITIONINGREQUESTAGENT_H

#include <QObject>
#include <QDBusVariant>
#include <QDBusAbstractAdaptor>

class QOfonoPositioningRequestAgentPrivate;
class QOfonoPositioningRequestAgent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QOfonoPositioningRequestAgent)
    Q_PROPERTY(QString agentPath READ agentPath WRITE setAgentPath)

public:
    explicit QOfonoPositioningRequestAgent(QObject *parent = 0);
    ~QOfonoPositioningRequestAgent();

    QString agentPath() const;
    void setAgentPath(const QString &path);

Q_SIGNALS:
    void request(const QString &xmlElement);
    void positioningRequest();
    void release();

public slots:
    
private:
    QOfonoPositioningRequestAgentPrivate *d_ptr;
    friend class QOfonoPositioningRequestAgentAdaptor;

private Q_SLOTS:
    void Request(const QString &xmlElement);
    void PositioningRequest();
    void Release();
};

class QOfonoPositioningRequestAgentAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO ("D-Bus Interface", "org.ofono.PositioningRequestAgent")
public:
    explicit QOfonoPositioningRequestAgentAdaptor(QOfonoPositioningRequestAgent *parent = 0);
    virtual ~QOfonoPositioningRequestAgentAdaptor();

public Q_SLOTS:
    Q_NOREPLY void Request(const QString &xmlElement);
    Q_NOREPLY void PositioningRequest();
    Q_NOREPLY void Release();

private:
   QOfonoPositioningRequestAgent* positioningAgent;

};
#endif // QOFONOQOFONOPOSITIONINGREQUESTAGENT_H
