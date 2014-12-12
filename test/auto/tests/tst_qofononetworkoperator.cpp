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

#include "qofononetworkregistration.h"
#include "qofononetworkoperator.h"

// Do it here to not break API compatibility - also see qRegisterMetaType below
Q_DECLARE_METATYPE(QOfonoNetworkOperator::Error)

class TestQOfonoNetworkOperator : public QObject
{
    Q_OBJECT

public:
    TestQOfonoNetworkOperator()
    {
        qRegisterMetaType<QOfonoNetworkOperator::Error>();
    }

private slots:
    void initTestCase()
    {
        m = new QOfonoNetworkRegistration(this);
        m->setModemPath("/phonesim");
        QTRY_VERIFY(m->isValid());
    }

    void testQOfonoNetworkOperator()
    {
        QSignalSpy scanFinished(m, SIGNAL(scanFinished()));
        QSignalSpy networkOperatorsChanged(m, SIGNAL(networkOperatorsChanged(QStringList)));
        m->scan();
        QTRY_COMPARE(scanFinished.count(), 1);
        scanFinished.takeFirst();
        QTRY_COMPARE(networkOperatorsChanged.count(), 1);
        QStringList opIdList = networkOperatorsChanged.takeFirst().at(0).toStringList();
        QVERIFY(opIdList.count() > 0);

        int op1 = -1;
        int op2 = -1;
        QList<QOfonoNetworkOperator *> opList;
        foreach(QString opId, opIdList)
        {
            QOfonoNetworkOperator *op = new QOfonoNetworkOperator(this);
            op->setOperatorPath(opId);
            QTRY_VERIFY(op->isValid());
            opList << op;
            if (op1 == -1 && op->status() == "current")
                op1 = opIdList.indexOf(opId);
            if (op2 == -1 && op->status() == "available")
                op2 = opIdList.indexOf(opId);
        }

        QVERIFY(op1 != -1);
        QVERIFY(op2 != -1);
        QVERIFY(opList[op1]->name().length() > 0);
        QVERIFY(opList[op2]->name().length() > 0);
        QVERIFY(opList[op1]->mcc().length() > 0);
        QVERIFY(opList[op2]->mcc().length() > 0);
        QVERIFY(opList[op1]->mnc().length() > 0);
        QVERIFY(opList[op2]->mnc().length() > 0);
        QVERIFY(opList[op1]->technologies().count() > 0);
        QVERIFY(opList[op2]->technologies().count() > 0);

        QSignalSpy op1Register(opList[op1],
                SIGNAL(registerComplete(QOfonoNetworkOperator::Error, QString)));
        QSignalSpy op2Register(opList[op2],
                SIGNAL(registerComplete(QOfonoNetworkOperator::Error, QString)));
        QSignalSpy op1Status(opList[op1], SIGNAL(statusChanged(QString)));
        QSignalSpy op2Status(opList[op2], SIGNAL(statusChanged(QString)));

        //QSignalSpy mcc(m, SIGNAL(mccChanged(QString)));
        QSignalSpy mnc(m, SIGNAL(mncChanged(QString)));
        QSignalSpy name(m, SIGNAL(nameChanged(QString)));

        opList[op2]->registerOperator();
        QTRY_COMPARE(op2Register.count(), 1);
        QCOMPARE(op2Register.takeFirst().at(0).value<QOfonoNetworkOperator::Error>(),
                QOfonoNetworkOperator::NoError);
        QTRY_COMPARE(op1Status.count(), 1);
        QCOMPARE(op1Status.takeFirst().at(0).toString(), QString("available"));
        QTRY_COMPARE(op2Status.count(), 1);
        QCOMPARE(op2Status.takeFirst().at(0).toString(), QString("current"));
        //QTRY_COMPARE(mcc.count(), 1); // both OPs have the same MCC
        QTRY_COMPARE(mnc.count(), 1);
        mnc.takeFirst();
        QTRY_COMPARE(name.count(), 1);
        name.takeFirst();

        opList[op1]->registerOperator();
        QTRY_COMPARE(op1Register.count(), 1);
        QCOMPARE(op1Register.takeFirst().at(0).value<QOfonoNetworkOperator::Error>(),
                QOfonoNetworkOperator::NoError);
        QTRY_COMPARE(op1Status.count(), 1);
        QCOMPARE(op1Status.takeFirst().at(0).toString(), QString("current"));
        QTRY_COMPARE(op2Status.count(), 1);
        QCOMPARE(op2Status.takeFirst().at(0).toString(), QString("available"));
        //QTRY_COMPARE(mcc.count(), 1); // both OPs have the same MCC
        QTRY_COMPARE(mnc.count(), 1);
        mnc.takeFirst();
        QTRY_COMPARE(name.count(), 1);
        name.takeFirst();

        qDeleteAll(opList);
    }

    void cleanupTestCase()
    {

    }

private:
    QOfonoNetworkRegistration *m;
};

QTEST_MAIN(TestQOfonoNetworkOperator)
#include "tst_qofononetworkoperator.moc"
