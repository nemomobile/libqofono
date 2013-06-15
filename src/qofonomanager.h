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

#ifndef QOFONOMANAGER_H
#define QOFONOMANAGER_H

#include <QObject>
#include <QVariant>
#include "dbustypes.h"

#include "qofono_global.h"
//! This class is used to access ofono connman context API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob;f=doc/manager-api.txt
 */

class QOfonoManagerPrivate;
class QOFONOSHARED_EXPORT QOfonoManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList modems READ modems NOTIFY modemsChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
public:
    explicit QOfonoManager(QObject *parent = 0);
    ~QOfonoManager();

    QStringList modems();
    bool available() const;

    bool isValid() const;
Q_SIGNALS: // SIGNALS
    void modemAdded(const QString &modem);
    void modemRemoved(const QString &modem);
    void availableChanged(bool available);
    void modemsChanged(const QStringList &modems);

private slots:
    void onModemAdd(const QDBusObjectPath &path, const QVariantMap &var);
    void onModemRemove(const QDBusObjectPath &path);
    void connectToOfono(const QString &);
    void ofonoUnregistered(const QString &);
private:
    QOfonoManagerPrivate *d_ptr;
};

#endif // QOFONOMANAGER_H
