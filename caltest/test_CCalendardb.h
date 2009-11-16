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
#include <sqlite3.h>
#include <sys/time.h>
#include <ctime>

//#include "dbcalls.h"
#include "CCalendarDB.h"

using namespace std;

class CCalendarDB_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CCalendarDB_TS1 );
	CPPUNIT_TEST(test_createUpdateDB);
	CPPUNIT_TEST(test_createTenTables);
	CPPUNIT_TEST(test_insertUpdateDB);
	CPPUNIT_TEST(test_invalidInsertUpdateDB);
	CPPUNIT_TEST(test_modifyUpdateDB);
	CPPUNIT_TEST(test_tenmodifications);
	CPPUNIT_TEST(test_modification7);
	CPPUNIT_TEST(test_invalidColModifyUpdateDB);
	CPPUNIT_TEST(test_deleteUpdateDB);
	CPPUNIT_TEST(test_invalidColDeleteUpdateDB);
	CPPUNIT_TEST(test_validGetRecords);
	CPPUNIT_TEST(test_timeForGetRecords);
//	CPPUNIT_TEST(test_invalidGetRecords);
//	CPPUNIT_TEST(test_nullGetRecords);
	CPPUNIT_TEST(test_commitDB);
	CPPUNIT_TEST(test_rollbackDB);
	CPPUNIT_TEST(test_validInsertRows);
	CPPUNIT_TEST(test_invalidInsertRows);
	CPPUNIT_TEST(test_insertTenRows);
	CPPUNIT_TEST(test_insertTenRowsV);
	CPPUNIT_TEST(test_sqliteErrorMapper);
	CPPUNIT_TEST(test_sendDBusMessage);
#if 0
	/*100,500,1000,5000,10000 cases*/
	CPPUNIT_TEST(test_createHunTables);
	CPPUNIT_TEST(test_createFiveHunTables);
	CPPUNIT_TEST(test_createThouTables);
	CPPUNIT_TEST(test_createFiveThouTables);
	CPPUNIT_TEST(test_createTenThouTables);
	CPPUNIT_TEST(test_hunmodifications);
	CPPUNIT_TEST(test_fivehunmodifications);
	CPPUNIT_TEST(test_thoumodifications);
	CPPUNIT_TEST(test_fivethoumodifications);
	CPPUNIT_TEST(test_tenthoumodifications);
	CPPUNIT_TEST(test_insertHunRows);
	CPPUNIT_TEST(test_insertFiveHunRows);
	CPPUNIT_TEST(test_insertThouRows);
	CPPUNIT_TEST(test_insertFiveThouRows);
	CPPUNIT_TEST(test_insertTenThouRows);
	CPPUNIT_TEST(test_insertHunRowsV);
	CPPUNIT_TEST(test_insertFiveHunRowsV);
	CPPUNIT_TEST(test_insertThouRowsV);
	CPPUNIT_TEST(test_insertFiveThouRowsV);
	CPPUNIT_TEST(test_insertTenThouRowsV);
	CPPUNIT_TEST(test_modification67);
	CPPUNIT_TEST(test_modification473);
	CPPUNIT_TEST(test_modification793);
#endif
	CPPUNIT_TEST_SUITE_END();
	public:
		CCalendarDB *pCdb;
		void setUp();
		void tearDown();
		void test_createUpdateDB();
		void test_createTenTables();
		void test_createHunTables();
		void test_createFiveHunTables();
		void test_createThouTables();
		void test_createFiveThouTables();
		void test_createTenThouTables();
		void test_insertUpdateDB();
		void test_invalidInsertUpdateDB();
		void test_modifyUpdateDB();
		void test_tenmodifications();
		void test_hunmodifications();
		void test_fivehunmodifications();
		void test_thoumodifications();
		void test_fivethoumodifications();
		void test_tenthoumodifications();
		void test_modification7();
		void test_modification67();
		void test_modification473();
		void test_modification793();
		void test_invalidColModifyUpdateDB();
		void test_deleteUpdateDB();
		void test_invalidColDeleteUpdateDB();
		void test_validGetRecords();
		void test_timeForGetRecords();
		void test_invalidGetRecords();
		void test_nullGetRecords();
		void test_commitDB();
		void test_rollbackDB();
		void test_validInsertRows();
		void test_invalidInsertRows();
		void test_insertTenRows();
		void test_insertHunRows();
		void test_insertFiveHunRows();
		void test_insertThouRows();
		void test_insertFiveThouRows();
		void test_insertTenThouRows();
		void test_insertTenRowsV();
		void test_insertHunRowsV();
		void test_insertFiveHunRowsV();
		void test_insertThouRowsV();
		void test_insertFiveThouRowsV();
		void test_insertTenThouRowsV();
		void test_sqliteErrorMapper();
		void test_destroy();

		void test_sendDBusMessage();
};
