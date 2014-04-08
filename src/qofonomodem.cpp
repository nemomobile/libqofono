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
#include <QStringList>

#include "qofonomodem.h"
#include "dbus/ofonomodem.h"
#include <QMap>
#include <QWeakPointer>
#include <QDBusPendingCallWatcher>

typedef QMap<QString,QWeakPointer<QOfonoModem> > ModemMap;
Q_GLOBAL_STATIC(ModemMap, modemMap)

class QOfonoModemPrivate
{
public:
    QOfonoModemPrivate();
    QString modemPath;
    OfonoModem *modem;
    QVariantMap properties;
};

QOfonoModemPrivate::QOfonoModemPrivate() :
    modemPath(QString())
  , modem(0)
{
}

QOfonoModem::QOfonoModem(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoModemPrivate)
{
}

QOfonoModem::~QOfonoModem()
{
    delete d_ptr;
}

void QOfonoModem::setModemPath(const QString &path)
{
    if (path != modemPath()) {
        d_ptr->modemPath = path;
        connectOfono();
        Q_EMIT modemPathChanged(path);
    }
}

QString QOfonoModem::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoModem::connectOfono()
{
    bool wasValid = isValid();
    if (d_ptr->modem) {
        delete d_ptr->modem;
        d_ptr->modem = 0;
        d_ptr->properties.clear();
    }

    d_ptr->modem = new OfonoModem("org.ofono", d_ptr->modemPath, QDBusConnection::systemBus(),this);

    if (d_ptr->modem->isValid()) {
        connect(d_ptr->modem,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));

        QDBusPendingReply<QVariantMap> reply;
        reply = d_ptr->modem->GetProperties();
        reply.waitForFinished();
        d_ptr->properties = reply.value();
    }

    if (wasValid != isValid())
        Q_EMIT validChanged(isValid());
}

bool QOfonoModem::powered() const
{
    if (d_ptr->modem) {
         return d_ptr->properties["Powered"].value<bool>();
    }
    return false;
}

bool QOfonoModem::online() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Online"].value<bool>();
    }
    return false;
}

bool QOfonoModem::lockdown() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Lockdown"].value<bool>();
    }
    return false;
}

bool QOfonoModem::emergency() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Emergency"].value<bool>();
    }
    return false;
}

QString QOfonoModem::name() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Name"].value<QString>();
    }
    return QString();
}

QString QOfonoModem::manufacturer() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Manufacturer"].value<QString>();
    }
    return QString();
}

QString QOfonoModem::model() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Model"].value<QString>();
    }
    return QString();
}

QString QOfonoModem::revision() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Revision"].value<QString>();
    }
    return QString();
}

QString QOfonoModem::serial() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Serial"].value<QString>();
    }
    return QString();
}

QString QOfonoModem::type() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Type"].value<QString>();
    }
    return QString();
}

QStringList QOfonoModem::features() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Features"].value<QStringList>();
    }
    return QStringList();
}

QStringList QOfonoModem::interfaces() const
{
    if (d_ptr->modem) {
        return d_ptr->properties["Interfaces"].value<QStringList>();
    }
    return QStringList();
}

void QOfonoModem::setPowered(bool powered)
{
    QString str("Powered");
    QDBusVariant var(powered);
    setOneProperty(str,var);
}

void QOfonoModem::setOnline(bool online)
{
    QString str("Online");
    QDBusVariant var(online);
    setOneProperty(str,var);
}

void QOfonoModem::setLockdown(bool lockdown)
{
    QString str = "Lockdown";
    QDBusVariant var(lockdown);
    setOneProperty(str,var);
}

void QOfonoModem::propertyChanged(const QString& property, const QDBusVariant& dbusValue)
{
    QVariant value = dbusValue.variant();
    d_ptr->properties.insert(property,value);
    if (property == QLatin1String("Online"))
        Q_EMIT onlineChanged(value.value<bool>());
    else if (property == QLatin1String("Powered"))
        Q_EMIT poweredChanged(value.value<bool>());
    else if (property == QLatin1String("Lockdown"))
        Q_EMIT lockdownChanged(value.value<bool>());
    else if (property == QLatin1String("Emergency"))
        Q_EMIT emergencyChanged(value.value<bool>());
    else if (property == QLatin1String("Name"))
        Q_EMIT nameChanged(value.value<QString>());
    else if (property == QLatin1String("Manufacturer"))
        Q_EMIT manufacturerChanged(value.value<QString>());
    else if (property == QLatin1String("Model"))
        Q_EMIT modelChanged(value.value<QString>());
    else if (property == QLatin1String("Revision"))
        Q_EMIT revisionChanged(value.value<QString>());
    else if (property == QLatin1String("Serial"))
        Q_EMIT serialChanged(value.value<QString>());
    else if (property == QLatin1String("Type"))
        Q_EMIT typeChanged(value.value<QString>());
    else if (property == QLatin1String("Features"))
        Q_EMIT featuresChanged(value.value<QStringList>());
    else if (property == QLatin1String("Interfaces"))
        Q_EMIT interfacesChanged(value.value<QStringList>());

}

bool QOfonoModem::isValid() const
{
    return d_ptr->modem && d_ptr->modem->isValid();
}

QSharedPointer<QOfonoModem> QOfonoModem::instance(const QString &modemPath)
{
    QSharedPointer<QOfonoModem> modem = modemMap()->value(modemPath);
    if (modem.isNull()) {
        modem = QSharedPointer<QOfonoModem>::create();
        modem->setModemPath(modemPath);
        modemMap()->insert(modemPath, QWeakPointer<QOfonoModem>(modem));
    } else if (!modem->isValid()) {
        modem->connectOfono();
    }

    return modem;
}

void QOfonoModem::setOneProperty(const QString &prop, const QDBusVariant &var)
{
    if (d_ptr->modem) {
        QDBusPendingReply <> reply = d_ptr->modem->SetProperty(prop,var);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setPropertyFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoModem::setPropertyFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<> reply = *watch;
    if(reply.isError()) {
        qDebug() << Q_FUNC_INFO  << reply.error().message();
        Q_EMIT reportError(reply.error().message());
    }
}

