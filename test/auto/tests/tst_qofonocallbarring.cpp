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

#include "../../../src/qofonocallbarring.h"

#include <QtDebug>

class TestQOfonoCallBarring : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoCallBarring(this);
        m->setModemPath("/phonesim");
        QCOMPARE(m->isValid(), true);

        QSignalSpy ready(m, SIGNAL(readyChanged()));
        QTRY_COMPARE(ready.count(), 1);
        QCOMPARE(m->isReady(), true);
    }

    void testQOfonoCallBarring()
    {
        QSignalSpy voiceIncomingComplete(m, SIGNAL(voiceIncomingComplete(bool)));
        QSignalSpy voiceOutgoingComplete(m, SIGNAL(voiceOutgoingComplete(bool)));
        QSignalSpy voiceIncomingChanged(m, SIGNAL(voiceIncomingChanged(QString)));
        QSignalSpy voiceOutgoingChanged(m, SIGNAL(voiceOutgoingChanged(QString)));

        QSignalSpy changePasswordComplete(m, SIGNAL(changePasswordComplete(bool)));
        //QSignalSpy disableAllComplete(m, SIGNAL(disableAllComplete(bool)));
        //QSignalSpy disableAllIncomingComplete(m, SIGNAL(disableAllIncomingComplete(bool)));
        //QSignalSpy disableAllOutgoingComplete(m, SIGNAL(disableAllOutgoingComplete(bool)));

        QCOMPARE(m->voiceIncoming(), QString("disabled"));
        QCOMPARE(m->voiceOutgoing(), QString("disabled"));

        voiceIncomingChanged.clear();
        voiceOutgoingChanged.clear();

        m->setVoiceIncoming("always", "0000");
        QTRY_COMPARE(voiceIncomingComplete.count(), 1);
        QCOMPARE(voiceIncomingComplete.takeFirst().at(0).toBool(), false);
        QCOMPARE(voiceIncomingChanged.count(), 0);
        m->setVoiceOutgoing("always", "0000");
        QTRY_COMPARE(voiceOutgoingComplete.count(), 1);
        QCOMPARE(voiceOutgoingComplete.takeFirst().at(0).toBool(), false);
        QCOMPARE(voiceOutgoingChanged.count(), 0);

        m->setVoiceIncoming("always", "3579");
        QTRY_COMPARE(voiceIncomingComplete.count(), 1);
        QCOMPARE(voiceIncomingComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(voiceIncomingChanged.count(), 1);
        QCOMPARE(voiceIncomingChanged.takeFirst().at(0).toString(), QString("always"));
        m->setVoiceOutgoing("international", "3579");
        QTRY_COMPARE(voiceOutgoingComplete.count(), 1);
        QCOMPARE(voiceOutgoingComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(voiceOutgoingChanged.count(), 1);
        QCOMPARE(voiceOutgoingChanged.takeFirst().at(0).toString(), QString("international"));

        // QOfonoCallBarring API incomplete
        //m->disableAllIncoming("3579");
        //QTRY_COMPARE(disableAllIncomingComplete.count(), 1);
        //QCOMPARE(disableAllIncomingComplete.takeFirst().at(0).toBool(), false);
        //QCOMPARE(m->errorName(), QString("org.ofono.Error.Failed"));
        //QCOMPARE(m->errorMessage(), QString("Operation failed"));
        //m->disableAllOutgoing("3579");
        //QTRY_COMPARE(disableAllOutgoingComplete.count(), 1);
        //QCOMPARE(disableAllOutgoingComplete.takeFirst().at(0).toBool(), false);
        //QCOMPARE(m->errorName(), QString("org.ofono.Error.Failed"));
        //QCOMPARE(m->errorMessage(), QString("Operation failed"));
        m->disableAll("3579");
        //QTRY_COMPARE(disableAllComplete.count(), 1);
        //QCOMPARE(disableAllComplete.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(voiceIncomingChanged.count(), 1);
        QCOMPARE(voiceIncomingChanged.takeFirst().at(0).toString(), QString("disabled"));
        QTRY_COMPARE(voiceOutgoingChanged.count(), 1);
        QCOMPARE(voiceOutgoingChanged.takeFirst().at(0).toString(), QString("disabled"));

        m->changePassword("3579", "1234");
        QTRY_COMPARE(changePasswordComplete.count(), 1);
        QCOMPARE(changePasswordComplete.takeFirst().at(0).toBool(), true);
        m->changePassword("1234", "3579");
        QTRY_COMPARE(changePasswordComplete.count(), 1);
        QCOMPARE(changePasswordComplete.takeFirst().at(0).toBool(), true);
    }

    void cleanupTestCase()
    {

    }

private:
    QOfonoCallBarring *m;
};

QTEST_MAIN(TestQOfonoCallBarring)
#include "tst_qofonocallbarring.moc"
