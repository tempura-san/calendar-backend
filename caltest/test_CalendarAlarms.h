/*
 * This file is part of calendar-backend
 *
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ilias Biris <ilias.biris@nokia.com>
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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>
#include <iostream>


#include "CAlarm.h"

class CalendarAlarms_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CalendarAlarms_TS1);
	CPPUNIT_TEST(test_alarminit);
	CPPUNIT_TEST(test_setAlarm);
	CPPUNIT_TEST(test_modifyAlarm);
	CPPUNIT_TEST(test_deleteAlarm);
	CPPUNIT_TEST(test_deleteAlarmInvalid);
	CPPUNIT_TEST(test_registerAlarmForAutoDeletion);
	CPPUNIT_TEST_SUITE_END();

	public:
	CAlarm *pCm;
	CMulticalendar *mc;
	void setUp();
	void tearDown();
	void test_alarminit();
	void test_setAlarm();
	void test_modifyAlarm();
	void test_deleteAlarm();
	void test_deleteAlarmInvalid();
	void test_registerAlarmForAutoDeletion();
};

