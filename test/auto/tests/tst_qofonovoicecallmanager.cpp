/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Alexander Kanavin <alex.kanavin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QtTest/QtTest>
#include <QtCore/QObject>

#include "../../../src/qofonovoicecallmanager.h"
#include "../../../src/qofonomodem.h"

#include <QtDebug>

// FIXME: Not aligned with the current API
class TestQOfonoVoiceCallManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoVoiceCallManager(this);
        m->setModemPath("/phonesim");

        QCOMPARE(m->isValid(), true);
    }

    void testQOfonoVoiceCallManager()
    {
        QVERIFY(m->emergencyNumbers().count() > 0);

        QSignalSpy emergencyNumbers(m, SIGNAL(emergencyNumbersChanged(QStringList)));
        QSignalSpy dialreg(m,SIGNAL(dialComplete(bool)));
        QSignalSpy dspy(m, SIGNAL(callAdded(QString)));
        QSignalSpy hupreg(m,SIGNAL(hangupAllComplete(bool)));
        QSignalSpy tonereg(m,SIGNAL(sendTonesComplete(bool)));
        QSignalSpy hspy(m, SIGNAL(callRemoved(QString)));

        QOfonoModem modem;
        modem.setModemPath(m->modemPath());
        modem.setPowered(false);
        QTest::qWait(5000);
        modem.setPowered(true);
        QTest::qWait(5000);
        modem.setOnline(true);
        QTest::qWait(5000);

        QCOMPARE(emergencyNumbers.count(), 1);
        QVERIFY(emergencyNumbers.takeFirst().at(0).toStringList().count() > 0);
        //Dial testing
        m->dial("123","");
        qDebug() << "Please find a call in 'Dialing' state in phonesim window and press 'Active' button";
        QTest::qWait(15000);
        QCOMPARE(dialreg.count(), 1);
        QCOMPARE(dialreg.takeFirst().at(0).toBool(),true);
        QCOMPARE(dspy.count(), 1);
        //Tones testing
        QTest::qWait(5000);
        m->sendTones("1234");
        QTest::qWait(5000);
        QCOMPARE(tonereg.count(), 1);
        QCOMPARE(tonereg.takeFirst().at(0).toBool(),true);
        QTest::qWait(5000);
        QStringList calls = m->getCalls();
        QVERIFY(calls.size()>0);
        //hangup testing
        m->hangupAll();
        QTest::qWait(5000);
        QCOMPARE(hupreg.count(), 1);
        QCOMPARE(hupreg.takeFirst().at(0).toBool(),true);
        QCOMPARE(hspy.count(), 1);

    }

    void testQOfonoVoiceCallManagerStep2()
    {
        // test dial failure and hangup of incoming alerting call
        QSignalSpy dialreg(m,SIGNAL(dialComplete(bool)));
        QSignalSpy hupreg(m,SIGNAL(hangupAllComplete(bool)));
        QSignalSpy dspy(m, SIGNAL(callAdded(QString)));
        QSignalSpy hspy(m, SIGNAL(callRemoved(QString)));


        m->dial("199","");
        QTest::qWait(5000);
        QCOMPARE(dialreg.count(), 1);
        QCOMPARE(dialreg.takeFirst().at(0).toBool(),false);
        QTest::qWait(10000);
        QCOMPARE(dspy.count(), 1);

        //hangup the alerting call
        m->hangupAll();
        QTest::qWait(5000);
        QCOMPARE(hupreg.count(), 1);
        QCOMPARE(hupreg.takeFirst().at(0).toBool(),true);

        QTest::qWait(10000);
        QCOMPARE(hspy.count(), 1);

    }

    void cleanupTestCase()
    {

    }


private:
    QOfonoVoiceCallManager *m;
};

QTEST_MAIN(TestQOfonoVoiceCallManager)
#include "tst_qofonovoicecallmanager.moc"
