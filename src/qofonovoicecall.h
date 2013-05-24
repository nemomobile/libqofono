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

#ifndef QOFONOVoiceCall_H
#define QOFONOVoiceCall_H

#include <QObject>
#include <QDBusVariant>
#include "qofono_global.h"
//! This class is used to access ofono voice call API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob;f=doc/voicecall-api.txt
 */

class QOfonoVoiceCallPrivate;
class QOFONOSHARED_EXPORT QOfonoVoiceCall : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString voiceCallPath READ voiceCallPath WRITE setVoiceCallPath)

    Q_PROPERTY(QString lineIdentification READ lineIdentification NOTIFY lineIdentificationChanged)
    Q_PROPERTY(QString incomingLine READ incomingLine NOTIFY incomingLineChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString startTime READ startTime NOTIFY startTimeChanged)
    Q_PROPERTY(QString information READ information NOTIFY informationChanged)
    Q_PROPERTY(bool multiparty READ multiparty NOTIFY multipartyChanged)
    Q_PROPERTY(bool emergency READ emergency NOTIFY emergencyChanged)
    Q_PROPERTY(quint8 icon READ icon NOTIFY iconChanged)

public:
    explicit QOfonoVoiceCall(QObject *parent = 0);
    ~QOfonoVoiceCall();

    QString voiceCallPath() const;
    void setVoiceCallPath(const QString &path);

    QString lineIdentification() const;
    QString incomingLine() const;
    QString name() const;
    QString state() const;
    QString startTime() const;
    QString information() const;
    bool multiparty() const;
    bool emergency() const;
    quint8 icon() const;
    bool remoteHeld() const;
    bool remoteMultiparty() const;

    bool isValid() const;
Q_SIGNALS:
    void lineIdentificationChanged(const QString &name);
    void nameChanged(const QString &name);
    void stateChanged(const QString &state);
    void startTimeChanged(const QString &time);
    void informationChanged(const QString &mcc);
    void incomingLineChanged(const QString &line);
    void disconnectReason(const QString &reason);
    void multipartyChanged(bool multiparty);
    void iconChanged(const quint8 &icon);
    void emergencyChanged(bool emergency);
    void remoteHeldChanged(bool remoteHeld);
    void remoteMultipartyChanged(bool remoteMultiparty);


public slots:
    void answer();
    void hangup();
    void deflect(const QString &number);

private:
    QOfonoVoiceCallPrivate *d_ptr;
private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONOVoiceCall_H
