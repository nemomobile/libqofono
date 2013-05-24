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

#ifndef QOFONOQOfonoHandsfreeAudioCard_H
#define QOFONOQOfonoHandsfreeAudioCard_H

#include <QObject>
#include <QDBusVariant>

class QOfonoHandsfreeAudioCardPrivate;
class QOfonoHandsfreeAudioCard : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(QString remoteAddress READ remoteAddress/* NOTIFY remoteAddressChanged*/)
    Q_PROPERTY(QString localAddress READ localAddress/* NOTIFY localAddressChanged*/)

public:
    explicit QOfonoHandsfreeAudioCard(QObject *parent = 0);
    ~QOfonoHandsfreeAudioCard();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QString remoteAddress() const;
    QString localAddress() const;

    bool isValid() const;
Q_SIGNALS:
//    void remoteAddressChanged();
//    void localAddressChanged();
    void modemPathChanged(const QString &path);

public slots:
    void connect() const;

private:
    QOfonoHandsfreeAudioCardPrivate *d_ptr;
private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONOQOfonoHandsfreeAudioCard_H
