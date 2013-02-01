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

#ifndef QOFONOMODEM_H
#define QOFONOMODEM_H

#include <QObject>
#include <QStringList>
#include "dbustypes.h"

#include "qofono_global.h"
//! This class is used to access an oFono modem object and its properties
/*!
 * oFono modem properties are documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/modem-api.txt
 */

class QOfonoModemPrivate;

class QOFONOSHARED_EXPORT QOfonoModem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool powered READ powered WRITE setPowered NOTIFY poweredChanged)
    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged) //flightmode
    Q_PROPERTY(bool lockdown READ lockdown WRITE setLockdown NOTIFY lockdownChanged)
    Q_PROPERTY(bool emergency READ emergency NOTIFY emergencyChanged)

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString manufacturer READ manufacturer NOTIFY manufacturerChanged)
    Q_PROPERTY(QString model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString revision READ revision NOTIFY revisionChanged)
    Q_PROPERTY(QString serial READ serial NOTIFY serialChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)

    Q_PROPERTY(QStringList features READ features NOTIFY featuresChanged)
    Q_PROPERTY(QStringList interfaces READ interfaces NOTIFY interfacesChanged)

    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)

public:
    explicit QOfonoModem(QObject *parent = 0);
    ~QOfonoModem();

    QString modemPath() const;
    void setModemPath(const QString &path);
    
    bool powered() const;
    void setPowered(bool powered);

    bool online() const;
    void setOnline(bool online);

    bool lockdown() const;
    void setLockdown(bool lockdown);

    bool emergency() const;

    QString name() const;
    QString manufacturer() const;
    QString model() const;
    QString revision() const;
    QString serial() const;
    QString type() const;

    QStringList features() const;
    QStringList interfaces() const;

Q_SIGNALS:
    void poweredChanged(bool powered);
    void onlineChanged(bool online);
    void lockdownChanged(bool lockdown);
    void emergencyChanged(bool emergency);
    void nameChanged(const QString &name);
    void manufacturerChanged(const QString &manufacturer);
    void modelChanged(const QString &model);
    void revisionChanged(const QString &revision);
    void serialChanged(const QString &serial);
    void typeChanged(const QString &type);

    void featuresChanged(const QStringList &features);
    void interfacesChanged(const QStringList &interfaces);

private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);

private:
    QOfonoModemPrivate *d_ptr;
};

#endif // QOFONOMODEM_H
