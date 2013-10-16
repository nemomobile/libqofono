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

#ifndef QOFONODECLARATIVEPLUGIN_H
#define QOFONODECLARATIVEPLUGIN_H

#include "qofono_global.h"

#if QT_VERSION < 0x050000
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeExtensionPlugin>
class QOFONOSHARED_EXPORT QOfonoDeclarativePlugin : public QDeclarativeExtensionPlugin
#else
#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>
class QOFONOSHARED_EXPORT QOfonoDeclarativePlugin : public QQmlExtensionPlugin
#endif
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface" FILE "plugin.json")
#endif
public:
    void registerTypes(const char *uri);
#if QT_VERSION < 0x050000
    void initializeEngine(QDeclarativeEngine *engine, const char *uri);
#endif
};

#endif // QOFONODECLARATIVEPLUGIN_H
