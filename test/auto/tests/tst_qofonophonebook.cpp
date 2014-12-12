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

#include "qofonophonebook.h"

class TestQOfonoPhonebook : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoPhonebook(this);
        m->setModemPath("/phonesim");
        QTRY_VERIFY(m->isValid());
    }

    void testQOfonoPhonebook()
    {
        QSignalSpy importingChanged(m, SIGNAL(importingChanged()));
        QSignalSpy importReady(m, SIGNAL(importReady(QString)));

        QCOMPARE(m->importing(), false);

        m->beginImport();

        QTRY_COMPARE(importingChanged.count(), 1);
        importingChanged.takeFirst();
        QCOMPARE(m->importing(), true);
        QTRY_COMPARE(importingChanged.count(), 1);
        importingChanged.takeFirst();
        QCOMPARE(m->importing(), false);
        QTRY_COMPARE(importReady.count(), 1);
        QVERIFY(!importReady.takeFirst().at(0).toString().isEmpty());
    }

    void cleanupTestCase()
    {
    }

private:
    QOfonoPhonebook *m;
};

QTEST_MAIN(TestQOfonoPhonebook)
#include "tst_qofonophonebook.moc"
