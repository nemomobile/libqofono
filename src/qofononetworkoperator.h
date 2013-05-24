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
    bool isValid() const;

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
