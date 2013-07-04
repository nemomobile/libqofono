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
#include <QDBusObjectPath>

#include <ofonomessagemanager.h>
#include <ofonomessage.h>

#include <QtDebug>


class TestOfonoMessageManager : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
	m = new OfonoMessageManager(OfonoModem::ManualSelect, "/phonesim", this);
	QCOMPARE(m->modem()->isValid(), true);	

	if (!m->modem()->powered()) {
  	    m->modem()->setPowered(true);
            QTest::qWait(5000);
        }
        if (!m->modem()->online()) {
  	    m->modem()->setOnline(true);
            QTest::qWait(5000);
        }
	QCOMPARE(m->isValid(), true);    
    }

    void testOfonoMessageManager()
    {
        QSignalSpy sendMessage(m, SIGNAL(sendMessageComplete(bool, QString)));
        QSignalSpy messageAdded(m, SIGNAL(messageAdded(QString)));
        QSignalSpy messageRemoved(m, SIGNAL(messageRemoved(QString)));
        QSignalSpy immediateMessage(m, SIGNAL(immediateMessage(QString, QVariantMap)));
        QSignalSpy incomingMessage(m, SIGNAL(incomingMessage(QString, QVariantMap)));


        QStringList messages = m->getMessages();
        QVERIFY(messages.count() == 0);

        m->sendMessage("99999", "success");

        QTest::qWait(1000);

        QCOMPARE(sendMessage.count(), 1);
        QVariantList params = sendMessage.takeFirst();
        QCOMPARE(params.at(0).toBool(), true);
        QString objectPath = params.at(1).value<QString>();
        QVERIFY(objectPath.length() > 0);
        qDebug() << objectPath;

        QCOMPARE(messageAdded.count(), 1);
        QCOMPARE(messageRemoved.count(), 1);

        QString messageId = messageAdded.takeFirst().at(0).toString();
        OfonoMessage* message = new OfonoMessage(messageId);
        QSignalSpy state(message, SIGNAL(stateChanged(const QString)));
        qDebug() << message->state();

        m->sendMessage("abc", "fail");

        QTest::qWait(1000);

        QCOMPARE(sendMessage.count(), 1);
        params = sendMessage.takeFirst();
        QCOMPARE(params.at(0).toBool(), false);
        objectPath = params.at(1).value<QString>();
        QVERIFY(objectPath.length() == 0);
    }

    void testOfonoMessageManagerSca()
    {
        QSignalSpy scaComplete(m, SIGNAL(serviceCenterAddressComplete(bool, QString)));
        m->requestServiceCenterAddress();
        for (int i=0; i<30; i++) {
            if (scaComplete.count() > 0)
                break;
            QTest::qWait(1000);
        }
	QCOMPARE(scaComplete.count(), 1);
	QVariantList params = scaComplete.takeFirst();
	QCOMPARE(params.at(0).toBool(), true);
	QString sca = params.at(1).toString();
        QVERIFY(sca.length() > 0);
        qDebug() << sca;

        QSignalSpy setScaFailed(m, SIGNAL(setServiceCenterAddressFailed()));
        QSignalSpy scaChanged(m, SIGNAL(serviceCenterAddressChanged(QString)));

        m->setServiceCenterAddress("+12345678");
        for (int i=0; i<30; i++) {
            if (setScaFailed.count() > 0 || scaChanged.count() > 0)
                break;
            QTest::qWait(1000);
        }
	QCOMPARE(setScaFailed.count(), 0);
	QCOMPARE(scaChanged.count(), 1);
	QCOMPARE(scaChanged.takeFirst().at(0).toString(), QString("+12345678"));

        m->setServiceCenterAddress("");
        for (int i=0; i<30; i++) {
            if (setScaFailed.count() > 0 || scaChanged.count() > 0)
                break;
            QTest::qWait(1000);
        }
	QCOMPARE(setScaFailed.count(), 1);
	setScaFailed.takeFirst();
	QCOMPARE(scaChanged.count(), 0);
	QCOMPARE(m->errorName(), QString("org.ofono.Error.InvalidFormat"));
	QCOMPARE(m->errorMessage(), QString("Argument format is not recognized"));
	
	m->setServiceCenterAddress(sca);
        for (int i=0; i<30; i++) {
            if (setScaFailed.count() > 0 || scaChanged.count() > 0)
                break;
            QTest::qWait(1000);
        }
	QCOMPARE(setScaFailed.count(), 0);
	QCOMPARE(scaChanged.count(), 1);
	QCOMPARE(scaChanged.takeFirst().at(0).toString(), sca);
    }

    void testOfonoMessageManagerUseDeliveryReports()
    {
        QSignalSpy useDeliveryReportsComplete(m, SIGNAL(useDeliveryReportsComplete(bool, bool)));
        QSignalSpy setUseDeliveryReportsFailed(m, SIGNAL(setUseDeliveryReportsFailed()));
        QSignalSpy useDeliveryReportsChanged(m, SIGNAL(useDeliveryReportsChanged(bool)));

        m->requestUseDeliveryReports();
        for (int i=0; i<30; i++) {
            if (useDeliveryReportsComplete.count() > 0)
                break;
            QTest::qWait(1000);
        }
        QCOMPARE(useDeliveryReportsComplete.count(), 1);
        QVariantList params = useDeliveryReportsComplete.takeFirst();
        QCOMPARE(params.at(0).toBool(), true);
        QCOMPARE(params.at(1).toBool(), false);

        m->setUseDeliveryReports(true);
        for (int i=0; i<30; i++) {
            if (setUseDeliveryReportsFailed.count() > 0 || useDeliveryReportsChanged.count() > 0)
                break;
            QTest::qWait(1000);
        }
        QCOMPARE(setUseDeliveryReportsFailed.count(), 0);
        QCOMPARE(useDeliveryReportsChanged.count(), 1);
        QCOMPARE(useDeliveryReportsChanged.takeFirst().at(0).toBool(), true);

         m->setUseDeliveryReports(false);
         for (int i=0; i<30; i++) {
             if (setUseDeliveryReportsFailed.count() > 0 || useDeliveryReportsChanged.count() > 0)
                 break;
             QTest::qWait(1000);
         }
         QCOMPARE(setUseDeliveryReportsFailed.count(), 0);
         QCOMPARE(useDeliveryReportsChanged.count(), 1);
         QCOMPARE(useDeliveryReportsChanged.takeFirst().at(0).toBool(), false);
    }

    void testOfonoMessageManagerAlphabet()
    {
        QSignalSpy alphabetComplete(m, SIGNAL(alphabetComplete(bool, QString)));
        QSignalSpy setAlphabetFailed(m, SIGNAL(setAlphabetFailed()));
        QSignalSpy alphabetChanged(m, SIGNAL(alphabetChanged(QString)));

        m->requestAlphabet();
        for (int i=0; i<30; i++) {
            if (alphabetComplete.count() > 0)
                break;
            QTest::qWait(1000);
        }

        QCOMPARE(alphabetComplete.count(), 1);
        QVariantList params = alphabetComplete.takeFirst();
        QCOMPARE(params.at(0).toBool(), true);
        QString alphabet = params.at(1).toString();
        QVERIFY(alphabet.length() > 0);
        QCOMPARE(alphabet, QString("default"));
        qDebug() << alphabet;

        m->setAlphabet("spanish");
        for (int i=0; i<30; i++) {
            if (setAlphabetFailed.count() > 0 || alphabetChanged.count() > 0)
                break;
            QTest::qWait(1000);
        }

        QCOMPARE(setAlphabetFailed.count(), 0);
        QCOMPARE(alphabetChanged.count(), 1);
        QCOMPARE(alphabetChanged.takeFirst().at(0).toString(), QString("spanish"));

        m->setAlphabet(alphabet);
        for (int i=0; i<30; i++) {
            if (setAlphabetFailed.count() > 0 || alphabetChanged.count() > 0)
                break;
            QTest::qWait(1000);
        }

        QCOMPARE(setAlphabetFailed.count(), 0);
        QCOMPARE(alphabetChanged.count(), 1);
        QCOMPARE(alphabetChanged.takeFirst().at(0).toString(), alphabet);
    }

    void testOfonoMessageManagerBearer()
    {
        QSignalSpy bearerComplete(m, SIGNAL(bearerComplete(bool, QString)));
        QSignalSpy setBearerFailed(m, SIGNAL(setBearerFailed()));
        QSignalSpy bearerChanged(m, SIGNAL(bearerChanged(QString)));

        m->requestBearer();
        for (int i=0; i<30; i++) {
            if (bearerComplete.count() > 0)
                break;
            QTest::qWait(1000);
        }

        QCOMPARE(bearerComplete.count(), 1);
        QVariantList params = bearerComplete.takeFirst();
        QCOMPARE(params.at(0).toBool(), true);
        QString bearer = params.at(1).toString();
        QVERIFY(bearer.length() > 0);
        QCOMPARE(bearer, QString("cs-preferred"));
        qDebug() << bearer;

        m->setBearer("ps-preferred");
        for (int i=0; i<30; i++) {
            if (setBearerFailed.count() > 0 || bearerChanged.count() > 0)
                break;
            QTest::qWait(1000);
        }

        QCOMPARE(setBearerFailed.count(), 0);
        QCOMPARE(bearerChanged.count(), 1);
        QCOMPARE(bearerChanged.takeFirst().at(0).toString(), QString("ps-preferred"));

        m->setBearer(bearer); // change value
        for (int i=0; i<30; i++) {
            if (setBearerFailed.count() > 0 || bearerChanged.count() > 0)
                break;
            QTest::qWait(1000);
        }

        QCOMPARE(setBearerFailed.count(), 0);
        QCOMPARE(bearerChanged.count(), 1);
        QCOMPARE(bearerChanged.takeFirst().at(0).toString(), bearer);        
    }

    void cleanupTestCase()
    {

    }


private:
    OfonoMessageManager *m;
};

QTEST_MAIN(TestOfonoMessageManager)
#include "test_ofonomessagemanager.moc"
