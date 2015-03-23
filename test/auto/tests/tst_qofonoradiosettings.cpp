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

#include "qofonoradiosettings.h"

class TestQOfonoRadioSettings : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoRadioSettings(this);
        m->setModemPath("/phonesim");
        QTRY_VERIFY(m->isValid());
    }

    void testQOfonoRadioSettings()
    {
        QCOMPARE(m->technologyPreference(), QString("any"));
    }

    void cleanupTestCase()
    {

    }

private:
    QOfonoRadioSettings *m;
};

QTEST_MAIN(TestQOfonoRadioSettings)
#include "tst_qofonoradiosettings.moc"
