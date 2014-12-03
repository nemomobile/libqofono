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

#include "qofonosupplementaryservices.h"

class TestQOfonoSupplementaryServices : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoSupplementaryServices( this);
        m->setModemPath("/phonesim");
        QTRY_VERIFY(m->isValid());
    }

    void testQOfonoSupplementaryServices()
    {
        QSignalSpy notification(m, SIGNAL(notificationReceived(QString)));
        QSignalSpy request(m, SIGNAL(requestReceived(QString)));

        QSignalSpy state(m, SIGNAL(stateChanged(QString)));

        QSignalSpy ussd(m, SIGNAL(ussdResponse(QString)));
        QSignalSpy barring(m, SIGNAL(callBarringResponse(QString, QString, QVariantMap)));
        QSignalSpy forwarding(m, SIGNAL(callForwardingResponse(QString, QString, QVariantMap)));
        QSignalSpy waiting(m, SIGNAL(callWaitingResponse(QString, QVariantMap)));
        QSignalSpy callingLinePresentation(m, SIGNAL(callingLinePresentationResponse(QString, QString)));
        QSignalSpy connectedLinePresentation(m, SIGNAL(connectedLinePresentationResponse(QString, QString)));
        QSignalSpy callingLineRestriction(m, SIGNAL(callingLineRestrictionResponse(QString, QString)));
        QSignalSpy connectedLineRestriction(m, SIGNAL(connectedLineRestrictionResponse(QString, QString)));

        QSignalSpy initiateFailed(m, SIGNAL(initiateFailed()));
        QSignalSpy respond(m, SIGNAL(respondComplete(bool, QString)));
        QSignalSpy cancel(m, SIGNAL(cancelComplete(bool)));

        QCOMPARE(m->state(), QString("idle"));

        m->initiate("*225#");
        QTRY_COMPARE(state.count(), 2);
        QCOMPARE(state.takeFirst().at(0).toString(), QString("active"));
        QCOMPARE(state.takeFirst().at(0).toString(), QString("idle"));
        QTRY_COMPARE(ussd.count(), 1);
        QCOMPARE(ussd.takeFirst().at(0).toString(), QString("Thank you, your request is being processed. A message will be sent to your phone."));

        m->cancel();
        QTRY_COMPARE(cancel.count(), 1);
        QCOMPARE(cancel.takeFirst().at(0).toBool(), false);

        m->respond("*225#");
        QTRY_COMPARE(respond.count(), 1);
        QCOMPARE(respond.takeFirst().at(0).toBool(), false);

        QEXPECT_FAIL("", "'*226#' case fails", Continue);
        QVERIFY(false);
        //m->initiate("*226#");
        //QTRY_COMPARE(state.count(), 2);
        //QCOMPARE(state.takeFirst().at(0).toString(), QString("active"));
        //QCOMPARE(state.takeFirst().at(0).toString(), QString("idle"));
        //QTRY_COMPARE(initiateFailed.count(), 1);
        //initiateFailed.takeFirst();

        m->initiate("*#331#");
        QTRY_COMPARE(barring.count(), 1);
        QVariantList list = barring.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("InternationalOutgoing"));
        QVariantMap map = list.at(2).toMap();
        QCOMPARE(map.count(), 3);
        QCOMPARE(map["DataInternationalOutgoing"].toString(), QString("disabled"));

        QEXPECT_FAIL("", "'*#002**11#' case fails", Continue);
        QVERIFY(false);
        //m->initiate("*#002**11#");
        //QTRY_COMPARE(forwarding.count(), 1);
        //list = forwarding.takeFirst();
        //QCOMPARE(list.at(0).toString(), QString("interrogation"));
        //QCOMPARE(list.at(1).toString(), QString("All"));
        //map = list.at(2).toMap();
        //QCOMPARE(map.count(), 5);
        //QCOMPARE(map["VoiceNoReplyTimeout"].toUInt(), uint(20));

        m->initiate("*#43#");
        QTRY_COMPARE(waiting.count(), 1);
        list = waiting.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        map = list.at(1).toMap();
        QCOMPARE(map.count(), 5);
        QCOMPARE(map["DataAsyncCallWaiting"].toString(), QString("disabled"));

        m->initiate("*#31#");
        QTRY_COMPARE(callingLineRestriction.count(), 1);
        list = callingLineRestriction.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("disabled"));

        m->initiate("*#30#");
        QTRY_COMPARE(callingLinePresentation.count(), 1);
        list = callingLinePresentation.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("enabled"));

        m->initiate("*#76#");
        QTRY_COMPARE(connectedLinePresentation.count(), 1);
        list = connectedLinePresentation.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("enabled"));

        m->initiate("*#77#");
        QTRY_COMPARE(connectedLineRestriction.count(), 1);
        list = connectedLineRestriction.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("enabled"));
    }

    void cleanupTestCase()
    {
    }

private:
    QOfonoSupplementaryServices *m;
};

QTEST_MAIN(TestQOfonoSupplementaryServices)
#include "tst_qofonosupplementaryservices.moc"
