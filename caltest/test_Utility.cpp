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
#include <string.h>
#include "test_Utility.h"
#include "Common.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CUtility_TS1);

void CUtility_TS1 :: setUp()
{
    pUt=CUtility::Instance();
}
void CUtility_TS1 :: tearDown()
{
}

void CUtility_TS1 :: test_getDateFromTime()
{
    time_t ret = 0;
    ret = pUt->getDateFromTime(123546986);
    CPPUNIT_ASSERT_MESSAGE("Error:get DateFromTime ",(ret>123400000));
}

void CUtility_TS1 :: test_parseId()
{
    string szId = "1,2,3,5,9,13";
    vector<string> vId;
    vId = pUt->parseIds(szId);
    CPPUNIT_ASSERT_MESSAGE("Error:get DateFromTime ",(vId.size()>0));
}
void CUtility_TS1 :: test_stringReplaceOne()
{
    string szId = "This is a test string";
    const char *change = "is";
    const char *to = "was";
    bool ret = false;
    ret = pUt->stringReplaceOne(szId,change,to);
    CPPUNIT_ASSERT_MESSAGE("Error:get DateFromTime ",(ret == true));
}
void CUtility_TS1 :: test_encodeQuotedPrintable()
{
    string szId = "This is a test string";
    string szQp = "¹ º » ¼ ½ ¾ ¿";
    string ret;
    ret = pUt->encodeQuotedPrintable(szId);
    ret.clear();
    ret = pUt->encodeQuotedPrintable(szQp);
    ret.clear();
    szQp.clear();
    szQp = "¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿";

    ret = pUt->encodeQuotedPrintable(szQp);
}
void CUtility_TS1 :: test_getSystemTimeZone()
{
    string ret;
    ret = pUt->getSystemTimeZone();
    icaltimezone *pTz;
    pTz = pUt->getSystemTimeZoneAsIcalTimeZone();

}

void CUtility_TS1 :: test_getRecurrentTimes()
{
    vector <time_t> vRet;
    int pErrorCode = -1;
    time_t Dst = 7614000;// "19700329T030000";
    string szRule = "FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU";
    icaltimezone* pTz= icaltimezone_get_builtin_timezone("EEST");
    vRet = pUt->getRecurrentTimes(szRule,icaltime_from_timet_with_zone(Dst,1,pTz),0,pTz,pErrorCode);
    Dst = 7603200;
    szRule = "FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU;UNTIL=20370329T111236Z";
    vRet = pUt->getRecurrentTimes(szRule,icaltime_from_timet_with_zone(Dst,1,pTz),time_get_time()+500,pTz,pErrorCode);
}
void CUtility_TS1 :: test_getApplicationName()
{
	pUt->setApplicationName ("AppName");
    	CPPUNIT_ASSERT_MESSAGE("Error:get Application Name ",(pUt->getApplicationName() == "AppName"));
	
}
