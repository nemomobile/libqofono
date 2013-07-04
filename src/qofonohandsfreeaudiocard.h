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
#include <QDBusPendingCallWatcher>

class QOfonoHandsfreeAudioCardPrivate;
class QOfonoHandsfreeAudioCard : public QObject
{
    Q_OBJECT
    Q_ENUMS(Error)
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(QString remoteAddress READ remoteAddress)
    Q_PROPERTY(QString localAddress READ localAddress)

public:
    enum Error {
        NoError,
        NotImplementedError,
        InProgressError,
        InvalidArgumentsError,
        InvalidFormatError,
        FailedError,
        UnknownError
    };
    explicit QOfonoHandsfreeAudioCard(QObject *parent = 0);
    ~QOfonoHandsfreeAudioCard();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QString remoteAddress() const;
    QString localAddress() const;

    bool isValid() const;
Q_SIGNALS:
    void modemPathChanged(const QString &path);
    void connectAudioComplete(QOfonoHandsfreeAudioCard::Error error, const QString &errorString);

public slots:
    void connectAudio();

private:
    QOfonoHandsfreeAudioCardPrivate *d_ptr;
    Error errorNameToEnum(const QString &errorName);

private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
    void connectAudioFinished(QDBusPendingCallWatcher *call);
};

#endif // QOFONOQOfonoHandsfreeAudioCard_H
