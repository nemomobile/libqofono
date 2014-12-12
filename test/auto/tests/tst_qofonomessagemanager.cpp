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

#include "qofonomessagemanager.h"
#include "qofonomessage.h"

class TestQOfonoMessageManager : public QObject
{
    Q_OBJECT

    // The same as the QTRY_* macros use
    static const int REASONABLE_TIMEOUT = 5000;

private slots:
    void initTestCase()
    {
        m = new QOfonoMessageManager(this);
        m->setModemPath("/phonesim");
        QTRY_VERIFY(m->isValid());
    }

    void testQOfonoMessageManager()
    {
        QSignalSpy sendMessage(m, SIGNAL(sendMessageComplete(bool, QString)));
        QSignalSpy messageAdded(m, SIGNAL(messageAdded(QString)));
        QSignalSpy messageRemoved(m, SIGNAL(messageRemoved(QString)));
        QSignalSpy immediateMessage(m, SIGNAL(immediateMessage(QString, QVariantMap)));
        QSignalSpy incomingMessage(m, SIGNAL(incomingMessage(QString, QVariantMap)));

        QStringList messages = m->messages();
        QVERIFY(messages.count() == 0);

        m->sendMessage("99999", "success");

        QTRY_COMPARE(sendMessage.count(), 1);
        QVariantList params = sendMessage.takeFirst();
        QCOMPARE(params.at(0).toBool(), true);
        QString objectPath = params.at(1).value<QString>();
        QVERIFY(objectPath.length() > 0);
        qDebug() << objectPath;

        QTRY_COMPARE(messageAdded.count(), 1);
        QString messageId = messageAdded.takeFirst().at(0).toString();
        QCOMPARE(messageId, objectPath);
        QTRY_COMPARE(messageRemoved.count(), 1);
        QCOMPARE(messageRemoved.takeFirst().at(0).toString(), objectPath);

        QOfonoMessage* message = new QOfonoMessage();
        message->setMessagePath(messageId);
        QSignalSpy state(message, SIGNAL(stateChanged(QString)));
        qDebug() << message->state();

        m->sendMessage("abc", "fail");

        QTRY_COMPARE(sendMessage.count(), 1);
        params = sendMessage.takeFirst();
        QCOMPARE(params.at(0).toBool(), false);
        objectPath = params.at(1).value<QString>();
        QVERIFY(objectPath.length() == 0);
    }

    void testQOfonoMessageManagerSca()
    {
        QString sca = m->serviceCenterAddress();
        QVERIFY(sca.length() > 0);
        qDebug() << sca;

        QSignalSpy setScaComplete(m, SIGNAL(setServiceCenterAddressComplete(bool)));
        QSignalSpy scaChanged(m, SIGNAL(serviceCenterAddressChanged(QString)));

        m->setServiceCenterAddress("+12345678");
        QTRY_COMPARE(setScaComplete.count(), 1);
        QCOMPARE(setScaComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(scaChanged.count(), 1);
        QCOMPARE(scaChanged.takeFirst().at(0).toString(), QString("+12345678"));
        QCOMPARE(m->serviceCenterAddress(), QString("+12345678"));

        m->setServiceCenterAddress("");
        QTRY_COMPARE(setScaComplete.count(), 1);
        QCOMPARE(setScaComplete.takeFirst().at(0).toBool(), false);
        QCOMPARE(m->serviceCenterAddress(), QString("+12345678"));

        m->setServiceCenterAddress(sca);
        QTRY_COMPARE(setScaComplete.count(), 1);
        QCOMPARE(setScaComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(scaChanged.count(), 1);
        QCOMPARE(scaChanged.takeFirst().at(0).toString(), sca);
        QCOMPARE(m->serviceCenterAddress(), sca);

        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(scaChanged.count(), 0);
    }

    void testQOfonoMessageManagerUseDeliveryReports()
    {
        QSignalSpy setUseDeliveryReportsComplete(m, SIGNAL(setUseDeliveryReportsComplete(bool)));
        QSignalSpy useDeliveryReportsChanged(m, SIGNAL(useDeliveryReportsChanged(bool)));

        QCOMPARE(m->useDeliveryReports(), false);

        m->setUseDeliveryReports(true);
        QTRY_COMPARE(setUseDeliveryReportsComplete.count(), 1);
        QCOMPARE(setUseDeliveryReportsComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(useDeliveryReportsChanged.count(), 1);
        QCOMPARE(useDeliveryReportsChanged.takeFirst().at(0).toBool(), true);
        QCOMPARE(m->useDeliveryReports(), true);

        m->setUseDeliveryReports(false);
        QTRY_COMPARE(setUseDeliveryReportsComplete.count(), 1);
        QCOMPARE(setUseDeliveryReportsComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(useDeliveryReportsChanged.count(), 1);
        QCOMPARE(useDeliveryReportsChanged.takeFirst().at(0).toBool(), false);
        QCOMPARE(m->useDeliveryReports(), false);
    }

    void testQOfonoMessageManagerAlphabet()
    {
        QSignalSpy setAlphabetComplete(m, SIGNAL(setAlphabetComplete(bool)));
        QSignalSpy alphabetChanged(m, SIGNAL(alphabetChanged(QString)));

        QString alphabet = m->alphabet();
        QVERIFY(alphabet.length() > 0);
        QCOMPARE(alphabet, QString("default"));

        m->setAlphabet("spanish");
        QTRY_COMPARE(setAlphabetComplete.count(), 1);
        QCOMPARE(setAlphabetComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(alphabetChanged.count(), 1);
        QCOMPARE(alphabetChanged.takeFirst().at(0).toString(), QString("spanish"));
        QCOMPARE(m->alphabet(), QString("spanish"));

        m->setAlphabet(alphabet);
        QTRY_COMPARE(setAlphabetComplete.count(), 1);
        QCOMPARE(setAlphabetComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(alphabetChanged.count(), 1);
        QCOMPARE(alphabetChanged.takeFirst().at(0).toString(), alphabet);
        QCOMPARE(m->alphabet(), alphabet);
    }

    void testQOfonoMessageManagerBearer()
    {
        QSignalSpy setBearerComplete(m, SIGNAL(setBearerComplete(bool)));
        QSignalSpy bearerChanged(m, SIGNAL(bearerChanged(QString)));

        QString bearer = m->bearer();
        QVERIFY(bearer.length() > 0);
        QCOMPARE(bearer, QString("cs-preferred"));

        m->setBearer("ps-preferred");
        QTRY_COMPARE(setBearerComplete.count(), 1);
        QCOMPARE(setBearerComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(bearerChanged.count(), 1);
        QCOMPARE(bearerChanged.takeFirst().at(0).toString(), QString("ps-preferred"));
        QCOMPARE(m->bearer(), QString("ps-preferred"));

        m->setBearer(bearer); // change value
        QTRY_COMPARE(setBearerComplete.count(), 1);
        QCOMPARE(setBearerComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(bearerChanged.count(), 1);
        QCOMPARE(bearerChanged.takeFirst().at(0).toString(), bearer);
        QCOMPARE(m->bearer(), bearer);
    }

    void cleanupTestCase()
    {

    }

private:
    QOfonoMessageManager *m;
};

QTEST_MAIN(TestQOfonoMessageManager)
#include "tst_qofonomessagemanager.moc"
