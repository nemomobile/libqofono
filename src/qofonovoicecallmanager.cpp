/***************************************************`*************************
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

#include "qofonovoicecallmanager.h"
#include "dbus/ofonovoicecallmanager.h"

class QOfonoVoiceCallManagerPrivate
{
public:
    QOfonoVoiceCallManagerPrivate();
    QString modemPath;
    OfonoVoiceCallManager *voiceCallManager;
    QVariantMap properties;
    QStringList callList;

};

QOfonoVoiceCallManagerPrivate::QOfonoVoiceCallManagerPrivate() :
    modemPath(QString())
  , voiceCallManager(0)
  , callList(QStringList())
{
}

QOfonoVoiceCallManager::QOfonoVoiceCallManager(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoVoiceCallManagerPrivate)
{
}

QOfonoVoiceCallManager::~QOfonoVoiceCallManager()
{
    delete d_ptr;
}

void QOfonoVoiceCallManager::setModemPath(const QString &path)
{
    if (!d_ptr->voiceCallManager) {
        d_ptr->modemPath = path;
        d_ptr->voiceCallManager = new OfonoVoiceCallManager("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->voiceCallManager) {
            connect(d_ptr->voiceCallManager,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            connectSignals();
            QDBusReply<QVariantMap> reply;
            reply = d_ptr->voiceCallManager->GetProperties();
            d_ptr->properties = reply.value();
        }
    }
}

QString QOfonoVoiceCallManager::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoVoiceCallManager::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("EmergencyNumbers")) {
        Q_EMIT emergencyNumbersChanged(value.value<QStringList>());
    }
}

QStringList QOfonoVoiceCallManager::emergencyNumbers() const
{
    if(d_ptr->voiceCallManager)
       return  d_ptr->properties["EmergencyNumbers"].toStringList();

    return QStringList();
}

QStringList QOfonoVoiceCallManager::getCalls() const
{
    if(d_ptr->voiceCallManager) {
        d_ptr->callList.clear();
        QDBusReply<QArrayOfPathProperties> reply = d_ptr->voiceCallManager->GetCalls();
        foreach(OfonoPathProperties calls, reply.value()) {
            d_ptr->callList << calls.path.path();
        }
    }
    return d_ptr->callList;
}

 void QOfonoVoiceCallManager::connectSignals()
 {
     disconnect(d_ptr->voiceCallManager,SIGNAL(CallAdded(QDBusObjectPath,QVariantMap)),
             this,SIGNAL(callAdded(QString,QVariantMap)));
     disconnect(d_ptr->voiceCallManager,SIGNAL(CallRemoved(QDBusObjectPath)),
             this,SIGNAL(callRemoved(QString,QVariantMap)));
     disconnect(d_ptr->voiceCallManager,SIGNAL(BarringActive(QString)),
             this,SIGNAL(barringActive(QString)));
     disconnect(d_ptr->voiceCallManager,SIGNAL(Forwarded(QString)),
             this,SIGNAL(forwarded(QString)));

     connect(d_ptr->voiceCallManager,SIGNAL(CallAdded(QDBusObjectPath,QVariantMap)),
             this,SIGNAL(callAdded(QString,QVariantMap)));
     connect(d_ptr->voiceCallManager,SIGNAL(CallRemoved(QDBusObjectPath)),
             this,SIGNAL(callRemoved(QString,QVariantMap)));
     connect(d_ptr->voiceCallManager,SIGNAL(BarringActive(QString)),
             this,SIGNAL(barringActive(QString)));
     connect(d_ptr->voiceCallManager,SIGNAL(Forwarded(QString)),
             this,SIGNAL(forwarded(QString)));

 }

 void QOfonoVoiceCallManager::dial(const QString &number, const QString &calleridHide)
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->Dial(number,calleridHide);
 }

 void QOfonoVoiceCallManager::hangupAll()
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->HangupAll();
 }

 void QOfonoVoiceCallManager::sendTones(const QString &tonestring)
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->SendTones(tonestring);
 }

 void QOfonoVoiceCallManager::transfer()
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->Transfer();
 }

 void QOfonoVoiceCallManager::swapCalls()
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->SwapCalls();
 }

 void QOfonoVoiceCallManager::releaseAndAnswer()
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->ReleaseAndAnswer();
 }

 void QOfonoVoiceCallManager::holdAndAnswer()
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->HoldAndAnswer();
 }

 void QOfonoVoiceCallManager::privateChat(const QString &path)
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->PrivateChat(QDBusObjectPath(path));
 }

 void QOfonoVoiceCallManager::createMultiparty()
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->CreateMultiparty();
 }

 void QOfonoVoiceCallManager::hangupMultiparty()
 {
     if(d_ptr->voiceCallManager)
         d_ptr->voiceCallManager->HangupMultiparty();
 }

