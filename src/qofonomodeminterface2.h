/****************************************************************************
**
** Copyright (C) 2014 Jolla Ltd.
** Contact: slava.monich@jolla.com
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

#ifndef QOFONOMODEMINTERFACE2_H
#define QOFONOMODEMINTERFACE2_H

#include "dbustypes.h"

/**
 * Modem interface without properties.
 */
class QOfonoModemInterface2 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)

public:
    class ExtData {
    public:
        virtual ~ExtData();
    };

protected:
    QOfonoModemInterface2(const QString &iface, ExtData *ext, QObject *parent = NULL);
    QOfonoModemInterface2(const QString &iface, QObject *parent = NULL);
    ~QOfonoModemInterface2();

    ExtData* extData() const;

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool isValid() const;

Q_SIGNALS:
    void modemPathChanged(const QString &path);
    void validChanged(bool valid);

protected:
    virtual QDBusAbstractInterface *createDbusInterface(const QString &path) = 0;
    virtual void dbusInterfaceDropped();
    QDBusAbstractInterface *dbusInterface() const;

private slots:
    void onModemInterfacesChanged(const QStringList &interfaces);

private:
    class Private;
    Private* d_ptr;
};

#endif // QOFONOMODEMINTERFACE2_H
