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

#include "CEvent.h"
#include "Common.h"
#include "VCalConverter.h"


class VCalConverter_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( VCalConverter_TS1 );
	CPPUNIT_TEST(test_vCalToICalRule1);
	CPPUNIT_TEST(test_vCalToICalRule2);
	CPPUNIT_TEST(test_vCalToICalRule3);
	CPPUNIT_TEST(test_vCalToICalRule4);
	CPPUNIT_TEST(test_iCalToVCalRule1);
	CPPUNIT_TEST(test_iCalToVCalRule2);
	CPPUNIT_TEST(test_iCalToVCalRule3);
	CPPUNIT_TEST(test_iCalToVCalRule4);
	CPPUNIT_TEST(test_iCalToVCalRule5);
	CPPUNIT_TEST(test_vCalToICalDate);
	CPPUNIT_TEST(test_iCalToVCalDate);
	CPPUNIT_TEST(test_vCalToICalAlarm);
	CPPUNIT_TEST(test_iCalToVCalAlarm);
	CPPUNIT_TEST(test_getTokens);
	CPPUNIT_TEST(test_checkContentsTypeVcal);
	CPPUNIT_TEST(test_checkContentsTypeIcal);
	CPPUNIT_TEST(test_VcalRSVP);
	CPPUNIT_TEST(test_getStatusFromVcal);
	CPPUNIT_TEST(test_getVcalStatus);
	CPPUNIT_TEST(test_getTranspFromVcal);
	CPPUNIT_TEST(test_getVcalPartRole);
	CPPUNIT_TEST_SUITE_END();
	public:
		VCalConverter *vCalConv;
		void setUp();
		void tearDown();
		void test_vCalToICalRule1();		
		void test_vCalToICalRule2();		
		void test_vCalToICalRule3();		
		void test_vCalToICalRule4();		
		void test_iCalToVCalRule1();		
		void test_iCalToVCalRule2();		
		void test_iCalToVCalRule3();		
		void test_iCalToVCalRule4();		
		void test_iCalToVCalRule5();		
		void test_vCalToICalDate();		
		void test_iCalToVCalDate();		
		void test_vCalToICalAlarm();		
		void test_iCalToVCalAlarm();		
		void test_checkContentsTypeVcal();		
		void test_checkContentsTypeIcal();		
		void test_getTokens();		
		void test_VcalRSVP();
		void test_getStatusFromVcal();
		void test_getVcalStatus();

		void test_getTranspFromVcal();
		void test_getVcalPartRole();
};
