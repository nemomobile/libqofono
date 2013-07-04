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

#include <ofonocallmeter.h>

#include <QtDebug>


class TestOfonoCallMeter : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
	m = new OfonoCallMeter(OfonoModem::ManualSelect, "/phonesim", this);
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

    void testOfonoCallMeter()
    {
    	QSignalSpy callMeterComplete(m, SIGNAL(callMeterComplete(bool, uint)));
    	QSignalSpy callMeterChanged(m, SIGNAL(callMeterChanged(uint)));

	m->requestCallMeter();
	QTest::qWait(1000);
	QCOMPARE(callMeterComplete.count(), 1);
	QVariantList list = callMeterComplete.takeFirst();
	QCOMPARE(list.at(0).toBool(), true);
	QCOMPARE(list.at(1).toUInt(), uint(0));
	QCOMPARE(callMeterChanged.count(), 1);
	QCOMPARE(callMeterChanged.takeFirst().at(0).toUInt(), uint(0));
	// should also test change in call meter as a result of voice call
    }


    void cleanupTestCase()
    {

    }


private:
    OfonoCallMeter *m;
};

QTEST_MAIN(TestOfonoCallMeter)
#include "test_ofonocallmeter.moc"
