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

#include "../../../src/qofonosimmanager.h"
#include "../../../src/qofonomodem.h"

#include <QtDebug>
#include <QVariant>


class TestQOfonoSimManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoSimManager(this);
        m->setModemPath("/phonesim");

        QCOMPARE(m->isValid(), true);
    }

    void testQOfonoSimManager()
    {
        QSignalSpy presence(m, SIGNAL(presenceChanged(bool)));
        QSignalSpy subscriberIdentity(m, SIGNAL(subscriberIdentityChanged(QString)));
        QSignalSpy mcc(m, SIGNAL(mobileCountryCodeChanged(QString)));
        QSignalSpy mnc(m, SIGNAL(mobileNetworkCodeChanged(QString)));
        QSignalSpy subscriberNumbers(m, SIGNAL(subscriberNumbersChanged(QStringList)));
        QSignalSpy serviceNumbers(m, SIGNAL(serviceNumbersChanged(OfonoServiceNumbers)));
        QSignalSpy pinRequired(m, SIGNAL(pinRequiredChanged(QString)));
        QSignalSpy lockedPins(m, SIGNAL(lockedPinsChanged(QStringList)));
        QSignalSpy cardIdentifier(m, SIGNAL(cardIdentifierChanged(QString)));
        QSignalSpy preferredLanguages(m, SIGNAL(preferredLanguagesChanged(QStringList)));
        QSignalSpy fixedDialing(m, SIGNAL(fixedDialingChanged(bool)));
        QSignalSpy barredDialing(m, SIGNAL(barredDialingChanged(bool)));

        QSignalSpy setSubscriberNumbersFailed(m, SIGNAL(setSubscriberNumbersFailed()));

        QSignalSpy changePin(m, SIGNAL(changePinComplete(bool)));
        QSignalSpy enterPin(m, SIGNAL(enterPinComplete(bool)));
        QSignalSpy resetPin(m, SIGNAL(resetPinComplete(bool)));
        QSignalSpy lockPin(m, SIGNAL(lockPinComplete(bool)));
        QSignalSpy unlockPin(m, SIGNAL(unlockPinComplete(bool)));

        QCOMPARE(m->present(), true);
        QCOMPARE(m->subscriberIdentity(), QString("246813579"));
        QCOMPARE(m->mobileCountryCode(), QString("246"));
        QCOMPARE(m->mobileNetworkCode(), QString("81"));
        QVERIFY(m->subscriberNumbers().count() > 0);
        QCOMPARE(m->subscriberNumbers()[0], QString("358501234567"));
        QVERIFY(m->serviceNumbers().count() > 0);
        QCOMPARE(m->serviceNumbers().value(".HELP DESK").toString(), QString("2601"));
        QCOMPARE(m->pinRequired(),QOfonoSimManager::NoPin);
        QCOMPARE(m->lockedPins().count(), 0);
        QCOMPARE(m->cardIdentifier(), QString("8949222074451242066"));
        QVERIFY(m->preferredLanguages().count() > 0);
        QCOMPARE(m->preferredLanguages()[0], QString("de"));
        QCOMPARE(m->pinRetries().count(), 0);
        QCOMPARE(m->fixedDialing(), false);
        QCOMPARE(m->barredDialing(), false);

        QStringList numbers = m->subscriberNumbers();
        QStringList newNumbers;
        newNumbers << "1234567";
        m->setSubscriberNumbers(newNumbers);
        QTest::qWait(1000);
        QCOMPARE(subscriberNumbers.count(), 1);
        QStringList newNumbersSignal = subscriberNumbers.takeFirst().at(0).toStringList();
        QCOMPARE(newNumbersSignal.count(), 1);
        QCOMPARE(newNumbersSignal, newNumbers);

        newNumbers.clear();
        newNumbers << "";
        m->setSubscriberNumbers(newNumbers);
        QTest::qWait(1000);
        QCOMPARE(setSubscriberNumbersFailed.count(), 1);
        setSubscriberNumbersFailed.takeFirst();

        m->setSubscriberNumbers(numbers);
        QTest::qWait(1000);
        QCOMPARE(subscriberNumbers.count(), 1);
        newNumbersSignal = subscriberNumbers.takeFirst().at(0).toStringList();
        QCOMPARE(newNumbersSignal.count(), 1);
        QCOMPARE(newNumbersSignal, numbers);

        QOfonoModem modem;
        modem.setModemPath(m->modemPath());
        //SIM hotswap is not testable with phonesim, so we simulate a modem reset
        modem.setOnline(false);
        QTest::qWait(5000);
        modem.setPowered(false);
        QTest::qWait(5000);
        modem.setPowered(true);
        QTest::qWait(5000);
        modem.setOnline(true);
        QTest::qWait(5000);
        QCOMPARE(presence.count(), 0);
        QCOMPARE(subscriberIdentity.count(), 1);
        QCOMPARE(subscriberIdentity.takeFirst().at(0).toString(), QString("246813579"));
        QCOMPARE(mcc.count(), 1);
        QCOMPARE(mcc.takeFirst().at(0).toString(), QString("246"));
        QCOMPARE(mnc.count(), 1);
        QCOMPARE(mnc.takeFirst().at(0).toString(), QString("81"));
        QCOMPARE(subscriberNumbers.count(), 1);
        numbers = subscriberNumbers.takeFirst().at(0).toStringList();
        QCOMPARE(numbers.count(), 1);
        QCOMPARE(numbers[0], QString("358501234567"));
        QCOMPARE(serviceNumbers.count(), 1);

        QVariantMap serviceNumbersMap = serviceNumbers.takeFirst().at(0).toMap();
        QVERIFY(serviceNumbersMap.count() > 0);
        QCOMPARE(serviceNumbersMap.value(".HELP DESK").toString(), QString("2601"));
        QCOMPARE(pinRequired.count(), 0);
        QCOMPARE(lockedPins.count(), 0);
        QCOMPARE(cardIdentifier.count(), 1);
        QCOMPARE(cardIdentifier.takeFirst().at(0).toString(), QString("8949222074451242066"));
        QCOMPARE(preferredLanguages.count(), 1);
        QStringList languages = preferredLanguages.takeFirst().at(0).toStringList();
        QVERIFY(languages.count() > 0);
        QCOMPARE(languages[0], QString("de"));
        QCOMPARE(fixedDialing.count(), 0);
        QCOMPARE(barredDialing.count(), 0);
    }
    
    void testQOfonoSimManagerPin()
    {
        QSignalSpy presence(m, SIGNAL(presenceChanged(bool)));
        QSignalSpy subscriberIdentity(m, SIGNAL(subscriberIdentityChanged(QString)));
        QSignalSpy mcc(m, SIGNAL(mobileCountryCodeChanged(QString)));
        QSignalSpy mnc(m, SIGNAL(mobileNetworkCodeChanged(QString)));
        QSignalSpy subscriberNumbers(m, SIGNAL(subscriberNumbersChanged(QStringList)));
        QSignalSpy serviceNumbers(m, SIGNAL(serviceNumbersChanged(OfonoServiceNumbers)));
        QSignalSpy pinRequired(m, SIGNAL(pinRequiredChanged(QString)));
        QSignalSpy lockedPins(m, SIGNAL(lockedPinsChanged(QStringList)));
        QSignalSpy cardIdentifier(m, SIGNAL(cardIdentifierChanged(QString)));
        QSignalSpy preferredLanguages(m, SIGNAL(preferredLanguagesChanged(QStringList)));
        QSignalSpy pinRetries(m, SIGNAL(pinRetriesChanged(OfonoPinRetries)));

        QSignalSpy changePin(m, SIGNAL(changePinComplete(bool)));
        QSignalSpy enterPin(m, SIGNAL(enterPinComplete(bool)));
        QSignalSpy resetPin(m, SIGNAL(resetPinComplete(bool)));
        QSignalSpy lockPin(m, SIGNAL(lockPinComplete(bool)));
        QSignalSpy unlockPin(m, SIGNAL(unlockPinComplete(bool)));

        m->lockPin(QOfonoSimManager::SimPin, "2468");
        QTest::qWait(1000);
        m->changePin(QOfonoSimManager::SimPin, "2468", "1234");
        QTest::qWait(1000);
        m->changePin(QOfonoSimManager::SimPin, "1234", "2468");
        QTest::qWait(1000);
        // entering and resetting PIN is not possible with phonesim's default config
        m->enterPin(QOfonoSimManager::SimPin, "2468");
        QTest::qWait(1000);
        m->resetPin(QOfonoSimManager::SimPuk, "13243546", "2468");
        QTest::qWait(1000);
        m->unlockPin(QOfonoSimManager::SimPin, "2468");
        QTest::qWait(1000);

        QCOMPARE(lockedPins.count(), 2);
        QCOMPARE(lockedPins.takeFirst().at(0).toStringList().count(), 1);
        QCOMPARE(lockedPins.takeFirst().at(0).toStringList().count(), 0);

        QCOMPARE(lockPin.count(), 1);
        QCOMPARE(lockPin.takeFirst().at(0).toBool(), true);
        QCOMPARE(unlockPin.count(), 1);
        QCOMPARE(unlockPin.takeFirst().at(0).toBool(), true);
        QCOMPARE(changePin.count(), 2);
        QCOMPARE(changePin.takeFirst().at(0).toBool(), true);
        QCOMPARE(changePin.takeFirst().at(0).toBool(), true);
        QCOMPARE(enterPin.count(), 1);
        QCOMPARE(enterPin.takeFirst().at(0).toBool(), false);
        QCOMPARE(resetPin.count(), 1);
        QCOMPARE(resetPin.takeFirst().at(0).toBool(), false);

        // entering PIN is not possible with phonesim's default config
        QCOMPARE(pinRetries.count(), 0);
    }

    void testQOfonoSimManagerIcon()
    {
        QSignalSpy getIcon(m, SIGNAL(getIconComplete(bool, QByteArray)));
        m->getIcon(0);
        QTest::qWait(1000);
        m->getIcon(1);
        QTest::qWait(1000);
        QCOMPARE(getIcon.count(), 2);
        QCOMPARE(getIcon.takeFirst().at(0).toBool(), false);
        QVariantList list = getIcon.takeFirst();
        QCOMPARE(list.at(0).toBool(), true);
        QVERIFY(list.at(1).toByteArray().length() > 0);
    }

    void cleanupTestCase()
    {

    }


private:
    QOfonoSimManager *m;
};

QTEST_MAIN(TestQOfonoSimManager)
#include "tst_qofonosimmanager.moc"
