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
    QString errorMessage;

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
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        if (d_ptr->voiceCallManager) {
            delete d_ptr->voiceCallManager;
            d_ptr->voiceCallManager = 0;
            d_ptr->properties.clear();
        }
        d_ptr->voiceCallManager = new OfonoVoiceCallManager("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->voiceCallManager->isValid()) {
            d_ptr->modemPath = path;

            connect(d_ptr->voiceCallManager,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            connectSignals();
            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->voiceCallManager->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            Q_EMIT emergencyNumbersChanged(emergencyNumbers());
            Q_EMIT modemPathChanged(path);
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
             this,SLOT(onCallAdded(QDBusObjectPath,QVariantMap)));
     disconnect(d_ptr->voiceCallManager,SIGNAL(CallRemoved(QDBusObjectPath)),
             this,SLOT(onCallRemoved(QDBusObjectPath)));

     disconnect(d_ptr->voiceCallManager,SIGNAL(BarringActive(QString)),
             this,SIGNAL(barringActive(QString)));
     disconnect(d_ptr->voiceCallManager,SIGNAL(Forwarded(QString)),
             this,SIGNAL(forwarded(QString)));

     connect(d_ptr->voiceCallManager,SIGNAL(CallAdded(QDBusObjectPath,QVariantMap)),
             this,SLOT(onCallAdded(QDBusObjectPath,QVariantMap)));
     connect(d_ptr->voiceCallManager,SIGNAL(CallRemoved(QDBusObjectPath)),
             this,SLOT(onCallRemoved(QDBusObjectPath)));

     connect(d_ptr->voiceCallManager,SIGNAL(BarringActive(QString)),
             this,SIGNAL(barringActive(QString)));
     connect(d_ptr->voiceCallManager,SIGNAL(Forwarded(QString)),
             this,SIGNAL(forwarded(QString)));

 }

 void QOfonoVoiceCallManager::dial(const QString &number, const QString &calleridHide)
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<> result = d_ptr->voiceCallManager->Dial(number,calleridHide);
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(dialFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::hangupAll()
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<> result =  d_ptr->voiceCallManager->HangupAll();
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(hangupAllFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::sendTones(const QString &tonestring)
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<> result = d_ptr->voiceCallManager->SendTones(tonestring);
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(sendTonesFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::transfer()
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<> result = d_ptr->voiceCallManager->Transfer();
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(transferFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::swapCalls()
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<> result =  d_ptr->voiceCallManager->SwapCalls();
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(swapCallsFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::releaseAndAnswer()
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<> result = d_ptr->voiceCallManager->ReleaseAndAnswer();
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(releaseAndAnswerFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::holdAndAnswer()
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<> result = d_ptr->voiceCallManager->HoldAndAnswer();
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(holdAndAnswerFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::privateChat(const QString &path)
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<QList<QDBusObjectPath> > result = d_ptr->voiceCallManager->PrivateChat(QDBusObjectPath(path));
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(privateChatFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::createMultiparty()
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<QList<QDBusObjectPath> > result = d_ptr->voiceCallManager->CreateMultiparty();
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(createMultipartyFinished(QDBusPendingCallWatcher*)));
     }
 }

 void QOfonoVoiceCallManager::hangupMultiparty()
 {
     if(d_ptr->voiceCallManager) {
         QDBusPendingReply<> result = d_ptr->voiceCallManager->HangupMultiparty();
         QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
         connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                 SLOT(hangupMultipartyFinished(QDBusPendingCallWatcher*)));
     }
 }

 bool QOfonoVoiceCallManager::isValid() const
 {
     return d_ptr->voiceCallManager->isValid();
 }

 void QOfonoVoiceCallManager::dialFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
         d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
         ok = false;
     }
     Q_EMIT dialComplete(ok);

 }

 void QOfonoVoiceCallManager::hangupAllFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
         d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
         ok = false;
     }
     Q_EMIT hangupAllComplete(ok);
 }

 void QOfonoVoiceCallManager::sendTonesFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
//     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
         d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
//         ok = false;
     }
   //  Q_EMIT sendTonesComplete(ok);
 }

 void QOfonoVoiceCallManager::transferFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
         d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
         ok = false;
     }
     Q_EMIT transferComplete(ok);
 }

 void QOfonoVoiceCallManager::swapCallsFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
         d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
         ok = false;
     }
     Q_EMIT swapCallsComplete(ok);
 }

 void QOfonoVoiceCallManager::releaseAndAnswerFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
         d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
         ok = false;
     }
     Q_EMIT releaseAndAnswerComplete(ok);
 }

 void QOfonoVoiceCallManager::holdAndAnswerFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
          d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
          ok = false;
     }
    Q_EMIT holdAndAnswerComplete(ok);
 }

 void QOfonoVoiceCallManager::privateChatFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<QList<QDBusObjectPath> > reply = *call;
     QStringList calls;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
         d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
         ok = false;
     } else {
         Q_FOREACH(const QDBusObjectPath &path, reply.value())
             calls << path.path();

     }
     Q_EMIT privateChatComplete(ok, calls);
 }

 void QOfonoVoiceCallManager::createMultipartyFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
          d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
          ok = false;
     }
     Q_EMIT createMultipartyComplete(ok, QStringList());
 }

 void QOfonoVoiceCallManager::hangupMultipartyFinished(QDBusPendingCallWatcher *call)
 {
     call->deleteLater();
     QDBusPendingReply<> reply = *call;
     bool ok = true;
     if (reply.isError()) {
         qWarning() << Q_FUNC_INFO << "failed:" << reply.error();
          d_ptr->errorMessage = reply.error().name() + " " + reply.error().message();
          ok = false;
     }
     Q_EMIT hangupMultipartyComplete(ok);
 }


 QString QOfonoVoiceCallManager::errorMessage() const
 {
     return d_ptr->errorMessage;
 }

 void QOfonoVoiceCallManager::onCallRemoved(const QDBusObjectPath &path)
 {
     Q_EMIT callRemoved(path.path());
 }

 void QOfonoVoiceCallManager::onCallAdded(const QDBusObjectPath &path, const QVariantMap &)
 {
     Q_EMIT callAdded(path.path());
 }
