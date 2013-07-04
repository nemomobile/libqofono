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

#include <ofonocallforwarding.h>

#include <QtDebug>


class TestOfonoCallForwarding : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
	m = new OfonoCallForwarding(OfonoModem::ManualSelect, "/phonesim", this);
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

    void testOfonoCallForwarding()
    {
        QSignalSpy voiceUnconditionalComplete(m, SIGNAL(voiceUnconditionalComplete(bool, QString)));
        QSignalSpy voiceBusyComplete(m, SIGNAL(voiceBusyComplete(bool, QString)));
        QSignalSpy voiceNoReplyComplete(m, SIGNAL(voiceNoReplyComplete(bool, QString)));
        QSignalSpy voiceNoReplyTimeoutComplete(m, SIGNAL(voiceNoReplyTimeoutComplete(bool, ushort)));
        QSignalSpy voiceNotReachableComplete(m, SIGNAL(voiceNotReachableComplete(bool, QString)));
        QSignalSpy forwardingFlagOnSimComplete(m, SIGNAL(forwardingFlagOnSimComplete(bool, bool)));

        QSignalSpy voiceUnconditionalChanged(m, SIGNAL(voiceUnconditionalChanged(QString)));        
        QSignalSpy voiceBusyChanged(m, SIGNAL(voiceBusyChanged(QString)));        
        QSignalSpy voiceNoReplyChanged(m, SIGNAL(voiceNoReplyChanged(QString)));        
        QSignalSpy voiceNoReplyTimeoutChanged(m, SIGNAL(voiceNoReplyTimeoutChanged(ushort)));        
        QSignalSpy voiceNotReachableChanged(m, SIGNAL(voiceNotReachableChanged(QString)));        
        QSignalSpy forwardingFlagOnSimChanged(m, SIGNAL(forwardingFlagOnSimChanged(bool)));

        QSignalSpy setVoiceUnconditionalFailed(m, SIGNAL(setVoiceUnconditionalFailed()));
        QSignalSpy setVoiceBusyFailed(m, SIGNAL(setVoiceBusyFailed()));
        QSignalSpy setVoiceNoReplyFailed(m, SIGNAL(setVoiceNoReplyFailed()));
        QSignalSpy setVoiceNoReplyTimeoutFailed(m, SIGNAL(setVoiceNoReplyTimeoutFailed()));
        QSignalSpy setVoiceNotReachableFailed(m, SIGNAL(setVoiceNotReachableFailed()));

	QSignalSpy disableAllComplete(m, SIGNAL(disableAllComplete(bool)));
    
	m->requestVoiceUnconditional();
	QTest::qWait(1000);
	QCOMPARE(voiceUnconditionalComplete.count(), 1);
	QVariantList list = voiceUnconditionalComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString(""));
	QCOMPARE(voiceUnconditionalChanged.count(), 1);	
	QCOMPARE(voiceUnconditionalChanged.takeFirst().at(0).toString(), QString(""));	
	m->requestVoiceBusy();
	QTest::qWait(1000);
	QCOMPARE(voiceBusyComplete.count(), 1);
	list = voiceBusyComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString(""));
	QCOMPARE(voiceBusyChanged.count(), 1);	
	QCOMPARE(voiceBusyChanged.takeFirst().at(0).toString(), QString(""));	
	m->requestVoiceNoReply();
	QTest::qWait(1000);
	QCOMPARE(voiceNoReplyComplete.count(), 1);
	list = voiceNoReplyComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString(""));
	QCOMPARE(voiceNoReplyChanged.count(), 1);	
	QCOMPARE(voiceNoReplyChanged.takeFirst().at(0).toString(), QString(""));	
	m->requestVoiceNoReplyTimeout();
	QTest::qWait(1000);
	QCOMPARE(voiceNoReplyTimeoutComplete.count(), 1);
	list = voiceNoReplyTimeoutComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toUInt(), uint(20));
	QCOMPARE(voiceNoReplyTimeoutChanged.count(), 1);	
	QCOMPARE(voiceNoReplyTimeoutChanged.takeFirst().at(0).toUInt(), uint(20));
	m->requestVoiceNotReachable();
	QTest::qWait(1000);
	QCOMPARE(voiceNotReachableComplete.count(), 1);
	list = voiceNotReachableComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString(""));
	QCOMPARE(voiceNotReachableChanged.count(), 1);	
	QCOMPARE(voiceNotReachableChanged.takeFirst().at(0).toString(), QString(""));
	m->requestForwardingFlagOnSim();
	QTest::qWait(1000);
	QCOMPARE(forwardingFlagOnSimComplete.count(), 1);
	list = forwardingFlagOnSimComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toBool(), false);
	QCOMPARE(forwardingFlagOnSimChanged.count(), 1);	
	QCOMPARE(forwardingFlagOnSimChanged.takeFirst().at(0).toBool(), false);
	
	m->setVoiceUnconditional("abc");
	QTest::qWait(1000);
	QCOMPARE(setVoiceUnconditionalFailed.count(), 1);
	setVoiceUnconditionalFailed.takeFirst();
	m->setVoiceBusy("abc");
	QTest::qWait(1000);
	QCOMPARE(setVoiceBusyFailed.count(), 1);
	setVoiceBusyFailed.takeFirst();
	m->setVoiceNoReply("abc");
	QTest::qWait(1000);
	QCOMPARE(setVoiceNoReplyFailed.count(), 1);
	setVoiceNoReplyFailed.takeFirst();
	m->setVoiceNoReplyTimeout(-30);
	QTest::qWait(1000);
	QCOMPARE(setVoiceNoReplyTimeoutFailed.count(), 1);
	setVoiceNoReplyTimeoutFailed.takeFirst();
	m->setVoiceNotReachable("abc");
	QTest::qWait(1000);
	QCOMPARE(setVoiceNotReachableFailed.count(), 1);
	setVoiceNotReachableFailed.takeFirst();

	m->setVoiceBusy("12345678");
	QTest::qWait(1000);
	m->setVoiceNoReply("12345678");
	QTest::qWait(1000);
	m->setVoiceNoReplyTimeout(30);
	QTest::qWait(1000);
	m->setVoiceNotReachable("12345678");
	QTest::qWait(1000);
	m->setVoiceUnconditional("12345678");
	QTest::qWait(1000);

	m->requestVoiceUnconditional();
	QTest::qWait(1000);
	QCOMPARE(voiceUnconditionalComplete.count(), 1);
	list = voiceUnconditionalComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("12345678"));
	QCOMPARE(voiceUnconditionalChanged.count(), 1);	
	QCOMPARE(voiceUnconditionalChanged.takeFirst().at(0).toString(), QString("12345678"));	
	m->requestVoiceBusy();
	QTest::qWait(1000);
	QCOMPARE(voiceBusyComplete.count(), 1);
	list = voiceBusyComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("12345678"));
	QCOMPARE(voiceBusyChanged.count(), 1);	
	QCOMPARE(voiceBusyChanged.takeFirst().at(0).toString(), QString("12345678"));	
	m->requestVoiceNoReply();
	QTest::qWait(1000);
	QCOMPARE(voiceNoReplyComplete.count(), 1);
	list = voiceNoReplyComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("12345678"));
	QCOMPARE(voiceNoReplyChanged.count(), 1);	
	QCOMPARE(voiceNoReplyChanged.takeFirst().at(0).toString(), QString("12345678"));	
	m->requestVoiceNoReplyTimeout();
	QTest::qWait(1000);
	QCOMPARE(voiceNoReplyTimeoutComplete.count(), 1);
	list = voiceNoReplyTimeoutComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toUInt(), uint(30));
	QCOMPARE(voiceNoReplyTimeoutChanged.count(), 1);	
	QCOMPARE(voiceNoReplyTimeoutChanged.takeFirst().at(0).toUInt(), uint(30));
	m->requestVoiceNotReachable();
	QTest::qWait(1000);
	QCOMPARE(voiceNotReachableComplete.count(), 1);
	list = voiceNotReachableComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("12345678"));
	QCOMPARE(voiceNotReachableChanged.count(), 1);	
	QCOMPARE(voiceNotReachableChanged.takeFirst().at(0).toString(), QString("12345678"));

	m->disableAll("all");
	QTest::qWait(1000);
	QCOMPARE(disableAllComplete.count(), 1);
	QCOMPARE(disableAllComplete.takeFirst().at(0).toBool(), true);
	QCOMPARE(voiceUnconditionalChanged.count(), 1);	
	QCOMPARE(voiceUnconditionalChanged.takeFirst().at(0).toString(), QString(""));	
	QCOMPARE(voiceBusyChanged.count(), 1);	
	QCOMPARE(voiceBusyChanged.takeFirst().at(0).toString(), QString(""));	
	QCOMPARE(voiceNoReplyChanged.count(), 1);	
	QCOMPARE(voiceNoReplyChanged.takeFirst().at(0).toString(), QString(""));	
	QCOMPARE(voiceNoReplyTimeoutChanged.count(), 1);	
	QCOMPARE(voiceNoReplyTimeoutChanged.takeFirst().at(0).toUInt(), uint(20));
	QCOMPARE(voiceNotReachableChanged.count(), 1);	
	QCOMPARE(voiceNotReachableChanged.takeFirst().at(0).toString(), QString(""));
    }


    void cleanupTestCase()
    {

    }


private:
    OfonoCallForwarding *m;
};

QTEST_MAIN(TestOfonoCallForwarding)
#include "test_ofonocallforwarding.moc"
