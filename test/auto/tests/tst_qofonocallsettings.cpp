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

#include <ofonocallsettings.h>

#include <QtDebug>


class TestOfonoCallSettings : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
	m = new OfonoCallSettings(OfonoModem::ManualSelect, "/phonesim", this);
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

    void testOfonoCallSettings()
    {
        QSignalSpy callingLinePresentationComplete(m, SIGNAL(callingLinePresentationComplete(bool, QString)));
        QSignalSpy calledLinePresentationComplete(m, SIGNAL(calledLinePresentationComplete(bool, QString)));
        QSignalSpy callingNamePresentationComplete(m, SIGNAL(callingNamePresentationComplete(bool, QString)));
        QSignalSpy connectedLinePresentationComplete(m, SIGNAL(connectedLinePresentationComplete(bool, QString)));
        QSignalSpy connectedLineRestrictionComplete(m, SIGNAL(connectedLineRestrictionComplete(bool, QString)));
        QSignalSpy callingLineRestrictionComplete(m, SIGNAL(callingLineRestrictionComplete(bool, QString)));
        QSignalSpy hideCallerIdComplete(m, SIGNAL(hideCallerIdComplete(bool, QString)));
        QSignalSpy voiceCallWaitingComplete(m, SIGNAL(voiceCallWaitingComplete(bool, QString)));

        QSignalSpy callingLinePresentationChanged(m, SIGNAL(callingLinePresentationChanged(QString)));        
        QSignalSpy calledLinePresentationChanged(m, SIGNAL(calledLinePresentationChanged(QString)));        
        QSignalSpy callingNamePresentationChanged(m, SIGNAL(callingNamePresentationChanged(QString)));        
        QSignalSpy connectedLinePresentationChanged(m, SIGNAL(connectedLinePresentationChanged(QString)));
        QSignalSpy connectedLineRestrictionChanged(m, SIGNAL(connectedLineRestrictionChanged(QString)));
        QSignalSpy callingLineRestrictionChanged(m, SIGNAL(callingLineRestrictionChanged(QString)));        
        QSignalSpy hideCallerIdChanged(m, SIGNAL(hideCallerIdChanged(QString)));        
        QSignalSpy voiceCallWaitingChanged(m, SIGNAL(voiceCallWaitingChanged(QString)));        

        QSignalSpy setHideCallerIdFailed(m, SIGNAL(setHideCallerIdFailed()));
        QSignalSpy setVoiceCallWaitingFailed(m, SIGNAL(setVoiceCallWaitingFailed()));

	m->requestCallingLinePresentation();
	QTest::qWait(5000);
	QCOMPARE(callingLinePresentationComplete.count(), 1);
	QVariantList list = callingLinePresentationComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("enabled"));
	QVERIFY(callingLinePresentationChanged.count() > 0);	
	QCOMPARE(callingLinePresentationChanged.takeFirst().at(0).toString(), QString("enabled"));	
	m->requestCalledLinePresentation();
	QTest::qWait(1000);
	QCOMPARE(calledLinePresentationComplete.count(), 1);
	list = calledLinePresentationComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("enabled"));
	QVERIFY(calledLinePresentationChanged.count() > 0);	
	QCOMPARE(calledLinePresentationChanged.takeFirst().at(0).toString(), QString("enabled"));	
	m->requestCallingNamePresentation();
	QTest::qWait(5000);
	QCOMPARE(callingNamePresentationComplete.count(), 1);
	list = callingNamePresentationComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("enabled"));
	QVERIFY(callingNamePresentationChanged.count() > 0);	
	QCOMPARE(callingNamePresentationChanged.takeFirst().at(0).toString(), QString("enabled"));	
        m->requestConnectedLinePresentation();
	QTest::qWait(1000);
        QCOMPARE(connectedLinePresentationComplete.count(), 1);
        list = connectedLinePresentationComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("enabled"));
        QVERIFY(connectedLinePresentationChanged.count() > 0);
        QCOMPARE(connectedLinePresentationChanged.takeFirst().at(0).toString(), QString("enabled"));
        m->requestConnectedLineRestriction();
	QTest::qWait(1000);
        QCOMPARE(connectedLineRestrictionComplete.count(), 1);
        list = connectedLineRestrictionComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("enabled"));
        QVERIFY(connectedLineRestrictionChanged.count() > 0);
        QCOMPARE(connectedLineRestrictionChanged.takeFirst().at(0).toString(), QString("enabled"));
	m->requestCallingLineRestriction();
	QTest::qWait(1000);
	QCOMPARE(callingLineRestrictionComplete.count(), 1);
	list = callingLineRestrictionComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("on"));
	QVERIFY(callingLineRestrictionChanged.count() > 0);	
	QCOMPARE(callingLineRestrictionChanged.takeFirst().at(0).toString(), QString("on"));	
	m->requestHideCallerId();
	QTest::qWait(1000);
	QCOMPARE(hideCallerIdComplete.count(), 1);
	list = hideCallerIdComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("default"));
	QVERIFY(hideCallerIdChanged.count() > 0);	
	QCOMPARE(hideCallerIdChanged.takeFirst().at(0).toString(), QString("default"));
	m->requestVoiceCallWaiting();
	QTest::qWait(1000);
	QCOMPARE(voiceCallWaitingComplete.count(), 1);
	list = voiceCallWaitingComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toString(), QString("enabled"));
	QVERIFY(voiceCallWaitingChanged.count() > 0);	
	QCOMPARE(voiceCallWaitingChanged.takeFirst().at(0).toString(), QString("enabled"));
	
	m->setHideCallerId("abc");
	QTest::qWait(1000);
	QCOMPARE(setHideCallerIdFailed.count(), 1);
	setHideCallerIdFailed.takeFirst();
	m->setVoiceCallWaiting("abc");
	QTest::qWait(1000);
	QCOMPARE(setVoiceCallWaitingFailed.count(), 1);
	setVoiceCallWaitingFailed.takeFirst();
	
	hideCallerIdChanged.clear();
	m->setHideCallerId("enabled");
	QTest::qWait(1000);
	m->setHideCallerId("default");
	QTest::qWait(1000);
	QCOMPARE(hideCallerIdChanged.count(), 2);	
	QCOMPARE(hideCallerIdChanged.takeFirst().at(0).toString(), QString("enabled"));
	QCOMPARE(hideCallerIdChanged.takeFirst().at(0).toString(), QString("default"));
	
	voiceCallWaitingChanged.clear();	
	m->setVoiceCallWaiting("disabled");
	QTest::qWait(5000);
	m->setVoiceCallWaiting("enabled");
	QTest::qWait(5000);
	QCOMPARE(voiceCallWaitingChanged.count(), 2);	
	QCOMPARE(voiceCallWaitingChanged.takeFirst().at(0).toString(), QString("disabled"));
	QCOMPARE(voiceCallWaitingChanged.takeFirst().at(0).toString(), QString("enabled"));
    }


    void cleanupTestCase()
    {

    }


private:
    OfonoCallSettings *m;
};

QTEST_MAIN(TestOfonoCallSettings)
#include "test_ofonocallsettings.moc"
