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

#include "qofonophonebook.h"
#include "dbus/ofonophonebook.h"

class QOfonoPhonebookPrivate
{
public:
    QOfonoPhonebookPrivate();
    QString modemPath;
    OfonoPhonebook *phonebook;
    bool importing;
};

QOfonoPhonebookPrivate::QOfonoPhonebookPrivate() :
    modemPath(QString())
  , phonebook(0)
  , importing(false)
{
}

QOfonoPhonebook::QOfonoPhonebook(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoPhonebookPrivate)
{
}

QOfonoPhonebook::~QOfonoPhonebook()
{
    delete d_ptr;
}

void QOfonoPhonebook::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        if (d_ptr->phonebook) {
            delete d_ptr->phonebook;
            d_ptr->phonebook = 0;
        }
        d_ptr->phonebook = new OfonoPhonebook("org.ofono", path, QDBusConnection::systemBus(),this);
        if (d_ptr->phonebook->isValid()) {
            d_ptr->modemPath = path;
            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoPhonebook::modemPath() const
{
    return d_ptr->modemPath;
}

bool QOfonoPhonebook::importing() const
{
    return d_ptr->importing;
}

void QOfonoPhonebook::beginImport()
{
    if (d_ptr->phonebook) {
        QDBusPendingReply<QString> result = d_ptr->phonebook->Import();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(importComplete(QDBusPendingCallWatcher*)));
        d_ptr->importing = true;
        emit importingChanged();
    }
}

void QOfonoPhonebook::importComplete(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString> reply = *call;
    if (!reply.isError()) {
        QString data = reply.value();
        emit importReady(data);
    } else {
        emit importFailed();
    }
    d_ptr->importing = false;
    call->deleteLater();
    emit importingChanged();
}

bool QOfonoPhonebook::isValid() const
{
    return d_ptr->phonebook->isValid();
}

