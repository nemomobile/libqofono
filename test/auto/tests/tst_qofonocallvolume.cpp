/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Alexander Kanavin <alex.kanavin@gmail.com>
 * Author: Arun Ravindran <ext-arun.1.ravindran@nokia.com>
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

#include "../../../src/qofonocallvolume.h"
#include "../../../src/qofonomodem.h"

#include <QtDebug>

class TestQOfonoCallVolume : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {

        m = new QOfonoCallVolume(this);
        m->setModemPath("/phonesim");
        QCOMPARE(m->isValid(), true);
    }

    void testQOfonoCallVolume()
    {

        QSignalSpy mutedChanged(m, SIGNAL(mutedChanged(const bool)));
        QSignalSpy speakerVolumeChanged(m, SIGNAL(speakerVolumeChanged(const quint8)));
        QSignalSpy microphoneVolumeChanged(m, SIGNAL(microphoneVolumeChanged(const quint8)));
        QSignalSpy spfail(m, SIGNAL(setSpeakerVolumeFailed()));
        QSignalSpy mvfail(m, SIGNAL(setMicrophoneVolumeFailed()));

        QOfonoModem modem;
        modem.setModemPath(m->modemPath());
        modem.setPowered(false);
        QTest::qWait(5000);
        modem.setPowered(true);
        QTest::qWait(5000);
        modem.setOnline(true);
        QTest::qWait(5000);

        m->setMuted(true);
        QTest::qWait(2000);
        QCOMPARE(mutedChanged.count(), 1);
        QVERIFY(mutedChanged.takeFirst().at(0).toBool()==bool(true));
        QVERIFY(m->muted()==bool(true));

        m->setMuted(false);
        QTest::qWait(2000);
        QCOMPARE(mutedChanged.count(), 1);
        QVERIFY(mutedChanged.takeFirst().at(0).toBool()==bool(false));
        QVERIFY(m->muted()==bool(false));


        m->setSpeakerVolume(quint8(15));
        QTest::qWait(2000);
        QCOMPARE(speakerVolumeChanged.count(), 1);
        QCOMPARE(quint8(speakerVolumeChanged.takeFirst().at(0).toUInt()), quint8(15));
        QCOMPARE(m->speakerVolume(),quint8(15));

        m->setSpeakerVolume(quint8(250));
        QTest::qWait(2000);
        QCOMPARE(spfail.count(), 1);

        m->setMicrophoneVolume(quint8(10));
        QTest::qWait(2000);
        QCOMPARE(mvfail.count(), 1);

    }
    void cleanupTestCase()
    {

    }


private:
    QOfonoCallVolume *m;
};

QTEST_MAIN(TestQOfonoCallVolume)
#include "tst_qofonocallvolume.moc"
