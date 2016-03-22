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

class CAlarm_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CAlarm_TS1);
    CPPUNIT_TEST(test_setTrigger_Valid);
    CPPUNIT_TEST(test_setTrigger_Invalid);
    CPPUNIT_TEST(test_setRepeat_Valid);
    CPPUNIT_TEST(test_setRepeat_Invalid);
    CPPUNIT_TEST(test_setDuration_Valid);
    CPPUNIT_TEST(test_setDuration_Invalid);
    CPPUNIT_TEST(test_setAction_Valid);
    CPPUNIT_TEST(test_setCookie_Valid);
    CPPUNIT_TEST(test_setCookie_Invalid);
    CPPUNIT_TEST(test_setAttach_Valid);
    CPPUNIT_TEST(test_setAttach_Empty);
    CPPUNIT_TEST(test_constructor);
    CPPUNIT_TEST_SUITE_END();

public:
    CAlarm *pCa;
    void setUp();
    void tearDown();
    void test_setTrigger_Valid();
    void test_setTrigger_Invalid();
    void test_setRepeat_Valid();
    void test_setRepeat_Invalid();
    void test_setDuration_Valid();
    void test_setDuration_Invalid();
    void test_setAction_Valid();
    void test_setCookie_Valid();
    void test_setCookie_Invalid();
    void test_setAttach_Valid();
    void test_setAttach_Empty();
    void test_constructor();
};

class CAlarm_TS2 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CAlarm_TS2);

    CPPUNIT_TEST(test_toString);
    CPPUNIT_TEST(test_toString_NULL);
    CPPUNIT_TEST_SUITE_END();

public:
    CAlarm *pCa1;
    void setUp();
    void tearDown();
    void test_toString();
    void test_toString_NULL();
};

