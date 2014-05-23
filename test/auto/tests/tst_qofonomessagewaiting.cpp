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

#include "../../../src/qofonomessagewaiting.h"
#include "../../../src/qofonomodem.h"

#include <QtDebug>

class TestQOfonoMessageWaiting : public QObject
{
    Q_OBJECT

    // The same as the QTRY_* macros use
    static const int REASONABLE_TIMEOUT = 5000;

private slots:
    void initTestCase()
    {
        m = new QOfonoMessageWaiting(this);
        m->setModemPath("/phonesim");
        QCOMPARE(m->isValid(), true);
    }

    void testQOfonoMessageWaiting()
    {
        QOfonoModem modem;
        modem.setModemPath(m->modemPath());
        QSignalSpy modemOnline(&modem, SIGNAL(onlineChanged(bool)));
        QSignalSpy waiting(m, SIGNAL(voicemailWaitingChanged(bool)));
        QSignalSpy messageCount(m, SIGNAL(voicemailMessageCountChanged(int)));
        QSignalSpy mailboxNumber(m, SIGNAL(voicemailMailboxNumberChanged(QString)));
        QSignalSpy setNumberComplete(m, SIGNAL(voicemailMailboxComplete(bool)));

        QCOMPARE(m->voicemailWaiting(), true);
        QCOMPARE(m->voicemailMessageCount(), 1);
        QCOMPARE(m->voicemailMailboxNumber(), QString("6789"));

        modem.setOnline(false);
        QTRY_COMPARE(modemOnline.count(), 1);
        QCOMPARE(modemOnline.takeFirst().at(0).toBool(), false);

        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(waiting.count(), 0);
        QCOMPARE(messageCount.count(), 0);
        QCOMPARE(mailboxNumber.count(), 0);
        QCOMPARE(setNumberComplete.count(), 0);

        modem.setOnline(true);
        QTRY_COMPARE(modemOnline.count(), 1);
        QCOMPARE(modemOnline.takeFirst().at(0).toBool(), true);
        // FIXME: This does not seem to be correct. These properties actually
        // do not change.
        QTRY_COMPARE(waiting.count(), 1);
        QCOMPARE(waiting.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(messageCount.count(), 1);
        QCOMPARE(messageCount.takeFirst().at(0).toInt(), 1);
        QTRY_COMPARE(mailboxNumber.count(), 1);
        QCOMPARE(mailboxNumber.takeFirst().at(0).toString(), QString("6789"));
        QTRY_COMPARE(setNumberComplete.count(), 0);
    }

    void testQOfonoMessageWaitingSet()
    {
        QSignalSpy waiting(m, SIGNAL(voicemailWaitingChanged(bool)));
        QSignalSpy messageCount(m, SIGNAL(voicemailMessageCountChanged(int)));
        QSignalSpy mailboxNumber(m, SIGNAL(voicemailMailboxNumberChanged(QString)));
        QSignalSpy setNumberComplete(m, SIGNAL(voicemailMailboxComplete(bool)));

        QString number = m->voicemailMailboxNumber();

        m->setVoicemailMailboxNumber("");
        QTRY_COMPARE(setNumberComplete.count(), 1);
        QCOMPARE(setNumberComplete.takeFirst().at(0).toBool(), false);

        m->setVoicemailMailboxNumber("1234");
        QTRY_COMPARE(mailboxNumber.count(), 1);
        QCOMPARE(mailboxNumber.takeFirst().at(0).toString(), QString("1234"));
        QTRY_COMPARE(setNumberComplete.count(), 1);
        QCOMPARE(setNumberComplete.takeFirst().at(0).toBool(), true);

        m->setVoicemailMailboxNumber(number);
        QTRY_COMPARE(mailboxNumber.count(), 1);
        QCOMPARE(mailboxNumber.takeFirst().at(0).toString(), number);
        QTRY_COMPARE(setNumberComplete.count(), 1);
        QCOMPARE(setNumberComplete.takeFirst().at(0).toBool(), true);

        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(waiting.count(), 0);
        QCOMPARE(messageCount.count(), 0);
        QCOMPARE(mailboxNumber.count(), 0);
    }

    void cleanupTestCase()
    {

    }

private:
    QOfonoMessageWaiting *m;
};

QTEST_MAIN(TestQOfonoMessageWaiting)
#include "tst_qofonomessagewaiting.moc"
