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

#ifndef QOFONOQOFONOHANDSFREEAUDIOMANAGER_H
#define QOFONOQOFONOHANDSFREEAUDIOMANAGER_H

#include <QObject>
#include <QDBusVariant>
#include <QStringList>

class QOfonoHandsfreeAudioManagerPrivate;
class QOfonoHandsfreeAudioManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_ENUMS(CodecType)
    Q_FLAGS(CodecType CodecTypeFlags)

public:

    enum CodecType {
       CVSD = 0x01,
       mSBC = 0x02
    };
    Q_DECLARE_FLAGS(CodecTypeFlags, CodecType)

    explicit QOfonoHandsfreeAudioManager(QObject *parent = 0);
    ~QOfonoHandsfreeAudioManager();

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool isValid() const;
Q_SIGNALS:
    void cardAdded(const QString &card);
    void cardRemoved(const QString &card);

    void modemPathChanged(const QString &path);

public slots:
    QStringList cards() const;
    void registerAgent(const QString &path, CodecTypeFlags codecs);

private:
    QOfonoHandsfreeAudioManagerPrivate *d_ptr;
};

#endif // QOFONOQOFONOHANDSFREEAUDIOMANAGER_H
