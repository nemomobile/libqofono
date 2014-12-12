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

#ifndef QOFONOCallVolume_H
#define QOFONOCallVolume_H

#include "qofonomodeminterface.h"
#include "qofono_global.h"

//! This class is used to access ofono call volume API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/call-volume-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoCallVolume : public QOfonoModemInterface
{
    Q_OBJECT
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(quint8 speakerVolume READ speakerVolume WRITE setSpeakerVolume NOTIFY speakerVolumeChanged)
    Q_PROPERTY(quint8 microphoneVolume READ microphoneVolume WRITE setMicrophoneVolume NOTIFY microphoneVolumeChanged)

public:
    explicit QOfonoCallVolume(QObject *parent = 0);
    ~QOfonoCallVolume();

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool muted() const;
    void setMuted(const bool mute);

    quint8 speakerVolume() const;
    void setSpeakerVolume(const quint8 &spvolume);

    quint8 microphoneVolume()const ;
    void setMicrophoneVolume(const quint8 &mpvolume);

    bool isValid() const;

Q_SIGNALS:
    void mutedChanged(bool muted);
    void speakerVolumeChanged(const quint8 &volume);
    void microphoneVolumeChanged(const quint8 &mvolume);

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void propertyChanged(const QString &property, const QVariant &value);
};

#endif // QOFONOCallVolume_H
