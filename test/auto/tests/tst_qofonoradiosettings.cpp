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

#include <ofonoradiosettings.h>

#include <QtDebug>


class TestOfonoRadioSettings : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
	m = new OfonoRadioSettings(OfonoModem::ManualSelect, "/phonesim", this);
	QCOMPARE(m->modem()->isValid(), true);	

	if (!m->modem()->powered()) {
  	    m->modem()->setPowered(true);
            QTest::qWait(5000);
        }
        if (!m->modem()->online()) {
  	    m->modem()->setOnline(true);
            QTest::qWait(5000);
        }
        qDebug() << "FIXME: radio settings interface is not supported by AT modems, and consequently, phonesim";
	QCOMPARE(m->isValid(), true);    
    }

    void testOfonoRadioSettings()
    {
        QSignalSpy preference(m, SIGNAL(technologyPreferenceChanged(QString)));
        QSignalSpy setPreferenceFailed(m, SIGNAL(setTechnologyPreferenceFailed()));

	qDebug() << "technologyPreference():" << m->technologyPreference();
    }


    void cleanupTestCase()
    {

    }


private:
    OfonoRadioSettings *m;
};

QTEST_MAIN(TestOfonoRadioSettings)
#include "test_ofonoradiosettings.moc"
