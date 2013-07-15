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

#include "qofonoradiosettings.h"
#include "dbus/ofonoradiosettings.h"

class QOfonoRadioSettingsPrivate
{
public:
    QOfonoRadioSettingsPrivate();
    QString modemPath;
    OfonoRadioSettings *radioSettings;
    QVariantMap properties;

};

QOfonoRadioSettingsPrivate::QOfonoRadioSettingsPrivate() :
    modemPath(QString())
  , radioSettings(0)
{
}

QOfonoRadioSettings::QOfonoRadioSettings(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoRadioSettingsPrivate)
{
}

QOfonoRadioSettings::~QOfonoRadioSettings()
{
    delete d_ptr;
}

void QOfonoRadioSettings::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    QStringList removedProperties = d_ptr->properties.keys();

    delete d_ptr->radioSettings;
    d_ptr->radioSettings = new OfonoRadioSettings("org.ofono", path, QDBusConnection::systemBus(),this);

    if (d_ptr->radioSettings->isValid()) {
        d_ptr->modemPath = path;

        connect(d_ptr->radioSettings,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));

        QVariantMap properties = d_ptr->radioSettings->GetProperties().value();
        for (QVariantMap::ConstIterator it = properties.constBegin();
             it != properties.constEnd(); ++it) {
            updateProperty(it.key(), it.value());
            removedProperties.removeOne(it.key());
        }

        Q_EMIT modemPathChanged(path);
    }

    foreach (const QString &p, removedProperties)
        updateProperty(p, QVariant());
}

QString QOfonoRadioSettings::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoRadioSettings::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    updateProperty(property, dbusvalue.variant());
}

void QOfonoRadioSettings::updateProperty(const QString &property, const QVariant &value)
{
    if (d_ptr->properties.value(property) == value)
        return;

    if (value.isValid())
        d_ptr->properties.insert(property, value);
    else
        d_ptr->properties.remove(property);

    if (property == QLatin1String("TechnologyPreference")) {
        Q_EMIT technologyPreferenceChanged(value.value<QString>());
    } else if (property == QLatin1String("GsmBand")) {
        Q_EMIT gsmBandChanged(value.value<QString>());
    } else if (property == QLatin1String("UtmsBand")) {
        Q_EMIT umtsBandChanged(value.value<QString>());
    } else if (property == QLatin1String("FastDormancy")) {
        Q_EMIT fastDormancyChanged(value.value<bool>());
    }
}

QString QOfonoRadioSettings::technologyPreference() const
{
    return d_ptr->properties["TechnologyPreference"].value<QString>();
}

void QOfonoRadioSettings::setTechnologyPreference(const QString &preference)
{
    if( d_ptr->radioSettings)
        d_ptr->radioSettings->SetProperty("TechnologyPreference", QDBusVariant(preference));
}

QString QOfonoRadioSettings::gsmBand() const
{
    return d_ptr->properties["GsmBand"].value<QString>();
}

void QOfonoRadioSettings::setGsmBand(const QString &gsmBand)
{
    if( d_ptr->radioSettings)
        d_ptr->radioSettings->SetProperty("GsmBand", QDBusVariant(gsmBand));
}

QString QOfonoRadioSettings::umtsBand() const
{
    return d_ptr->properties["UmtsBand"].value<QString>();
}

void QOfonoRadioSettings::setUmtsBand(const QString &umtsBand)
{
    if( d_ptr->radioSettings)
        d_ptr->radioSettings->SetProperty("UmtsBand", QDBusVariant(umtsBand));
}

bool QOfonoRadioSettings::fastDormancy() const
{
    return d_ptr->properties["FastDormancy"].value<bool>();
}

void QOfonoRadioSettings::setFastDormancy(bool fastDormancy)
{
    if( d_ptr->radioSettings)
        d_ptr->radioSettings->SetProperty("FastDormancy", QDBusVariant(fastDormancy));
}

bool QOfonoRadioSettings::isValid() const
{
    return d_ptr->radioSettings->isValid();
}
