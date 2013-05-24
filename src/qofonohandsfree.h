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

#ifndef QOFONOQOFONOHANDSFREE_H
#define QOFONOQOFONOHANDSFREE_H

#include <QObject>
#include <QDBusVariant>
#include <QStringList>

class QOfonoHandsfreePrivate;
class QOfonoHandsfree : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(QStringList features READ features)
    Q_PROPERTY(bool inbandRinging READ inbandRinging)
    Q_PROPERTY(bool voiceRecognition READ voiceRecognition WRITE setVoiceRecognition WRITE voiceRecognitionChanged)
    Q_PROPERTY(bool echoCancelingNoiseReduction READ echoCancelingNoiseReduction WRITE setEchoCancelingNoiseReduction NOTIFY echoCancelingNoiseReductionChanged)
    Q_PROPERTY(uchar batteryChargeLevel READ batteryChargeLevel)

public:
    explicit QOfonoHandsfree(QObject *parent = 0);
    ~QOfonoHandsfree();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QStringList features() const;
    bool inbandRinging() const;
    bool voiceRecognition() const;
    void setVoiceRecognition(bool on);

    bool echoCancelingNoiseReduction() const;
    void setEchoCancelingNoiseReduction(bool on);
    uchar batteryChargeLevel() const;

    bool isValid() const;
Q_SIGNALS:
    void voiceRecognitionChanged(bool on);
    void echoCancelingNoiseReductionChanged(bool on);
    void modemPathChanged(const QString &path);

private:
    QOfonoHandsfreePrivate *d_ptr;

private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONOQOFONOHANDSFREE_H
