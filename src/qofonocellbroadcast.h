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

#ifndef QOFONOCELLBROADCAST_H
#define QOFONOCELLBROADCAST_H

#include "qofonoobject.h"
#include "qofono_global.h"

//! This class is used to access ofono cell broadcast API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/cell-broadcast-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoCellBroadcast : public QOfonoObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString topics READ topics NOTIFY topicsChanged)

public:
    explicit QOfonoCellBroadcast(QObject *parent = 0);
    ~QOfonoCellBroadcast();

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool enabled() const;
    void setEnabled(bool b);

    QString topics() const;
    void setTopics(const QString &) const; // to maintain ABI

    bool isValid() const;

Q_SIGNALS:
    void enabledChanged(bool);
    void topicsChanged(const QString &);
    void incomingBroadcast(const QString &, quint16);
    void emergencyBroadcast(const QString &, const QVariantMap &);
    void modemPathChanged(const QString &path);

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void propertyChanged(const QString &key, const QVariant &value);
    void objectPathChanged(const QString &path, const QVariantMap *properties);
};

#endif // QOFONOCELLBROADCAST_H
