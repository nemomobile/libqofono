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

#include "../../../src/qofononetworkregistration.h"
#include "../../../src/qofonomodem.h"

#include <QtDebug>


class TestQOfonoNetworkRegistration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoNetworkRegistration(this);
        m->setModemPath("/phonesim");
	QCOMPARE(m->isValid(), true);
    }

    void testQOfonoNetworkRegistration()
    {
        QOfonoModem modem;
        modem.setModemPath(m->modemPath());
        QSignalSpy mode(m, SIGNAL(modeChanged(QString)));
        QSignalSpy status(m, SIGNAL(statusChanged(QString)));
        QSignalSpy lac(m, SIGNAL(locationAreaCodeChanged(uint)));
        QSignalSpy cellId(m, SIGNAL(cellIdChanged(uint)));
        QSignalSpy mcc(m, SIGNAL(mccChanged(QString)));
        QSignalSpy mnc(m, SIGNAL(mncChanged(QString)));
        QSignalSpy tech(m, SIGNAL(technologyChanged(QString)));
        QSignalSpy name(m, SIGNAL(nameChanged(QString)));
        QSignalSpy strength(m, SIGNAL(strengthChanged(uint)));
        QSignalSpy base(m, SIGNAL(baseStationChanged(QString)));

        QSignalSpy registerS(m, SIGNAL(registerComplete(bool)));
        QSignalSpy scan(m, SIGNAL(scanComplete(bool, QStringList)));
        QSignalSpy getOp(m, SIGNAL(getOperatorsComplete(bool, QStringList)));

        QVERIFY(m->mode().length() > 0);
        QCOMPARE(m->status(), QString("registered"));
        QCOMPARE(m->mcc(), QString("234"));
        QCOMPARE(m->mnc(), QString("01"));
        QVERIFY(m->name().length() > 0);
        QCOMPARE(m->strength(), uint(100));

        m->scan();
        QTest::qWait(5000);
        QCOMPARE(scan.count(), 1);
        QVariantList scanList = scan.takeFirst();
        QCOMPARE(scanList.at(0).toBool(), true);
        QStringList scanOpList = scanList.at(1).toStringList();
        QVERIFY(scanOpList.count() > 0);

        m->networkOperators();
        QTest::qWait(1000);
        QCOMPARE(getOp.count(), 1);
        QVariantList getList = getOp.takeFirst();
        QCOMPARE(getList.at(0).toBool(), true);
        QStringList getOpList = getList.at(1).toStringList();
        QVERIFY(getOpList.count() > 0);
        QCOMPARE(scanOpList, getOpList);

        m->registration();
        QTest::qWait(5000);
        QCOMPARE(registerS.count(), 1);
        QCOMPARE(registerS.takeFirst().at(0).toBool(), true);
        QCOMPARE(m->mode(), QString("auto"));

        modem.setOnline(false);
        QTest::qWait(5000);
        QCOMPARE(m->isValid(), false);
        modem.setOnline(true);
        QTest::qWait(5000);
        QCOMPARE(status.count(), 1);
        QCOMPARE(status.takeFirst().at(0).toString(), QString("registered"));
        QCOMPARE(mcc.count(), 1);
        QCOMPARE(mcc.takeFirst().at(0).toString(), QString("234"));
        QCOMPARE(mnc.count(), 1);
        QCOMPARE(mnc.takeFirst().at(0).toString(), QString("01"));
        QCOMPARE(name.count(), 2);
        QVERIFY(name.takeFirst().at(0).toString().length() > 0);
        QVERIFY(name.takeFirst().at(0).toString().length() > 0);
        QCOMPARE(strength.count(), 1);
        QCOMPARE(strength.takeFirst().at(0).toUInt(), uint(100));
    }


    void cleanupTestCase()
    {

    }


private:
    QOfonoNetworkRegistration *m;

};

QTEST_MAIN(TestQOfonoNetworkRegistration)
#include "tst_qofononetworkregistration.moc"
