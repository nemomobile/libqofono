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

#ifndef QOFONOPhonebook_H
#define QOFONOPhonebook_H

#include <QObject>
#include <QDBusVariant>
#include "qofono_global.h"
//! This class is used to access ofono phonebook API
/*!
 * oFono phonebook API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/phonebook-api.txt
 */

class QOfonoPhonebookPrivate;
class QOFONOSHARED_EXPORT QOfonoPhonebook : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(bool importing READ importing NOTIFY importingChanged)

public:
    explicit QOfonoPhonebook(QObject *parent = 0);
    ~QOfonoPhonebook();

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool importing() const;

    bool isValid() const;
Q_SIGNALS:
    void modemPathChanged(const QString &path);
    void importReady(const QString &vcardData);
    void importFailed();
    void importingChanged();

public slots:
    void beginImport();

private slots:
    void importComplete(QDBusPendingCallWatcher *call);

private:
    QOfonoPhonebookPrivate *d_ptr;

};

#endif // QOFONOPhonebook_H
