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

#ifndef QOFONO_GLOBAL_H
#define QOFONO_GLOBAL_H

#include <QtCore/qglobal.h>
#include "dbustypes.h"

#if defined(QOFONO_LIBRARY)
#  define QOFONOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QOFONOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QOFONO_GLOBAL_H
