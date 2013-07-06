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

#include "../../../src/qofonosupplementaryservices.h"

#include <QtDebug>


class TestQOfonoSupplementaryServices : public QObject
{
    Q_OBJECT

public slots:

private slots:
    void initTestCase()
    {
        m = new QOfonoSupplementaryServices( this);
        m->setModemPath("/phonesim");
        QCOMPARE(m->isValid(), true);
    }

    void testQOfonoSupplementaryServices()
    {
        QSignalSpy notification(m, SIGNAL(notificationReceived(QString)));
        QSignalSpy request(m, SIGNAL(requestReceived(QString)));

        QSignalSpy state(m, SIGNAL(stateChanged(QString)));

        QSignalSpy initiateUSSD(m, SIGNAL(initiateUSSDComplete(QString)));
        QSignalSpy barring(m, SIGNAL(barringComplete(QString, QString, QVariantMap)));
        QSignalSpy forwarding(m, SIGNAL(forwardingComplete(QString, QString, QVariantMap)));
        QSignalSpy waiting(m, SIGNAL(waitingComplete(QString, QVariantMap)));
        QSignalSpy callingLinePresentation(m, SIGNAL(callingLinePresentationComplete(QString, QString)));
        QSignalSpy connectedLinePresentation(m, SIGNAL(connectedLinePresentationComplete(QString, QString)));
        QSignalSpy callingLineRestriction(m, SIGNAL(callingLineRestrictionComplete(QString, QString)));
        QSignalSpy connectedLineRestriction(m, SIGNAL(connectedLineRestrictionComplete(QString, QString)));

        QSignalSpy initiateFailed(m, SIGNAL(initiateFailed()));
        QSignalSpy respond(m, SIGNAL(respondComplete(bool, QString)));
        QSignalSpy cancel(m, SIGNAL(cancelComplete(bool)));

        QCOMPARE(m->state(), QString("idle"));

        m->initiate("*225#");
        QTest::qWait(1000);
        QCOMPARE(state.count(), 2);
        QCOMPARE(state.takeFirst().at(0).toString(), QString("active"));
        QCOMPARE(state.takeFirst().at(0).toString(), QString("idle"));
        QCOMPARE(initiateUSSD.count(), 1);
        QCOMPARE(initiateUSSD.takeFirst().at(0).toString(), QString("Thank you, your request is being processed. A message will be sent to your phone."));

        m->cancel();
        QTest::qWait(1000);
        QCOMPARE(cancel.count(), 1);
        QCOMPARE(cancel.takeFirst().at(0).toBool(), false);

        m->respond("*225#");
        QTest::qWait(1000);
        QCOMPARE(respond.count(), 1);
        QCOMPARE(respond.takeFirst().at(0).toBool(), false);

        m->initiate("*226#");
        QTest::qWait(1000);
        QCOMPARE(state.count(), 2);
        QCOMPARE(state.takeFirst().at(0).toString(), QString("active"));
        QCOMPARE(state.takeFirst().at(0).toString(), QString("idle"));
        QCOMPARE(initiateFailed.count(), 1);
        initiateFailed.takeFirst();

        m->initiate("*#331#");
        QTest::qWait(1000);
        QCOMPARE(barring.count(), 1);
        QVariantList list = barring.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("InternationalOutgoing"));
        QVariantMap map = list.at(2).toMap();
        QCOMPARE(map.count(), 3);
        QCOMPARE(map["DataInternationalOutgoing"].toString(), QString("disabled"));

        m->initiate("*#002**11#");
        QTest::qWait(1000);
        QCOMPARE(forwarding.count(), 1);
        list = forwarding.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("All"));
        map = list.at(2).toMap();
        QCOMPARE(map.count(), 5);
        QCOMPARE(map["VoiceNoReplyTimeout"].toUInt(), uint(20));

        m->initiate("*#43#");
        QTest::qWait(5000);
        QCOMPARE(waiting.count(), 1);
        list = waiting.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        map = list.at(1).toMap();
        QCOMPARE(map.count(), 5);
        QCOMPARE(map["DataAsyncCallWaiting"].toString(), QString("disabled"));

        m->initiate("*#31#");
        QTest::qWait(1000);
        QCOMPARE(callingLineRestriction.count(), 1);
        list = callingLineRestriction.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("disabled"));

        m->initiate("*#30#");
        QTest::qWait(1000);
        QCOMPARE(callingLinePresentation.count(), 1);
        list = callingLinePresentation.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("enabled"));

        m->initiate("*#76#");
        QTest::qWait(1000);
        QCOMPARE(connectedLinePresentation.count(), 1);
        list = connectedLinePresentation.takeFirst();
        QCOMPARE(list.at(0).toString(), QString("interrogation"));
        QCOMPARE(list.at(1).toString(), QString("enabled"));

        m->initiate("*#77#");
        QTest::qWait(1000);
        QCOMPARE(connectedLineRestriction.count(), 1);
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
