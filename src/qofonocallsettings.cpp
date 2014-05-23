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

#include "qofonocallsettings.h"
#include "dbus/ofonocallsettings.h"
#include "qofonomodem.h"

class QOfonoCallSettingsPrivate
{
public:
    QOfonoCallSettingsPrivate();
    QString modemPath;
    OfonoCallSettings *callSettings;
    QVariantMap properties;
    bool propertiesPending;
    QSharedPointer<QOfonoModem> modem;
};

QOfonoCallSettingsPrivate::QOfonoCallSettingsPrivate() :
    modemPath(QString())
  , callSettings(0)
  , propertiesPending(false)
{
}

QOfonoCallSettings::QOfonoCallSettings(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoCallSettingsPrivate)
{
}

QOfonoCallSettings::~QOfonoCallSettings()
{
    delete d_ptr;
}

void QOfonoCallSettings::setModemPath(const QString &path)
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

QString QOfonoCallSettings::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoCallSettings::modemInterfacesChanged(const QStringList &interfaces)
{
    bool haveIface = interfaces.contains("org.ofono.CallSettings");
    if (haveIface != (isValid() && (isReady() || d_ptr->propertiesPending)))
        connectOfono();
}

void QOfonoCallSettings::connectOfono()
{
    if (d_ptr->callSettings) {
        bool wasReady = isReady();
        delete d_ptr->callSettings;
        d_ptr->callSettings = 0;
        d_ptr->properties.clear();
        d_ptr->propertiesPending = false;
        if (wasReady != isReady())
            Q_EMIT readyChanged();
    }

    d_ptr->callSettings = new OfonoCallSettings("org.ofono", d_ptr->modemPath, QDBusConnection::systemBus(),this);

    if (d_ptr->callSettings->isValid()) {
        connect(d_ptr->callSettings,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this,SLOT(propertyChanged(QString,QDBusVariant)));
        d_ptr->propertiesPending = true;
        QDBusPendingReply<QVariantMap> reply = d_ptr->callSettings->GetProperties();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(getPropertiesComplete(QDBusPendingCallWatcher*)));
    }
}


void QOfonoCallSettings::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("CallingLinePresentation")) {
        Q_EMIT callingLinePresentationChanged(value.value<QString>());
    } else if (property == QLatin1String("CalledLinePresentation")) {
        Q_EMIT calledLinePresentationChanged(value.value<QString>());
    } else if (property == QLatin1String("CallingNamePresentation")) {
        Q_EMIT callingNamePresentationChanged(value.value<QString>());
    } else if (property == QLatin1String("ConnectedLinePresentation")) {
        Q_EMIT connectedLinePresentationChanged(value.value<QString>());
    } else if (property == QLatin1String("ConnectedLineRestriction")) {
        Q_EMIT connectedLineRestrictionChanged(value.value<QString>());
    } else if (property == QLatin1String("CallingLineRestriction")) {
        Q_EMIT callingLineRestrictionChanged(value.value<QString>());
    } else if (property == QLatin1String("HideCallerId")) {
        Q_EMIT hideCallerIdChanged(value.value<QString>());
    } else if (property == QLatin1String("VoiceCallWaiting")) {
        Q_EMIT voiceCallWaitingChanged(value.value<QString>());
    }
}

QString QOfonoCallSettings::callingLinePresentation()
{
    if (d_ptr->callSettings)
        return d_ptr->properties.value("CallingLinePresentation").value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::calledLinePresentation()
{
    if (d_ptr->callSettings)
        return d_ptr->properties.value("CalledLinePresentation").value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::callingNamePresentation()
{
    if (d_ptr->callSettings)
        return d_ptr->properties.value("CallingNamePresentation").value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::connectedLinePresentation()
{
    if (d_ptr->callSettings)
        return d_ptr->properties.value("ConnectedLinePresentation").value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::connectedLineRestriction()
{
    if (d_ptr->callSettings)
        return d_ptr->properties.value("ConnectedLineRestriction").value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::callingLineRestriction()
{
    if (d_ptr->callSettings)
        return d_ptr->properties.value("CallingLineRestriction").value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::hideCallerId()
{
    if (d_ptr->callSettings)
        return d_ptr->properties.value("HideCallerId").value<QString>();
    else
        return QString();
}

void QOfonoCallSettings::setHideCallerId(const QString &setting)
{
    if (d_ptr->callSettings) {
        QDBusPendingReply<> reply = d_ptr->callSettings->SetProperty("HideCallerId", QDBusVariant(setting));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setHideCallerIdComplete(QDBusPendingCallWatcher*)));
    }
}

QString QOfonoCallSettings::voiceCallWaiting()
{
    if (d_ptr->callSettings)
        return d_ptr->properties.value("VoiceCallWaiting").value<QString>();
    else
        return QString();
}

void QOfonoCallSettings::setVoiceCallWaiting(const QString &setting)
{
    if (d_ptr->callSettings) {
        QDBusPendingReply<> reply = d_ptr->callSettings->SetProperty("VoiceCallWaiting", QDBusVariant(setting));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(setVoiceCallWaitingComplete(QDBusPendingCallWatcher*)));
    }
}

bool QOfonoCallSettings::isValid() const
{
    return d_ptr->callSettings->isValid();
}

bool QOfonoCallSettings::isReady() const
{
    return !d_ptr->properties.isEmpty();
}

void QOfonoCallSettings::getPropertiesComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<QVariantMap> reply = *call;
    if (!reply.isError()) {
        d_ptr->properties = reply.value();
        Q_EMIT callingLinePresentationChanged(callingLinePresentation());
        Q_EMIT calledLinePresentationChanged(calledLinePresentation());
        Q_EMIT callingNamePresentationChanged(callingNamePresentation());
        Q_EMIT connectedLinePresentationChanged(connectedLinePresentation());
        Q_EMIT connectedLineRestrictionChanged(connectedLineRestriction());
        Q_EMIT callingLineRestrictionChanged(callingLineRestriction());
        Q_EMIT hideCallerIdChanged(hideCallerId());
        Q_EMIT voiceCallWaitingChanged(voiceCallWaiting());
        Q_EMIT readyChanged();
    } else {
        Q_EMIT getPropertiesFailed();
    }
    d_ptr->propertiesPending = false;
}

void QOfonoCallSettings::setHideCallerIdComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT hideCallerIdComplete(!reply.isError());
}

void QOfonoCallSettings::setVoiceCallWaitingComplete(QDBusPendingCallWatcher *call)
{
    call->deleteLater();
    QDBusPendingReply<> reply = *call;
    Q_EMIT voiceCallWaitingComplete(!reply.isError());
}
