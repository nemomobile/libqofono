/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include  <QtCore/QObject>

#include "../../../src/qofonoconnectionmanager.h"

#include <QtDebug>


class TestOfonoConnMan : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoConnectionManager(this);
        m->setModemPath("/phonesim");

        QCOMPARE(m->isValid(), true);
    }

    void testOfonoConnMan()
    {
        QSignalSpy attch(m, SIGNAL(attachedChanged(const bool)));
        QSignalSpy sus(m,SIGNAL(suspendedChanged(const bool)));
        QSignalSpy ber(m, SIGNAL(bearerChanged(const QString&)));
        QSignalSpy roam(m,SIGNAL(roamingAllowedChanged(const bool)));
        QSignalSpy pow(m,SIGNAL(poweredChanged(const bool)));
        QSignalSpy add(m, SIGNAL(contextAdded(const QString&)));
        QSignalSpy rem(m, SIGNAL(contextRemoved(const QString&)));
        QSignalSpy cadd(m,SIGNAL(addContextComplete(bool, const QString&)));
        QSignalSpy crem(m,SIGNAL(removeContextComplete(bool)));
        QSignalSpy deact(m,SIGNAL(deactivateAllComplete(bool)));

        m->setPowered(false);
        QTest::qWait(5000);
        m->setPowered(true);
        QTest::qWait(5000);
        m->setRoamingAllowed(false);
        QTest::qWait(5000);
        m->setRoamingAllowed(true);
        QTest::qWait(5000);
        m->addContext(QString("internet"));
        QTest::qWait(10000);
        QCOMPARE(m->powered(),true);
        QCOMPARE(m->attached(),true);
        QCOMPARE(m->suspended(),false);
        QCOMPARE(m->roamingAllowed(),true);

        QCOMPARE(cadd.count(), 1);
        QCOMPARE(pow.count(), 2);
        QCOMPARE(roam.count(), 2);
        QCOMPARE(add.count(), 1);
        QString path = add.takeFirst().at(0).toString();
        QString path2 = cadd.takeFirst().at(1).toString();
        QCOMPARE(path, path2);
        m->removeContext(path);
        QTest::qWait(10000);
        QCOMPARE(rem.count(), 1);
        QCOMPARE(crem.count(), 1);
        m->deactivateAll();
        QTest::qWait(5000);
        QCOMPARE(deact.count(), 1);

    }

    void cleanupTestCase()
    {

    }


private:
    QOfonoConnectionManager *m;
};

QTEST_MAIN(TestOfonoConnMan)
#include "tst_qofonoconnman.moc"
