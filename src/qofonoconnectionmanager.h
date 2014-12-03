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

#ifndef QOFONOCONNECTIONMANAGER_H
#define QOFONOCONNECTIONMANAGER_H

#include "qofonomodeminterface.h"
#include "qofono_global.h"

//! This class is used to access ofono connman context API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob;f=doc/connman-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoConnectionManager : public QOfonoModemInterface
{
    Q_OBJECT
    Q_PROPERTY(bool attached READ attached NOTIFY attachedChanged)
    Q_PROPERTY(QString bearer READ bearer NOTIFY bearerChanged)
    Q_PROPERTY(bool suspended READ suspended NOTIFY suspendedChanged)
    Q_PROPERTY(bool roamingAllowed READ roamingAllowed WRITE setRoamingAllowed NOTIFY roamingAllowedChanged)
    Q_PROPERTY(bool powered READ powered WRITE setPowered NOTIFY poweredChanged)

    Q_PROPERTY(QStringList contexts READ contexts NOTIFY contextsChanged)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)

public:
    explicit QOfonoConnectionManager(QObject *parent = 0);
    ~QOfonoConnectionManager();

    bool attached() const;
    QString bearer() const;
    bool suspended() const;

    bool roamingAllowed() const;
    void setRoamingAllowed(bool);

    bool powered() const;
    void setPowered(bool);

    QStringList contexts();
    bool isValid() const;

    QString filter() const;
    void setFilter(const QString &filter);

Q_SIGNALS:
    void attachedChanged(bool value);
    void bearerChanged(const QString &bearer);
    void suspendedChanged(bool suspnd);
    void roamingAllowedChanged(bool roaming);
    void poweredChanged(bool powered);

    void contextAdded(const QString &path);
    void contextRemoved(const QString &path);
    void contextsChanged(const QStringList &contexts);
    void filterChanged(const QString &filter);

public slots:
    void deactivateAll();
    void addContext(const QString &type);
    void removeContext(const QString &path);

private slots:
    void onAddContextFinished(QDBusPendingCallWatcher *watch);
    void onRemoveContextFinished(QDBusPendingCallWatcher *watch);
    void onGetContextsFinished(QDBusPendingCallWatcher *watch);
    void onContextAdded(const QDBusObjectPath &path, const QVariantMap &propertyMap);
    void onContextRemoved(const QDBusObjectPath &path);

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void dbusInterfaceDropped();
    void propertyChanged(const QString &property, const QVariant &value);

private:
    class Private;
    Private* d_ptr;
};

#endif // QOFONOCONNECTIONMANAGER_H
