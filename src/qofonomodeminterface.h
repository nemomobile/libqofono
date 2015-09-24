/****************************************************************************
**
** Copyright (C) 2014-2015 Jolla Ltd.
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

#ifndef QOFONOMODEMINTERFACE_H
#define QOFONOMODEMINTERFACE_H

#include "qofonoobject.h"

/**
 * Modem interface with properties.
 */
class QOfonoModemInterface : public QOfonoObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)

protected:
    QOfonoModemInterface(const QString &iface, ExtData *ext, QObject *parent = NULL);
    QOfonoModemInterface(const QString &iface, QObject *parent = NULL);
    ~QOfonoModemInterface();

    virtual ExtData* extData() const;

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool isReady() const;

public:
    bool isValid() const;
    void fixModemPath(const QString &path);

Q_SIGNALS:
    void modemPathChanged(const QString &path);
    void readyChanged(/* No parameter for historical reasons */);

protected:
    void getPropertiesFinished(const QVariantMap &properties, const QDBusError *error);
    void updateProperty(const QString &key, const QVariant &value);
    void objectPathChanged(const QString &path, const QVariantMap *properties);

private Q_SLOTS:
    void onModemInterfacesChanged(const QStringList &interfaces);
    void onModemValidChanged(bool valid);

private:
    class Private;
    Private* privateData() const;
};

#endif // QOFONOMODEMINTERFACE_H
