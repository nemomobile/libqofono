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

#include "qofonocallvolume.h"

class TestQOfonoCallVolume : public QObject
{
    Q_OBJECT

    // The same as the QTRY_* macros use
    static const int REASONABLE_TIMEOUT = 5000;

private slots:
    void initTestCase()
    {
        m = new QOfonoCallVolume(this);
        m->setModemPath("/phonesim");
        QTRY_VERIFY(m->isValid());
    }

    void testQOfonoCallVolume()
    {
        QSignalSpy mutedChanged(m, SIGNAL(mutedChanged(bool)));
        QSignalSpy speakerVolumeChanged(m, SIGNAL(speakerVolumeChanged(quint8)));
        QSignalSpy microphoneVolumeChanged(m, SIGNAL(microphoneVolumeChanged(quint8)));

        m->setMuted(true);
        QTRY_COMPARE(mutedChanged.count(), 1);
        QVERIFY(mutedChanged.takeFirst().at(0).toBool()==bool(true));
        QVERIFY(m->muted()==bool(true));

        m->setMuted(false);
        QTRY_COMPARE(mutedChanged.count(), 1);
        QVERIFY(mutedChanged.takeFirst().at(0).toBool()==bool(false));
        QVERIFY(m->muted()==bool(false));

        m->setSpeakerVolume(quint8(15));
        QTRY_COMPARE(speakerVolumeChanged.count(), 1);
        QCOMPARE(quint8(speakerVolumeChanged.takeFirst().at(0).toUInt()), quint8(15));
        QCOMPARE(m->speakerVolume(),quint8(15));

        m->setSpeakerVolume(quint8(250));
        m->setMicrophoneVolume(quint8(10));
        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(speakerVolumeChanged.count(), 0);
        QCOMPARE(microphoneVolumeChanged.count(), 0);
    }

    void cleanupTestCase()
    {
    }

private:
    QOfonoCallVolume *m;
};

QTEST_MAIN(TestQOfonoCallVolume)
#include "tst_qofonocallvolume.moc"
