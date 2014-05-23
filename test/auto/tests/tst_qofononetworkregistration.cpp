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
#include "../../../src/qofonomanager.h"
#include "../../../src/qofonomodem.h"

#include <QtDebug>

class TestQOfonoNetworkRegistration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QOfonoManager manager;
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

        QSignalSpy scanFinished(m, SIGNAL(scanFinished()));
        QSignalSpy scanError(m, SIGNAL(scanError(QString)));

        QVERIFY(m->mode().length() > 0);
        QCOMPARE(m->status(), QString("registered"));
        QCOMPARE(m->mcc(), QString("234"));
        QCOMPARE(m->mnc(), QString("01"));
        QVERIFY(m->name().length() > 0);
        QCOMPARE(m->strength(), uint(100));

        m->scan();
        QTRY_COMPARE(scanFinished.count(), 1);
        scanFinished.clear();
        QVERIFY(scanError.isEmpty());
        QVERIFY(m->networkOperators().count() > 0);

        modem.setOnline(false);
        QTRY_COMPARE(modem.online(), false);
        modem.setOnline(true);
        QTRY_COMPARE(modem.online(), true);

        QEXPECT_FAIL("", "Network registration status does not change when modem goes offline",
                Abort);
        QTRY_COMPARE(status.count(), 1);
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
