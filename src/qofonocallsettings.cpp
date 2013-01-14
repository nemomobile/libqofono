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
    if (!d_ptr->callSettings) {
        d_ptr->callSettings = new OfonoCallSettings("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->callSettings->isValid()) {
            d_ptr->modemPath = path;
            connect(d_ptr->callSettings,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            QDBusReply<QVariantMap> reply;
            reply = d_ptr->callSettings->GetProperties();
            d_ptr->properties = reply.value();
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

