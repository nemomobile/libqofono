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

#ifndef QOFONOMessage_H
#define QOFONOMessage_H

#include "qofonoobject.h"
#include "qofono_global.h"

//! This class is used to access ofono message API
/*!
 * oFono message API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/message-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoMessage : public QOfonoObject
{
    Q_OBJECT
    Q_PROPERTY(QString messagePath READ messagePath WRITE setMessagePath NOTIFY messagePathChanged)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)

public:
    explicit QOfonoMessage(QObject *parent = 0);
    ~QOfonoMessage();

    QString messagePath() const;
    void setMessagePath(const QString &path);

    QString state() const;
    Q_INVOKABLE void cancel();

    bool isValid() const;

Q_SIGNALS:
    void stateChanged(const QString &state);
    void messagePathChanged(const QString &path);

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void propertyChanged(const QString &key, const QVariant &value);
    void objectPathChanged(const QString &path, const QVariantMap *properties);
};

#endif // QOFONOMessage_H
