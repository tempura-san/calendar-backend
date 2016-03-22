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

#include "CComponentDetails.h"
#include "CCalendar.h"
#include "CMulticalendar.h"

class ComponentDetails_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ComponentDetails_TS1);
    CPPUNIT_TEST(test_setCreatedTimeValid);
    CPPUNIT_TEST(test_setDateStampZero);
    CPPUNIT_TEST(test_setDateStampValid);
    CPPUNIT_TEST(test_setOrganizer_Null);
    CPPUNIT_TEST(test_setOrganizer_Valid);
    CPPUNIT_TEST(test_setHashMap_Valid);
    CPPUNIT_TEST(test_setAttachments_Valid);
    CPPUNIT_TEST(test_setAttendees_Valid);
    CPPUNIT_TEST(test_setSequence_Valid);
    CPPUNIT_TEST(test_setSequence_Invalid);
    CPPUNIT_TEST(test_setUid_Valid);
    CPPUNIT_TEST(test_setUid_Invalid);
    CPPUNIT_TEST(test_setUrl_Valid);
    CPPUNIT_TEST(test_setUrl_Empty);
    CPPUNIT_TEST(test_setCategories_Valid);
    CPPUNIT_TEST(test_setCategories_Empty);
    CPPUNIT_TEST(test_setContact_Valid);
    CPPUNIT_TEST(test_setContact_Empty);
    CPPUNIT_TEST(test_setComments_Valid);
    CPPUNIT_TEST(test_setComments_Empty);
    CPPUNIT_TEST(test_setRelated_Valid);
    CPPUNIT_TEST(test_setRelated_Empty);
    CPPUNIT_TEST(test_setResources_Valid);
    CPPUNIT_TEST(test_setResources_Empty);
    CPPUNIT_TEST(test_setXProperties_Valid);
    CPPUNIT_TEST(test_setClas_Valid);
    CPPUNIT_TEST(test_retrieveOrganizerDetails);
    CPPUNIT_TEST(test_retrieveAttendeeDetails);
    CPPUNIT_TEST(test_retrieveXPropertyDetails);
    CPPUNIT_TEST(test_retrieveParameterDetails);
    CPPUNIT_TEST_SUITE_END();

public:
    CMulticalendar *multi;
    CCalendar *calendar;
    CComponentDetails *pCd;
    void setUp();
    void tearDown();
    void test_setCreatedTimeValid();
    void test_setDateStampZero();
    void test_setDateStampValid();
    void test_setOrganizer_Null();
    void test_setOrganizer_Valid();
    void test_setAttachments_Valid();
    void test_setAttendees_Valid();
    void test_setHashMap_Valid();
    void test_setSequence_Valid();
    void test_setSequence_Invalid();
    void test_setUid_Valid();
    void test_setUid_Invalid();
    void test_setUrl_Valid();
    void test_setUrl_Empty();
    void test_setCategories_Valid();
    void test_setCategories_Empty();
    void test_setContact_Valid();
    void test_setContact_Empty();
    void test_setComments_Valid();
    void test_setComments_Empty();
    void test_setRelated_Valid();
    void test_setRelated_Empty();
    void test_setResources_Valid();
    void test_setResources_Empty();
    void test_setXProperties_Valid();
    void test_setClas_Valid();
    void test_setClas_Empty();
    void test_retrieveOrganizerDetails();
    void test_retrieveAttendeeDetails();
    void test_retrieveXPropertyDetails();
    void test_retrieveParameterDetails();
};


class ComponentDetails_TS2 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ComponentDetails_TS2);
    CPPUNIT_TEST(test_toString);
    CPPUNIT_TEST(test_toString_NULL);
    CPPUNIT_TEST(test_toString_flags);
    CPPUNIT_TEST_SUITE_END();

public:
    CMulticalendar *multi;
    CCalendar *calendar;

    CComponentDetails *pCd;
    void setUp();
    void tearDown();
    void test_toString();
    void test_toString_NULL();
    void test_toString_flags();
};


