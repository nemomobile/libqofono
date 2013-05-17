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
    if (path.isEmpty())
        return;

    if (!d_ptr->modem) {
        d_ptr->modem = new OfonoModem("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->modem->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->modem,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->modem->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoModem::modemPath() const
{
    return d_ptr->modemPath;
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
    if (d_ptr->modem) {
        d_ptr->modem->SetProperty("Powered",QDBusVariant(powered));
    }
}

void QOfonoModem::setOnline(bool online)
{
    if (d_ptr->modem) {
        d_ptr->modem->SetProperty("Online",QDBusVariant(online));
    }
}

void QOfonoModem::setLockdown(bool lockdown)
{
    if (d_ptr->modem) {
        d_ptr->modem->setProperty("Lockdown",qVariantFromValue(lockdown));
    }
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
    return d_ptr->modem->isValid();
}
