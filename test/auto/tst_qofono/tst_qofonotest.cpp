/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@gmail.com
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

#include <QString>
#include <QtTest>
#include "qofonomanager.h"
#include "qofonoconnectionmanager.h"
#include "qofonoconnectioncontext.h"
#include "qofonomodem.h"
#include "qofononetworkregistration.h"

// These auto tests require
// phonesim or real modem
class Tst_qofonoTest : public QObject
{
    Q_OBJECT
    
public:
    Tst_qofonoTest();
    
private Q_SLOTS:
    void testManager();
    void testModem();
    void testConnectionManager();
    void testContextConnection();
    void testNetworkRegistration();
};

Tst_qofonoTest::Tst_qofonoTest()
{

}

void Tst_qofonoTest::testManager()
{
    QOfonoManager manager;
    QStringList modems = manager.modems();
    QVERIFY(!modems.isEmpty());
    //TODO modemAdded, modemRemoved signals ?

}

void Tst_qofonoTest::testModem()
{
    QOfonoManager manager;
    QOfonoModem modem;
    QVERIFY(modem.modemPath().isEmpty());
    QStringList modemList = manager.modems();
    modem.setModemPath(modemList[0]);
    QVERIFY(!modem.modemPath().isEmpty());

    if (!modem.powered()) {
        modem.setPowered(true);
        QVERIFY(modem.powered());
    } else {
        if (modemList[0] != "/phonesim") {
            // power off phonesim modem doesnt work
            modem.setPowered(false);
            QVERIFY(!modem.powered());
            modem.setPowered(true);
            QVERIFY(modem.powered());
        }
    }
    if (!modem.online()) {
        modem.setOnline(true);
        QTest::qWait(1000);
    }
    QVERIFY(modem.online());
}

void Tst_qofonoTest::testConnectionManager()
{
    QOfonoManager manager;
    QOfonoConnectionManager connman;

    QVERIFY(connman.modemPath().isEmpty());
    connman.setModemPath(manager.modems()[0]);
    QVERIFY(!connman.modemPath().isEmpty());

    if (!connman.contexts().isEmpty()) {
        Q_FOREACH(const QString &path, connman.contexts()) {
            connman.removeContext(path);
        }
    }
    // signals
    QVERIFY(connman.contexts().isEmpty());
    QSignalSpy spy(&connman, SIGNAL(contextAdded(QString)));
    connman.addContext("internet");
    QTest::qWait(1000);

    QCOMPARE(spy.count(),1);
    QList<QVariant> arguments ;
    arguments = spy.takeFirst();

    // attached

    // bearer
    // suspended
    // roamingAllowed
    QSignalSpy spy_roaming(&connman, SIGNAL(roamingAllowedChanged(bool)));

    connman.setRoamingAllowed(true);
    QTest::qWait(1000);

    QCOMPARE(spy_roaming.count(),1);
    QList<QVariant> argumentsspy_roaming;
    argumentsspy_roaming = spy_roaming.takeFirst();
    QCOMPARE(argumentsspy_roaming[0].toBool(), true);

    connman.setRoamingAllowed(false);
    QTest::qWait(1000);

    QCOMPARE(spy_roaming.count(),1);
    QList<QVariant> argumentsspy_roaming2;
    argumentsspy_roaming2 = spy_roaming.takeFirst();
    if (manager.modems()[0].contains("phonesim"))
        QEXPECT_FAIL("","seems to nt work on phonesim",Continue);
    QCOMPARE(argumentsspy_roaming[0].toBool(), false);

    // powered

    QSignalSpy spy_powered(&connman, SIGNAL(poweredChanged(bool)));
    connman.setPowered(true);

    QTest::qWait(1000);
    //    if (manager.modems()[0].contains("phonesim"))
    //            QEXPECT_FAIL("","seems to nt work on phonesim",Continue);
    QCOMPARE(spy_powered.count(),1);
    QList<QVariant> argumentsspy_powered;
    argumentsspy_powered = spy_powered.takeFirst();
    QCOMPARE(argumentsspy_powered[0].toBool(), true);

    connman.setPowered(false);
    QTest::qWait(1000);
    QCOMPARE(spy_powered.count(),1);
    QList<QVariant> argumentsspy_powered2;
    argumentsspy_powered2 = spy_powered.takeFirst();
    QCOMPARE(argumentsspy_powered[0].toBool(), false);

    ////

    QVERIFY(!connman.contexts().isEmpty());
    QSignalSpy spy2(&connman, SIGNAL(contextRemoved(QString)));
    connman.removeContext(arguments[0].toString());
    QTest::qWait(1000);

    QCOMPARE(spy2.count(),1);
    QList<QVariant> arguments2 ;
    arguments2 = spy2.takeFirst();

    QVERIFY(connman.contexts().isEmpty());

    //TODO other signals
}

void Tst_qofonoTest::testContextConnection()
{
    QOfonoManager manager;
    QStringList modems = manager.modems();

    QOfonoConnectionManager connman;
    connman.setModemPath(modems[0]);

    QOfonoConnectionContext connContext;
    QStringList contextList = connman.contexts();
    QVERIFY(contextList.isEmpty());

    connman.addContext("internet");
    QTest::qWait(1000);

    contextList = connman.contexts();

    QVERIFY(!contextList.isEmpty());

    connContext.setContextPath(contextList[0]);

    QVERIFY(!connContext.contextPath().isEmpty());

    QVERIFY(connContext.accessPointName().isEmpty());
    QSignalSpy spy1(&connContext, SIGNAL(accessPointNameChanged(QString)));
    connContext.setAccessPointName("Jolla");
    QTest::qWait(1000);
    QCOMPARE(spy1.count(),1);
    QList<QVariant> arguments1;
    arguments1 = spy1.takeFirst();
    QCOMPARE(arguments1[0].toString(),QString("Jolla"));

    if (!connman.contexts().isEmpty()) {
        Q_FOREACH(const QString &path, connman.contexts()) {
            connman.removeContext(path);
        }
    }

    //        QVERIFY(!connContext.active());
    //        QSignalSpy spy2(&connContext, SIGNAL(activeChanged(bool)));
    //        connContext.setActive(true);
    //        QTest::qWait(1000);
    //        QCOMPARE(spy2.count(),1);

    //        QList<QVariant> arguments2 = spy2.takeFirst();
    //        QCOMPARE(arguments2[0].toBool(),true);

}

void Tst_qofonoTest::testNetworkRegistration()
{

    QOfonoManager manager;
    QStringList modems = manager.modems();

    QOfonoNetworkRegistration netreg;

    QVERIFY(netreg.modemPath().isEmpty());
    netreg.setModemPath(modems[0]);

    QVERIFY(!netreg.name().isEmpty());
    QVERIFY(!netreg.mode().isEmpty());
    QVERIFY(!netreg.status().isEmpty());


    QVERIFY(!netreg.locationAreaCode() != 0);
    QVERIFY(!netreg.mcc().isEmpty());
    QVERIFY(!netreg.mnc().isEmpty());

    //    QVERIFY(!netreg.cellId() != 0);
    //    QVERIFY(!netreg.strength() != 0);
    //    QVERIFY(!netreg.baseStation().isEmpty());
}

QTEST_MAIN(Tst_qofonoTest)

#include "tst_qofonotest.moc"
