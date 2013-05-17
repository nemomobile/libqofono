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

#ifndef QOFONONETWORKREGISTRATION_H
#define QOFONONETWORKREGISTRATION_H

#include <QObject>
#include "dbustypes.h"

#include "qofono_global.h"
//! This class is used to access ofono network operator API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/network-api.txt
 */

class QOfonoNetworkRegistrationPrivate;

struct OfonoPathProps
{
    QDBusObjectPath path;
    QVariantMap properties;
};
typedef QList<OfonoPathProps> QArrayOfPathProps;
Q_DECLARE_METATYPE(OfonoPathProps)
Q_DECLARE_METATYPE (QArrayOfPathProps)

class QOFONOSHARED_EXPORT QOfonoNetworkRegistration : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(uint locationAreaCode READ locationAreaCode NOTIFY locationAreaCodeChanged)
    Q_PROPERTY(uint cellId READ cellId NOTIFY cellIdChanged)
    Q_PROPERTY(QString mcc READ mcc NOTIFY mccChanged)
    Q_PROPERTY(QString mnc READ mnc NOTIFY mncChanged)
    Q_PROPERTY(QString technology READ technology NOTIFY technologyChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(uint strength READ strength NOTIFY strengthChanged)
    Q_PROPERTY(QString baseStation READ baseStation NOTIFY baseStationChanged)

    Q_PROPERTY(QStringList networkOperators READ networkOperators NOTIFY networkOperatorsChanged)
    Q_PROPERTY(QString currentOperatorPath READ currentOperatorPath NOTIFY currentOperatorPathChanged)

    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)

public:
    explicit QOfonoNetworkRegistration(QObject *parent = 0);
    ~QOfonoNetworkRegistration();
    
    QString modemPath() const;
    void setModemPath(const QString &path);

    QString mode() const;
    QString status() const;
    uint locationAreaCode() const;
    uint cellId() const;
    QString mcc() const;
    QString mnc() const;
    QString technology() const;
    QString name() const;
    uint strength() const;
    QString baseStation() const;

    QStringList networkOperators();

    Q_INVOKABLE void registration();
    Q_INVOKABLE void scan();

    QString currentOperatorPath();

Q_SIGNALS:
    void modeChanged(const QString &mode);
    void statusChanged(const QString &status);
    void locationAreaCodeChanged(uint locationAreaCode);
    void cellIdChanged(uint cellId);
    void mccChanged(const QString &mcc);
    void mncChanged(const QString &mnc);
    void technologyChanged(const QString &technology);
    void nameChanged(const QString &name);
    void strengthChanged(uint strength);
    void baseStationChanged(const QString &baseStation);

    void networkOperatorsChanged(const QStringList &networkOperators);
    void currentOperatorPathChanged(const QString &);
    void scanFinished();
    void scanError(const QString &message);

public slots:

private:
    QOfonoNetworkRegistrationPrivate *d_ptr;

private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
    void scanError(QDBusError error);
    void scanFinish(const QArrayOfPathProps &list);
};

#endif // QOFONONETWORKREGISTRATION_H
