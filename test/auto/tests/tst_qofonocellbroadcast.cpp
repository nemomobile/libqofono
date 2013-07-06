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

#include "../../../src/qofonocellbroadcast.h"
#include "../../../src/qofonomodem.h"

#include <QtDebug>

class TestQOfonoCellBroadcast : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoCellBroadcast( this);
        m->setModemPath("/phonesim");

        QCOMPARE(m->isValid(), true);
    }
    void testQOfonoCellBroadcast()
    {
        QSignalSpy cellPowered(m, SIGNAL(powerChanged(bool)));
        QSignalSpy inBroadcast(m, SIGNAL(incomingBroadcast( QString ,quint16)));
     //   QSignalSpy emBroadcast(m, SIGNAL(emergencyBroadcast( QString , QVariantMap)));
        QSignalSpy topicsSpy(m, SIGNAL(topicsChanged(QString)));

        QOfonoModem modem;
        modem.setModemPath(m->modemPath());
        qDebug() << modem.powered() << m->topics();
        bool isPowered = modem.powered();
        modem.setPowered(!isPowered);
        QTest::qWait(2000);
        QCOMPARE(cellPowered.count(), 1);
        QVariantList list = cellPowered.takeFirst();
        QCOMPARE(list.at(0).toBool(), !isPowered);
        QCOMPARE(modem.powered(),!isPowered);

        modem.setPowered(true);

        qDebug() << "Please send CBM using phonesim";
        QTest::qWait(10000);

//        QCOMPARE(emBroadcast.count(), 1);
        QCOMPARE(inBroadcast.count(), 1);
        QVariantList list2 = cellPowered.takeFirst();
        qDebug() << list2.at(0).toString();

        QString topicsList = "20,50-51,60";
        m->setTopics("");
        m->setTopics(topicsList);
        qDebug() << modem.powered() << m->topics();
        QTest::qWait(4000);
        QCOMPARE(topicsSpy.count(), 1);
    }

private:
    QOfonoCellBroadcast *m;
};

QTEST_MAIN(TestQOfonoCellBroadcast)
#include "tst_qofonocellbroadcast.moc"
