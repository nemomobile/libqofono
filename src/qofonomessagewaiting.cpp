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

#include "qofonomessagewaiting.h"
#include "dbus/ofonomessagewaiting.h"
#include "qofonomodem.h"
#include "qofonoutils_p.h"

class QOfonoMessageWaitingPrivate
{
public:
    QOfonoMessageWaitingPrivate();
    QString modemPath;
    OfonoMessageWaiting *messageWaiting;
    QVariantMap properties;
    QSharedPointer<QOfonoModem> modem;
};

QOfonoMessageWaitingPrivate::QOfonoMessageWaitingPrivate() :
    modemPath(QString())
  , messageWaiting(0)
{
}

QOfonoMessageWaiting::QOfonoMessageWaiting(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoMessageWaitingPrivate)
{
}

QOfonoMessageWaiting::~QOfonoMessageWaiting()
{
    delete d_ptr;
}

void QOfonoMessageWaiting::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath || path.isEmpty())
        return;

    if (!d_ptr->modem.isNull())
        disconnect(d_ptr->modem.data(), SIGNAL(interfacesChanged(QStringList)),
                   this, SLOT(modemInterfacesChanged(QStringList)));

    d_ptr->modemPath = path;
    connectOfono();

    d_ptr->modem = QOfonoModem::instance(path);
    connect(d_ptr->modem.data(), SIGNAL(interfacesChanged(QStringList)),
            this, SLOT(modemInterfacesChanged(QStringList)));

    Q_EMIT modemPathChanged(path);
}

QString QOfonoMessageWaiting::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoMessageWaiting::modemInterfacesChanged(const QStringList &interfaces)
{
    bool haveIface = interfaces.contains("org.ofono.MessageWaiting");
    if (haveIface != (isValid() && isReady()))
        connectOfono();
}

void QOfonoMessageWaiting::connectOfono()
{
    bool wasReady = isReady();
    // FIXME: Clearing properties here results in false *Changed signal
    // emissions. Ideally ready() should not be derived from
    // properties.isEmpty(). Also compare with QOfonoSimManager.
    if (d_ptr->messageWaiting) {
        delete d_ptr->messageWaiting;
        d_ptr->messageWaiting = 0;
        d_ptr->properties.clear();
    }

    d_ptr->messageWaiting = new OfonoMessageWaiting("org.ofono", d_ptr->modemPath, QDBusConnection::systemBus(),this);

    if (d_ptr->messageWaiting->isValid()) {
        connect(d_ptr->messageWaiting,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));

        QDBusPendingReply<QVariantMap> reply;
        reply = d_ptr->messageWaiting->GetProperties();
        reply.waitForFinished();
        if (reply.isError()) {
            Q_EMIT getPropertiesFailed();
        } else {
            QVariantMap properties = reply.value();
            for (QVariantMap::ConstIterator it = properties.constBegin();
                    it != properties.constEnd(); ++it) {
                updateProperty(it.key(), it.value());
            }
        }
    }

    if (wasReady != isReady())
        Q_EMIT readyChanged();
}

void QOfonoMessageWaiting::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    updateProperty(property, dbusvalue.variant());
}

void QOfonoMessageWaiting::updateProperty(const QString& property, const QVariant& value)
{
    QVariant old = d_ptr->properties.value(property);

    d_ptr->properties.insert(property,value);

    if (qofono::safeVariantEq(old, value))
        return;

    if (property == QLatin1String("VoicemailWaiting")) {
        Q_EMIT voicemailWaitingChanged(value.value<bool>());
    } else if (property == QLatin1String("VoicemailMessageCount")) {
        Q_EMIT voicemailMessageCountChanged(value.value<int>());
    } else if (property == QLatin1String("VoicemailMailboxNumber")) {
        Q_EMIT voicemailMailboxNumberChanged(value.value<QString>());
    }
}


bool QOfonoMessageWaiting::voicemailWaiting() const
{
    if ( d_ptr->messageWaiting)
        return d_ptr->properties.value("VoicemailWaiting").value<bool>();
    else
        return false;
}

int QOfonoMessageWaiting::voicemailMessageCount() const
{
    if ( d_ptr->messageWaiting)
        return d_ptr->properties.value("VoicemailMessageCount").value<int>();
    else
        return false;
}

QString QOfonoMessageWaiting::voicemailMailboxNumber() const
{
    if (d_ptr->messageWaiting)
        return d_ptr->properties.value("VoicemailMailboxNumber").value<QString>();
    else
        return QString();
}

void QOfonoMessageWaiting::setVoicemailMailboxNumber(const QString &mailboxnumber)
{
    if (d_ptr->messageWaiting) {
        QDBusPendingReply<> reply = d_ptr->messageWaiting->SetProperty(QLatin1String("VoicemailMailboxNumber"),QDBusVariant(mailboxnumber));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoicemailMailboxNumberComplete(QDBusPendingCallWatcher*)));
    }
}

bool QOfonoMessageWaiting::isValid() const
{
    return d_ptr->messageWaiting && d_ptr->messageWaiting->isValid();
}

bool QOfonoMessageWaiting::isReady() const
{
    return !d_ptr->properties.isEmpty();
}

void QOfonoMessageWaiting::setVoicemailMailboxNumberComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT voicemailMailboxComplete(!reply.isError());
}
