/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2012 Jolla
 *
 * Contact: Lorn Potter <lorn.potter@jollamobile.com>
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

#include <ofonocellbroadcast.h>

#include <QtDebug>

class TestOfonoCellBroadcast : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new OfonoCellBroadcast(OfonoModem::ManualSelect, "/phonesim", this);
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
    void testOfonoCellBroadcast()
    {
        QSignalSpy cellPowered(m, SIGNAL(powerChanged(bool)));
        QSignalSpy inBroadcast(m, SIGNAL(incomingBroadcast( QString ,quint16)));
     //   QSignalSpy emBroadcast(m, SIGNAL(emergencyBroadcast( QString , QVariantMap)));
        QSignalSpy topicsSpy(m, SIGNAL(topicsChanged(QString)));

        qDebug() << m->powered() << m->topics();
        bool isPowered = m->powered();
        m->setPowered(!isPowered);
        QTest::qWait(2000);
        QCOMPARE(cellPowered.count(), 1);
        QVariantList list = cellPowered.takeFirst();
        QCOMPARE(list.at(0).toBool(), !isPowered);
        QCOMPARE(m->powered(),!isPowered);

        m->setPowered(true);

        qDebug() << "Please send CBM using phonesim";
        QTest::qWait(10000);

//        QCOMPARE(emBroadcast.count(), 1);
        QCOMPARE(inBroadcast.count(), 1);
        QVariantList list2 = cellPowered.takeFirst();
        qDebug() << list2.at(0).toString();

        QString topicsList = "20,50-51,60";
        m->setTopics("");
        m->setTopics(topicsList);
        qDebug() << m->powered() << m->topics();
        QTest::qWait(4000);
        QCOMPARE(topicsSpy.count(), 1);
    }

private:
    OfonoCellBroadcast *m;
};

QTEST_MAIN(TestOfonoCellBroadcast)
#include "test_ofonocellbroadcast.moc"
