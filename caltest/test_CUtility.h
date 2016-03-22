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
#include "CUtility.h"

using namespace std;


class CUtility_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CUtility_TS1);

    CPPUNIT_TEST(test_getDateFromTime);
    CPPUNIT_TEST(test_parseId);
    CPPUNIT_TEST(test_stringReplaceOne);
    CPPUNIT_TEST(test_encodeQuotedPrintable);
    CPPUNIT_TEST(test_getSystemTimeZone);
    CPPUNIT_TEST(test_getRecurrentTimes);
    CPPUNIT_TEST(test_getApplicationName);
    CPPUNIT_TEST_SUITE_END();

public:
    CUtility *pUt;
    void setUp();
    void tearDown();
    void test_getDateFromTime();
    void test_stringReplaceOne();
    void test_encodeQuotedPrintable();
    void test_getSystemTimeZone();
    void test_getRecurrentTimes();
    void test_parseId();
    void test_getApplicationName();
};
