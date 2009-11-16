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

#include "CRecurrenceRule.h"

class CRecurrenceRule_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CRecurrenceRule_TS1 );
	CPPUNIT_TEST(test_RuleType);
	CPPUNIT_TEST(test_RRule);
	CPPUNIT_TEST(test_frequency);
	CPPUNIT_TEST(test_Until);
	CPPUNIT_TEST(test_count);
	CPPUNIT_TEST(test_second);
	CPPUNIT_TEST(test_minute);
	CPPUNIT_TEST(test_hour);
	CPPUNIT_TEST(test_weekday);
	CPPUNIT_TEST(test_monthday);
	CPPUNIT_TEST(test_yearday);
	CPPUNIT_TEST(test_weeknumber);
	CPPUNIT_TEST(test_month);
	CPPUNIT_TEST(test_position);
	CPPUNIT_TEST(test_WeekStart);
	CPPUNIT_TEST(test_interval);
	CPPUNIT_TEST(test_rruleParser);
	CPPUNIT_TEST(test_operator);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		CRecurrenceRule * pCrr;
		void setUp();
		void tearDown();
		void test_RuleType();
		void test_RRule();
		void test_frequency();
		void test_Until();
		void test_count();
		void test_second();
		void test_minute();
		void test_hour();
		void test_weekday();
		void test_monthday();
		void test_yearday();
		void test_weeknumber();
		void test_month();
		void test_position();
		void test_WeekStart();
		void test_interval();
		void test_rruleParser();
		void test_operator();

};

class CRecurrenceRule_TS2 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CRecurrenceRule_TS2 );
	CPPUNIT_TEST(test_toString);
	CPPUNIT_TEST(test_toString_NULL);
		CPPUNIT_TEST_SUITE_END();
	
	public:
		CRecurrenceRule * pCrr;
		void setUp();
		void tearDown();
		void test_toString();
		void test_toString_NULL();
};

