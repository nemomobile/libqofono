/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**
**
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
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
    if (!d_ptr->radioSettings) {
        d_ptr->radioSettings = new OfonoRadioSettings("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->radioSettings->isValid()) {
            d_ptr->modemPath = path;

            connect(d_ptr->radioSettings,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->radioSettings->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoRadioSettings::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoRadioSettings::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

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

