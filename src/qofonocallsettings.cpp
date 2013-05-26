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

class QOfonoCallSettingsPrivate
{
public:
    QOfonoCallSettingsPrivate();
    QString modemPath;
    OfonoCallSettings *callSettings;
    QVariantMap properties;

};

QOfonoCallSettingsPrivate::QOfonoCallSettingsPrivate() :
    modemPath(QString())
  , callSettings(0)
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
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        if (d_ptr->callSettings) {
            delete d_ptr->callSettings;
            d_ptr->callSettings = 0;
            d_ptr->properties.clear();
        }
        d_ptr->callSettings = new OfonoCallSettings("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->callSettings->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->callSettings,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->callSettings->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoCallSettings::modemPath() const
{
    return d_ptr->modemPath;
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
        return d_ptr->properties["CallingLinePresentation"].value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::calledLinePresentation()
{
    if (d_ptr->callSettings)
        return d_ptr->properties["CalledLinePresentation"].value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::callingNamePresentation()
{
    if (d_ptr->callSettings)
        return d_ptr->properties["CallingNamePresentation"].value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::connectedLinePresentation()
{
    if (d_ptr->callSettings)
        return d_ptr->properties["ConnectedLinePresentation"].value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::connectedLineRestriction()
{
    if (d_ptr->callSettings)
        return d_ptr->properties["ConnectedLineRestriction"].value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::callingLineRestriction()
{
    if (d_ptr->callSettings)
        return d_ptr->properties["CallingLineRestriction"].value<QString>();
    else
        return QString();
}

QString QOfonoCallSettings::hideCallerId()
{
    if (d_ptr->callSettings)
        return d_ptr->properties["Currency"].value<QString>();
    else
        return QString();
}

void QOfonoCallSettings::setHideCallerId(const QString &setting)
{
    if (d_ptr->callSettings)
        d_ptr->callSettings->setProperty("HideCallerId", setting);
}

QString QOfonoCallSettings::voiceCallWaiting()
{
    if (d_ptr->callSettings)
        return d_ptr->properties["Currency"].value<QString>();
    else
        return QString();
}

void QOfonoCallSettings::setVoiceCallWaiting(const QString &setting)
{
    if (d_ptr->callSettings)
        d_ptr->callSettings->setProperty("VoiceCallWaiting", setting);
}

bool QOfonoCallSettings::isValid() const
{
    return d_ptr->callSettings->isValid();
}
