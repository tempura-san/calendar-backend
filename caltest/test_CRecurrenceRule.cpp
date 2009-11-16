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

#include "test_CRecurrenceRule.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CRecurrenceRule_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CRecurrenceRule_TS2);

using namespace std;

void CRecurrenceRule_TS1::setUp()
{
	pCrr= new CRecurrenceRule();
}
void CRecurrenceRule_TS1::tearDown()
{
	delete pCrr;
}
void CRecurrenceRule_TS2::setUp()
{
	pCrr= new CRecurrenceRule();
}
void CRecurrenceRule_TS2::tearDown()
{
	delete pCrr;
}

void CRecurrenceRule_TS1::test_RuleType()
{
	bool ret =  pCrr->setRuleType(RECURRENCE_RULE);
	CPPUNIT_ASSERT_MESSAGE("Set recurrence rule type",(ret==true));
	RuleType rtype = pCrr->getRuleType();
	CPPUNIT_ASSERT_MESSAGE("Get recurrence rule type",(rtype==RECURRENCE_RULE));
}

void CRecurrenceRule_TS1::test_RRule()
{
	string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
	bool ret = pCrr->setRrule(rruleset);
	CPPUNIT_ASSERT_MESSAGE("Set recurrence rule",(ret==true));
	string rrule = pCrr->getRrule();
	CPPUNIT_ASSERT_MESSAGE("Get recurrence rule type",(rrule==rruleset));
}

void CRecurrenceRule_TS1::test_frequency()
{
	bool ret = pCrr->setFrequency(WEEKLY_RECURRENCE);
	CPPUNIT_ASSERT_MESSAGE("Set recurrence",(ret==true));
	FREQUENCY freq = pCrr->getFrequency();
	CPPUNIT_ASSERT_MESSAGE("get recurrence",(freq==WEEKLY_RECURRENCE));
}

void CRecurrenceRule_TS1::test_Until()
{
	bool ret = pCrr->setUntil(10000);
	CPPUNIT_ASSERT_MESSAGE("Set Until",(ret==true));
	time_t until = pCrr->getUntil();
	CPPUNIT_ASSERT_MESSAGE("Get Until",(until==10000));
}

void CRecurrenceRule_TS1::test_count()
{
	bool ret = pCrr->setCount(1);
	CPPUNIT_ASSERT_MESSAGE("Set count",(ret==true));
	int count = pCrr->getCount();
	CPPUNIT_ASSERT_MESSAGE("Get Count",(count==1));
}

void CRecurrenceRule_TS1::test_second()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setSecond(sec);
	CPPUNIT_ASSERT_MESSAGE("Set Second",(ret==true));
	vector < short > secret = pCrr->getSecond();
	CPPUNIT_ASSERT_MESSAGE("Get Seconds",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get Seconds",(secret[0]==12));
	
}

void CRecurrenceRule_TS1::test_minute()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setMinute(sec);
	CPPUNIT_ASSERT_MESSAGE("Set Minute",(ret==true));
	vector < short > secret = pCrr->getMinute();
	CPPUNIT_ASSERT_MESSAGE("Get Minute",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get Minute",(secret[0]==12));
}

void CRecurrenceRule_TS1::test_hour()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setHour(sec);
	CPPUNIT_ASSERT_MESSAGE("Set Hour",(ret==true));
	vector < short > secret = pCrr->getHour();
	CPPUNIT_ASSERT_MESSAGE("Get Hour",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get Hour",(secret[0]==12));
}

void CRecurrenceRule_TS1::test_weekday()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setWeekDay(sec);
	CPPUNIT_ASSERT_MESSAGE("Set Weekday",(ret==true));
	vector < short > secret = pCrr->getWeekDay();
	CPPUNIT_ASSERT_MESSAGE("Get Weekday",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get Weekday",(secret[0]==12));
}

void CRecurrenceRule_TS1::test_monthday()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setMonthDay(sec);
	CPPUNIT_ASSERT_MESSAGE("Set monthday",(ret==true));
	vector < short > secret = pCrr->getMonthDay();
	CPPUNIT_ASSERT_MESSAGE("Get monthday",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get monthday",(secret[0]==12));
}

void CRecurrenceRule_TS1::test_yearday()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setYearDay(sec);
	CPPUNIT_ASSERT_MESSAGE("Set year day",(ret==true));
	vector < short > secret = pCrr->getYearDay();
	CPPUNIT_ASSERT_MESSAGE("Get year day",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get year day",(secret[0]==12));
}

void CRecurrenceRule_TS1::test_weeknumber()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setWeekNumber(sec);
	CPPUNIT_ASSERT_MESSAGE("Set WeekNumber",(ret==true));
	vector < short > secret = pCrr->getWeekNumber();
	CPPUNIT_ASSERT_MESSAGE("Get WeekNumber",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get WeekNumber",(secret[0]==12));
}

void CRecurrenceRule_TS1::test_month()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setMonth(sec);
	CPPUNIT_ASSERT_MESSAGE("Set month",(ret==true));
	vector < short > secret = pCrr->getMonth();
	CPPUNIT_ASSERT_MESSAGE("Get month",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get month",(secret[0]==12));
}

void CRecurrenceRule_TS1::test_position()
{
	vector < short > sec ;
	sec.push_back(12);
	bool ret = pCrr->setPos(sec);
	CPPUNIT_ASSERT_MESSAGE("Set position",(ret==true));
	vector < short > secret = pCrr->getPos();
	CPPUNIT_ASSERT_MESSAGE("Get position",(secret.size()>0));
	CPPUNIT_ASSERT_MESSAGE("Get position",(secret[0]==12));
}

void CRecurrenceRule_TS1::test_WeekStart()
{
	bool ret =  pCrr->setWeekStart(ICAL_SUNDAY_WEEKDAY);
	CPPUNIT_ASSERT_MESSAGE("Set week start",(ret==true));
	icalrecurrencetype_weekday setstart = pCrr->getWeekStart();
	CPPUNIT_ASSERT_MESSAGE("Get week start",(setstart==ICAL_SUNDAY_WEEKDAY));
}

void CRecurrenceRule_TS1::test_interval()
{
	bool ret =  pCrr->setInterval(10);
	CPPUNIT_ASSERT_MESSAGE("Set interval",(ret==true));
	short val = pCrr->getInterval();
	CPPUNIT_ASSERT_MESSAGE("Get interval",(val==10));
}

void CRecurrenceRule_TS1::test_rruleParser()
{
	time_t start,until;
        time(&start);
        until=start+50000;
	string rrule;
	string s1("FREQ=");
	string s2(";UNTIL=");
	string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
        struct icaltimetype uical;
        int repeattype = 3;
        uical = icaltime_from_timet(until, 0);
        string ustring = icaltime_as_ical_string(uical);
        rrule = s1 + test[repeattype - 1] + s2 + ustring;
	printf("%s---rrule--\n",rrule.c_str());
	bool ret = pCrr->rruleParser(rrule);
	CPPUNIT_ASSERT_MESSAGE("rruleParser",(ret==true));
}

void CRecurrenceRule_TS1::test_operator()
{
	 pCrr->setRuleType(RECURRENCE_RULE);
         string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
         pCrr->setRrule(rruleset);
         pCrr->setFrequency(WEEKLY_RECURRENCE);
         pCrr->setUntil(10000);
         pCrr->setCount(1);
         vector < short > sec ;
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
         pCrr->setWeekStart(ICAL_SUNDAY_WEEKDAY);
         pCrr->setInterval(10);

         CRecurrenceRule pCrr1(*(pCrr));	 
         CRecurrenceRule pCrr2 ;
	 pCrr2 = *(pCrr);	 
}

void CRecurrenceRule_TS2::test_toString()
{
	pCrr->setRuleType(RECURRENCE_RULE);
	string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
	pCrr->setRrule(rruleset);
	pCrr->setFrequency(WEEKLY_RECURRENCE);
	pCrr->setUntil(10000);
	pCrr->setCount(1);
	vector < short > sec ;
	sec.push_back(12);
	pCrr->setSecond(sec);
	pCrr->setMinute(sec);
	pCrr->setHour(sec);
	pCrr->setWeekDay(sec);
	pCrr->setMonthDay(sec);
	pCrr->setYearDay(sec);
	pCrr->setInterval(10);
	pCrr->setPos(sec);
	pCrr->setWeekNumber(sec);
	pCrr->setWeekStart(ICAL_SUNDAY_WEEKDAY);
	cout<<pCrr->toString();
}
void CRecurrenceRule_TS2::test_toString_NULL()
{
	pCrr->setRuleType(RECURRENCE_RULE);
	string rruleset = "";
	pCrr->setRrule(rruleset);
	pCrr->setFrequency(WEEKLY_RECURRENCE);
	pCrr->setUntil(10000);
	pCrr->setCount(1);
	vector < short > sec ;
	
	pCrr->setSecond(sec);
	pCrr->setMinute(sec);
	pCrr->setHour(sec);
	pCrr->setWeekDay(sec);
	pCrr->setMonthDay(sec);
	pCrr->setYearDay(sec);
	pCrr->setInterval(10);
	pCrr->setPos(sec);
	pCrr->setWeekNumber(sec);
	pCrr->setWeekStart(ICAL_SUNDAY_WEEKDAY);
	cout<<pCrr->toString();
}

