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

#include <qofonointerface.h>

#include <QtDebug>

class TestOfonoInterface : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase()
    {
	oi = new OfonoInterface("/phonesim", "org.ofono.Modem", OfonoGetAllOnStartup, this);

        if (oi->properties()["Powered"].toBool() == false) {
            oi->setProperty("Powered", qVariantFromValue(true));
            QTest::qWait(5000);
        }
        if (oi->properties()["Online"].toBool() == false) {
            oi->setProperty("Online", qVariantFromValue(true));
            QTest::qWait(5000);
        }
    }

    void testGetProperties()
    {
        QCOMPARE(oi->properties()["Manufacturer"].toString(), QString("MeeGo"));

	oi_async = new OfonoInterface("/phonesim", "org.ofono.Modem", OfonoGetAllOnFirstRequest, this);
        QCOMPARE(oi_async->properties().count(), 0);

        QSignalSpy spy_request(oi_async, SIGNAL(requestPropertyComplete(bool, const QString &, const QVariant &)));
        oi_async->requestProperty("Manufacturer");
        oi_async->requestProperty("Model");

        QCOMPARE(spy_request.count(), 1);
        QVariantList list = spy_request.takeFirst();
        QCOMPARE(list[0].toBool(), false);
        QCOMPARE(list[1].toString(), QString("Model"));
        QCOMPARE(oi_async->errorMessage(), QString("Already in progress"));

        while (spy_request.count() != 1) {
            QTest::qWait(100);
        }
        list = spy_request.takeFirst();
        QCOMPARE(list[0].toBool(), true);
        QCOMPARE(list[1].toString(), QString("Manufacturer"));
        QCOMPARE(list[2].value<QVariant>().toString(), QString("MeeGo"));

        oi_async->requestProperty("Model");
        QCOMPARE(spy_request.count(), 1);
        list = spy_request.takeFirst();
        QCOMPARE(list[0].toBool(), true);
        QCOMPARE(list[1].toString(), QString("Model"));
        QCOMPARE(list[2].value<QVariant>().toString(), QString("Synthetic Device"));
        
        oi_async->requestProperty("UnknownProperty");
        while (spy_request.count() != 1) {
            QTest::qWait(100);
        }
        QCOMPARE(spy_request.count(), 1);
        list = spy_request.takeFirst();
        QCOMPARE(list[0].toBool(), false);
        QCOMPARE(list[1].toString(), QString("UnknownProperty"));
        QCOMPARE(oi_async->errorMessage(), QString("Property not available"));
    }
    
    void testSetProperty()
    {
        QSignalSpy spy_changed(oi, SIGNAL(propertyChanged(const QString &, const QVariant &)));
        QSignalSpy spy_failed(oi, SIGNAL(setPropertyFailed(const QString &)));
        QVariantList list;
        bool online;
        bool online_found;
    
        oi->setProperty("Online", qVariantFromValue(false));
        while (spy_changed.count() != 3 && spy_failed.count() == 0) {
            QTest::qWait(100);
        }
        QCOMPARE(spy_changed.count(), 3);
        QCOMPARE(spy_failed.count(), 0);
        online = false;
        online_found = false;
        while (spy_changed.count() > 0) {
            list = spy_changed.takeFirst();
            if (list[0] == "Online") {
                online = list[1].value<QVariant>().toBool();
                online_found = true;
            }
        }
        QCOMPARE(online_found, true);
        QCOMPARE(online, false);
        
        oi->setProperty("Online", qVariantFromValue(true));
        while (spy_changed.count() < 3 && spy_failed.count() == 0) {
            QTest::qWait(100);
        }
        QVERIFY(spy_changed.count() > 3);
        QCOMPARE(spy_failed.count(), 0);
        online = false;
        online_found = false;
        while (spy_changed.count() > 0) {
            list = spy_changed.takeFirst();
            if (list[0] == "Online") {
                online = list[1].value<QVariant>().toBool();
                online_found = true;
            }
        }
        QCOMPARE(online_found, true);
        QCOMPARE(online, true);
        QTest::qWait(5000);
    }

    void testSetPropertyFailed()
    {
        QSignalSpy spy_changed(oi, SIGNAL(propertyChanged(const QString &, const QVariant &)));
        QSignalSpy spy_failed(oi, SIGNAL(setPropertyFailed(const QString &)));
        QVariantList list;
    
        oi->setProperty("Manufacturer", qVariantFromValue(QString("Nokia")));
        while (spy_changed.count() == 0 && spy_failed.count() == 0) {
            QTest::qWait(100);
        }
        QCOMPARE(spy_changed.count(), 0);
        QCOMPARE(spy_failed.count(), 1);
        list = spy_failed.takeFirst();
        QCOMPARE(list[0].toString(), QString("Manufacturer"));
        QCOMPARE(oi->errorName(), QString("org.ofono.Error.InvalidArguments"));
        QCOMPARE(oi->errorMessage(), QString("Invalid arguments in method call"));
    }


    void cleanupTestCase()
    {

    }


private:
    OfonoInterface *oi;
    OfonoInterface *oi_async;
};

QTEST_MAIN(TestOfonoInterface)
#include "test_ofonointerface.moc"
