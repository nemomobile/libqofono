/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Alexander Kanavin <alex.kanavin@gmail.com>
 *
 * Portions of this file are Copyright (C) 2011 Intel Corporation
 * Contact: Shane Bryan <shane.bryan@linux.intel.com>
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
#include "../../../src/qofonovoicecall.h"
#include <QtDebug>

// FIXME: Not aligned with the current API
class TestOfonoMultipartyCall : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoVoiceCallManager(this);
        m->setModemPath("/phonesim");

        QCOMPARE(m->isValid(), true);
    }

    void testOfonoMultipartyCalls()
    {
        // Test for hangupMultiparty():
        // 1.  Dial a call
        // 2.  Receive a call (dial "199" to trigger phonesim callback)
        // 3.  Combine into multiparty call (2 participants)
        // 4.  Verify createMultiparty() works as expected
        // 5.  Put multiparty call on hold
        // 6.  Dial a 3rd call
        // 7.  Combine into multiparty call (3 participants)
        // 8.  Split one call off to "private chat"
        // 9.  Verify privateChat() works as expected
        // 10. Hangup multiparty call
        // 11. Verify hangupMultiparty() works as expected
        // 12. Hangup all calls

        QVariantList variantList;

        // VoiceCallManager Spy's
        QSignalSpy dialreg(m,SIGNAL(dialComplete(bool)));
        QSignalSpy dspy(m, SIGNAL(callAdded(QString)));
        QSignalSpy rspy(m, SIGNAL(callRemoved(QString)));
        QSignalSpy haspy(m, SIGNAL(hangupAllComplete(bool)));
        QSignalSpy haaspy(m, SIGNAL(holdAndAnswerComplete(bool)));
        QSignalSpy cmspy(m, SIGNAL(createMultipartyComplete(bool, QStringList)));
        QSignalSpy hmspy(m, SIGNAL(hangupMultipartyComplete(bool)));
        QSignalSpy scspy(m, SIGNAL(swapCallsComplete(bool)));
        QSignalSpy pcspy(m, SIGNAL(privateChatComplete(bool, QStringList)));

        // 1. Dial a call (outgoing)
        m->dial("123","");
        QTest::qWait(1000);

        QCOMPARE(dialreg.count(), 1);
        QCOMPARE(dialreg.takeFirst().at(0).toBool(),true);
        QCOMPARE(dspy.count(), 1);
        QString c1id = dspy.takeFirst().at(0).toString();

        QOfonoVoiceCall* c1 = new QOfonoVoiceCall();
        c1->setVoiceCallPath(c1id);

        QSignalSpy c1state(c1, SIGNAL(stateChanged(QString)));
        QSignalSpy c1discreason(c1,SIGNAL(disconnectReason(QString)));
        QSignalSpy c1li (c1, SIGNAL(lineIdentificationChanged(QString)));
        QSignalSpy c1name (c1, SIGNAL(nameChanged(QString)));
        QSignalSpy c1info (c1, SIGNAL(informationChanged(QString)));
        QSignalSpy c1mp (c1, SIGNAL(multipartyChanged(bool)));
        QSignalSpy c1em (c1, SIGNAL(emergencyChanged(bool)));
        QSignalSpy c1st (c1, SIGNAL(startTimeChanged(QString)));
        QSignalSpy c1ic (c1, SIGNAL(iconChanged(quint8)));

        qDebug() << "Please find a call in 'Dialing' state in phonesim window and press 'Active' button";
        QTest::qWait(15000);
        //    - new call should have a state change, and be in "active" state
        QVERIFY(c1state.count()>0);
        QVERIFY(c1st.count()>0);
        QVERIFY(c1ic.count()==0);
        QVERIFY(c1em.count()==0);
        QVERIFY(c1mp.count()==0);
        QVERIFY(c1info.count()==0);
        QVERIFY(c1name.count()==0);
        QVERIFY(c1li.count()==0);

        QCOMPARE(c1->state(),QString("active"));
        QCOMPARE(c1->lineIdentification(),QString("123"));
        QCOMPARE(c1->emergency(),false);
        QCOMPARE(c1->multiparty(),false);
        QCOMPARE(c1->name(),QString(""));
        QCOMPARE(c1->information(),QString(""));

        // 2. Receive a call
        //    - dial "199" to trigger phonesim callback
        m->dial("199","");
        QTest::qWait(8000);

        QCOMPARE(dialreg.count(), 1);
        QCOMPARE(dialreg.takeFirst().at(0).toBool(),false);
        QCOMPARE(dspy.count(),1);
        QString c2id = dspy.takeFirst().at(0).toString();

        QOfonoVoiceCall* c2 = new QOfonoVoiceCall();
        c2->setVoiceCallPath(c2id);
        QSignalSpy c2state(c2, SIGNAL(stateChanged(QString)));
        QSignalSpy c2time(c2,SIGNAL(startTimeChanged(QString)));
        QSignalSpy c2discreason(c2,SIGNAL(disconnectReason(QString)));

        QSignalSpy c2li (c2, SIGNAL(lineIdentificationChanged(QString)));
        QSignalSpy c2name (c2, SIGNAL(nameChanged(QString)));
        QSignalSpy c2info (c2, SIGNAL(informationChanged(QString)));
        QSignalSpy c2mp (c2, SIGNAL(multipartyChanged(bool)));
        QSignalSpy c2em (c2, SIGNAL(emergencyChanged(bool)));
        QSignalSpy c2ic (c2, SIGNAL(iconChanged(quint8)));

        QTest::qWait(3000);
        QCOMPARE(c2->state(),QString("waiting"));

        //    - accept incoming call, placing first call on hold
        m->holdAndAnswer();
        QTest::qWait(1000);

        QCOMPARE(haaspy.count(),1);
        QCOMPARE(haaspy.takeFirst().at(0).toBool(),true);

        QTest::qWait(3000);
        //    - call #1 should be in "held" state
        QVERIFY(c1state.count()>0);
        QCOMPARE(c1->state(),QString("held"));

        //    - call #2 should be in "active" state
        QVERIFY(c2state.count()>0);
        QCOMPARE(c2->state(),QString("active"));
        QCOMPARE(c2->lineIdentification(),QString("1234567"));
        QVERIFY(c2time.count()>0);

        QVERIFY(c2ic.count()==0);
        QVERIFY(c2em.count()==0);
        QVERIFY(c2mp.count()==0);
        QVERIFY(c2info.count()==0);
        QVERIFY(c2name.count()==0);
        QVERIFY(c2li.count()==0);

        // 3. Combine into multiparty call (2 participants)
        m->createMultiparty();
        QTest::qWait(1000);
        QCOMPARE(cmspy.count(),1);
        variantList = cmspy.takeFirst();
        QCOMPARE(variantList.at(0).toBool(),true);
        QVERIFY(variantList.at(1).toStringList().length() > 0);

        QTest::qWait(3000);
        // 4. Verify createMultiparty() works as expected
        //    - call #1 should have a stateChanged signal triggered
        QVERIFY(c1state.count()>0);
        //    - both calls should have a multipartyChanged signal triggered,
        //      be in "active" state and have multiparty property set to "true"
        QVERIFY(c1mp.count()>0);
        QVERIFY(c2mp.count()>0);
        QCOMPARE(c1->state(),QString("active"));
        QCOMPARE(c2->state(),QString("active"));
        QCOMPARE(c1->multiparty(),true);
        QCOMPARE(c2->multiparty(),true);

        // 5. Put multiparty call on hold
        m->swapCalls();
        QTest::qWait(1000);

        QCOMPARE(scspy.count(),1);
        QCOMPARE(scspy.takeFirst().at(0).toBool(),true);

        QTest::qWait(3000);
        //    - both calls should have a stateChanged signal triggered
        //      and be in "held" state
        QVERIFY(c1state.count()>0);
        QVERIFY(c2state.count()>0);
        QCOMPARE(c1->state(),QString("held"));
        QCOMPARE(c2->state(),QString("held"));

        // 6. Dial a 3rd call
        m->dial("456","");
        QTest::qWait(1000);

        QCOMPARE(dialreg.count(), 1);
        QCOMPARE(dialreg.takeFirst().at(0).toBool(),true);
        QCOMPARE(dspy.count(), 1);
        QString c3id = dspy.takeFirst().at(0).toString();

        QOfonoVoiceCall* c3 = new QOfonoVoiceCall();
        c3->setVoiceCallPath(c3id);

        QSignalSpy c3state(c3, SIGNAL(stateChanged(QString)));
        QSignalSpy c3discreason(c3,SIGNAL(disconnectReason(QString)));
        QSignalSpy c3li (c3, SIGNAL(lineIdentificationChanged(QString)));
        QSignalSpy c3name (c3, SIGNAL(nameChanged(QString)));
        QSignalSpy c3info (c3, SIGNAL(informationChanged(QString)));
        QSignalSpy c3mp (c3, SIGNAL(multipartyChanged(bool)));
        QSignalSpy c3em (c3, SIGNAL(emergencyChanged(bool)));
        QSignalSpy c3st (c3, SIGNAL(startTimeChanged(QString)));
        QSignalSpy c3ic (c3, SIGNAL(iconChanged(quint8)));

        qDebug() << "Please find a call in 'Dialing' state in phonesim window and press 'Active' button";
        QTest::qWait(15000);
        //    - 3rd call should have a state change, and be in "active" state
        QVERIFY(c3state.count()>0);
        QVERIFY(c3st.count()>0);
        QVERIFY(c3ic.count()==0);
        QVERIFY(c3em.count()==0);
        QVERIFY(c3mp.count()==0);
        QVERIFY(c3info.count()==0);
        QVERIFY(c3name.count()==0);
        QVERIFY(c3li.count()==0);

        QCOMPARE(c3->state(),QString("active"));
        QCOMPARE(c3->lineIdentification(),QString("456"));
        QCOMPARE(c3->emergency(),false);
        QCOMPARE(c3->multiparty(),false);
        QCOMPARE(c3->name(),QString(""));
        QCOMPARE(c3->information(),QString(""));

        // 7. Combine into multiparty call (3 participants)
        m->createMultiparty();
        QTest::qWait(1000);

        QCOMPARE(cmspy.count(),1);
        variantList = cmspy.takeFirst();
        QCOMPARE(variantList.at(0).toBool(),true);
        QVERIFY(variantList.at(1).toStringList().length() > 0);

        QTest::qWait(3000);
        //    - calls #1 and #2 should have a stateChanged signal triggered
        QVERIFY(c1state.count()>0);
        QVERIFY(c2state.count()>0);
        //    - all calls should have a multipartyChanged signal triggered,
        //      be in "active" state and have multiparty property set to "true"
        QVERIFY(c1mp.count()>0);
        QVERIFY(c2mp.count()>0);
        QVERIFY(c3mp.count()>0);
        QCOMPARE(c1->state(),QString("active"));
        QCOMPARE(c2->state(),QString("active"));
        QCOMPARE(c3->state(),QString("active"));
        QCOMPARE(c1->multiparty(),true);
        QCOMPARE(c2->multiparty(),true);
        QCOMPARE(c3->multiparty(),true);

        // 8.  Split call #1 off to "private chat"
        m->privateChat(c1id);
        QTest::qWait(1000);

        QCOMPARE(pcspy.count(),1);
        variantList = pcspy.takeFirst();
        QCOMPARE(variantList.at(0).toBool(),true);
        QVERIFY(variantList.at(1).toStringList().length() > 0);

        QTest::qWait(3000);
        // 9.  Verify privateChat() works as expected
        //    - call #1 should have a multipartyChanged signal triggered,
        //      it's multiparty property set to false and be in "active" state
        QVERIFY(c1mp.count()>0);
        QCOMPARE(c1->multiparty(),false);
        QCOMPARE(c1->state(),QString("active"));
        //    - calls #2 and #3 should have a stateChanged signal triggered
        //      and be back in "held" state
        QVERIFY(c2state.count()>0);
        QVERIFY(c3state.count()>0);
        QCOMPARE(c2->state(),QString("held"));
        QCOMPARE(c3->state(),QString("held"));


        // 10. Hangup multiparty call
        m->hangupMultiparty();
        QTest::qWait(1000);
        QCOMPARE(hmspy.count(), 1);
        QCOMPARE(hmspy.takeFirst().at(0).toBool(),true);

        QTest::qWait(3000);
        // 11. Verify hangupMultiparty() works as expected
        //    - two calls should have been removed
        QVERIFY(rspy.count()==2);
        QString r2id = rspy.takeFirst().at(0).toString();
        QString r3id = rspy.takeFirst().at(0).toString();
        QCOMPARE(r2id,c2id);
        QCOMPARE(r3id,c3id);

        //    - call #1 should still be in "active" state
        QCOMPARE(c1->state(),QString("active"));

        //    - calls #2 and #3 should have a stateChanged signal triggered
        //      and have a disconnect reason of "local"
        QVERIFY(c2state.count()>0);
        QVERIFY(c3state.count()>0);
        QCOMPARE(c2discreason.count(), 1);
        QCOMPARE(c3discreason.count(), 1);
        QCOMPARE(c2discreason.takeFirst().at(0).toString(), QString("local"));
        QCOMPARE(c3discreason.takeFirst().at(0).toString(), QString("local"));

        // 12. Hangup all calls
        m->hangupAll();
        QTest::qWait(1000);
        QCOMPARE(haspy.count(), 1);
        QCOMPARE(haspy.takeFirst().at(0).toBool(),true);

        QTest::qWait(3000);
        QCOMPARE(c1discreason.count(), 1);
        QCOMPARE(c1discreason.takeFirst().at(0).toString(), QString("local"));

        delete c1;
        delete c2;
        delete c3;
    }

    void cleanupTestCase()
    {

    }


private:
    QOfonoVoiceCallManager *m;
};

QTEST_MAIN(TestOfonoMultipartyCall)
#include "tst_qofonomultipartycall.moc"
