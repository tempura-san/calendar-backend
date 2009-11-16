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
#include "CParticipant.h"
#include "COrganizer.h"
#include <iostream>

class CParticipant_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CParticipant_TS1 );
	CPPUNIT_TEST(test_SetLanguage_Valid);
	CPPUNIT_TEST(test_SetSentBy_Valid);
	CPPUNIT_TEST(test_SetSentBy_Empty);
	CPPUNIT_TEST(test_SetXParameters_Valid);
	CPPUNIT_TEST(test_SetXParameters_Empty);
	CPPUNIT_TEST(test_SetDirectoryParameter_Valid);
	CPPUNIT_TEST(test_SetDirectoryParameter_Empty);
	CPPUNIT_TEST(test_SetCalAddress_Valid);
	CPPUNIT_TEST(test_SetCommonName_Valid);
	CPPUNIT_TEST(test_MailTo);
	CPPUNIT_TEST_SUITE_END();

	public:
		CParticipant * pCp;
		void setUp();
		void tearDown();
		void test_SetLanguage_Valid();
		void test_SetSentBy_Valid();
		void test_SetSentBy_Empty();
		void test_SetXParameters_Valid();
		void test_SetXParameters_Empty();
		void test_SetDirectoryParameter_Valid();
		void test_SetDirectoryParameter_Empty();
		void test_SetCalAddress_Valid();
		void test_SetCommonName_Valid();
		void test_MailTo();
};

class ValidCParticipantConstructorTestCase : public CppUnit::TestFixture
{
        public:
             CParticipant* p;
             void setUp();
             void tearDown();
             void test_validCParticipantConstructor();

};

class CParticipant_TS2 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CParticipant_TS2 );
	CPPUNIT_TEST(test_toString);
	CPPUNIT_TEST(test_toString_NULL);
		CPPUNIT_TEST_SUITE_END();
	
	public:
		CParticipant * pCp;
		void setUp();
		void tearDown();
		void test_toString();
		void test_toString_NULL();
};

