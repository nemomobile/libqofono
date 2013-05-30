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

#include "qofonoconnectioncontext.h"
#include "qofonoconnectionmanager.h"


// These auto tests require
// phonesim or real modem

static bool waitForSignal(QObject *obj, const char *signal, int timeout = 0)
{
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit()));
    QTimer timer;
    QSignalSpy timeoutSpy(&timer, SIGNAL(timeout()));
    if (timeout > 0) {
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.setSingleShot(true);
        timer.start(timeout);
    }
    loop.exec();
    return timeoutSpy.isEmpty();
}


class Tst_qofonoTest : public QObject
{
    Q_OBJECT
    
public:
    Tst_qofonoTest();
    
private Q_SLOTS:

    void testManager();
    void testModem();
    void testScan();

    void testContextConnection();
    void testConnectionManager();

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
    // something controls the modem power.
    // TODO

//    QOfonoManager manager;
//    QOfonoModem modem;
//    QVERIFY(modem.modemPath().isEmpty());

//    QStringList modemList = manager.modems();

//    QSignalSpy spy(&modem, SIGNAL(modemPathChanged(QString)));
//    modem.setModemPath(modemList[0]);
//    QTest::qWait(2000);
//    QCOMPARE(spy.count(),1);

//    QVERIFY(!modem.modemPath().isEmpty());

//    if (!modem.powered()) {
//        modem.setPowered(true);
//        QTest::qWait(2000);
//        QVERIFY(modem.powered());
//    } else {
//        if (modemList[0] != "/phonesim") {
//            // power off phonesim modem doesnt work
//            modem.setPowered(false);
//             QTest::qWait(2000);
//            QVERIFY(!modem.powered());

//            modem.setPowered(true);
//             QTest::qWait(2000);
//            QVERIFY(modem.powered());
//        }
//    }
//    bool isOnline = modem.online();
//    modem.setOnline(!isOnline);
//    QTest::qWait(3000);
//    QVERIFY(!isOnline);

//     modem.setOnline(true);
}

void Tst_qofonoTest::testScan()
{
    QOfonoManager manager;
    QOfonoModem modem;
    modem.setModemPath(manager.modems()[0]);
    QOfonoNetworkRegistration netreg;
    netreg.setModemPath(manager.modems()[0]);

    QSignalSpy spy_scanFinished(&netreg, SIGNAL(scanFinished()));
    QSignalSpy spy_scanError(&netreg, SIGNAL(scanError(QString)));

    netreg.scan();
    ::waitForSignal(&netreg, SIGNAL(scanFinished()), 1000 * 500);

    QCOMPARE(spy_scanError.count(),0);
    QCOMPARE(spy_scanFinished.count(),1);

}

void Tst_qofonoTest::testConnectionManager()
{
    QOfonoManager manager;
    QOfonoConnectionManager connman;
    QOfonoModem modem;
    modem.setModemPath(manager.modems()[0]);

    if (!modem.online()) {
        modem.setOnline(true);
    }
    QVERIFY(modem.online());

    connman.setModemPath(manager.modems()[0]);
    QVERIFY(!connman.modemPath().isEmpty());

    QSignalSpy spy(&connman, SIGNAL(contextAdded(QString)));
    connman.addContext("internet");
    ::waitForSignal(&connman,SIGNAL(contextAdded(QString)),1000);
    QCOMPARE(spy.count(),1);

//    QList<QVariant> arguments ;
//    arguments = spy.takeFirst();

    // attached

    // bearer
    // suspended
    // roamingAllowed
//    QSignalSpy spy_roaming(&connman, SIGNAL(roamingAllowedChanged(bool)));
//    bool expected = !connman.roamingAllowed();
//    connman.setRoamingAllowed(expected);
//    QTest::qWait(3000);
//    QCOMPARE(spy_roaming.count(),1);
//    QList<QVariant> argumentsspy_roaming;
//    argumentsspy_roaming = spy_roaming.takeFirst();
//    QCOMPARE(argumentsspy_roaming[0].toBool(), expected);

//    connman.setRoamingAllowed(false);
//    QTest::qWait(3000);
//    QCOMPARE(spy_roaming.count(),1);
//    QList<QVariant> argumentsspy_roaming2;
//    argumentsspy_roaming2 = spy_roaming.takeFirst();
//    if (manager.modems()[0].contains("phonesim"))
//        QEXPECT_FAIL("","seems to nt work on phonesim",Continue);
//    QCOMPARE(argumentsspy_roaming[0].toBool(), false);

//    // powered

//    QSignalSpy spy_powered(&connman, SIGNAL(poweredChanged(bool)));
//    QList<QVariant> argumentsspy_powered;
  //  bool isPowered = connman.powered();

//    qDebug() << Q_FUNC_INFO << "modem powered" << connman.powered();

//    connman.setPowered(!isPowered);
//    QTest::qWait(3000);
//    QCOMPARE(spy_powered.count(),2); //powers itself back on
//    argumentsspy_powered = spy_powered.takeFirst();
//    QCOMPARE(argumentsspy_powered[0].toBool(), !isPowered);

//    if(!connman.powered()) {
//        connman.setPowered(true);
//        QTest::qWait(3000);
//        QVERIFY(connman.powered());
//    }

    //TODO other signals
}

void Tst_qofonoTest::testContextConnection()
{
    QOfonoManager manager;
    QOfonoModem modem;
    QStringList modems = manager.modems();

    modem.setModemPath(manager.modems()[0]);
    qDebug() << Q_FUNC_INFO << manager.modems()[0] << modem.online();

    if (!modem.online()) {
        QSignalSpy spy_bogusContext(&modem, SIGNAL(onlineChanged(bool)));
        modem.setOnline(true);
        ::waitForSignal(&modem,SIGNAL(onlineChanged(bool)),3000);
        QCOMPARE(spy_bogusContext.count(),1);
    }

    QVERIFY(modem.online());

    QVERIFY(modem.interfaces().contains("org.ofono.ConnectionManager"));

    QOfonoConnectionManager connman;
    connman.setModemPath(modems[0]);
    QVERIFY(connman.isValid());

    if (!connman.powered()) {
        QSignalSpy spy_connPowered(&connman, SIGNAL(poweredChanged(bool)));
        connman.setPowered(true);
        ::waitForSignal(&connman,SIGNAL(poweredChanged(bool)),3000);

        QCOMPARE(spy_connPowered.count(),1);
    }

    QOfonoConnectionContext connContext;
    QStringList contextList = connman.contexts();

    int contextCount = contextList.count();

    QSignalSpy spy_bogusContext(&connman, SIGNAL(contextAdded(QString)));
    QSignalSpy spy_contextError(&connman, SIGNAL(reportError(QString)));

    connman.addContext(QString("test"));
    ::waitForSignal(&connman,SIGNAL(reportError(QString)),3000);

    QCOMPARE(spy_bogusContext.count(),0);
    QCOMPARE(spy_contextError.count(),1);

    QSignalSpy spy_realContext(&connman, SIGNAL(contextAdded(QString)));
    connman.addContext(QString("internet"));
    ::waitForSignal(&connman,SIGNAL(contextAdded(QString)),3000);

    QCOMPARE(spy_realContext.count(),1);

    contextList = connman.contexts();
    QVERIFY(!contextList.isEmpty());    
    QVERIFY(contextList.count() == contextCount + 1);


    Q_FOREACH(const QString &path, connman.contexts()) {

        QSignalSpy spy2(&connContext, SIGNAL(contextPathChanged(QString)));
        connContext.setContextPath(path);
        ::waitForSignal(&connContext,SIGNAL(contextPathChanged(QString)),1000);

        QCOMPARE(spy2.count(),1);
        QVERIFY(!connContext.contextPath().isEmpty());

        QVERIFY(connContext.accessPointName().isEmpty());
        QSignalSpy spy1(&connContext, SIGNAL(accessPointNameChanged(QString)));
        connContext.setAccessPointName("Jolla");
        ::waitForSignal(&connman,SIGNAL(accessPointNameChanged(QString)),2000);

        QCOMPARE(spy1.count(),1);

        QList<QVariant> arguments1;
        arguments1 = spy1.takeFirst();
        QCOMPARE(arguments1[0].toString(),QString("Jolla"));

        QSignalSpy spy_name(&connContext, SIGNAL(nameChanged(QString)));
        connContext.setName("Test AP");
        ::waitForSignal(&connman,SIGNAL(nameChanged(QString)),2000);

        QTest::qWait(2000);
        QCOMPARE(spy_name.count(),1);
        QList<QVariant> arguments ;
        arguments = spy_name.takeFirst();
        QCOMPARE(arguments[0].toString(), QString("Test AP"));

        connman.removeContext(path);
    }

    QVERIFY(!connContext.active());
    QSignalSpy spy_active(&connContext, SIGNAL(activeChanged(bool)));
    connContext.setActive(true);
    ::waitForSignal(&connman,SIGNAL(activeChanged(bool)),3000);

    QCOMPARE(spy_active.count(),1);

    QList<QVariant> arguments2 = spy_active.takeFirst();
    QCOMPARE(arguments2[0].toBool(),true);

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
    QVERIFY(netreg.cellId() > 0);
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

//        qDebug() << netOp.name()
//                 << netOp.status()
//                 << netOp.mcc()
//                 << netOp.mnc()
//                 << netOp.technologies()
//                 << netOp.additionalInfo();
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
    ::waitForSignal(&simManager,SIGNAL(modemPathChanged(QString)),1000);
    QCOMPARE(spy.count(),1);

}

QTEST_MAIN(Tst_qofonoTest)

#include "tst_qofonotest.moc"
