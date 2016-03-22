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

#include "CComponent.h"
#include "Common.h"
#include "CalendarErrors.h"

using namespace std;

/*Valid Location Test Case*/
class CComponent_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CComponent_TS1);
    CPPUNIT_TEST(test_validLocation);
    CPPUNIT_TEST(test_validSummary);
    CPPUNIT_TEST(test_emptySummary);
    CPPUNIT_TEST(test_validModifiedTime);
    CPPUNIT_TEST(test_Status);
    CPPUNIT_TEST(test_Type);
    CPPUNIT_TEST(test_Id);
    CPPUNIT_TEST(test_Description);
    CPPUNIT_TEST(test_emptyDescription);
    CPPUNIT_TEST(test_validDateStart);
    CPPUNIT_TEST(test_validDateEnd);

    CPPUNIT_TEST(test_retrieveRecurreceRuleObject_invalid);
    CPPUNIT_TEST(test_getInstanceNumber_invalid);
    CPPUNIT_TEST(test_getRecurrenceProperties_invalid);
    CPPUNIT_TEST(test_generateInstanceTimes_invalid);
    CPPUNIT_TEST(test_setAlarm);
    CPPUNIT_TEST(test_getAlarmProperties);
    CPPUNIT_TEST(test_getDuration);
    CPPUNIT_TEST(test_setRecurrence);
    CPPUNIT_TEST(test_updateUntilInDB);
    CPPUNIT_TEST(test_retrieveRecurreceRuleObject);
    CPPUNIT_TEST(test_copyConstructor);
    CPPUNIT_TEST_SUITE_END();

public:
    CComponent *pCc;
    void setUp();
    void tearDown();
    void test_validLocation();
    void test_validSummary();
    void test_emptySummary();
    void test_validModifiedTime();
    void test_Status();
    void test_Type();
    void test_Id();
    void test_Description();
    void test_emptyDescription();
    void test_validDateStart();
    void test_validDateEnd();

    void test_getRecurrenceProperties_invalid();
    void test_retrieveRecurreceRuleObject_invalid();
    void test_getInstanceNumber_invalid();
    void test_generateInstanceTimes_invalid();
    void test_setAlarm();
    void test_getAlarmProperties();
    void test_getDuration();
    void test_setRecurrence();
    void test_updateUntilInDB();
    void test_retrieveRecurreceRuleObject();
    void test_copyConstructor();
};

class CComponent_TS2 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CComponent_TS2);
    CPPUNIT_TEST(test_toString);
    CPPUNIT_TEST(test_toString_flags);
    CPPUNIT_TEST(test_toString_NULL);
    CPPUNIT_TEST_SUITE_END();

public:
    CComponent *pCc;
    void setUp();
    void tearDown();
    void test_toString();
    void test_toString_flags();
    void test_toString_NULL();
};

