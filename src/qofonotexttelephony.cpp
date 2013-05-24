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

#include "qofonotexttelephony.h"
#include "dbus/ofonotexttelephony.h"

class QOfonoTextTelephonyPrivate
{
public:
    QOfonoTextTelephonyPrivate();
    QString modemPath;
    OfonoTextTelephony *textTelephony;
    QVariantMap properties;

};

QOfonoTextTelephonyPrivate::QOfonoTextTelephonyPrivate() :
    modemPath(QString())
  , textTelephony(0)
{
}

QOfonoTextTelephony::QOfonoTextTelephony(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoTextTelephonyPrivate)
{
}

QOfonoTextTelephony::~QOfonoTextTelephony()
{
    delete d_ptr;
}

void QOfonoTextTelephony::setModemPath(const QString &path)
{
    if (!d_ptr->textTelephony) {
        if (path != modemPath()) {
            d_ptr->textTelephony = new OfonoTextTelephony("org.ofono", path, QDBusConnection::systemBus(),this);

            if (d_ptr->textTelephony->isValid()) {
                d_ptr->modemPath = path;

                connect(d_ptr->textTelephony,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                        this,SLOT(propertyChanged(QString,QDBusVariant)));

                QDBusReply<QVariantMap> reply;
                reply = d_ptr->textTelephony->GetProperties();
                d_ptr->properties = reply.value();
                Q_EMIT modemPathChanged(path);
            }
        }
    }
}

QString QOfonoTextTelephony::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoTextTelephony::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("Enabled")) {
        Q_EMIT setTtyEnabled(value.value<bool>());
    }
}


bool QOfonoTextTelephony::ttyEnabled() const
{
    if (d_ptr->textTelephony)
        return d_ptr->properties["Enabled"].value<bool>();
    else
        return false;
}

void QOfonoTextTelephony::setTtyEnabled(bool enabled)
{
    if (d_ptr->textTelephony)
        d_ptr->textTelephony->SetProperty(QLatin1String("Enabled"),QDBusVariant(enabled));
}

bool QOfonoTextTelephony::isValid() const
{
    return d_ptr->textTelephony->isValid();
}
