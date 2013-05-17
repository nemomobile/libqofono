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
};

QOfonoPhonebookPrivate::QOfonoPhonebookPrivate() :
    modemPath(QString())
  , phonebook(0)
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
    if (!d_ptr->phonebook) {
        d_ptr->phonebook = new OfonoPhonebook("org.ofono", path, QDBusConnection::systemBus(),this);
        if (d_ptr->phonebook->isValid())
            d_ptr->modemPath = path;
    }
}

QString QOfonoPhonebook::modemPath() const
{
    return d_ptr->modemPath;
}

QString QOfonoPhonebook::import()
{
    if (d_ptr->phonebook) {
        QDBusReply<QString> reply =  d_ptr->phonebook->Import();
        return reply.value();
    }
    return QString();
}

