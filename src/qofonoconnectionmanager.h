/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**

**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QOFONOCONNECTIONMANAGER_H
#define QOFONOCONNECTIONMANAGER_H

#include <QObject>
#include "dbustypes.h"
#include "qofono_global.h"
//! This class is used to access ofono connman context API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob;f=doc/connman-api.txt
 */

class QOfonoConnectionManagerPrivate;

class QOFONOSHARED_EXPORT QOfonoConnectionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool attached READ attached NOTIFY attachedChanged)
    Q_PROPERTY(QString bearer READ bearer NOTIFY bearerChanged)
    Q_PROPERTY(bool suspended READ suspended NOTIFY suspendedChanged)
    Q_PROPERTY(bool roamingAllowed READ roamingAllowed WRITE setRoamingAllowed NOTIFY roamingAllowedChanged)
    Q_PROPERTY(bool powered READ powered WRITE setPowered NOTIFY poweredChanged)

    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)
    Q_PROPERTY(QStringList contexts READ contexts NOTIFY contextsChanged)

public:
    explicit QOfonoConnectionManager(QObject *parent = 0);
    ~QOfonoConnectionManager();

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool attached() const;
    QString bearer() const;
    bool suspended() const;

    bool roamingAllowed() const;
    void setRoamingAllowed(bool);

    bool powered() const;
    void setPowered(bool);

    QStringList contexts();

Q_SIGNALS:
    void attachedChanged(bool value);
    void bearerChanged(const QString &bearer);
    void suspendedChanged(bool suspnd);
    void roamingAllowedChanged(bool roaming);
    void poweredChanged(bool powered);

    void contextAdded(const QString &path);
    void contextRemoved(const QString &path);
    void contextsChanged(const QStringList &contexts);

public slots:
    void deactivateAll();
    void addContext(const QString &type);
    void removeContext(const QString &path);

private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
    void onContextAdd(const QDBusObjectPath &path, const QVariantMap &propertyMap);
    void onContextRemove(const QDBusObjectPath &path);

private:
    QOfonoConnectionManagerPrivate *d_ptr;
};

#endif // QOFONOCONNECTIONMANAGER_H
