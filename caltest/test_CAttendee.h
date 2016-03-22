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


#include "CAttendee.h"


class CAttendee_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CAttendee_TS1);
    CPPUNIT_TEST(test_SetCalendarUserType_Valid);
    CPPUNIT_TEST(test_SetMember_Valid);
    CPPUNIT_TEST(test_SetMember_Empty);
    CPPUNIT_TEST(test_SetRole_Valid);
    CPPUNIT_TEST(test_SetDelegator_Valid);
    CPPUNIT_TEST(test_SetDelegator_Empty);
    CPPUNIT_TEST(test_SetParticipationStatus_Valid);
    CPPUNIT_TEST(test_SetDelegatees_Valid);
    CPPUNIT_TEST(test_SetDelegatees_Empty);
    CPPUNIT_TEST(test_SetRSVP_true);
    CPPUNIT_TEST(test_SetRSVP_false);
    CPPUNIT_TEST(test_SetLanguage_Valid);
    CPPUNIT_TEST(test_SetSentBy_Valid);
    CPPUNIT_TEST(test_SetSentBy_Empty);
    CPPUNIT_TEST(test_SetXParameters_Valid);
    CPPUNIT_TEST(test_SetXParameters_Empty);
    CPPUNIT_TEST(test_SetDirectoryParameter_Valid);
    CPPUNIT_TEST(test_SetDirectoryParameter_Empty);
    CPPUNIT_TEST(test_SetCalAddress_Valid);
    CPPUNIT_TEST(test_SetCalAddress_Empty);
    CPPUNIT_TEST(test_SetCommonName_Valid);
    CPPUNIT_TEST_SUITE_END();

public:
    CAttendee *pCa;
    void setUp();
    void tearDown();
    void test_SetCalendarUserType_Valid();
    void test_SetMember_Valid();
    void test_SetMember_Empty();
    void test_SetRole_Valid();
    void test_SetRole_Empty();
    void test_SetDelegator_Valid();
    void test_SetDelegator_Empty();
    void test_SetParticipationStatus_Valid();
    void test_SetParticipationStatus_Empty();
    void test_SetDelegatees_Valid();
    void test_SetDelegatees_Empty();
    void test_SetRSVP_true();
    void test_SetRSVP_false();
    void test_SetLanguage_Valid();
    void test_SetSentBy_Valid();
    void test_SetSentBy_Empty();
    void test_SetXParameters_Valid();
    void test_SetXParameters_Empty();
    void test_SetDirectoryParameter_Valid();
    void test_SetDirectoryParameter_Empty();
    void test_SetCalAddress_Valid();
    void test_SetCalAddress_Empty();
    void test_SetCommonName_Valid();
    void test_SetCommonName_Empty();
};

class ValidCAttendeeConstructorTestCase : public CppUnit::TestFixture
{
public:
    CAttendee *p;
    void setUp();
    void tearDown();
    void test_validCAttendeeConstructor();

};

class CAttendee_TS2 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CAttendee_TS2);
    CPPUNIT_TEST(test_toString);
    CPPUNIT_TEST(test_toString_NULL);
    CPPUNIT_TEST_SUITE_END();

public:
    CAttendee *pCa;
    void setUp();
    void tearDown();
    void test_toString();
    void test_toString_NULL();
};

