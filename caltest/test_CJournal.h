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
#include "CJournal.h"
#include "COrganizer.h"
#include "CRecurrence.h"
#include "CAlarm.h"
#include "Common.h"

#include <iostream>

class CJournal_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CJournal_TS1 );
	CPPUNIT_TEST_SUITE_END();
	public:
};

class ValidCJournalConstructorTestCase : public CppUnit::TestFixture
{
        public:
             CJournal* p;
             void setUp();
             void tearDown();
             void test_validCJournalConstructor();

};

class CJournal_TS2 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CJournal_TS2 );
	CPPUNIT_TEST(test_toString);
	CPPUNIT_TEST(test_toString_NULL);
	CPPUNIT_TEST(test_toString_flags);
	CPPUNIT_TEST_SUITE_END();
	public:
	CJournal* p;
	void setUp();
    void tearDown();
    void test_toString();
    void test_toString_NULL();
    void test_toString_flags();
};

