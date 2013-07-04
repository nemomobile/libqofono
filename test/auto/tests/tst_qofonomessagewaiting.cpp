/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ofonomessagewaiting.h>

#include <QtDebug>

class TestOfonoMessageWaiting : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
	m = new OfonoMessageWaiting(OfonoModem::ManualSelect, "/phonesim", this);
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

    void testOfonoMessageWaiting()
    {
        QSignalSpy waiting(m, SIGNAL(voicemailWaitingChanged(bool)));    
        QSignalSpy messageCount(m, SIGNAL(voicemailMessageCountChanged(int)));
        QSignalSpy mailboxNumber(m, SIGNAL(voicemailMailboxNumberChanged(QString)));
        QSignalSpy setNumberFailed(m, SIGNAL(setVoicemailMailboxNumberFailed()));

	QCOMPARE(m->voicemailWaiting(), true);
	QCOMPARE(m->voicemailMessageCount(), 1);
	QCOMPARE(m->voicemailMailboxNumber(), QString("6789"));

	m->modem()->setOnline(false);
	QTest::qWait(5000);
        QCOMPARE(waiting.count(), 0);
        QCOMPARE(messageCount.count(), 0);
        QCOMPARE(mailboxNumber.count(), 0);
        QCOMPARE(setNumberFailed.count(), 0);

	m->modem()->setOnline(true);
	QTest::qWait(5000);
        QCOMPARE(waiting.count(), 1);
        QCOMPARE(waiting.takeFirst().at(0).toBool(), true);
        QCOMPARE(messageCount.count(), 1);
        QCOMPARE(messageCount.takeFirst().at(0).toInt(), 1);
        QCOMPARE(mailboxNumber.count(), 1);
        QCOMPARE(mailboxNumber.takeFirst().at(0).toString(), QString("6789"));
        QCOMPARE(setNumberFailed.count(), 0);
    }

    void testOfonoMessageWaitingSet()
    {
        QSignalSpy waiting(m, SIGNAL(voicemailWaitingChanged(bool)));    
        QSignalSpy messageCount(m, SIGNAL(voicemailMessageCountChanged(int)));
        QSignalSpy mailboxNumber(m, SIGNAL(voicemailMailboxNumberChanged(QString)));
        QSignalSpy setNumberFailed(m, SIGNAL(setVoicemailMailboxNumberFailed()));

	QString number = m->voicemailMailboxNumber();
	
        m->setVoicemailMailboxNumber("");
	QTest::qWait(1000);
        QCOMPARE(waiting.count(), 0);
        QCOMPARE(messageCount.count(), 0);
        QCOMPARE(mailboxNumber.count(), 0);
        QCOMPARE(setNumberFailed.count(), 1);
        setNumberFailed.takeFirst();

        m->setVoicemailMailboxNumber("1234");
	QTest::qWait(1000);
        QCOMPARE(waiting.count(), 0);
        QCOMPARE(messageCount.count(), 0);
        QCOMPARE(mailboxNumber.count(), 1);
        QCOMPARE(mailboxNumber.takeFirst().at(0).toString(), QString("1234"));
        QCOMPARE(setNumberFailed.count(), 0);

        m->setVoicemailMailboxNumber(number);
	QTest::qWait(1000);
        QCOMPARE(waiting.count(), 0);
        QCOMPARE(messageCount.count(), 0);
        QCOMPARE(mailboxNumber.count(), 1);
        QCOMPARE(mailboxNumber.takeFirst().at(0).toString(), number);
        QCOMPARE(setNumberFailed.count(), 0);
    }


    void cleanupTestCase()
    {

    }


private:
    OfonoMessageWaiting *m;
};

QTEST_MAIN(TestOfonoMessageWaiting)
#include "test_ofonomessagewaiting.moc"
