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

#include "qofonotexttelephony.h"
#include "ofono_text_telephony_interface.h"

#define SUPER QOfonoModemInterface

QOfonoTextTelephony::QOfonoTextTelephony(QObject *parent) :
    SUPER(OfonoTextTelephony::staticInterfaceName(), parent)
{
}

QOfonoTextTelephony::~QOfonoTextTelephony()
{
}

QDBusAbstractInterface *QOfonoTextTelephony::createDbusInterface(const QString &path)
{
    return new OfonoTextTelephony("org.ofono", path, QDBusConnection::systemBus(), this);
}

void QOfonoTextTelephony::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

QString QOfonoTextTelephony::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoTextTelephony::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("Enabled")) {
        Q_EMIT setTtyEnabled(value.toBool());
    }
}

bool QOfonoTextTelephony::ttyEnabled() const
{
    return getBool("Enabled");
}

void QOfonoTextTelephony::setTtyEnabled(bool enabled)
{
    setProperty("Enabled", enabled);
}

bool QOfonoTextTelephony::isValid() const
{
    return SUPER::isValid();
}
