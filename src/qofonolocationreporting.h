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

#ifndef QOFONOQLOCATIONREPORTING_H
#define QOFONOQLOCATIONREPORTING_H

#include <QObject>
#include <QDBusVariant>
#include <QStringList>

class QOfonoLocationReportingPrivate;
class QOfonoLocationReporting : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(bool enabled READ enabled)

public:
    explicit QOfonoLocationReporting(QObject *parent = 0);
    ~QOfonoLocationReporting();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QString type() const;
    bool enabled() const;

    bool isValid() const;
public slots:
    void release();
    int request();
Q_SIGNALS:
    void modemPathChanged(const QString &path);

private:
    QOfonoLocationReportingPrivate *d_ptr;

private slots:
};

#endif // QOFONOQLOCATIONREPORTING_H
