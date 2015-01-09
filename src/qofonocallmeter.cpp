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

#include "qofonocallmeter.h"
#include "ofono_callmeter_interface.h"

#define SUPER QOfonoModemInterface

QOfonoCallMeter::QOfonoCallMeter(QObject *parent) :
    SUPER(OfonoCallMeter::staticInterfaceName(), parent)
{
}

QOfonoCallMeter::~QOfonoCallMeter()
{
}

QDBusAbstractInterface *QOfonoCallMeter::createDbusInterface(const QString &path)
{
    OfonoCallMeter *iface = new OfonoCallMeter("org.ofono", path, QDBusConnection::systemBus(), this);
    connect(iface, SIGNAL(NearMaximumWarning()), SIGNAL(nearMaximumWarning()));
    return iface;
}

void QOfonoCallMeter::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == QLatin1String("AccumulatedCallMeterMaximum")) {
        Q_EMIT accumulatedCallMeterMaximumChanged(value.value<quint32>());
    } else if (property == QLatin1String("PricePerUnit")) {
        Q_EMIT pricePerUnitChanged(value.value<qreal>());
    }
}

quint32 QOfonoCallMeter::callMeter() const
{
    return getProperty("CallMeter").value<quint32>();
}

quint32 QOfonoCallMeter::accumulatedCallMeter() const
{
    return getProperty("AccumulatedCallMeter").value<quint32>();
}

quint32 QOfonoCallMeter::accumulatedCallMeterMaximum() const
{
    return getProperty("AccumulatedCallMeterMaximum").value<quint32>();
}

void QOfonoCallMeter::setAccumulatedCallMeterMaximum(quint32 max, const QString &password)
{
    OfonoCallMeter *iface = (OfonoCallMeter*)dbusInterface();
    if (iface) {
        QVariantList arguments;
        arguments << QVariant(max);
        iface->SetProperty("AccumulatedCallMeterMaximum", QDBusVariant(arguments), password);
    }
}

qreal QOfonoCallMeter::pricePerUnit() const
{
    return getProperty("PricePerUnit").value<qreal>();
}

void QOfonoCallMeter::setPricePerUnit(qreal unit, const QString &password)
{
    OfonoCallMeter *iface = (OfonoCallMeter*)dbusInterface();
    if (iface) {
        QVariantList arguments;
        arguments << QVariant(unit);
        iface->SetProperty("PricePerUnit", QDBusVariant(arguments), password);
    }
}

QString QOfonoCallMeter:: currency() const
{
    return getString("Currency");
}

void QOfonoCallMeter::reset(const QString &password)
{
    OfonoCallMeter *iface = (OfonoCallMeter*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(iface->Reset(password), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onResetFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoCallMeter::onResetFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    QOfonoCallMeter::Error error = NoError;
    if (reply.isError()) {
        qWarning() << "QOfonoCallMeter::reset() failed:" << reply.error();
        QString errorName(reply.error().name());
        error = errorNameToEnum(errorName);
        Q_EMIT resetComplete(error, errorName + " " + reply.error().message());
    }
}

QOfonoCallMeter::Error QOfonoCallMeter::errorNameToEnum(const QString &errorName)
{
    if (errorName == "")
        return NoError;
    else if (errorName == "org.ofono.Error.NotImplemented")
        return NotImplementedError;
    else if (errorName == "org.ofono.Error.InProgress")
        return InProgressError;
    else if (errorName == "org.ofono.Error.InvalidArguments")
        return InvalidArgumentsError;
    else if (errorName == "org.ofono.Error.InvalidFormat")
        return InvalidFormatError;
    else if (errorName == "org.ofono.Error.Failed")
        return FailedError;
    else
        return UnknownError;
}

QString QOfonoCallMeter::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoCallMeter::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

bool QOfonoCallMeter::isValid() const
{
    return SUPER::isValid();
}
