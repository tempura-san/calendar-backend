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
#include <string>
#include <vector>
#include <stdlib.h>

#include "test_CCalendarProcs.h"
#include "Common.h"
#include "CalendarErrors.h"
#include "CBdayEvent.h"

using namespace std;

static time_t mktime(int day, int month, int year)
{
    struct tm t;
    memset(&t, 0, sizeof(t));

    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = 12;

    return mktime(&t);
};

static void delete_bdays(std::vector<CBdayEvent*> &comps)
{
    for(std::vector<CBdayEvent*>::iterator it=comps.begin();
        it != comps.end();
        it++)
    {
        delete *it;
    }

    comps.clear();
}

CPPUNIT_TEST_SUITE_REGISTRATION(CCalendarProcs_TS1);

void CCalendarProcs_TS1 :: setUp()
{
    pProcs = new CCalendarProcs(CCalendarDB::Instance());

    pBDayCalendar = CMulticalendar::MCInstance()->getBirthdayCalendar();

    if (pBDayCalendar == 0) {

        CCalendar bdayCal("BdayTestCalendar",COLOUR_RED,0,1,BIRTHDAY_CALENDAR,"Music Tune.yz","Version-1.0");

        int error_code;
        BDayCalendarId = CMulticalendar::MCInstance()->addCalendar (&bdayCal, error_code);
    }

    pBDayCalendar = CMulticalendar::MCInstance()->getBirthdayCalendar();
}
void CCalendarProcs_TS1 :: tearDown()
{
    delete pProcs;
    if (pBDayCalendar)
    {
        int error_code;
        CMulticalendar::MCInstance()->deleteCalendar(pBDayCalendar->getCalendarId(), error_code);
    }
    delete pBDayCalendar;
}

// void CCalendarProcs_TS1 :: test_getDateFromTime()
// {
//     time_t ret = 0;
//     ret = pUt->getDateFromTime(123546986);
//     CPPUNIT_ASSERT_MESSAGE("Error:get DateFromTime ",(ret>123400000));
// }
// 
// void CCalendarProcs_TS1 :: test_parseId()
// {
//     string szId = "1,2,3,5,9,13";
//     vector<string> vId;
//     vId = pUt->parseIds(szId);
//     CPPUNIT_ASSERT_MESSAGE("Error:get DateFromTime ",(vId.size()>0));
// }
// void CCalendarProcs_TS1 :: test_stringReplaceOne()
// {
//     string szId = "This is a test string";
//     const char *change = "is";
//     const char *to = "was";
//     bool ret = false;
//     ret = pUt->stringReplaceOne(szId,change,to);
//     CPPUNIT_ASSERT_MESSAGE("Error:get DateFromTime ",(ret == true));
// }
// void CCalendarProcs_TS1 :: test_encodeQuotedPrintable()
// {
//     string szId = "This is a test string";
//     string szQp = "¹ º » ¼ ½ ¾ ¿";
//     string ret;
//     ret = pUt->encodeQuotedPrintable(szId);
//     ret.clear();
//     ret = pUt->encodeQuotedPrintable(szQp);
//     ret.clear();
//     szQp.clear();
//     szQp = "¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿ ¹ º » ¼ ½ ¾ ¿";
// 
//     ret = pUt->encodeQuotedPrintable(szQp);
// }
// void CCalendarProcs_TS1 :: test_getSystemTimeZone()
// {
//     string ret;
//     ret = pUt->getSystemTimeZone();
//     icaltimezone *pTz;
//     pTz = pUt->getSystemTimeZoneAsIcalTimeZone();
// 
// }
// 
// void CCalendarProcs_TS1 :: test_getRecurrentTimes()
// {
//     vector <time_t> vRet;
//     int pErrorCode = -1;
//     time_t Dst = 7614000;// "19700329T030000";
//     string szRule = "FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU";
//     icaltimezone* pTz= icaltimezone_get_builtin_timezone("EEST");
//     vRet = pUt->getRecurrentTimes(szRule,icaltime_from_timet_with_zone(Dst,1,pTz),0,pTz,pErrorCode);
//     Dst = 7603200;
//     szRule = "FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU;UNTIL=20370329T111236Z";
//     vRet = pUt->getRecurrentTimes(szRule,icaltime_from_timet_with_zone(Dst,1,pTz),time_get_time()+500,pTz,pErrorCode);
// }
// void CCalendarProcs_TS1 :: test_getApplicationName()
// {
// 	pUt->setApplicationName ("AppName");
//     	CPPUNIT_ASSERT_MESSAGE("Error:get Application Name ",(pUt->getApplicationName() == "AppName"));
// 	
// }

void CCalendarProcs_TS1::test_Init()
{
    CPPUNIT_ASSERT(pProcs != 0);
    CPPUNIT_ASSERT(pProcs->initBDay() == SQLITE_OK);
}

void CCalendarProcs_TS1::test_AddBDay()
{
    CPPUNIT_ASSERT(pProcs->dropBDays() == SQLITE_OK);

    time_t d0=1980;
    time_t d1=345198000;
    time_t d2=345208800;

    CPPUNIT_ASSERT(pProcs->addBDay("1980", d0) == SQLITE_OK);
    CPPUNIT_ASSERT(pProcs->addBDay("1980", d1) == SQLITE_OK);
    CPPUNIT_ASSERT(pProcs->addBDay("1980", d2) == SQLITE_OK);

    std::vector<std::string> ids;
    CPPUNIT_ASSERT(pProcs->getBDays(1, 345198000, ids) == SQLITE_OK);

    CPPUNIT_ASSERT(ids.size() == 1);
    CPPUNIT_ASSERT(ids[0] == "1980");

    // validate date
}

void CCalendarProcs_TS1::test_DelBDay()
{
    CPPUNIT_ASSERT(pProcs->dropBDays() == SQLITE_OK);

    CPPUNIT_ASSERT(pProcs->addBDay("1980", 12345) == SQLITE_OK);
    CPPUNIT_ASSERT(pProcs->addBDay("1981", 12345) == SQLITE_OK);

    std::vector<std::string> ids;
    CPPUNIT_ASSERT(pProcs->getBDays(1, 12345, ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 2);

    CPPUNIT_ASSERT(pProcs->delBDay("1980") == SQLITE_OK);
    CPPUNIT_ASSERT(pProcs->delBDay("1981") == SQLITE_OK);


    CPPUNIT_ASSERT(pProcs->getBDays(1, 12345, ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 0);
}

void CCalendarProcs_TS1::test_GetBDays()
{
    CPPUNIT_ASSERT(pProcs->dropBDays() == SQLITE_OK);

    char s[10];

    int id = 1;
    for (int y=1980; y<=1981; y++)
    {
        for (int m=1; m <= 12; m++)
        {
            sprintf(s, "%d", id++);
            CPPUNIT_ASSERT(pProcs->addBDay(s, mktime(1,m,y)) == SQLITE_OK);

            sprintf(s, "%d", id++);
            CPPUNIT_ASSERT(pProcs->addBDay(s, mktime(15,m,y)) == SQLITE_OK);

            sprintf(s, "%d", id++);
            CPPUNIT_ASSERT(pProcs->addBDay(s, mktime(28,m,y)) == SQLITE_OK);
        }
    }

    std::vector<std::string> ids;

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1979), mktime(31,12,1979), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 0);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(1,1,1980), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 1);
    CPPUNIT_ASSERT(ids[0] == "1");


    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(14,1,1980), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 1);
    CPPUNIT_ASSERT(ids[0] == "1");

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(15,1,1980), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 2);
    CPPUNIT_ASSERT(ids[0] == "1");
    CPPUNIT_ASSERT(ids[1] == "2");

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(27,1,1980), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 2);
    CPPUNIT_ASSERT(ids[0] == "1");
    CPPUNIT_ASSERT(ids[1] == "2");

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(28,1,1980), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 3);
    CPPUNIT_ASSERT(ids[0] == "1");
    CPPUNIT_ASSERT(ids[1] == "2");
    CPPUNIT_ASSERT(ids[2] == "3");

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(31,12,1980), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 12*3);


    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,4,1980), mktime(1,5,1980), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 4);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(1,1,1981), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 12*3 + 1 );

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,12,1980), mktime(1,1,1981), ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 5 );
}

void CCalendarProcs_TS1::test_DropBDays()
{
    CPPUNIT_ASSERT(pProcs->addBDay("1980", 12345) == SQLITE_OK);
    CPPUNIT_ASSERT(pProcs->addBDay("1981", 12345) == SQLITE_OK);
    CPPUNIT_ASSERT(pProcs->dropBDays() == SQLITE_OK);

    std::vector<std::string> ids;
    CPPUNIT_ASSERT(pProcs->getBDays(1, 123453, ids) == SQLITE_OK);
    CPPUNIT_ASSERT(ids.size() == 0);
}

void CCalendarProcs_TS1::test_GetBDayEvents()
{
    CPPUNIT_ASSERT(pProcs->dropBDays() == SQLITE_OK);

    CPPUNIT_ASSERT(pBDayCalendar != 0);
    addTestBDayEvents();

    std::vector<CBdayEvent*> events;

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1979), mktime(31,12,1979), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 0);
    delete_bdays(events);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(1,1,1980), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 1);
    CPPUNIT_ASSERT(events[0]->getGUid() == "1");
    CPPUNIT_ASSERT(events[0]->getCalendarId() == pBDayCalendar->getCalendarId());
    delete_bdays(events);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(14,1,1980), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 1);
    CPPUNIT_ASSERT(events[0]->getGUid() == "1");
    delete_bdays(events);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(15,1,1980), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 2);
    CPPUNIT_ASSERT(events[0]->getGUid() == "1");
    CPPUNIT_ASSERT(events[1]->getGUid() == "2");
    delete_bdays(events);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(27,1,1980), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 2);
    CPPUNIT_ASSERT(events[0]->getGUid() == "1");
    CPPUNIT_ASSERT(events[1]->getGUid() == "2");
    delete_bdays(events);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(28,1,1980), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 3);
    CPPUNIT_ASSERT(events[0]->getGUid() == "1");
    CPPUNIT_ASSERT(events[1]->getGUid() == "2");
    CPPUNIT_ASSERT(events[2]->getGUid() == "3");
    delete_bdays(events);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(31,12,1980), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 12*3);
    delete_bdays(events);


    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,4,1980), mktime(1,5,1980), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 4);
    delete_bdays(events);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,1,1980), mktime(1,1,1981), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 12*3 + 1 );
    delete_bdays(events);

    CPPUNIT_ASSERT(pProcs->getBDays(mktime(1,12,1980), mktime(1,1,1981), events) == SQLITE_OK);
    CPPUNIT_ASSERT(events.size() == 5 );
    delete_bdays(events);
}


void CCalendarProcs_TS1::test_GetComponentsAllCalendars_repeating_events()
{
    addTestBDayEvents();

    std::vector<CComponent*> comps;

    CPPUNIT_ASSERT(pProcs->getComponentsAllCalendars(mktime(1,1,1979), mktime(31,12,1979), 1000, 0, comps, 2) == CALENDAR_FETCH_NOITEMS);
    CPPUNIT_ASSERT(comps.size() == 0);

    CPPUNIT_ASSERT(pProcs->getComponentsAllCalendars(mktime(1,1,1980), mktime(31,12,1981), 1000, 0, comps, 2) == CALENDAR_OPERATION_SUCCESSFUL);
    CPPUNIT_ASSERT(comps.size() == 12*3*2);

    CPPUNIT_ASSERT(pProcs->getComponentsAllCalendars(mktime(1,1,1980), mktime(31,12,1980), 1000, 0, comps, 2) == CALENDAR_OPERATION_SUCCESSFUL);
    CPPUNIT_ASSERT(comps.size() == 12*3);

    CPPUNIT_ASSERT(pProcs->getComponentsAllCalendars(mktime(1,1,1980), mktime(31,12,1981), 1000, 1000, comps, 2) == CALENDAR_FETCH_NOITEMS);
    CPPUNIT_ASSERT(comps.size() == 0);
}

void CCalendarProcs_TS1::test_GetComponentsAllCalendars_all_events()
{
    addTestBDayEvents();

    std::vector<CComponent*> comps;

    CPPUNIT_ASSERT(pProcs->getComponentsAllCalendars(mktime(1,1,1979), mktime(31,12,1979), 1000, 0, comps, 0) == CALENDAR_FETCH_NOITEMS);
    CPPUNIT_ASSERT(comps.size() == 0);

    CPPUNIT_ASSERT(pProcs->getComponentsAllCalendars(mktime(1,1,1980), mktime(31,12,1981), 1000, 0, comps, 0) == CALENDAR_OPERATION_SUCCESSFUL);
    CPPUNIT_ASSERT(comps.size() == 12*3*2);

    CPPUNIT_ASSERT(pProcs->getComponentsAllCalendars(mktime(1,1,1980), mktime(31,12,1980), 1000, 0, comps, 0) == CALENDAR_OPERATION_SUCCESSFUL);
    CPPUNIT_ASSERT(comps.size() == 12*3);

    CPPUNIT_ASSERT(pProcs->getComponentsAllCalendars(mktime(1,1,1980), mktime(31,12,1981), 1000, 1000, comps, 0) == CALENDAR_FETCH_NOITEMS);
    CPPUNIT_ASSERT(comps.size() == 0);

}

void CCalendarProcs_TS1::addTestBDayEvents()
{
    char s[10];
    std::vector<CBdayEvent*> bdays2add;
    int id = 1;

    for (int y=1980; y<=1981; y++)
    {
        for (int m=1; m <= 12; m++)
        {
            CBdayEvent *e = 0;
            int error;

            sprintf(s, "%d", id++);
            e = new CBdayEvent(s, s, mktime(1,m,y));
            e->setGUid(s);
            bdays2add.push_back(e);

            sprintf(s, "%d", id++);
            e = new CBdayEvent(s, s, mktime(15,m,y));
            e->setGUid(s);
            bdays2add.push_back(e);

            sprintf(s, "%d", id++);
            e = new CBdayEvent(s, s, mktime(28,m,y));
            e->setGUid(s);
            bdays2add.push_back(e);
        }
    }

    int ErrorCode;

    CMulticalendar::MCInstance()->addBirthdays(bdays2add, ErrorCode);

    CPPUNIT_ASSERT(ErrorCode == CALENDAR_OPERATION_SUCCESSFUL);

    delete_bdays(bdays2add);
}
