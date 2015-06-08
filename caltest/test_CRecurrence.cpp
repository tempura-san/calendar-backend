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

#include "test_CRecurrence.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>

#include <algorithm>
#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION(CRecurrence_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CRecurrence_TS2);
using namespace std;

void CRecurrence_TS1::setUp()
{
    pCr = new CRecurrence();
}
void CRecurrence_TS1::tearDown()
{
    delete pCr;
}

void CRecurrence_TS1::test_setRecurId_Valid()
{
    bool ret = pCr->setRecurId(1);
    CPPUNIT_ASSERT_MESSAGE("Failed to set valid recurrence ID", (ret == true));

    int ret1 = pCr->getRecurId();
    CPPUNIT_ASSERT_MESSAGE("Getter returned wrong recurrence ID ", (ret1 == 1));

    ret = pCr->setRecurId(-2);
    CPPUNIT_ASSERT_MESSAGE("Unexpected success when setting invalid ID",
            (ret == false));
}

void CRecurrence_TS1::test_setRrule_Valid()
{
    vector<string> vrules;
    bool ret = pCr->setRrule(vrules);
    CPPUNIT_ASSERT_MESSAGE("Unexpected success when setting empty RRule",
            (ret == false));

    vrules.push_back("FREQ=YEARLY;UNTIL=20000131T090000Z");
    ret = pCr->setRrule(vrules);
    CPPUNIT_ASSERT_MESSAGE("Failed to set non-empty RRule", (ret == true));

    vrules = pCr->getRrule();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of RRules", (vrules.size() == 1));
    vrules = pCr->getErule();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of ERules", (vrules.size() == 0));
}

void CRecurrence_TS1::test_setErule_Valid()
{
    vector<string> vrules;
    bool ret = pCr->setErule(vrules);
    CPPUNIT_ASSERT_MESSAGE("Unexpected success when setting empty ERule",
            (ret == false));

    vrules.push_back("FREQ=WEEKLY;COUNT=4;INTERVAL=2;BYDAY=TU,TH");
    ret = pCr->setErule(vrules);
    CPPUNIT_ASSERT_MESSAGE("Failed to set non-empty ERule", (ret == true));

    vrules = pCr->getErule();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of ERules", vrules.size() == 1);
    vrules = pCr->getRrule();
    CPPUNIT_ASSERT_MESSAGE("Wrong number of RRules", vrules.size() == 0);
}

void CRecurrence_TS1::test_setRDays_Valid()
{
    vector<string> szrDateList;
    vector<string> szRetrDateList;
    bool ret = pCr->setRDays(szrDateList);
    CPPUNIT_ASSERT_MESSAGE("Unexpected success when setting empty RDays",
            (ret == false));

    szrDateList.push_back("19970714T123000Z");
    szrDateList.push_back("19980714T123000Z");
    szrDateList.push_back("19990714T123000Z");
    ret = pCr->setRDays(szrDateList);
    CPPUNIT_ASSERT_MESSAGE("Failed to set non-empty RDays", (ret == true));

    szRetrDateList = pCr->getRDays();
    // order does not matter, so use std::find here
    CPPUNIT_ASSERT_MESSAGE("Failed to get RDays[0]",
            find(szRetrDateList.begin(), szRetrDateList.end(), szrDateList[0])
                    != szRetrDateList.end());
    CPPUNIT_ASSERT_MESSAGE("Failed to get RDays[1]",
            find(szRetrDateList.begin(), szRetrDateList.end(), szrDateList[1])
                    != szRetrDateList.end());
    CPPUNIT_ASSERT_MESSAGE("Failed to get RDays[2]",
            find(szRetrDateList.begin(), szRetrDateList.end(), szrDateList[2])
                    != szRetrDateList.end());
}

void CRecurrence_TS1::test_setEDays_Valid()
{
    vector<string> szeDateList;
    vector<string> szReteDateList;
    bool ret = pCr->setEDays(szeDateList);
    CPPUNIT_ASSERT_MESSAGE("Unexpected success when setting empty EDays",
            (ret == false));

    szeDateList.push_back("19970714T123000Z");
    szeDateList.push_back("19980714T123000Z");
    szeDateList.push_back("19990714T123000Z");
    ret = pCr->setEDays(szeDateList);
    CPPUNIT_ASSERT_MESSAGE("Failed to set non-empty EDays", (ret == true));

    szReteDateList = pCr->getEDays();
    // order does not matter, so use std::find here
    CPPUNIT_ASSERT_MESSAGE("Failed to get EDays[0]",
            find(szReteDateList.begin(), szReteDateList.end(), szeDateList[0])
                    != szReteDateList.end());
    CPPUNIT_ASSERT_MESSAGE("Failed to get EDays[1]",
            find(szReteDateList.begin(), szReteDateList.end(), szeDateList[0])
                    != szReteDateList.end());
    CPPUNIT_ASSERT_MESSAGE("Failed to get EDays[2]",
            find(szReteDateList.begin(), szReteDateList.end(), szeDateList[0])
                    != szReteDateList.end());
}

void CRecurrence_TS1::test_generateInstanceTimes()
{
    time_t start, end;
    time(&start);
    end = start + (86400 * 15);
    time_t span = end - start;
    //dates not in range
    vector<string> szeDateList;
    szeDateList.push_back("19970714T123000Z");
    szeDateList.push_back("19980714T123000Z");
    szeDateList.push_back("19990714T123000Z");
    pCr->setEDays(szeDateList);
    pCr->setRDays(szeDateList);
    vector<string> vrules;
    vrules.push_back("FREQ=WEEKLY;UNTIL=20090831T090000Z");
    pCr->setRrule(vrules);
    vector<string> erules;
    erules.push_back("FREQ=WEEKLY;COUNT=4;INTERVAL=2;BYDAY=TU,TH");
    pCr->setErule(erules);
    vector<time_t> ret = pCr->generateInstanceTimes(start, end, start, span, 1,
            "UTC");
    CPPUNIT_ASSERT_MESSAGE("Error: Generate recurrence times",
            (ret.size() >= 0));
}

void CRecurrence_TS1::test_setRecurrenceRule()
{
    vector<CRecurrenceRule *> vRRule;
    bool ret = pCr->setRecurrenceRule(vRRule);
    CPPUNIT_ASSERT_MESSAGE("Unexpected success when setting empty RRule",
            (ret == false));

    CRecurrenceRule *pCrr = new CRecurrenceRule();
    pCrr->setRuleType(RECURRENCE_RULE);
    string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
    pCrr->setRrule(rruleset);
    pCrr->setFrequency(WEEKLY_RECURRENCE);
    pCrr->setUntil(10000);
    pCrr->setCount(1);
    vector<short> sec;
    sec.push_back(12);
    pCrr->setSecond(sec);
    pCrr->setMinute(sec);
    pCrr->setHour(sec);
    pCrr->setWeekDay(sec);
    pCrr->setMonthDay(sec);
    pCrr->setYearDay(sec);
    pCrr->setWeekNumber(sec);
    pCrr->setMonth(sec);
    pCrr->setPos(sec);

    vRRule.push_back(pCrr);
    ret = pCr->setRecurrenceRule(vRRule);
    CPPUNIT_ASSERT_MESSAGE("Failed to set non-empty RRule", (ret == true));
}

void CRecurrence_TS1::test_Recurrence()
{
    vector<string> vRRule;
    string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
    vRRule.push_back(rruleset);

    CRecurrence *p = new CRecurrence(vRRule, 100);
    CRecurrence q(*p);

    CPPUNIT_ASSERT_MESSAGE("Failed getting RRule from p",
            (p->getRrule().size() == 1));
    CPPUNIT_ASSERT_MESSAGE("Failed getting RRule from q",
            (q.getRrule().size() == 1));

    delete p;
}

void CRecurrence_TS1::test_assignment_operator()
{
    vector<string> vRRule;
    string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
    vRRule.push_back(rruleset);

    CRecurrence *q = new CRecurrence(vRRule, 100);
    CRecurrence *p = new CRecurrence(vRRule, 100);
    *q = *p;

    CPPUNIT_ASSERT_MESSAGE("Failed getting RRule from p",
            (p->getRrule().size() == 1));
    CPPUNIT_ASSERT_MESSAGE("Failed getting RRule from q",
            (q->getRrule().size() == 1));

    delete q;
    delete p;
}

void CRecurrence_TS2::test_toString()
{
    vector<string> vRule;
    string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
    vRule.push_back(rruleset);

    CRecurrence *p = new CRecurrence(vRule, 100);
    vector<string> szeDateList;
    szeDateList.push_back("19970714T123000Z");
    szeDateList.push_back("19980714T123000Z");
    szeDateList.push_back("19990714T123000Z");
    p->setEDays(szeDateList);
    p->setRecurId(1);

    vector<CRecurrenceRule *> vRRule;
    CRecurrenceRule *pCrr = new CRecurrenceRule();
    pCrr->setRuleType(RECURRENCE_RULE);
    pCrr->setRrule(rruleset);
    pCrr->setFrequency(WEEKLY_RECURRENCE);
    pCrr->setUntil(10000);
    pCrr->setCount(1);
    vector<short> sec;
    sec.push_back(12);
    pCrr->setSecond(sec);
    pCrr->setMinute(sec);
    pCrr->setHour(sec);
    pCrr->setWeekDay(sec);
    pCrr->setMonthDay(sec);
    pCrr->setYearDay(sec);
    pCrr->setWeekNumber(sec);
    pCrr->setMonth(sec);
    pCrr->setPos(sec);
    vRRule.push_back(pCrr);
    p->setRecurrenceRule(vRRule);
    vector<string> vrules;
    vrules.push_back("FREQ=YEARLY;UNTIL=20000131T090000Z");
    p->setRrule(vrules);

    string result = "Recurrence type=0,Recurrence ID=1,ExceptionDate=[19970714T123000Z,19980714T123000Z,19990714T123000Z],RecRuleList=[[Count=0 ,Interval=0,Until=-1,Rule=RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10,WeekStart=1,Freq=7],[Count=1 ,Interval=0,Until=10000,Rule=RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10,WeekStart=1,Freq=4,BySec=12;,ByMin=12;,ByHour=12;,WeekDay=12;,MonthDay=12;,YearDay=12;,WeekNo=12;,Pos=12;],[Count=0 ,Interval=0,Until=-1,Rule=FREQ=YEARLY;UNTIL=20000131T090000Z,WeekStart=1,Freq=7]]";
    CPPUNIT_ASSERT_EQUAL(result, p->toString());
    delete p;
}

void CRecurrence_TS2::test_toString_NULL()
{
    vector<string> vRRule;
    vRRule.push_back("");
    CRecurrence *p = new CRecurrence(vRRule, 100);

    string result = "Recurrence type=0,Recurrence ID=100,RecRuleList=[[Count=0 ,Interval=0,Until=-1,Rule=,WeekStart=1,Freq=7]]";
    CPPUNIT_ASSERT_EQUAL(result, p->toString());
    delete p;
}
