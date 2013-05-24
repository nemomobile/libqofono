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

#ifndef QOFONOTextTelephony_H
#define QOFONOTextTelephony_H

#include <QObject>
#include <QDBusVariant>
#include "qofono_global.h"
//! This class is used to access ofono cell broadcast API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/text-telephony-api.txt
 */

class QOfonoTextTelephonyPrivate;
class QOFONOSHARED_EXPORT QOfonoTextTelephony : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)
    Q_PROPERTY(bool ttyEnabled READ ttyEnabled WRITE setTtyEnabled NOTIFY ttyEnabledChanged)

public:
    explicit QOfonoTextTelephony(QObject *parent = 0);
    ~QOfonoTextTelephony();

    QString modemPath() const;
    void setModemPath(const QString &path);


    bool ttyEnabled() const;
    void setTtyEnabled(bool enabled);

    bool isValid() const;
Q_SIGNALS:

    void ttyEnabledChanged(bool);

public slots:
    
private:
    QOfonoTextTelephonyPrivate *d_ptr;
private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONOTextTelephony_H
