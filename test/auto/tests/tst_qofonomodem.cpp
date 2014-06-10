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

#include "../../../src/qofonomanager.h"
#include "../../../src/qofonomodem.h"

#include <QtDebug>

class TestQOfonoModem : public QObject
{
    Q_OBJECT

    static const int INTERACTIVE_STEP_TIMEOUT = 30000;
    // The same as the QTRY_* macros use
    static const int REASONABLE_TIMEOUT = 5000;
    static const int REASONABLE_TIMEOUT_AFTER_RESTART = 10000;

public:
    TestQOfonoModem()
        : mm(0)
    {
    }

private slots:
    void init()
    {
        delete mm;
        mm = new QOfonoModem(this);
        mm->setModemPath("/phonesim");

        mm->setPowered(true);
        QTRY_VERIFY(mm->powered());
        mm->setOnline(true);
        QTRY_VERIFY(mm->online());
    }

    void testQOfonoModem()
    {
        QVERIFY(mm->isValid());
        QVERIFY(mm->powered());
        QVERIFY(mm->online());
        QVERIFY(!mm->lockdown());
        QVERIFY(!mm->emergency());
        QVERIFY(mm->features().count() > 0);
        QVERIFY(mm->interfaces().count() > 0);

        QCOMPARE(mm->modemPath(), QString("/phonesim"));
        QCOMPARE(mm->name(), QString());
        QCOMPARE(mm->manufacturer(), QString("MeeGo"));
        QCOMPARE(mm->model(), QString("Synthetic Device"));
        QCOMPARE(mm->revision(), QString("REV1"));
        QCOMPARE(mm->serial(), QString("1234567890"));
        QCOMPARE(mm->type(), QString("hardware"));
    }

    void testQOfonoModemAddRemove()
    {
        QSignalSpy valid(mm, SIGNAL(validChanged(bool)));
        QSignalSpy modemPath(mm, SIGNAL(modemPathChanged(QString)));

        QVERIFY(mm->isValid());

        QOfonoManager *manager = new QOfonoManager(this);
        QSignalSpy availableChanged(manager, SIGNAL(availableChanged(bool)));

        qDebug() << "Please stop oFono and then start it again";
        QTRY_COMPARE_WITH_TIMEOUT(availableChanged.count(), 2, INTERACTIVE_STEP_TIMEOUT);
        QCOMPARE(availableChanged.takeFirst().at(0).toBool(), false);
        QCOMPARE(availableChanged.takeFirst().at(0).toBool(), true);

        QTRY_VERIFY(manager->modems().contains("/phonesim"));
        QTest::qWait(REASONABLE_TIMEOUT);
        QEXPECT_FAIL("", "Modem not invalidated when oFono is stopped", Continue);
        QCOMPARE(valid.count(), 1);
        // Uncomment when above is fixed
        //QCOMPARE(valid.takeFirst().at(0).toBool(), false);
        //QVERIFY(!mm->isValid());
    }

    void testQOfonoModemPowercycle()
    {
        // TODO: Workaround. Find a way to detect oFono is fully up. Without this
        // the QCOMPARE(manufacturer.count(), 0) test below fails.
        QTest::qWait(REASONABLE_TIMEOUT_AFTER_RESTART);

        QSignalSpy powered(mm, SIGNAL(poweredChanged(bool)));
        QSignalSpy online(mm, SIGNAL(onlineChanged(bool)));
        QSignalSpy lockdown(mm, SIGNAL(lockdownChanged(bool)));
        QSignalSpy emergency(mm, SIGNAL(emergencyChanged(bool)));
        QSignalSpy name(mm, SIGNAL(nameChanged(QString)));
        QSignalSpy manufacturer(mm, SIGNAL(manufacturerChanged(QString)));
        QSignalSpy model(mm, SIGNAL(modelChanged(QString)));
        QSignalSpy revision(mm, SIGNAL(revisionChanged(QString)));
        QSignalSpy serial(mm, SIGNAL(serialChanged(QString)));
        QSignalSpy type(mm, SIGNAL(typeChanged(QString)));
        QSignalSpy features(mm, SIGNAL(featuresChanged(QStringList)));
        QSignalSpy interfaces(mm, SIGNAL(interfacesChanged(QStringList)));

        mm->setOnline(false);
        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(powered.count(), 0);
        QCOMPARE(online.count(), 1);
        QCOMPARE(online.takeFirst().at(0).toBool(), false);
        QCOMPARE(emergency.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(manufacturer.count(), 0);
        QCOMPARE(model.count(), 0);
        QCOMPARE(revision.count(), 0);
        QCOMPARE(serial.count(), 0);
        QCOMPARE(type.count(), 0);
        QVERIFY(features.count() > 0);
        QVERIFY(features.takeLast().at(0).toStringList().count() > 0);
        features.clear();
        QVERIFY(interfaces.count() > 0);
        QVERIFY(interfaces.takeLast().at(0).toStringList().count() > 0);
        interfaces.clear();

        mm->setPowered(false);
        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(powered.count(), 1);
        QCOMPARE(powered.takeFirst().at(0).toBool(), false);
        QCOMPARE(online.count(), 0);
        QCOMPARE(emergency.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(manufacturer.count(), 0);
        QCOMPARE(model.count(), 0);
        QCOMPARE(revision.count(), 0);
        QCOMPARE(serial.count(), 0);
        QCOMPARE(type.count(), 0);
        QVERIFY(features.count() > 0);
        QCOMPARE(features.takeLast().at(0).toStringList().count(), 0);
        features.clear();
        QVERIFY(interfaces.count() > 0);
        QEXPECT_FAIL("", "org.ofono.SmsHistory remains available even when poweroff - ok?",
                Continue);
        qDebug() << "interfaces: " << interfaces.last().at(0).toStringList();
        QCOMPARE(interfaces.takeLast().at(0).toStringList().count(), 0);
        interfaces.clear();

        mm->setLockdown(true);
        QTRY_COMPARE(lockdown.count(), 1);
        QCOMPARE(lockdown.takeFirst().at(0).toBool(), true);

        mm->setLockdown(false);
        QTRY_COMPARE(lockdown.count(), 1);
        QCOMPARE(lockdown.takeFirst().at(0).toBool(), false);

        mm->setOnline(true);
        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(powered.count(), 0);
        QCOMPARE(online.count(), 0);
        QCOMPARE(emergency.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(manufacturer.count(), 0);
        QCOMPARE(model.count(), 0);
        QCOMPARE(revision.count(), 0);
        QCOMPARE(serial.count(), 0);
        QCOMPARE(type.count(), 0);
        QCOMPARE(features.count(), 0);
        QCOMPARE(interfaces.count(), 0);

        // FIXME: This does not seem to be valid - These properties actually
        // do not change.
        mm->setPowered(true);
        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(powered.count(), 1);
        QCOMPARE(powered.takeFirst().at(0).toBool(), true);
        QCOMPARE(online.count(), 0);
        QCOMPARE(emergency.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(manufacturer.count(), 1);
        manufacturer.takeFirst();
        QCOMPARE(model.count(), 1);
        model.takeFirst();
        QCOMPARE(revision.count(), 1);
        revision.takeFirst();
        QCOMPARE(serial.count(), 1);
        serial.takeFirst();
        QCOMPARE(type.count(), 0);
        QVERIFY(features.count() > 0);
        QVERIFY(features.takeLast().at(0).toStringList().count() > 0);
        features.clear();
        QVERIFY(interfaces.count() > 0);
        QVERIFY(interfaces.takeLast().at(0).toStringList().count() > 0);
        interfaces.clear();

        mm->setOnline(true);
        QTest::qWait(REASONABLE_TIMEOUT);
        QCOMPARE(powered.count(), 0);
        QCOMPARE(online.count(), 1);
        QCOMPARE(online.takeFirst().at(0).toBool(), true);
        QCOMPARE(emergency.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(manufacturer.count(), 0);
        QCOMPARE(model.count(), 0);
        QCOMPARE(revision.count(), 0);
        QCOMPARE(serial.count(), 0);
        QCOMPARE(type.count(), 0);
        QVERIFY(features.count() > 0);
        QVERIFY(features.takeLast().at(0).toStringList().count() > 0);
        features.clear();
        QVERIFY(interfaces.count() > 0);
        QVERIFY(interfaces.takeLast().at(0).toStringList().count() > 0);
        interfaces.clear();
    }

    void cleanupTestCase()
    {

    }


private:
    QOfonoModem *mm;
};

QTEST_MAIN(TestQOfonoModem)
#include "tst_qofonomodem.moc"
