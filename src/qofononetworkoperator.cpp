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

QOfonoNetworkOperator::QOfonoNetworkOperator(const QOfonoNetworkOperator &op)
    : QObject(op.parent()),
        d_ptr(op.d_ptr)
{
    setOperatorPath(op.operatorPath());
}

QOfonoNetworkOperator::~QOfonoNetworkOperator()
{
    delete d_ptr;
}

bool QOfonoNetworkOperator::operator==(const QOfonoNetworkOperator &op)
{
    return operatorPath() == op.operatorPath();
}

//QOfonoNetworkOperator QOfonoNetworkOperator::operator=(const QOfonoNetworkOperator &other)
//{
//    d_ptr = other.d_ptr;
//    return *this;
//}

void QOfonoNetworkOperator::setOperatorPath(const QString &path)
{
    if (path != operatorPath()) {
        if (d_ptr->networkOperator) {
            delete d_ptr->networkOperator;
            d_ptr->networkOperator = 0;
        }
        d_ptr->networkOperator = new OfonoNetworkOperator("org.ofono", path, QDBusConnection::systemBus(),this);
        if (d_ptr->networkOperator->isValid()) {
            d_ptr->modemPath = path;

            connect(d_ptr->networkOperator,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));

            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->networkOperator->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            Q_EMIT operatorPathChanged(path);
        }
    }
}

QString QOfonoNetworkOperator::operatorPath() const
{
    return d_ptr->modemPath;
}

void QOfonoNetworkOperator::registerOperator()
{
    QDBusPendingReply<void> result = d_ptr->networkOperator->Register();

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(registerFinished(QDBusPendingCallWatcher*)));
}

void QOfonoNetworkOperator::registerFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<> reply = *call;
    QOfonoNetworkOperator::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoNetworkOperator::registerOperator() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }
    Q_EMIT registerComplete(error,errorString);
}

QString QOfonoNetworkOperator::name() const
{
    if (d_ptr->networkOperator)
        return d_ptr->properties["Name"].value<QString>();
    else
        return QString();
}

QString QOfonoNetworkOperator::status() const
{
    if (d_ptr->networkOperator)
        return d_ptr->properties["Status"].value<QString>();
    else
        return QString();
}

QString QOfonoNetworkOperator::mcc() const
{
    if (d_ptr->networkOperator)
        return d_ptr->properties["MobileCountryCode"].value<QString>();
    else
        return QString();
}

QString QOfonoNetworkOperator::mnc() const
{
    if (d_ptr->networkOperator)
        return d_ptr->properties["MobileNetworkCode"].value<QString>();
    else
        return QString();
}

QStringList QOfonoNetworkOperator::technologies() const
{
    if (d_ptr->networkOperator) {
        return d_ptr->properties["Technologies"].value<QStringList>();
    } else {
        return QStringList();
    }
}

QString QOfonoNetworkOperator::additionalInfo() const
{
    if (d_ptr->networkOperator)
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

bool QOfonoNetworkOperator::isValid() const
{
    return d_ptr->networkOperator->isValid();
}

QOfonoNetworkOperator::Error QOfonoNetworkOperator::errorNameToEnum(const QString &errorName)
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
