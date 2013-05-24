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

#ifndef QOFONOQOFONOHANDSFREEAUDIOAGENT_H
#define QOFONOQOFONOHANDSFREEAUDIOAGENT_H

#include <QObject>
#include <QDBusVariant>
#include <QDBusAbstractAdaptor>
#include <QDBusUnixFileDescriptor>

class QOfonoHandsfreeAudioAgentPrivate;
class QOfonoHandsfreeAudioAgent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QOfonoHandsfreeAudioAgent)
    Q_PROPERTY(QString agentPath READ agentPath WRITE setAgentPath)

public:
    explicit QOfonoHandsfreeAudioAgent(QObject *parent = 0);
    ~QOfonoHandsfreeAudioAgent();

    QString agentPath() const;
    void setAgentPath(const QString &path);

    bool isValid() const;
Q_SIGNALS:
    void newConnection(const QString &card, QDBusUnixFileDescriptor sco, const QByteArray &codec);
    void release();

public slots:
    
private:
    QOfonoHandsfreeAudioAgentPrivate *d_ptr;
    friend class QOfonoHandsfreeAudioAgentAdaptor;

private Q_SLOTS:
    void NewConnection(const QString &card, QDBusUnixFileDescriptor sco, const QByteArray &codec);
    void Release();
};

class QOfonoHandsfreeAudioAgentAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO ("D-Bus Interface", "org.ofono.HandsfreeAudioAgent")
public:
    explicit QOfonoHandsfreeAudioAgentAdaptor(QOfonoHandsfreeAudioAgent *parent = 0);
    virtual ~QOfonoHandsfreeAudioAgentAdaptor();

public Q_SLOTS:
     Q_NOREPLY void NewConnection(const QString &card, QDBusUnixFileDescriptor sco, const QByteArray &codec);
     Q_NOREPLY void Release();

private:
   QOfonoHandsfreeAudioAgent* audioAgent;

};
#endif // QOFONOQOFONOHANDSFREEAUDIOAGENT_H
