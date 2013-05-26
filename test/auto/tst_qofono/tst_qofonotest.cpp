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
#include "qofonohandsfree.h"
#include "qofonohandsfreeaudiocard.h"

#include "qofononetworkoperator.h"
#include "qofononetworkregistration.h"
#include "qofonomessagemanager.h"
#include "qofonosimmanager.h"

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
    void testNetworkOperator();
    void testSimManager();

    void testHandsfree();
    void testHandsfreeAudioCard();
    void testMessageManager();
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

    QSignalSpy spy(&modem, SIGNAL(modemPathChanged(QString)));
    modem.setModemPath(modemList[0]);
    QTest::qWait(1000);
    QCOMPARE(spy.count(),1);
//    QList<QVariant> arguments ;
//    arguments = spy.takeFirst();

    QVERIFY(!modem.modemPath().isEmpty());

    if (!modem.powered()) {
        modem.setPowered(true);
        QVERIFY(modem.powered());
    } else {
        if (modemList[0] != "/phonesim") {
            // power off phonesim modem doesnt work
            modem.setPowered(false);
             QTest::qWait(2000);
            QVERIFY(!modem.powered());

            modem.setPowered(true);
             QTest::qWait(2000);
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

    connman.setModemPath(manager.modems()[0]);
    QVERIFY(!connman.modemPath().isEmpty());

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

    QList<QVariant> argumentsspy_powered;
    qDebug() << connman.powered();

    if (!connman.powered()) {
        connman.setPowered(true);
        QTest::qWait(1000);
        QCOMPARE(spy_powered.count(),1);
        argumentsspy_powered = spy_powered.takeFirst();
        QCOMPARE(spy_powered.count(),0);
        QCOMPARE(argumentsspy_powered[0].toBool(), true);
    }
    QCOMPARE(spy_powered.count(),0);
    connman.setPowered(false);
    QTest::qWait(1000);
    if (!manager.modems()[0].contains("phonesim")) {
    QCOMPARE(spy_powered.count(),1);

    QList<QVariant> argumentsspy_powered2;
    argumentsspy_powered2 = spy_powered.takeFirst();
    QCOMPARE(argumentsspy_powered[0].toBool(), false);

}
      connman.setPowered(true);
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
//    QVERIFY(contextList.isEmpty());

    connman.addContext("test");
    QTest::qWait(1000);
    contextList = connman.contexts();
    QVERIFY(!contextList.isEmpty());

    Q_FOREACH(const QString &path, connman.contexts()) {

        connContext.setContextPath(path);
        QVERIFY(!connContext.contextPath().isEmpty());

        if (connContext.name() == "test") {
            QVERIFY(connContext.accessPointName().isEmpty());
            QSignalSpy spy1(&connContext, SIGNAL(accessPointNameChanged(QString)));
            connContext.setAccessPointName("Jolla");
            QTest::qWait(1000);
            QCOMPARE(spy1.count(),1);

            QList<QVariant> arguments1;
            arguments1 = spy1.takeFirst();
            QCOMPARE(arguments1[0].toString(),QString("Jolla"));

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
    netreg.setModemPath(modems.at(0));

    QVERIFY(!netreg.name().isEmpty());
    QVERIFY(!netreg.mode().isEmpty());
    QVERIFY(!netreg.status().isEmpty());


    QVERIFY(!netreg.locationAreaCode() != 0);
    QVERIFY(!netreg.mcc().isEmpty());
    QVERIFY(!netreg.mnc().isEmpty());

    QVERIFY(netreg.baseStation().isEmpty());
    QVERIFY(netreg.cellId() == 0);
    QVERIFY(!netreg.currentOperatorPath().isEmpty());
    QVERIFY(netreg.status() == "registered");
    QVERIFY(netreg.locationAreaCode() == 0);
    QVERIFY(netreg.mcc() == "234");
    QVERIFY(netreg.mnc() == "01");
    QVERIFY(netreg.technology().isEmpty());
    QVERIFY(!netreg.networkOperators().isEmpty());
}

void Tst_qofonoTest::testHandsfree()
{
    QOfonoHandsfree handsfree;
    handsfree.setModemPath("/phonesim");

    QVERIFY(handsfree.features().isEmpty());
    QVERIFY(!handsfree.inbandRinging());
    QVERIFY(!handsfree.voiceRecognition());
    QVERIFY(!handsfree.echoCancelingNoiseReduction());
    QVERIFY(handsfree.batteryChargeLevel() == 0);

}

void Tst_qofonoTest::testHandsfreeAudioCard()
{
    QOfonoHandsfreeAudioCard handsfreeAudioCard;
    handsfreeAudioCard.setModemPath("/phonesim");
    QVERIFY(handsfreeAudioCard.remoteAddress().isEmpty());
    QVERIFY(handsfreeAudioCard.localAddress().isEmpty());
}


void Tst_qofonoTest::testNetworkOperator()
{
    QOfonoManager manager;
    QStringList modems = manager.modems();
    QOfonoNetworkRegistration netreg;
    netreg.setModemPath(modems.at(0));
    QStringList operators = netreg.networkOperators();

    Q_FOREACH(const QString &onetopPath, operators) {
        QOfonoNetworkOperator netOp;
        netOp.setOperatorPath(onetopPath);

        qDebug() << netOp.name()
                 << netOp.status()
                 << netOp.mcc()
                 << netOp.mnc()
                 << netOp.technologies()
                 << netOp.additionalInfo();
    }
}

void Tst_qofonoTest::testMessageManager()
{
    QOfonoManager manager;
    QStringList modems = manager.modems();
    QOfonoMessageManager messageManager;
    messageManager.setModemPath(modems.at(0));

 //   QSignalSpy spy1(&messageManager, SIGNAL(messagesFinished()));

}

void Tst_qofonoTest::testSimManager()
{
    QOfonoManager manager;
    QStringList modems = manager.modems();
    QOfonoSimManager simManager;

    QSignalSpy spy(&simManager, SIGNAL(modemPathChanged(QString)));
    simManager.setModemPath(modems[0]);
    QTest::qWait(1000);
    QCOMPARE(spy.count(),1);

}

QTEST_MAIN(Tst_qofonoTest)

#include "tst_qofonotest.moc"
