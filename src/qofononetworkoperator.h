/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**
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

#ifndef QOFONONETWORKOPERATOR_H
#define QOFONONETWORKOPERATOR_H

#include <QObject>
#include <QDBusVariant>
#include <QStringList>
#include "qofono_global.h"
//! This class is used to access ofono network operator API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/network-api.txt
 */

class QOfonoNetworkOperatorPrivate;
class QOFONOSHARED_EXPORT QOfonoNetworkOperator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString operatorPath READ operatorPath WRITE setOperatorPath)

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString mcc READ mcc NOTIFY mccChanged)
    Q_PROPERTY(QString mnc READ mnc NOTIFY mncChanged)
    Q_PROPERTY(QStringList technologies READ technologies NOTIFY technologiesChanged)
    Q_PROPERTY(QString additionalInfo READ additionalInfo NOTIFY additionalInfoChanged)

public:
    explicit QOfonoNetworkOperator(QObject *parent = 0);
    ~QOfonoNetworkOperator();

    QString operatorPath() const;
    void setOperatorPath(const QString &path);

    QString name() const;
    QString status() const;
    QString mcc() const;
    QString mnc() const;
    QStringList technologies() const;
    QString additionalInfo() const;

    Q_INVOKABLE void registerOperator();
Q_SIGNALS:

    void nameChanged(const QString &name);
    void statusChanged(const QString &status);
    void mccChanged(const QString &mcc);
    void mncChanged(const QString &mnc);
    void technologiesChanged(const QStringList &technologies);
    void additionalInfoChanged(const QString &additionalInfo);

public slots:

private:
    QOfonoNetworkOperatorPrivate *d_ptr;

private slots:
    void propertyChanged(const QString &property, const QDBusVariant &value);

};

#endif // QOFONONETWORKOPERATOR_H
