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

#include <qofono/qofonomodem.h>

#include <QtDebug>

class TestQOfonoModem : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
    mm = new QOfonoModem(this);
    mm->setModemPath("/phonesim");
    ma = new QOfonoModem(this);
    ma->setModemPath(QString());
	
	if (!mm->powered()) {
  	    mm->setPowered(true);
            QTest::qWait(5000);
        }
        if (!mm->online()) {
  	    mm->setOnline(true);
            QTest::qWait(5000);
        }

    }

    void testQOfonoModemManual()
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

    void testQOfonoModemAutomatic()
    {
        QVERIFY(ma->isValid());
        QVERIFY(ma->modemPath().length() > 1);
    }

    void testQOfonoModemAddRemove()
    {
        QSignalSpy avalid(ma, SIGNAL(validityChanged(bool)));
        QSignalSpy valid(mm, SIGNAL(validityChanged(bool)));
        QSignalSpy amodemPath(ma, SIGNAL(pathChanged(QString)));
        QSignalSpy modemPath(mm, SIGNAL(pathChanged(QString)));
        
        qDebug() << "Please stop oFono and then start it again";
    
        for (int i=0; i<30; i++) {
            if (avalid.count() == 2 &&
                valid.count() == 1 &&
                amodemPath.count() == 2 &&
                modemPath.count() == 0)
                break;
            QTest::qWait(1000);
        }
        QCOMPARE(avalid.count(), 2);
        QCOMPARE(valid.count(), 1);
        QCOMPARE(amodemPath.count(), 2);
        QCOMPARE(modemPath.count(), 0);
        QCOMPARE(avalid.takeFirst().at(0).toBool(), false);
        QCOMPARE(avalid.takeFirst().at(0).toBool(), true);        
        QCOMPARE(valid.takeFirst().at(0).toBool(), false);
        amodemPath.takeFirst();
        QVERIFY(amodemPath.takeFirst().at(0).toString().length() > 1);

        QVERIFY(!mm->isValid());
        QVERIFY(ma->isValid());
        QVERIFY(ma->modemPath().length() > 1);
        delete mm;
        mm = new QOfonoModem(this);
        mm->setModemPath("/phonesim");
	if (!mm->powered()) {
  	    mm->setPowered(true);
            QTest::qWait(5000);
        }
        if (!mm->online()) {
  	    mm->setOnline(true);
            QTest::qWait(5000);
        }
    }

    void testQOfonoModemPowercycle()
    {
        QSignalSpy powered(mm, SIGNAL(poweredChanged(bool)));
        QSignalSpy poweredFailed(mm, SIGNAL(setPoweredFailed()));
        QSignalSpy online(mm, SIGNAL(onlineChanged(bool)));
        QSignalSpy onlineFailed(mm, SIGNAL(setOnlineFailed()));
        QSignalSpy lockdown(mm, SIGNAL(lockdownChanged(bool)));
        QSignalSpy lockdownFailed(mm, SIGNAL(setLockdownFailed()));
        QSignalSpy emergency(mm, SIGNAL(emergencyChanged(bool)));
        QSignalSpy name(mm, SIGNAL(nameChanged(const QString &)));
        QSignalSpy manufacturer(mm, SIGNAL(manufacturerChanged(const QString &)));
        QSignalSpy model(mm, SIGNAL(modelChanged(const QString &)));
        QSignalSpy revision(mm, SIGNAL(revisionChanged(const QString &)));
        QSignalSpy serial(mm, SIGNAL(serialChanged(const QString &)));
        QSignalSpy type(mm, SIGNAL(typeChanged(const QString &)));
        QSignalSpy features(mm, SIGNAL(featuresChanged(const QStringList &)));
        QSignalSpy interfaces(mm, SIGNAL(interfacesChanged(const QStringList &)));
        
        mm->setOnline(false);
        QTest::qWait(5000);
        QCOMPARE(powered.count(), 0);
        QCOMPARE(poweredFailed.count(), 0);
        QCOMPARE(online.count(), 1);
        QCOMPARE(online.takeFirst().at(0).toBool(), false);
        QCOMPARE(onlineFailed.count(), 0);
        QCOMPARE(emergency.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(manufacturer.count(), 0);
        QCOMPARE(model.count(), 0);
        QCOMPARE(revision.count(), 0);
        QCOMPARE(serial.count(), 0);
        QCOMPARE(type.count(), 0);
        QCOMPARE(features.count(), 1);
        QVERIFY(features.takeFirst().at(0).toStringList().count() > 0);
        QCOMPARE(interfaces.count(), 1);
        QVERIFY(interfaces.takeFirst().at(0).toStringList().count() > 0);

        mm->setPowered(false);
        QTest::qWait(5000);
        QCOMPARE(powered.count(), 1);
        QCOMPARE(powered.takeFirst().at(0).toBool(), false);
        QCOMPARE(poweredFailed.count(), 0);
        QCOMPARE(online.count(), 0);
        QCOMPARE(onlineFailed.count(), 0);
        QCOMPARE(emergency.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(manufacturer.count(), 0);
        QCOMPARE(model.count(), 0);
        QCOMPARE(revision.count(), 0);
        QCOMPARE(serial.count(), 0);
        QCOMPARE(type.count(), 0);
        QCOMPARE(features.count(), 1);
        QCOMPARE(features.takeFirst().at(0).toStringList().count(), 0);
        QCOMPARE(interfaces.count(), 1);
        QCOMPARE(interfaces.takeFirst().at(0).toStringList().count(), 0);

        mm->setLockdown(true);
        QTest::qWait(5000);
        QCOMPARE(lockdown.count(), 1);
        QCOMPARE(lockdown.takeFirst().at(0).toBool(), true);
        QCOMPARE(lockdownFailed.count(), 0);

        mm->setLockdown(false);
        QTest::qWait(5000);
        QCOMPARE(lockdown.count(), 1);
        QCOMPARE(lockdown.takeFirst().at(0).toBool(), false);
        QCOMPARE(lockdownFailed.count(), 0);

	mm->setOnline(true);
        QTest::qWait(5000);
        QCOMPARE(onlineFailed.count(), 1);
        onlineFailed.takeFirst();

        mm->setPowered(true);
        QTest::qWait(5000);
        QCOMPARE(powered.count(), 1);
        QCOMPARE(powered.takeFirst().at(0).toBool(), true);
        QCOMPARE(poweredFailed.count(), 0);
        QCOMPARE(online.count(), 0);
        QCOMPARE(onlineFailed.count(), 0);
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
        QTest::qWait(5000);
        QCOMPARE(powered.count(), 0);
        QCOMPARE(poweredFailed.count(), 0);
        QCOMPARE(online.count(), 1);
        QCOMPARE(online.takeFirst().at(0).toBool(), true);
        QCOMPARE(onlineFailed.count(), 0);
        QCOMPARE(emergency.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(manufacturer.count(), 0);
        QCOMPARE(model.count(), 0);
        QCOMPARE(revision.count(), 0);
        QCOMPARE(serial.count(), 0);
        QCOMPARE(type.count(), 0);
        QVERIFY(features.count() > 0);
        QVERIFY(features.takeLast().at(0).toStringList().count() > 0);
        QVERIFY(interfaces.count() > 0);
        QVERIFY(interfaces.takeLast().at(0).toStringList().count() > 0);

    }    

    void cleanupTestCase()
    {

    }


private:
    QOfonoModem *mm;
    QOfonoModem *ma;
};

QTEST_MAIN(TestQOfonoModem)
#include "tst_qofonomodem.moc"
