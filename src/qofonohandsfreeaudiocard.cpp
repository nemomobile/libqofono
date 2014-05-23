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

#include "qofonohandsfreeaudiocard.h"
#include "dbus/ofonohandsfreeaudiocard.h"


class QOfonoHandsfreeAudioCardPrivate
{
public:
    QOfonoHandsfreeAudioCardPrivate();
    QString modemPath;
    OfonoHandsfreeAudioCard *ofonoHandsfreeAudioCard;
    QVariantMap properties;

};

QOfonoHandsfreeAudioCardPrivate::QOfonoHandsfreeAudioCardPrivate() :
    modemPath(QString())
  , ofonoHandsfreeAudioCard(0)
{
}

QOfonoHandsfreeAudioCard::QOfonoHandsfreeAudioCard(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoHandsfreeAudioCardPrivate)
{
}

QOfonoHandsfreeAudioCard::~QOfonoHandsfreeAudioCard()
{
    delete d_ptr;
}

void QOfonoHandsfreeAudioCard::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        if (d_ptr->ofonoHandsfreeAudioCard) {
            delete d_ptr->ofonoHandsfreeAudioCard;
            d_ptr->ofonoHandsfreeAudioCard = 0;
            d_ptr->properties.clear();
        }
        d_ptr->modemPath = path;
        d_ptr->ofonoHandsfreeAudioCard = new OfonoHandsfreeAudioCard("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->ofonoHandsfreeAudioCard) {

            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->ofonoHandsfreeAudioCard->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoHandsfreeAudioCard::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoHandsfreeAudioCard::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

}

QString QOfonoHandsfreeAudioCard::remoteAddress() const
{
    if (d_ptr->ofonoHandsfreeAudioCard) {
        return d_ptr->properties["RemoteAddress"].value<QString>();
    }
    return QString();
}

QString QOfonoHandsfreeAudioCard::localAddress() const
{
    if (d_ptr->ofonoHandsfreeAudioCard) {
        return d_ptr->properties["LocalAddress"].value<QString>();
    }
    return QString();
}

void QOfonoHandsfreeAudioCard::connectAudio()
{
    if (d_ptr->ofonoHandsfreeAudioCard) {
        QDBusPendingReply<> result = d_ptr->ofonoHandsfreeAudioCard->Connect();

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(connectAudioFinished(QDBusPendingCallWatcher*)));
    }
}

bool QOfonoHandsfreeAudioCard::isValid() const
{
    return d_ptr->ofonoHandsfreeAudioCard->isValid();
}

void QOfonoHandsfreeAudioCard::connectAudioFinished(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    QOfonoHandsfreeAudioCard::Error error = NoError;
    QString errorString;

    if (reply.isError()) {
         qWarning() << "QOfonoHandsfreeAudioCard::connectAudio() failed:" << reply.error();
         error = errorNameToEnum(reply.error().name());
         errorString = reply.error().name() + " " + reply.error().message();
    }

    emit connectAudioComplete(error, errorString);
}

QOfonoHandsfreeAudioCard::Error QOfonoHandsfreeAudioCard::errorNameToEnum(const QString &errorName)
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
