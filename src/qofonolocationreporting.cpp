/****************************************************************************
**
** Copyright (C) 2013-2015 Jolla Ltd.
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

#include "qofonolocationreporting.h"
#include "ofono_location_reporting_interface.h"
#include <unistd.h>

class QOfonoLocationReportingPrivate
{
public:
    QOfonoLocationReportingPrivate();
    QString modemPath;
    OfonoLocationReporting *ofonoLocationReporting;
    QVariantMap properties;

};

QOfonoLocationReportingPrivate::QOfonoLocationReportingPrivate() :
    modemPath(QString())
  , ofonoLocationReporting(0)
{
}

QOfonoLocationReporting::QOfonoLocationReporting(QObject *parent) :
    QObject(parent)
  , d_ptr(new QOfonoLocationReportingPrivate)
{
}

QOfonoLocationReporting::~QOfonoLocationReporting()
{
    delete d_ptr;
}

void QOfonoLocationReporting::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        if (d_ptr->ofonoLocationReporting) {
            delete d_ptr->ofonoLocationReporting;
            d_ptr->ofonoLocationReporting = 0;
            d_ptr->properties.clear();
        }
        d_ptr->ofonoLocationReporting = new OfonoLocationReporting("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->ofonoLocationReporting) {
            d_ptr->modemPath = path;
            QDBusPendingReply<QVariantMap> reply;
            reply = d_ptr->ofonoLocationReporting->GetProperties();
            reply.waitForFinished();
            d_ptr->properties = reply.value();
            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoLocationReporting::modemPath() const
{
    return d_ptr->modemPath;
}

QString QOfonoLocationReporting::type() const
{
    if (d_ptr->ofonoLocationReporting) {
        return d_ptr->properties["Type"].value<QString>();
    }
    return QString();
}

bool QOfonoLocationReporting::enabled() const
{
    if (d_ptr->ofonoLocationReporting) {
        return d_ptr->properties["Enabled"].value<bool>();
    }
    return false;
}

void QOfonoLocationReporting::release()
{
    if (d_ptr->ofonoLocationReporting)
        d_ptr->ofonoLocationReporting->Release();
}

int QOfonoLocationReporting::request()
{
    if (d_ptr->ofonoLocationReporting) {
        QDBusPendingReply<QDBusUnixFileDescriptor> reply = d_ptr->ofonoLocationReporting->Request();
        if (!reply.isError() && reply.value().isValid()) {
            return dup(reply.value().fileDescriptor()); //pass on fd
        } else {
            qDebug() << Q_FUNC_INFO << reply.error().message();
        }
    }
    return 0;
}

bool QOfonoLocationReporting::isValid() const
{
    return d_ptr->ofonoLocationReporting->isValid();
}
