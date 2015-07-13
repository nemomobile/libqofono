/****************************************************************************
**
** Copyright (C) 2013-2015 Jolla Ltd.
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
#include "ofono_radio_settings_interface.h"

#define SUPER QOfonoModemInterface

QOfonoRadioSettings::QOfonoRadioSettings(QObject *parent) :
    SUPER(OfonoRadioSettings::staticInterfaceName(), parent)
{
}

QOfonoRadioSettings::~QOfonoRadioSettings()
{
}

QDBusAbstractInterface *QOfonoRadioSettings::createDbusInterface(const QString &path)
{
    return new OfonoRadioSettings("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoRadioSettings::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

QString QOfonoRadioSettings::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoRadioSettings::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("TechnologyPreference")) {
        Q_EMIT technologyPreferenceChanged(value.value<QString>());
    } else if (property == QLatin1String("AvailableTechnologies")) {
        Q_EMIT availableTechnologiesChanged(value.value<QStringList>());
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
    return getString("TechnologyPreference");
}

void QOfonoRadioSettings::setTechnologyPreference(const QString &preference)
{
    setProperty("TechnologyPreference",  preference);
}

QStringList QOfonoRadioSettings::availableTechnologies() const
{
    return getStringList("AvailableTechnologies");
}

QString QOfonoRadioSettings::gsmBand() const
{
    return getString("GsmBand");
}

void QOfonoRadioSettings::setGsmBand(const QString &gsmBand)
{
    setProperty("GsmBand", gsmBand);
}

QString QOfonoRadioSettings::umtsBand() const
{
    return getString("UmtsBand");
}

void QOfonoRadioSettings::setUmtsBand(const QString &umtsBand)
{
    setProperty("UmtsBand", umtsBand);
}

bool QOfonoRadioSettings::fastDormancy() const
{
    return getBool("FastDormancy");
}

void QOfonoRadioSettings::setFastDormancy(bool fastDormancy)
{
    setProperty("FastDormancy", fastDormancy);
}

bool QOfonoRadioSettings::isValid() const
{
    return SUPER::isValid();
}
