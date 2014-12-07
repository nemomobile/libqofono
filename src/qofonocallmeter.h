/****************************************************************************
**
** Copyright (C) 2013-2014 Jolla Ltd.
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

#include "qofonomodeminterface.h"
#include "qofono_global.h"

//! This class is used to access ofono call meter API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/call-meter-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoCallMeter : public QOfonoModemInterface
{
    Q_OBJECT
    Q_ENUMS(Error)
    Q_PROPERTY(quint32 callMeter READ callMeter CONSTANT)
    Q_PROPERTY(quint32 accumulatedCallMeter READ accumulatedCallMeter CONSTANT)
    Q_PROPERTY(quint32 accumulatedCallMeterMaximum READ accumulatedCallMeterMaximum NOTIFY accumulatedCallMeterMaximumChanged)
    Q_PROPERTY(qreal pricePerUnit READ pricePerUnit NOTIFY pricePerUnitChanged)
    Q_PROPERTY(QString currency READ currency CONSTANT)

public:
    enum Error {
        NoError,
        NotImplementedError,
        InProgressError,
        InvalidArgumentsError,
        InvalidFormatError,
        FailedError,
        UnknownError
    };

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
    void resetComplete(QOfonoCallMeter::Error error,const QString &errorString);

private:
    static Error errorNameToEnum(const QString &errorName);

private slots:
    void onResetFinished(QDBusPendingCallWatcher *call);

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void propertyChanged(const QString &property, const QVariant &value);
};

#endif // QOFONOCallMeter_H
