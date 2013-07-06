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
    }

    void testQOfonoCallBarring()
    {
        QSignalSpy voiceIncomingComplete(m, SIGNAL(voiceIncomingComplete(bool, QString)));
        QSignalSpy voiceOutgoingComplete(m, SIGNAL(voiceOutgoingComplete(bool, QString)));
        QSignalSpy voiceIncomingChanged(m, SIGNAL(voiceIncomingChanged(QString)));
        QSignalSpy voiceOutgoingChanged(m, SIGNAL(voiceOutgoingChanged(QString)));
        QSignalSpy setVoiceIncomingFailed(m, SIGNAL(setVoiceIncomingFailed()));
        QSignalSpy setVoiceOutgoingFailed(m, SIGNAL(setVoiceOutgoingFailed()));

        QSignalSpy changePasswordComplete(m, SIGNAL(changePasswordComplete(bool)));
        QSignalSpy disableAllComplete(m, SIGNAL(disableAllComplete(bool)));
        QSignalSpy disableAllIncomingComplete(m, SIGNAL(disableAllIncomingComplete(bool)));
        QSignalSpy disableAllOutgoingComplete(m, SIGNAL(disableAllOutgoingComplete(bool)));
        
        m->voiceIncoming();
        QTest::qWait(1000);
        QCOMPARE(voiceIncomingComplete.count(), 1);
        QVariantList list = voiceIncomingComplete.takeFirst();
        QCOMPARE(list.at(0).toBool(), true);
        QCOMPARE(list.at(1).toString(), QString("disabled"));
        QCOMPARE(voiceIncomingChanged.count(), 1);
        QCOMPARE(voiceIncomingChanged.takeFirst().at(0).toString(), QString("disabled"));
        m->voiceOutgoing();
        QTest::qWait(1000);
        QCOMPARE(voiceOutgoingComplete.count(), 1);
        list = voiceOutgoingComplete.takeFirst();
        QCOMPARE(list.at(0).toBool(), true);
        QCOMPARE(list.at(1).toString(), QString("disabled"));
        QCOMPARE(voiceOutgoingChanged.count(), 1);
        QCOMPARE(voiceOutgoingChanged.takeFirst().at(0).toString(), QString("disabled"));

        m->setVoiceIncoming("always", "0000");
        QTest::qWait(1000);
        QCOMPARE(setVoiceIncomingFailed.count(), 1);
        setVoiceIncomingFailed.takeFirst();
        m->setVoiceOutgoing("always", "0000");
        QTest::qWait(1000);
        QCOMPARE(setVoiceOutgoingFailed.count(), 1);
        setVoiceOutgoingFailed.takeFirst();

        m->setVoiceIncoming("always", "3579");
        QTest::qWait(1000);
        QCOMPARE(voiceIncomingChanged.count(), 1);
        QCOMPARE(voiceIncomingChanged.takeFirst().at(0).toString(), QString("always"));
        m->setVoiceOutgoing("international", "3579");
        QTest::qWait(1000);
        QCOMPARE(voiceOutgoingChanged.count(), 1);
        QCOMPARE(voiceOutgoingChanged.takeFirst().at(0).toString(), QString("international"));

//        m->disableAllIncoming("3579");
//        QTest::qWait(1000);
//        QCOMPARE(disableAllIncomingComplete.count(), 1);
//        QCOMPARE(disableAllIncomingComplete.takeFirst().at(0).toBool(), false);
//        QCOMPARE(m->errorName(), QString("org.ofono.Error.Failed"));
//        QCOMPARE(m->errorMessage(), QString("Operation failed"));
//        m->disableAllOutgoing("3579");
//        QTest::qWait(1000);
//        QCOMPARE(disableAllOutgoingComplete.count(), 1);
//        QCOMPARE(disableAllOutgoingComplete.takeFirst().at(0).toBool(), false);
//        QCOMPARE(m->errorName(), QString("org.ofono.Error.Failed"));
//        QCOMPARE(m->errorMessage(), QString("Operation failed"));
//        m->disableAll("3579");
//        QTest::qWait(1000);
//        QCOMPARE(disableAllComplete.count(), 1);
//        QCOMPARE(disableAllComplete.takeFirst().at(0).toBool(), true);
//        QCOMPARE(voiceIncomingChanged.count(), 1);
//        QCOMPARE(voiceIncomingChanged.takeFirst().at(0).toString(), QString("disabled"));
//        QCOMPARE(voiceOutgoingChanged.count(), 1);
//        QCOMPARE(voiceOutgoingChanged.takeFirst().at(0).toString(), QString("disabled"));

        m->changePassword("3579", "1234");
        QTest::qWait(1000);
        m->changePassword("1234", "3579");
        QTest::qWait(1000);
        QCOMPARE(changePasswordComplete.count(), 2);
        QCOMPARE(changePasswordComplete.takeFirst().at(0).toBool(), true);
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
