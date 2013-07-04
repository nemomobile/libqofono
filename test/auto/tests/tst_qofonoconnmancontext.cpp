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
#include <QtCore/QObject>

#include <qofono/qofonoconnectionmanager.h>
#include <qofono/qofonoconnectioncontext.h>

#include <QtDebug>


class TestQOfonoConnectionManagerContext : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoConnectionManager(this);
        m->setModemPath("/phonesim");
        QCOMPARE(m->isValid(), true);
    }

    void testQOfonoConnectionManagerContext ()
    {
//        QSignalSpy addcon(m,SIGNAL(addContextComplete(bool, const QString&)));
        QSignalSpy conadd(m, SIGNAL(contextAdded(const QString&)));
        QSignalSpy conrem(m, SIGNAL(contextRemoved(const QString&)));

        m->addContext("internet");
        QTest::qWait(1000);

//        QCOMPARE(addcon.count(), 1);
//        QVariantList list = addcon.takeFirst();
//        QCOMPARE(list.at(0).toBool(),true);
        QCOMPARE(conadd.count(), 1);
        QString contextid = conadd.takeFirst().at(0).toString();
        QCOMPARE(contextid, m->contexts().at(1));

        QOfonoConnectionContext* context = new QOfonoConnectionContext(this);
        context->setContextPath(contextid);

        QSignalSpy active(context, SIGNAL(activeChanged(const bool)));
        QSignalSpy apn(context,SIGNAL(accessPointNameChanged(const QString&)));
        QSignalSpy name(context, SIGNAL(nameChanged(const QString&)));
        QSignalSpy type (context, SIGNAL(typeChanged(const QString&)));
        QSignalSpy uname (context, SIGNAL(usernameChanged(const QString&)));
        QSignalSpy pw (context, SIGNAL(passwordChanged(const QString&)));
        QSignalSpy proto (context, SIGNAL(protocolChanged(const QString&)));
        QSignalSpy sett (context, SIGNAL(settingsChanged(const QVariantMap&)));
        QSignalSpy sett6 (context, SIGNAL(IPv6SettingsChanged(const QVariantMap&)));

        context->setAccessPointName("hyva");
        QTest::qWait(5000);
        context->setUsername("huomenta");
        QTest::qWait(5000);
        context->setPassword("HYVA");
        QTest::qWait(5000);
        context->setName("yota");
        QTest::qWait(5000);
        context->setType("mms");
        QTest::qWait(5000);
        context->setProtocol("ipv6");
        QTest::qWait(5000);

        context->setActive(true);
        QTest::qWait(10000);

        QCOMPARE(apn.count(),1);
        QCOMPARE(apn.takeFirst().at(0).toString(),QString("hyva"));
        QCOMPARE(uname.count(),1);
        QCOMPARE(uname.takeFirst().at(0).toString(),QString("huomenta"));
        QCOMPARE(pw.count(),1);
        QCOMPARE(pw.takeFirst().at(0).toString(),QString("HYVA"));
        QCOMPARE(name.count(),1);
        QCOMPARE(name.takeFirst().at(0).toString(),QString("yota"));
        QCOMPARE(type.count(),1);
        QCOMPARE(type.takeFirst().at(0).toString(),QString("mms"));
        QCOMPARE(sett.count(),0);
        QCOMPARE(sett6.count(),1);
        QVariantMap settings = context->IPv6Settings();
        QCOMPARE(settings["Interface"].value<QString>().left(5),QString("dummy")); // "dummy" plus number
        QCOMPARE(proto.count(),1);
        QCOMPARE(proto.takeFirst().at(0).toString(),QString("ipv6"));
        QCOMPARE(active.count(),1);
        QCOMPARE(context->active(),true);
        context->setActive(false);
        QTest::qWait(5000);
        delete context;
        QTest::qWait(5000);
        m->removeContext(contextid);
        QTest::qWait(5000);
        QCOMPARE(active.count(),2);
        QCOMPARE(sett.count(),0);
        QCOMPARE(sett6.count(),2);
        QCOMPARE(conrem.count(), 1);

    }

    void cleanupTestCase()
    {

    }

private:
    QOfonoConnectionManager *m;
};

QTEST_MAIN(TestQOfonoConnectionManagerContext)
#include "tst_qofonoconnmancontext.moc"
