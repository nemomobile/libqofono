/****************************************************************************
**
** Copyright (C) 2013 Lorn Potter
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSensors module of the Qt Toolkit.
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

#include "qofononetworkoperator.h"
#include "dbus/ofononetworkoperator.h"

class QOfonoNetworkOperatorPrivate
{
public:
    QOfonoNetworkOperatorPrivate();
    QString modemPath;
    OfonoNetworkOperator *networkOperator;
    QVariantMap properties;
};

QOfonoNetworkOperatorPrivate::QOfonoNetworkOperatorPrivate() :
    modemPath(QString())
  , networkOperator(0)
{
}

QOfonoNetworkOperator::QOfonoNetworkOperator(QObject *parent) :
    QObject(parent),
    d_ptr(new QOfonoNetworkOperatorPrivate)
{
}

QOfonoNetworkOperator::~QOfonoNetworkOperator()
{
    delete d_ptr;
}

void QOfonoNetworkOperator::setOperatorPath(const QString &path)
{
    if (!d_ptr->networkOperator) {
        d_ptr->modemPath = path;
        d_ptr->networkOperator = new OfonoNetworkOperator("org.ofono", path, QDBusConnection::systemBus(),this);
        if (d_ptr->networkOperator) {
            connect(d_ptr->networkOperator,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->networkOperator->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoNetworkOperator::operatorPath() const
{
    return d_ptr->modemPath;
}

void QOfonoNetworkOperator::registerOperator()
{
     d_ptr->networkOperator->Register();
}

QString QOfonoNetworkOperator::name() const
{
    if ( d_ptr->networkOperator)
        return d_ptr->properties["Name"].value<QString>();
    else
        return QString();
}

QString QOfonoNetworkOperator::status() const
{
    if ( d_ptr->networkOperator)
        return d_ptr->properties["Status"].value<QString>();
    else
        return QString();
}

QString QOfonoNetworkOperator::mcc() const
{
    if ( d_ptr->networkOperator)
        return d_ptr->properties["MobileCountryCode"].value<QString>();
    else
        return QString();
}

QString QOfonoNetworkOperator::mnc() const
{
    if ( d_ptr->networkOperator)
        return d_ptr->properties["MobileNetworkCOde"].value<QString>();
    else
        return QString();
}

QStringList QOfonoNetworkOperator::technologies() const
{
    if ( d_ptr->networkOperator)
        return d_ptr->properties["Techologies"].value<QStringList>();
    else
        return QStringList();
}

QString QOfonoNetworkOperator::additionalInfo() const
{
    if ( d_ptr->networkOperator)
        return d_ptr->properties["AdditionalInformation"].value<QString>();
    else
        return QString();
}


void QOfonoNetworkOperator::propertyChanged(const QString &property, const QDBusVariant &dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("Name")) {
        Q_EMIT nameChanged(value.value<QString>());
    } else if (property == QLatin1String("Status")) {
        Q_EMIT statusChanged(value.value<QString>());
    } else if (property == QLatin1String("MobileCountryCode")) {
        Q_EMIT mccChanged(value.value<QString>());
    } else if (property == QLatin1String("MobileNetworkCode")) {
        Q_EMIT mncChanged(value.value<QString>());
    } else if (property == QLatin1String("Technologies")) {
        Q_EMIT technologiesChanged(value.value<QStringList>());
    } else if (property == QLatin1String("AdditionalInformation")) {
        Q_EMIT additionalInfoChanged(value.value<QString>());
    }
}

