/***************************************************`*************************
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
        d_ptr->voiceCallManager = new OfonoVoiceCallManager("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->voiceCallManager->isValid()) {
            d_ptr->modemPath = path;

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
        QDBusReply<ObjectPathPropertiesList> reply = d_ptr->voiceCallManager->GetCalls();
        foreach(ObjectPathProperties calls, reply.value()) {
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

 bool QOfonoVoiceCallManager::isValid() const
 {
     return d_ptr->voiceCallManager->isValid();
 }
