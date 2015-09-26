/****************************************************************************
**
** Copyright (C) 2015 Jolla Ltd.
** Contact: slava.monich@jolla.com
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

#ifndef QOFONOSIMWATCHER_H
#define QOFONOSIMWATCHER_H

#include "qofono_global.h"
#include "qofonosimmanager.h"

// Watches present SIM cards
class QOFONOSHARED_EXPORT QOfonoSimWatcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(int presentSimCount READ presentSimCount NOTIFY presentSimCountChanged)

public:
    explicit QOfonoSimWatcher(QObject *parent = 0);
    ~QOfonoSimWatcher();

    bool isValid() const;
    int presentSimCount() const;
    QList<QOfonoSimManager::SharedPointer> presentSimList() const;

Q_SIGNALS:
    void validChanged();
    void presentSimCountChanged();
    void presentSimListChanged();

private:
    class Private;
    Private* d_ptr;
};

#endif // QOFONOSIMWATCHER_H
