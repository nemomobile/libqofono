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

#ifndef QOFONOASSISTEDSATELLITENAVIGATION_H
#define QOFONOASSISTEDSATELLITENAVIGATION_H

#include <QObject>
#include <QDBusVariant>
#include <QStringList>

class QOfonoAssistedSatelliteNavigationPrivate;
class QOfonoAssistedSatelliteNavigation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)

public:

    explicit QOfonoAssistedSatelliteNavigation(QObject *parent = 0);
    ~QOfonoAssistedSatelliteNavigation();

    QString modemPath() const;
    void setModemPath(const QString &path);

    bool isValid() const;
Q_SIGNALS:
    void modemPathChanged(const QString &path);

public slots:

    void registerPositioningRequestAgent(const QString &path);
    void sendPositioningElement(const QString &xmlElement);
    void unregisterPositioningRequestAgent(const QString &path);

private:
    QOfonoAssistedSatelliteNavigationPrivate *d_ptr;
};

#endif // QOFONOASSISTEDSATELLITENAVIGATION_H
