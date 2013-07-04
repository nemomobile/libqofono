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

#include <ofonomodeminterface.h>

#include <QtDebug>

class TestOfonoModemInterface : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
	mi = new OfonoModemInterface(OfonoModem::ManualSelect, "/phonesim", "org.ofono.NetworkRegistration", OfonoGetAllOnStartup, this);
	QCOMPARE(mi->modem()->isValid(), true);
	
	if (!mi->modem()->powered()) {
  	    mi->modem()->setPowered(true);
            QTest::qWait(5000);
        }
        if (!mi->modem()->online()) {
  	    mi->modem()->setOnline(true);
            QTest::qWait(5000);
        }

    }

    void testOfonoModemInterfaceOnlineOfflineModem()
    {
        QSignalSpy validity(mi, SIGNAL(validityChanged(bool)));
        QCOMPARE(mi->isValid(), true);

        mi->modem()->setOnline(false);
        QTest::qWait(5000);
        QCOMPARE(validity.count(), 1);
        QCOMPARE(validity.takeFirst().at(0).toBool(), false);

        mi->modem()->setOnline(true);
        QTest::qWait(5000);
        QCOMPARE(validity.count(), 1);
        QCOMPARE(validity.takeFirst().at(0).toBool(), true);
    }

    void testOfonoModemInterfaceAddRemoveModem()
    {
        QSignalSpy validity(mi, SIGNAL(validityChanged(bool)));
        qDebug() << "Please stop oFono and then start it again";
    
        for (int i=0; i<30; i++) {
            if (validity.count() > 0)
                break;
            QTest::qWait(1000);
        }
        QCOMPARE(validity.count(), 1);
        QCOMPARE(validity.takeFirst().at(0).toBool(), false);
        delete mi;
	QTest::qWait(10000);
      	mi = new OfonoModemInterface(OfonoModem::ManualSelect, "/phonesim", "org.ofono.NetworkRegistration", OfonoGetAllOnStartup, this);
	if (!mi->modem()->powered()) {
  	    mi->modem()->setPowered(true);
            QTest::qWait(5000);
        }
        if (!mi->modem()->online()) {
  	    mi->modem()->setOnline(true);
            QTest::qWait(5000);
        }
        QCOMPARE(validity.count(), 0);
    }

    void cleanupTestCase()
    {

    }


private:
    OfonoModemInterface *mi;
};

QTEST_MAIN(TestOfonoModemInterface)
#include "test_ofonomodeminterface.moc"
