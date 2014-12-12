/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2012 Jolla
 *
 * Contact: Lorn Potter <lorn.potter@jollamobile.com>
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

#include "qofonocellbroadcast.h"

class TestQOfonoCellBroadcast : public QObject
{
    Q_OBJECT

    static const int INTERACTIVE_STEP_TIMEOUT = 30000;

private slots:
    void initTestCase()
    {
        m = new QOfonoCellBroadcast( this);
        m->setModemPath("/phonesim");
        QTRY_VERIFY(m->isValid());
    }

    void testQOfonoCellBroadcast()
    {
        QSignalSpy inBroadcast(m, SIGNAL(incomingBroadcast( QString ,quint16)));
        //QSignalSpy emBroadcast(m, SIGNAL(emergencyBroadcast( QString , QVariantMap)));
        QSignalSpy topicsSpy(m, SIGNAL(topicsChanged(QString)));

        qDebug() << "Please send CBM using phonesim";
        //QCOMPARE(emBroadcast.count(), 1);
        QTRY_COMPARE_WITH_TIMEOUT(inBroadcast.count(), 1, INTERACTIVE_STEP_TIMEOUT);

        QString topicsList = "20,50-51,60";
        m->setTopics("");
        QTRY_COMPARE(topicsSpy.count(), 1);
        QCOMPARE(topicsSpy.takeFirst().at(0).toString(), QString());
        m->setTopics(topicsList);
        QTRY_COMPARE(topicsSpy.count(), 1);
        QCOMPARE(topicsSpy.takeFirst().at(0).toString(), topicsList);
    }

private:
    QOfonoCellBroadcast *m;
};

QTEST_MAIN(TestQOfonoCellBroadcast)
#include "tst_qofonocellbroadcast.moc"
