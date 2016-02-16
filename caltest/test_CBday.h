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

#include "CBdayEvent.h"

class CBday_TS1: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE( CBday_TS1 );
    CPPUNIT_TEST(test_setEBookUid);
    CPPUNIT_TEST(test_invalidsetEBookUid);
    CPPUNIT_TEST(test_setFirstName);
    CPPUNIT_TEST(test_invalidsetFirstName);
    CPPUNIT_TEST(test_setBirthDate);
    CPPUNIT_TEST(test_invalidsetBirthDate);
    CPPUNIT_TEST(test_defaultConstructor);
    CPPUNIT_TEST(test_paramConstructor);
    CPPUNIT_TEST_SUITE_END();

public:
    CBdayEvent *bd;
    void setUp();
    void tearDown();
    void test_setEBookUid();
    void test_invalidsetEBookUid();
    void test_setFirstName();
    void test_invalidsetFirstName();
    void test_setBirthDate();
    void test_invalidsetBirthDate();
    void test_paramConstructor();
    void test_defaultConstructor();
};

class CBday_TS2: public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE( CBday_TS2 );
    CPPUNIT_TEST(test_toString);
    CPPUNIT_TEST(test_toString_NULL);
    CPPUNIT_TEST_SUITE_END();

public:
    CBdayEvent *bd;
    void setUp();
    void tearDown();
    void test_toString();
    void test_toString_NULL();
};
