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


#include "CTodo.h"
#include "Common.h"


class CTodo_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CTodo_TS1);
    CPPUNIT_TEST(test_SetGeo_Valid);
    CPPUNIT_TEST(test_SetGeo_Empty);
    CPPUNIT_TEST(test_SetDue_Valid);
    CPPUNIT_TEST(test_SetCompleted_Valid);
    CPPUNIT_TEST(test_SetPercentComplete_Negative);
    CPPUNIT_TEST(test_SetPercentComplete_Valid);
    CPPUNIT_TEST(test_SetPercentComplete_GTHundred);
    CPPUNIT_TEST(test_SetPriority_Negative);
    CPPUNIT_TEST(test_SetPriority_Valid);
    CPPUNIT_TEST(test_SetPriority_Inrange);
    CPPUNIT_TEST_SUITE_END();
public:
    CTodo *pCt;
    void setUp();
    void tearDown();
    void test_SetGeo_Valid();
    void test_SetGeo_Empty();
    void test_SetDue_Valid();
    void test_SetCompleted_Valid();
    void test_SetPercentComplete_Negative();
    void test_SetPercentComplete_Valid();
    void test_SetPercentComplete_GTHundred();
    void test_SetPriority_Negative();
    void test_SetPriority_Valid();
    void test_SetPriority_Inrange();

};
class ValidCTodoConstructorTestCase : public CppUnit::TestFixture
{
public:
    CTodo *p;
    void setUp();
    void tearDown();
    void test_validCTodoConstructor();

};

class CTodo_TS2 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CTodo_TS2);
    CPPUNIT_TEST(test_toString);
    CPPUNIT_TEST(test_toString_NULL);
    CPPUNIT_TEST(test_toString_flags);
    CPPUNIT_TEST_SUITE_END();
public:
    CTodo *pCt;
    void setUp();
    void tearDown();
    void test_toString();
    void test_toString_NULL();
    void test_toString_flags();
};

