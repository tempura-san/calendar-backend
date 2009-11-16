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
#include <string>

#include "CRecurrence.h"

class CRecurrence_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CRecurrence_TS1 );
	CPPUNIT_TEST(test_setRecurId_Valid);
	CPPUNIT_TEST(test_setRrule_Valid);
	CPPUNIT_TEST(test_setErule_Valid);
	CPPUNIT_TEST(test_setRDays_Valid);
	CPPUNIT_TEST(test_setEDays_Valid);
	CPPUNIT_TEST(test_generateInstanceTimes);
	CPPUNIT_TEST(test_setRecurrenceRule);
	CPPUNIT_TEST(test_Recurrence);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		CRecurrence * pCr;
		void setUp();
		void tearDown();
		void test_setRecurId_Valid();
		void test_setRrule_Valid();
		void test_setErule_Valid();
		void test_setRDays_Valid();
		void test_setEDays_Valid();
		void test_generateInstanceTimes();
		void test_setRecurrenceRule();
		void test_Recurrence();
};


class CRecurrence_TS2 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CRecurrence_TS2 );
	CPPUNIT_TEST(test_toString);
	CPPUNIT_TEST(test_toString_NULL);
	CPPUNIT_TEST_SUITE_END();
	public:
	CRecurrence * pCr;
	void setUp();
	void tearDown();
	void test_toString();
	void test_toString_NULL();
};

