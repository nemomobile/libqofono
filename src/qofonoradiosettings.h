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

#ifndef QOFONORadioSettings_H
#define QOFONORadioSettings_H

#include <QObject>
#include <QDBusVariant>

#include "qofono_global.h"
//! This class is used to access ofono radio settings API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/radio-settings-api.txt
 */

class QOfonoRadioSettingsPrivate;
class QOFONOSHARED_EXPORT QOfonoRadioSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath)

    Q_PROPERTY(QString technologyPreference READ technologyPreference WRITE setTechnologyPreference NOTIFY technologyPreferenceChanged)
    Q_PROPERTY(QString gsmBand READ gsmBand WRITE setGsmBand NOTIFY gsmBandChanged)
    Q_PROPERTY(QString umtsBand READ umtsBand WRITE setUmtsBand NOTIFY umtsBandChanged)
    Q_PROPERTY(bool fastDormancy READ fastDormancy WRITE setFastDormancy NOTIFY fastDormancyChanged)

public:
    explicit QOfonoRadioSettings(QObject *parent = 0);
    ~QOfonoRadioSettings();

    QString modemPath() const;
    void setModemPath(const QString &path);

    QString technologyPreference() const;
    void setTechnologyPreference(const QString &preference);

    QString gsmBand() const;
    void setGsmBand(const QString &gsmBand);

    QString umtsBand() const;
    void setUmtsBand(const QString &umtsBand);

    bool fastDormancy() const;
    void setFastDormancy(bool fastDormancy);

Q_SIGNALS:
    void technologyPreferenceChanged(const QString &preference);
    void gsmBandChanged(const QString &gsmBand);
    void umtsBandChanged(const QString &umtsBand);
    void fastDormancyChanged(bool fastDormancy);

public slots:
    
private:
    QOfonoRadioSettingsPrivate *d_ptr;
private slots:
    void propertyChanged(const QString &property,const QDBusVariant &value);
};

#endif // QOFONORadioSettings_H
