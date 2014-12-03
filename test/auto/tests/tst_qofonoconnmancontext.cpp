/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qofonomanager.h"
#include "qofonoconnectionmanager.h"
#include "qofonoconnectioncontext.h"

#include <QtDebug>

class TestQOfonoConnectionManagerContext : public QObject
{
    Q_OBJECT

    // The same as the QTRY_* macros use
    static const int REASONABLE_TIMEOUT = 5000;

private slots:
    void initTestCase()
    {
        QOfonoManager manager;
        m = new QOfonoConnectionManager(this);
        m->setModemPath("/phonesim");
        QTRY_VERIFY(m->isValid());
        QTRY_VERIFY(m->powered());
    }

    void testQOfonoConnectionManagerContext ()
    {
        QSignalSpy conadd(m, SIGNAL(contextAdded(QString)));
        QSignalSpy conrem(m, SIGNAL(contextRemoved(QString)));

        m->addContext("internet");
        QTRY_COMPARE(conadd.count(), 1);
        QString contextid = conadd.takeFirst().at(0).toString();
        QVERIFY(m->contexts().contains(contextid));

        QOfonoConnectionContext* context = new QOfonoConnectionContext(this);
        context->setContextPath(contextid);
        QTRY_COMPARE(context->isValid(), true);

        QSignalSpy active(context, SIGNAL(activeChanged(bool)));
        QSignalSpy apn(context,SIGNAL(accessPointNameChanged(QString)));
        QSignalSpy name(context, SIGNAL(nameChanged(QString)));
        QSignalSpy type (context, SIGNAL(typeChanged(QString)));
        QSignalSpy uname (context, SIGNAL(usernameChanged(QString)));
        QSignalSpy pw (context, SIGNAL(passwordChanged(QString)));
        QSignalSpy proto (context, SIGNAL(protocolChanged(QString)));
        QSignalSpy sett (context, SIGNAL(settingsChanged(QVariantMap)));
        QSignalSpy sett6 (context, SIGNAL(IPv6SettingsChanged(QVariantMap)));

        context->setAccessPointName("hyva");
        QTRY_COMPARE(apn.count(), 1);
        QCOMPARE(apn.takeFirst().at(0).toString(), QString("hyva"));
        context->setUsername("huomenta");
        QTRY_COMPARE(uname.count(), 1);
        QCOMPARE(uname.takeFirst().at(0).toString(), QString("huomenta"));
        context->setPassword("HYVA");
        QTRY_COMPARE(pw.count(), 1);
        QCOMPARE(pw.takeFirst().at(0).toString(), QString("HYVA"));
        context->setName("yota");
        QTRY_COMPARE(name.count(), 1);
        QCOMPARE(name.takeFirst().at(0).toString(), QString("yota"));
        context->setType("mms");
        QTRY_COMPARE(type.count(), 1);
        QCOMPARE(type.takeFirst().at(0).toString(), QString("mms"));
        context->setProtocol("ipv6");
        QTRY_COMPARE(proto.count(), 1);
        QCOMPARE(proto.takeFirst().at(0).toString(), QString("ipv6"));

        context->setActive(true);
        QTRY_COMPARE(active.count(), 1);
        QCOMPARE(active.takeFirst().at(0).toBool(), true);

        QTRY_COMPARE(sett6.count(), 1);
        QCOMPARE(sett6.takeFirst().at(0).toMap()["Interface"].value<QString>().left(5),
            QString("dummy")); // "dummy" plus number
        QVariantMap settings = context->IPv6Settings();
        QCOMPARE(settings["Interface"].value<QString>().left(5),QString("dummy")); // "dummy" plus number

        QTest::qWait(REASONABLE_TIMEOUT);

        QCOMPARE(apn.count(), 0);
        QCOMPARE(uname.count(), 0);
        QCOMPARE(pw.count(), 0);
        QCOMPARE(name.count(), 0);
        QCOMPARE(type.count(), 0);
        QCOMPARE(sett.count(), 0);
        QCOMPARE(proto.count(), 0);
        QCOMPARE(active.count(), 0);

        context->setActive(false);
        QTRY_COMPARE(active.count(), 1);
        QCOMPARE(active.takeFirst().at(0).toBool(), false);

        delete context;

        m->removeContext(contextid);
        QTRY_COMPARE(conrem.count(), 1);
        QCOMPARE(conrem.takeFirst().at(0).toString(), contextid);
    }

    void cleanupTestCase()
    {

    }

    void tst_provisioning()
    {
        QSignalSpy conadd(m, SIGNAL(contextAdded(QString)));

        Q_FOREACH (const QString con, m->contexts()) {
            m->removeContext(con);
        }
        QTRY_COMPARE(m->contexts(), QStringList());

        m->addContext("internet");
        QTRY_COMPARE(conadd.count(), 1);
        QString contextid = conadd.takeFirst().at(0).toString();
        QVERIFY(m->contexts().contains(contextid));

        QOfonoConnectionContext* context = new QOfonoConnectionContext(this);
        context->setContextPath(contextid);
        QTRY_VERIFY(context->isValid());

        QSignalSpy apn(context, SIGNAL(accessPointNameChanged(QString)));
        context->setAccessPointName("internet");
        QTRY_COMPARE(apn.count(), 1);
        QCOMPARE(apn.takeFirst().at(0).toString(), QString("internet"));

        QString operatorString = "Optus";
        QString mcc = "505";
        QString mnc = "02";

        QCOMPARE(context->validateProvisioning(operatorString, mcc, mnc),true);

        context->setAccessPointName("yesinternet");
        QTRY_COMPARE(apn.count(), 1);
        QCOMPARE(apn.takeFirst().at(0).toString(), QString("yesinternet"));
        QCOMPARE(context->validateProvisioning(operatorString, mcc, mnc),true);

        context->setAccessPointName("connect");
        QTRY_COMPARE(apn.count(), 1);
        QCOMPARE(apn.takeFirst().at(0).toString(), QString("connect"));
        QCOMPARE(context->validateProvisioning(operatorString, mcc, mnc),true);

        context->setAccessPointName("connectcap");
        QTRY_COMPARE(apn.count(), 1);
        QCOMPARE(apn.takeFirst().at(0).toString(), QString("connectcap"));
        QCOMPARE(context->validateProvisioning(operatorString, mcc, mnc),true);

        context->setAccessPointName("test");
        QTRY_COMPARE(apn.count(), 1);
        QCOMPARE(apn.takeFirst().at(0).toString(), QString("test"));
        QCOMPARE(context->validateProvisioning(operatorString, mcc, mnc),false);
    }

private:
    QOfonoConnectionManager *m;
};

QTEST_MAIN(TestQOfonoConnectionManagerContext)
#include "tst_qofonoconnmancontext.moc"
