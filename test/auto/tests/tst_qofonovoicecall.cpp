/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include  "../../../src/qofonovoicecallmanager.h"
#include  "../../../src/qofonovoicecall.h"
#include <QtDebug>


class TestQOfonoVoiceCall : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoVoiceCallManager( this);
        m->setModemPath("/phonesim");

        QCOMPARE(m->isValid(), true);
    }

    void testQOfonoVoiceCall()
    {
        QSignalSpy dialreg(m,SIGNAL(dialComplete(bool)));
        QSignalSpy dspy(m, SIGNAL(callAdded(QString)));

        // Dial and hangup
        m->dial("123","");
        QTest::qWait(1000);

        QCOMPARE(dialreg.count(), 1);
        QCOMPARE(dialreg.takeFirst().at(0).toBool(),true);
        QCOMPARE(dspy.count(), 1);
        QString callid = dspy.takeFirst().at(0).toString();

        QOfonoVoiceCall* call = new QOfonoVoiceCall(this);
        call->setVoiceCallPath(callid);

        QSignalSpy state(call, SIGNAL(stateChanged(const QString)));
        QSignalSpy discreason(call,SIGNAL(disconnectReason(const QString)));
        QSignalSpy hspy(call, SIGNAL(hangupComplete(bool)));
        QSignalSpy li (call, SIGNAL(lineIdentificationChanged(QString)));
        QSignalSpy name (call, SIGNAL(nameChanged(QString)));
        QSignalSpy info (call, SIGNAL(informationChanged(QString)));
        QSignalSpy mp (call, SIGNAL(multipartyChanged(bool)));
        QSignalSpy em (call, SIGNAL(emergencyChanged(bool)));
        QSignalSpy st (call, SIGNAL(startTimeChanged(QString)));
        QSignalSpy ic (call, SIGNAL(iconChanged(quint8)));

        qDebug() << "Please find a call in 'Dialing' state in phonesim window and press 'Active' button";
        QTest::qWait(15000);

        QVERIFY(state.count()>0);
        QVERIFY(st.count()>0);
        QVERIFY(ic.count()==0);
        QVERIFY(em.count()==0);
        QVERIFY(mp.count()==0);
        QVERIFY(info.count()==0);
        QVERIFY(name.count()==0);
        QVERIFY(li.count()==0);

        QCOMPARE(call->state(),QString("active"));
        QCOMPARE(call->lineIdentification(),QString("123"));
        QCOMPARE(call->emergency(),false);
        QCOMPARE(call->multiparty(),false);
        QCOMPARE(call->name(),QString(""));
        QCOMPARE(call->information(),QString(""));

        call->hangup();
        QTest::qWait(5000);
        QCOMPARE(hspy.count(), 1);
        QCOMPARE(hspy.takeFirst().at(0).toBool(),true);
        QCOMPARE(discreason.count(), 1);
        QCOMPARE(discreason.takeFirst().at(0).toString(), QString("local"));
        delete call;
    }

    void testQOfonoVoiceCallStep2()
    {
        //Dial failure, incoming, answer and local hangup
        QSignalSpy callsignal(m, SIGNAL(callAdded(const QString)));

        m->dial("199","");
        QTest::qWait(8000);

        QCOMPARE(callsignal.count(),1);
        QString callid = callsignal.takeFirst().at(0).toString();

        QOfonoVoiceCall* call = new QOfonoVoiceCall(this);
        call->setVoiceCallPath(callid);
        QSignalSpy state(call, SIGNAL(stateChanged(const QString)));
        QSignalSpy time(call,SIGNAL(startTimeChanged(const QString)));
        QSignalSpy discreason(call,SIGNAL(disconnectReason(const QString)));
        QSignalSpy hspy(call, SIGNAL(hangupComplete(bool)));
        QSignalSpy aspy(call, SIGNAL(answerComplete(bool)));

        QSignalSpy li (call, SIGNAL(lineIdentificationChanged(QString)));
        QSignalSpy name (call, SIGNAL(nameChanged(QString)));
        QSignalSpy info (call, SIGNAL(informationChanged(QString)));
        QSignalSpy mp (call, SIGNAL(multipartyChanged(bool)));
        QSignalSpy em (call, SIGNAL(emergencyChanged(bool)));
        QSignalSpy ic (call, SIGNAL(iconChanged(quint8)));


        QTest::qWait(8000);
        QCOMPARE(call->state(),QString("incoming"));
        call->answer();
        QTest::qWait(10000);
        QVERIFY(state.count()>0);
        QCOMPARE(call->lineIdentification(),QString("1234567")); //PhoneSim specific value
        QVERIFY(time.count()>0);
        QCOMPARE(aspy.count(), 1);
        QCOMPARE(aspy.takeFirst().at(0).toBool(),true);

        QVERIFY(ic.count()==0);
        QVERIFY(em.count()==0);
        QVERIFY(mp.count()==0);
        QVERIFY(info.count()==0);
        QVERIFY(name.count()==0);
        QVERIFY(li.count()==0);

        call->hangup();
        QTest::qWait(5000);
        QCOMPARE(hspy.count(), 1);
        QCOMPARE(hspy.takeFirst().at(0).toBool(), true);
        QCOMPARE(discreason.count(), 1);
        QCOMPARE(discreason.takeFirst().at(0).toString(), QString("local"));
        delete call;
    }

    void testQOfonoVoiceCallStep3()
    {
        //Dial failed, incoming, no answer and state change to disconnect
        QSignalSpy callsignal(m, SIGNAL(callAdded(const QString)));

        m->dial("177","");
        QTest::qWait(3000);

        QCOMPARE(callsignal.count(),1);
        QString callid = callsignal.takeFirst().at(0).toString();

        QOfonoVoiceCall* call = new QOfonoVoiceCall(this);
        call->setVoiceCallPath(callid);
        QSignalSpy state(call, SIGNAL(stateChanged(const QString)));
        QSignalSpy discreason(call,SIGNAL(disconnectReason(const QString)));

        QTest::qWait(1000);
        QCOMPARE(call->state(),QString("incoming"));
        QTest::qWait(8000);
        QVERIFY(state.count()>0);
        QCOMPARE(discreason.count(), 1);
        QCOMPARE(discreason.takeFirst().at(0).toString(), QString("remote"));
        delete call;

    }
    void testQOfonoVoiceCallStep4()
    {
        //Deflect
        QSignalSpy callsignal(m, SIGNAL(callAdded(const QString)));
        m->dial("199","");
        QTest::qWait(8000);

        QCOMPARE(callsignal.count(),1);
        QString callid = callsignal.takeFirst().at(0).toString();

        QOfonoVoiceCall* call = new QOfonoVoiceCall(this);
        call->setVoiceCallPath(callid);
        QSignalSpy dfspy(call, SIGNAL(deflectComplete(bool)));

        QTest::qWait(8000);
        QCOMPARE(call->state(),QString("incoming"));
        QCOMPARE(call->lineIdentification(),QString("1234567")); //PhoneSim specific value
        call->deflect("2345");
        QTest::qWait(8000);
        QCOMPARE(dfspy.count(), 1);
        QCOMPARE(dfspy.takeFirst().at(0).toBool(), true);
        QTest::qWait(10000);

        delete call;

    }
    void cleanupTestCase()
    {

    }


private:
    QOfonoVoiceCallManager *m;
};

QTEST_MAIN(TestQOfonoVoiceCall)
#include "tst_qofonovoicecall.moc"
