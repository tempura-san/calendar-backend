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
#include "CCalendarDB.h"
#include "CCalendarProcs.h"
#include "CCalendar.h"

using namespace std;


class CCalendarProcs_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CCalendarProcs_TS1 );

    CPPUNIT_TEST(test_Init);
    CPPUNIT_TEST(test_AddBDay);
    CPPUNIT_TEST(test_DropBDays);
    CPPUNIT_TEST(test_DelBDay);
    CPPUNIT_TEST(test_GetBDays);
    CPPUNIT_TEST(test_GetBDayEvents);
    CPPUNIT_TEST(test_GetComponentsAllCalendars_repeating_events);
    CPPUNIT_TEST(test_GetComponentsAllCalendars_all_events);
    CPPUNIT_TEST_SUITE_END();

    public:
        CCalendarProcs *pProcs;
        void setUp();
        void tearDown();

        void test_Init();
        void test_DropBDays();
        void test_AddBDay();
        void test_DelBDay();
        void test_GetBDays();
        void test_GetBDayEvents();
        void test_GetComponentsAllCalendars_repeating_events();
        void test_GetComponentsAllCalendars_all_events();


    CCalendar *pBDayCalendar;
    std::string BDayCalendarId;

    void addTestBDayEvents();
};
