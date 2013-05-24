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

#ifndef QOFONOCallMeter_H
#define QOFONOCallMeter_H

#include <QObject>
#include <QDBusVariant>

#include "qofono_global.h"
//! This class is used to access ofono call meter API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/call-meter-api.txt
 */

class QOfonoCallMeterPrivate;
class QOFONOSHARED_EXPORT QOfonoCallMeter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(quint32 callMeter READ callMeter CONSTANT)
    Q_PROPERTY(quint32 accumulatedCallMeter READ accumulatedCallMeter CONSTANT)
    Q_PROPERTY(quint32 accumulatedCallMeterMaximum READ accumulatedCallMeterMaximum NOTIFY accumulatedCallMeterMaximumChanged)
    Q_PROPERTY(qreal pricePerUnit READ pricePerUnit NOTIFY pricePerUnitChanged)
    Q_PROPERTY(QString currency READ currency CONSTANT)

public:
    explicit QOfonoCallMeter(QObject *parent = 0);
    ~QOfonoCallMeter();

    QString modemPath() const;
    void setModemPath(const QString &path);

    quint32 callMeter() const;
    quint32 accumulatedCallMeter() const;

    quint32 accumulatedCallMeterMaximum() const;
    void setAccumulatedCallMeterMaximum(quint32 max, const QString &password);

    qreal pricePerUnit() const;
    void setPricePerUnit(qreal unit, const QString &password);

    QString currency() const;

    Q_INVOKABLE void reset(const QString &password);

    bool isValid() const;
Q_SIGNALS:
    void nearMaximumWarning();
    void accumulatedCallMeterMaximumChanged(quint32);
    void pricePerUnitChanged(qreal);
    void modemPathChanged(const QString &path);

public slots:
    
private:
    QOfonoCallMeterPrivate *d_ptr;
private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONOCallMeter_H
