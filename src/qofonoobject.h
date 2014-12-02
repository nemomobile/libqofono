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

#ifndef QOFONOOBJECT_H
#define QOFONOOBJECT_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include "qofono_global.h"

class QOfonoObject : public QObject
{
    class Private;
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

protected:
    explicit QOfonoObject(QObject *parent = NULL);
    ~QOfonoObject();

public:
    QString objectPath() const;
    void setObjectPath(const QString &path);
    bool isValid() const;

Q_SIGNALS:
    void validChanged(bool valid);
    void setPropertyFinished();
    void reportError(const QString &message);

protected:
    virtual void objectPathChanged(const QString &name) = 0;
    virtual QDBusAbstractInterface* createDbusInterface(const QString &path) = 0;
    virtual QVariant convertProperty(const QString &key, const QVariant &value);
    virtual void propertyChanged(const QString &key,const QVariant &value);

    void setProperty(const QString &key,const QVariant &value);
    bool setPropertySync(const QString &key,const QVariant &value);

    QVariant getProperty(const QString &key) const;
    QString getString(const QString &key) const;
    QStringList getStringList(const QString &key) const;
    QVariantMap getVariantMap(const QString &key) const;
    bool getBool(const QString &key) const;

    QDBusAbstractInterface* dbusInterface() const;
    void resetDbusInterface();

private:
    void setDbusInterface(QDBusAbstractInterface* dbus);

private slots:
    void onGetPropertiesFinished(QDBusPendingCallWatcher *watch);
    void onSetPropertyFinished(QDBusPendingCallWatcher *watch);
    void onPropertyChanged(const QString &key, const QDBusVariant &value);

private:
    Private *d_ptr;
};

inline QString QOfonoObject::getString(const QString &key) const
    { return getProperty(key).value<QString>(); }
inline QStringList QOfonoObject::getStringList(const QString &key) const
    { return getProperty(key).value<QStringList>(); }
inline QVariantMap QOfonoObject::getVariantMap(const QString &key) const
    { return getProperty(key).value<QVariantMap>(); }
inline bool QOfonoObject::getBool(const QString &key) const
    { return getProperty(key).value<bool>(); }

#endif // QOFONOOBJECT_H
