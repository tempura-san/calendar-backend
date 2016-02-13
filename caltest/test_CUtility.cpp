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
#include <stdlib.h>
#include <string.h>
#include "test_CUtility.h"
#include "Common.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CUtility_TS1);

void CUtility_TS1::setUp()
{
    pUt = CUtility::Instance();
}

void CUtility_TS1::tearDown()
{
}

void CUtility_TS1::test_getDateFromTime()
{
    struct tm timein;
    struct tm *timeout;
    char temp[DEFAULT_SIZE];

    strptime("11 Nov 2009 12:33:45", "%d %b %Y %H:%M:%S", &timein);
    time_t ret = pUt->getDateFromTime(mktime(&timein));
    timeout = localtime(&ret);
    strftime(temp, DEFAULT_SIZE, "%d %b %Y %H:%M:%S", timeout);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected date value",
            string("11 Nov 2009 00:00:00"), string(temp));
}

void CUtility_TS1::test_parseId()
{
    vector<string> vId;

    vId = pUt->parseIds("");
    CPPUNIT_ASSERT_MESSAGE("Token(s) found for empty string", vId.size() == 0);

    vId = pUt->parseIds("1,2,3,5,9,13");
    CPPUNIT_ASSERT_MESSAGE("Unexpected number of tokens", vId.size() == 6);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected token at [0]", string("1"), vId[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected token at [1]", string("2"), vId[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected token at [2]", string("3"), vId[2]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected token at [3]", string("5"), vId[3]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected token at [4]", string("9"), vId[4]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected token at [5]", string("13"), vId[5]);

    vId = pUt->parseIds("1,2,3,");
    CPPUNIT_ASSERT_MESSAGE("Unexpected number of tokens", vId.size() == 3);

    vId = pUt->parseIds("1,2,,3");
    CPPUNIT_ASSERT_MESSAGE("Unexpected number of tokens", vId.size() == 3);

    vId = pUt->parseIds(",1,2,3");
    CPPUNIT_ASSERT_MESSAGE("Unexpected number of tokens", vId.size() == 3);
}

void CUtility_TS1::test_stringReplaceOne()
{
    bool ret;
    string szId = "That's it. That's all there is.";

    ret = pUt->stringReplaceOne(szId, "Some", "Text");
    CPPUNIT_ASSERT_MESSAGE("Unexpected return code of string replacement",
            ret == false);

    ret = pUt->stringReplaceOne(szId, "That's", "This will be");
    CPPUNIT_ASSERT_MESSAGE("Unexpected return code of string replacement",
            ret == true);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected outcome of string replacement",
            string("This will be it. That's all there is."), szId);
}

void CUtility_TS1::test_encodeQuotedPrintable()
{
    string test, ret;

    test = "Supercalifragilisticexpialidocious.";
    ret = pUt->encodeQuotedPrintable(test);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected result of string encoding",
            ret, ("=\n" + test));

    // use hex codes here, as the development system is using UTF-8 which
    // would produce multibyte characters for these
    test = "S\xF8me \xF8ther test.\n"
           "A new line.\n"
           "Special=;:,\n"
           "Some more text to exceed the 76 character limit.";
    ret = pUt->encodeQuotedPrintable(test);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected result of string encoding",
            string("=\nS=F8me=20=F8ther=20test.=0D=0A"
                   "A=20new=20line.=0D=0A"
                   "Special=3D=3B=3A=2C=0D=0A=\n"
                   "Some=20more=20text=20to=20exceed=20the=2076=20character=20limit."), ret);
}

void CUtility_TS1::test_getRecurrentTimes()
{
    int pErrorCode = -1;
    icaltimezone* pTz = icaltimezone_get_builtin_timezone("EEST"); // UTC+3
    time_t Dst, Ded;
    string szRule;
    vector<time_t> vRet;

    Dst = 7527600; // 19700329T030000 (Sunday)
    Ded = 0;       // all possible recurrences (68)
    szRule = "FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU"; // the last Sunday in March
    vRet = pUt->getRecurrentTimes(szRule,
            icaltime_from_timet_with_zone(Dst, 1, pTz), Ded, pTz, pErrorCode);

    if(vRet.size() != 68) {
        stringstream ss;
        ss << "Unexpected number of recurrences (" << vRet.size() << ")\n";
        for(vector<time_t>::iterator it = vRet.begin(); it != vRet.end(); ++it) {
            time_t t = *it;
            ss << ctime(&t);
        }
        ss << "Starting at date: " << icaltime_as_ical_string(icaltime_from_timet_with_zone(Dst, 1, pTz));
        CPPUNIT_FAIL(ss.str());
    }

    Dst = 7516800; // 19700329T000000
    Ded = 7560000; // 19700329T120000 (one recurrence expected)
    szRule = "FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU;UNTIL=20370329T111236Z";
    vRet = pUt->getRecurrentTimes(szRule,
            icaltime_from_timet_with_zone(Dst, 1, pTz), Ded, pTz, pErrorCode);

    if(vRet.size() != 1) {
        stringstream ss;
        ss << "Unexpected number of recurrences (" << vRet.size() << ")\n";
        for(vector<time_t>::iterator it = vRet.begin(); it != vRet.end(); ++it) {
            time_t t = *it;
            ss << ctime(&t);
        }
        ss << "Starting at date: " << icaltime_as_ical_string(icaltime_from_timet_with_zone(Dst, 1, pTz));
        CPPUNIT_FAIL(ss.str());
    }
}

void CUtility_TS1::test_getSystemTimeZone()
{
    string ret = pUt->getSystemTimeZone();
    CPPUNIT_ASSERT_MESSAGE("No system time zone found", ret.length() != 0);

    icaltimezone *pTz = pUt->getSystemTimeZoneAsIcalTimeZone();
    CPPUNIT_ASSERT_MESSAGE("No ICAL time zone found", pTz != 0);
}
