/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
**
**
** $QT_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QString>
#include <QtTest>

#include "qofonomanager.h"
#include "qofonomodem.h"
#include "qofonoassistedsatellitenavigation.h"
#include "qofonopositioningrequestagent.h"

class Tst_locationTest : public QObject
{
    Q_OBJECT
    
public:
    Tst_locationTest();
    
private Q_SLOTS:
    void testCase1();
};

Tst_locationTest::Tst_locationTest()
{
    QOfonoManager oMan;

    QOfonoAssistedSatelliteNavigation satNav;
    satNav.setModemPath(oMan.modems().at(0));

    QOfonoPositioningRequestAgent posAgent;
    posAgent.setAgentPath("/QOfonoTest");
    satNav.registerPositioningRequestAgent("/QOfonoTest");

}

void Tst_locationTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(Tst_locationTest)

#include "tst_locationtest.moc"
