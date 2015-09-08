/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2014-2015 Jolla Ltd.
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

#include "qofonosimmanager.h"
#include "qofonomodem.h"

#include <QtDebug>

// Do it here to not break API compatibility - also see qRegisterMetaType below
Q_DECLARE_METATYPE(QOfonoSimManager::Error)

class TestQOfonoSimManager : public QObject
{
    Q_OBJECT

    // The same as the QTRY_* macros use
    static const int REASONABLE_TIMEOUT = 5000;

public:
    TestQOfonoSimManager()
    {
        qRegisterMetaType<QOfonoSimManager::Error>();
    }

private slots:
    void initTestCase()
    {
        m = new QOfonoSimManager(this);
        m->setModemPath("/phonesim");

        QTRY_COMPARE(m->isValid(), true);
    }

    void testQOfonoSimManager()
    {
        QSignalSpy presence(m, SIGNAL(presenceChanged(bool)));
        QSignalSpy subscriberIdentity(m, SIGNAL(subscriberIdentityChanged(QString)));
        QSignalSpy mcc(m, SIGNAL(mobileCountryCodeChanged(QString)));
        QSignalSpy mnc(m, SIGNAL(mobileNetworkCodeChanged(QString)));
        QSignalSpy subscriberNumbers(m, SIGNAL(subscriberNumbersChanged(QStringList)));
        QSignalSpy serviceNumbers(m, SIGNAL(serviceNumbersChanged(QVariantMap)));
        QSignalSpy pinRequired(m, SIGNAL(pinRequiredChanged(int)));
        QSignalSpy lockedPins(m, SIGNAL(lockedPinsChanged(QVariantList)));
        QSignalSpy cardIdentifier(m, SIGNAL(cardIdentifierChanged(QString)));
        QSignalSpy preferredLanguages(m, SIGNAL(preferredLanguagesChanged(QStringList)));
        QSignalSpy fixedDialing(m, SIGNAL(fixedDialingChanged(bool)));
        QSignalSpy barredDialing(m, SIGNAL(barredDialingChanged(bool)));

        QSignalSpy changePin(m, SIGNAL(changePinComplete(QOfonoSimManager::Error, QString)));
        QSignalSpy enterPin(m, SIGNAL(enterPinComplete(QOfonoSimManager::Error, QString)));
        QSignalSpy resetPin(m, SIGNAL(resetPinComplete(QOfonoSimManager::Error, QString)));
        QSignalSpy lockPin(m, SIGNAL(lockPinComplete(QOfonoSimManager::Error, QString)));
        QSignalSpy unlockPin(m, SIGNAL(unlockPinComplete(QOfonoSimManager::Error, QString)));

        QTRY_COMPARE(m->present(), true);
        QTRY_COMPARE(m->subscriberIdentity(), QString("246813579"));
        QTRY_COMPARE(m->mobileCountryCode(), QString("246"));
        QTRY_COMPARE(m->mobileNetworkCode(), QString("81"));
        QTRY_VERIFY(m->subscriberNumbers().count() > 0);
        QCOMPARE(m->subscriberNumbers()[0], QString("358501234567"));
        QTRY_VERIFY(m->serviceNumbers().count() > 0);
        QCOMPARE(m->serviceNumbers().value(".HELP DESK").toString(), QString("2601"));
        QCOMPARE(m->pinRequired(),QOfonoSimManager::NoPin);
        QCOMPARE(m->lockedPins().count(), 0);
        QTRY_COMPARE(m->cardIdentifier(), QString("8949222074451242066"));
        QTRY_VERIFY(m->preferredLanguages().count() > 0);
        QCOMPARE(m->preferredLanguages()[0], QString("de"));
        QCOMPARE(m->pinRetries().count(), 0);
        QCOMPARE(m->fixedDialing(), false);
        QCOMPARE(m->barredDialing(), false);

        QStringList numbers = m->subscriberNumbers();
        QStringList newNumbers;
        newNumbers << "1234567";
        m->setSubscriberNumbers(newNumbers);
        QTRY_COMPARE(subscriberNumbers.count(), 1);
        QStringList newNumbersSignal = subscriberNumbers.takeFirst().at(0).toStringList();
        QCOMPARE(newNumbersSignal.count(), 1);
        QCOMPARE(newNumbersSignal, newNumbers);

        newNumbers.clear();
        newNumbers << "";
        m->setSubscriberNumbers(newNumbers);
        // previous call is expected to fail -> only one signal will be emitted
        m->setSubscriberNumbers(numbers);
        QTRY_COMPARE(subscriberNumbers.count(), 1);
        newNumbersSignal = subscriberNumbers.takeFirst().at(0).toStringList();
        QCOMPARE(newNumbersSignal.count(), 1);
        QCOMPARE(newNumbersSignal, numbers);

        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(presence.count(), 0);
        QCOMPARE(subscriberIdentity.count(), 0);
        QCOMPARE(mcc.count(), 0);
        QCOMPARE(mnc.count(), 0);
        QCOMPARE(subscriberNumbers.count(), 0);
        QCOMPARE(serviceNumbers.count(), 0);
        QCOMPARE(cardIdentifier.count(), 0);
        QCOMPARE(preferredLanguages.count(), 0);
        QCOMPARE(pinRequired.count(), 0);
        QCOMPARE(lockedPins.count(), 0);
        QCOMPARE(fixedDialing.count(), 0);
        QCOMPARE(barredDialing.count(), 0);

        QOfonoModem modem;
        modem.setModemPath(m->modemPath());
        QSignalSpy modemOnline(&modem, SIGNAL(onlineChanged(bool)));
        QSignalSpy modemPowered(&modem, SIGNAL(poweredChanged(bool)));

        // Wait for modem to become ready
        QTRY_COMPARE(modemOnline.count(), 1);
        QCOMPARE(modemOnline.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(modemPowered.count(), 1);
        QCOMPARE(modemPowered.takeFirst().at(0).toBool(), true);

        //SIM hotswap is not testable with phonesim, so we simulate a modem reset
        modem.setOnline(false);
        QTRY_COMPARE(modemOnline.count(), 1);
        QCOMPARE(modemOnline.takeFirst().at(0).toBool(), false);
        modem.setPowered(false);
        QTRY_COMPARE(modemPowered.count(), 1);
        QCOMPARE(modemPowered.takeFirst().at(0).toBool(), false);
        QTRY_VERIFY(!m->isValid());

        QTRY_COMPARE(presence.count(), 1);
        QCOMPARE(presence.takeFirst().at(0).toBool(), false);
        QTRY_COMPARE(subscriberIdentity.count(), 1);
        QCOMPARE(subscriberIdentity.takeFirst().at(0).toString(), QString(""));
        QTRY_COMPARE(mcc.count(), 1);
        QCOMPARE(mcc.takeFirst().at(0).toString(), QString(""));
        QTRY_COMPARE(mnc.count(), 1);
        QCOMPARE(mnc.takeFirst().at(0).toString(), QString(""));
        QTRY_COMPARE(subscriberNumbers.count(), 1);
        numbers = subscriberNumbers.takeFirst().at(0).toStringList();
        QCOMPARE(numbers.count(), 0);
        QTRY_COMPARE(serviceNumbers.count(), 1);
        QVariantMap serviceNumbersMap = serviceNumbers.takeFirst().at(0).toMap();
        QCOMPARE(serviceNumbersMap.count(), 0);
        QTRY_COMPARE(cardIdentifier.count(), 1);
        QCOMPARE(cardIdentifier.takeFirst().at(0).toString(), QString(""));
        QTRY_COMPARE(preferredLanguages.count(), 1);
        QStringList languages = preferredLanguages.takeFirst().at(0).toStringList();
        QCOMPARE(languages.count(), 0);
        QCOMPARE(pinRequired.count(), 0);
        QCOMPARE(lockedPins.count(), 0);
        QCOMPARE(fixedDialing.count(), 0);
        QCOMPARE(barredDialing.count(), 0);

        QCOMPARE(presence.count(), 0);
        QCOMPARE(subscriberIdentity.count(), 0);
        QCOMPARE(mcc.count(), 0);
        QCOMPARE(mnc.count(), 0);
        QCOMPARE(subscriberNumbers.count(), 0);
        QCOMPARE(serviceNumbers.count(), 0);
        QCOMPARE(cardIdentifier.count(), 0);
        QCOMPARE(preferredLanguages.count(), 0);
        QCOMPARE(pinRequired.count(), 0);
        QCOMPARE(lockedPins.count(), 0);
        QCOMPARE(fixedDialing.count(), 0);
        QCOMPARE(barredDialing.count(), 0);

        modem.setPowered(true);
        QTRY_COMPARE(modemPowered.count(), 1);
        QCOMPARE(modemPowered.takeFirst().at(0).toBool(), true);
        modem.setOnline(true);
        QTRY_COMPARE(modemOnline.count(), 1);
        QCOMPARE(modemOnline.takeFirst().at(0).toBool(), true);
        QTRY_VERIFY(m->isValid());

        QTRY_COMPARE(presence.count(), 1);
        QCOMPARE(presence.takeFirst().at(0).toBool(), true);
        QTRY_COMPARE(subscriberIdentity.count(), 1);
        QCOMPARE(subscriberIdentity.takeFirst().at(0).toString(), QString("246813579"));
        QTRY_COMPARE(mcc.count(), 1);
        QCOMPARE(mcc.takeFirst().at(0).toString(), QString("246"));
        QTRY_COMPARE(mnc.count(), 1);
        QCOMPARE(mnc.takeFirst().at(0).toString(), QString("81"));
        QTRY_COMPARE(subscriberNumbers.count(), 1);
        numbers = subscriberNumbers.takeFirst().at(0).toStringList();
        QCOMPARE(numbers.count(), 1);
        QCOMPARE(numbers[0], QString("358501234567"));
        QTRY_COMPARE(serviceNumbers.count(), 1);
        serviceNumbersMap = serviceNumbers.takeFirst().at(0).toMap();
        QVERIFY(serviceNumbersMap.count() > 0);
        QCOMPARE(serviceNumbersMap.value(".HELP DESK").toString(), QString("2601"));
        QTRY_COMPARE(cardIdentifier.count(), 1);
        QCOMPARE(cardIdentifier.takeFirst().at(0).toString(), QString("8949222074451242066"));
        QTRY_COMPARE(preferredLanguages.count(), 1);
        languages = preferredLanguages.takeFirst().at(0).toStringList();
        QVERIFY(languages.count() > 0);
        QCOMPARE(languages[0], QString("de"));

        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(presence.count(), 0);
        QCOMPARE(subscriberIdentity.count(), 0);
        QCOMPARE(mcc.count(), 0);
        QCOMPARE(mnc.count(), 0);
        QCOMPARE(subscriberNumbers.count(), 0);
        QCOMPARE(serviceNumbers.count(), 0);
        QCOMPARE(cardIdentifier.count(), 0);
        QCOMPARE(preferredLanguages.count(), 0);
        QCOMPARE(pinRequired.count(), 0);
        QCOMPARE(lockedPins.count(), 0);
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
        QSignalSpy serviceNumbers(m, SIGNAL(serviceNumbersChanged(QVariantMap)));
        QSignalSpy pinRequired(m, SIGNAL(pinRequiredChanged(int)));
        QSignalSpy lockedPins(m, SIGNAL(lockedPinsChanged(QVariantList)));
        QSignalSpy cardIdentifier(m, SIGNAL(cardIdentifierChanged(QString)));
        QSignalSpy preferredLanguages(m, SIGNAL(preferredLanguagesChanged(QStringList)));
        QSignalSpy pinRetries(m, SIGNAL(pinRetriesChanged(QVariantMap)));

        QSignalSpy changePin(m, SIGNAL(changePinComplete(QOfonoSimManager::Error, QString)));
        QSignalSpy enterPin(m, SIGNAL(enterPinComplete(QOfonoSimManager::Error, QString)));
        QSignalSpy resetPin(m, SIGNAL(resetPinComplete(QOfonoSimManager::Error, QString)));
        QSignalSpy lockPin(m, SIGNAL(lockPinComplete(QOfonoSimManager::Error, QString)));
        QSignalSpy unlockPin(m, SIGNAL(unlockPinComplete(QOfonoSimManager::Error, QString)));

        m->lockPin(QOfonoSimManager::SimPin, "2468");
        QTRY_COMPARE(lockPin.count(), 1);
        QCOMPARE(lockPin.takeFirst().at(0).value<QOfonoSimManager::Error>(),
            QOfonoSimManager::NoError);
        QTRY_COMPARE(lockedPins.count(), 1);
        QCOMPARE(lockedPins.takeFirst().at(0).toList().count(), 1);
        m->changePin(QOfonoSimManager::SimPin, "2468", "1234");
        QTRY_COMPARE(changePin.count(), 1);
        QCOMPARE(changePin.takeFirst().at(0).value<QOfonoSimManager::Error>(),
            QOfonoSimManager::NoError);
        m->changePin(QOfonoSimManager::SimPin, "1234", "2468");
        QTRY_COMPARE(changePin.count(), 1);
        QCOMPARE(changePin.takeFirst().at(0).value<QOfonoSimManager::Error>(),
            QOfonoSimManager::NoError);
        // entering and resetting PIN is not possible with phonesim's default config
        m->enterPin(QOfonoSimManager::SimPin, "2468");
        QTRY_COMPARE(enterPin.count(), 1);
        QCOMPARE(enterPin.takeFirst().at(0).value<QOfonoSimManager::Error>(),
            QOfonoSimManager::InvalidFormatError);
        m->resetPin(QOfonoSimManager::SimPuk, "13243546", "2468");
        QTRY_COMPARE(resetPin.count(), 1);
        QCOMPARE(resetPin.takeFirst().at(0).value<QOfonoSimManager::Error>(),
            QOfonoSimManager::InvalidFormatError);
        m->unlockPin(QOfonoSimManager::SimPin, "2468");
        QTRY_COMPARE(unlockPin.count(), 1);
        QCOMPARE(unlockPin.takeFirst().at(0).value<QOfonoSimManager::Error>(),
            QOfonoSimManager::NoError);
        QTRY_COMPARE(lockedPins.count(), 1);
        QCOMPARE(lockedPins.takeFirst().at(0).toList().count(), 0);

        // entering PIN is not possible with phonesim's default config
        QCOMPARE(pinRetries.count(), 0);
    }

    void testQOfonoSimManagerIcon()
    {
        QByteArray icon0 = m->getIcon(0);
        QVERIFY(icon0.isEmpty());
        QByteArray icon1 = m->getIcon(1);
        QVERIFY(!icon1.isEmpty());
    }

    void cleanupTestCase()
    {

    }

private:
    QOfonoSimManager *m;
};

QTEST_MAIN(TestQOfonoSimManager)
#include "tst_qofonosimmanager.moc"
