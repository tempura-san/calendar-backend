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

#ifndef __CTIMEZONE_TEST_H__
#define __CTIMEZONE_TEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include <string>

#include "CTimezone.h"

class CTimezone_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CTimezone_TS1 );
    CPPUNIT_TEST(test_guessInvalidTZ);
    CPPUNIT_TEST(test_guessZeroes);
    CPPUNIT_TEST(test_getByLocation_invalidLocation);
    CPPUNIT_TEST(test_getByLocation_validLocations);
    CPPUNIT_TEST(test_getLocation_wrong_location);
    CPPUNIT_TEST(test_getLocation_somelocation);
    // CPPUNIT_TEST(test_guessInvalidTZWithRules);

    CPPUNIT_TEST_SUITE_END();

public:
    void test_guessInvalidTZ();
    void test_guessInvalidTZWithRules();
    void test_guessZeroes();
    void test_getByLocation_invalidLocation();
    void test_getByLocation_validLocations();
    void test_getLocation_wrong_location();
    void test_getLocation_somelocation();

private:
    void check_timezone(time_t StartStd,
                        time_t StartDst,
                        time_t OffsetStd,
                        time_t OffsetDst,
                        std::string RuleStd,
                        std::string RuleDst);

    void check_byLocation(std::string Location);
};

#endif // #ifndef __CTIMEZONE_TEST_H__
