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

#include "qofonocallsettings.h"

class TestQOfonoCallSettings : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoCallSettings(this);
        m->setModemPath("/phonesim");
        QSignalSpy ready(m, SIGNAL(readyChanged()));

        QTRY_VERIFY(m->isValid());
        QTRY_VERIFY(m->isReady());
        QTRY_COMPARE(ready.count(), 1);
    }

    void testQOfonoCallSettings()
    {
        QSignalSpy hideCallerIdComplete(m, SIGNAL(hideCallerIdComplete(bool)));
        QSignalSpy voiceCallWaitingComplete(m, SIGNAL(voiceCallWaitingComplete(bool)));

        QSignalSpy callingLinePresentationChanged(m, SIGNAL(callingLinePresentationChanged(QString)));
        QSignalSpy calledLinePresentationChanged(m, SIGNAL(calledLinePresentationChanged(QString)));
        QSignalSpy callingNamePresentationChanged(m, SIGNAL(callingNamePresentationChanged(QString)));
        QSignalSpy connectedLinePresentationChanged(m, SIGNAL(connectedLinePresentationChanged(QString)));
        QSignalSpy connectedLineRestrictionChanged(m, SIGNAL(connectedLineRestrictionChanged(QString)));
        QSignalSpy callingLineRestrictionChanged(m, SIGNAL(callingLineRestrictionChanged(QString)));
        QSignalSpy hideCallerIdChanged(m, SIGNAL(hideCallerIdChanged(QString)));
        QSignalSpy voiceCallWaitingChanged(m, SIGNAL(voiceCallWaitingChanged(QString)));

        QCOMPARE(m->callingLinePresentation(), QString("enabled"));
        QCOMPARE(m->calledLinePresentation(), QString("enabled"));
        QCOMPARE(m->callingNamePresentation(), QString("enabled"));
        QCOMPARE(m->connectedLinePresentation(), QString("enabled"));
        QCOMPARE(m->connectedLineRestriction(), QString("enabled"));
        QCOMPARE(m->callingLineRestriction(), QString("on"));
        QCOMPARE(m->hideCallerId(), QString("default"));
        QCOMPARE(m->voiceCallWaiting(), QString("enabled"));

        m->setHideCallerId("abc");
        QTRY_COMPARE(hideCallerIdComplete.count(), 1);
        QCOMPARE(hideCallerIdComplete.takeFirst().at(0).toBool(), false);
        m->setVoiceCallWaiting("abc");
        QTRY_COMPARE(voiceCallWaitingComplete.count(), 1);
        QCOMPARE(voiceCallWaitingComplete.takeFirst().at(0).toBool(), false);

        m->setHideCallerId("enabled");
        QTRY_COMPARE(hideCallerIdComplete.count(), 1);
        QCOMPARE(hideCallerIdComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(hideCallerIdChanged.count(), 1);
        QCOMPARE(hideCallerIdChanged.takeFirst().at(0).toString(), QString("enabled"));
        m->setHideCallerId("default");
        QTRY_COMPARE(hideCallerIdComplete.count(), 1);
        QCOMPARE(hideCallerIdComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(hideCallerIdChanged.count(), 1);
        QCOMPARE(hideCallerIdChanged.takeFirst().at(0).toString(), QString("default"));

        m->setVoiceCallWaiting("disabled");
        QTRY_COMPARE(voiceCallWaitingComplete.count(), 1);
        QCOMPARE(voiceCallWaitingComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(voiceCallWaitingChanged.count(), 1);
        QCOMPARE(voiceCallWaitingChanged.takeFirst().at(0).toString(), QString("disabled"));
        m->setVoiceCallWaiting("enabled");
        QTRY_COMPARE(voiceCallWaitingComplete.count(), 1);
        QCOMPARE(voiceCallWaitingComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(voiceCallWaitingChanged.count(), 1);
        QCOMPARE(voiceCallWaitingChanged.takeFirst().at(0).toString(), QString("enabled"));
    }

    void cleanupTestCase()
    {

    }

private:
    QOfonoCallSettings *m;
};

QTEST_MAIN(TestQOfonoCallSettings)
#include "tst_qofonocallsettings.moc"
