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

#include "test_CRecurrence.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CRecurrence_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CRecurrence_TS2);
using namespace std;

void CRecurrence_TS1::setUp()
{
	pCr= new CRecurrence();
}
void CRecurrence_TS1::tearDown()
{
	delete pCr;
}

void CRecurrence_TS2::setUp()
{
	        pCr= new CRecurrence();
}
void CRecurrence_TS2::tearDown()
{
	        delete pCr;
}

void CRecurrence_TS1::test_setRecurId_Valid()
{
	bool ret = pCr->setRecurId(1);
	CPPUNIT_ASSERT_MESSAGE("Error : settng recurr id ",(ret==true));
	int ret1 = pCr->getRecurId();
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong ID ",(ret1==1));
	ret = pCr->setRecurId(-2);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set invalid ID",(ret==false));
}

void CRecurrence_TS1 :: test_setRrule_Valid()
{
	vector<string> vrules;
	bool ret = pCr->setRrule(vrules);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set empty rule ",(ret==false));
	string szS = "FREQ=YEARLY;UNTIL=20000131T090000Z";
	vrules.push_back("FREQ=YEARLY;UNTIL=20000131T090000Z");
	ret = pCr->setRrule(vrules);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set proper rule ",(ret==true));
	vector<string> vrules1  = pCr->getRrule();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get Rrule",(vrules1.size()>0));
}

void CRecurrence_TS1 :: test_setErule_Valid()
{
	vector<string> vrules;
	bool ret = pCr->setErule(vrules);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set empty Erule ",(ret==false));
	string szS = "FREQ=YEARLY;UNTIL=20000131T090000Z";
	vrules.push_back("FREQ=WEEKLY;COUNT=4;INTERVAL=2;BYDAY=TU,TH");
	ret = pCr->setErule(vrules);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set proper Erule ",(ret==true));
	vector<string> vrules1  = pCr->getErule();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get Erule",(vrules1.size()>0));
}

void CRecurrence_TS1::test_setRDays_Valid()
{
	vector <string> szrDateList;
	vector <string> szRetrDateList;
	bool bRet = pCr->setRDays(szrDateList);
	szrDateList.push_back("19970714T123000Z");
	szrDateList.push_back("19980714T123000Z");
	szrDateList.push_back("19990714T123000Z");
	bool ret = pCr->setRDays(szrDateList);
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to set RDays",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set invalid RDays",(bRet==false));
	szRetrDateList = pCr->getRDays();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get RDays",szrDateList[0] == szRetrDateList[0]);
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get RDays",szrDateList[1] == szRetrDateList[1]);
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get RDays",szrDateList[2] == szRetrDateList[2]);
}

void CRecurrence_TS1::test_setEDays_Valid()
{
	vector <string> szeDateList;
	vector <string> szReteDateList;
	bool bRet = pCr->setEDays(szeDateList);
       	szeDateList.push_back("19970714T123000Z");
       	szeDateList.push_back("19980714T123000Z");
       	szeDateList.push_back("19990714T123000Z");

	bool ret = pCr->setEDays(szeDateList);
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to set EDays",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set invalid EDays",(bRet==false));
	szReteDateList = pCr->getEDays();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to set EDays",szeDateList[0] == szReteDateList[0]);
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to set EDays",szeDateList[1] == szReteDateList[1]);
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to set EDays",szeDateList[2] == szReteDateList[2]);
}

void CRecurrence_TS1::test_generateInstanceTimes()
{
	time_t start,end;
	time(&start);
	end = start + (86400*15);
	time_t span = end - start;
	//dates not in range 
	vector <string> szeDateList;
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
	vector < time_t > ret = pCr->generateInstanceTimes(start, end, start, span,1, "UTC");
	CPPUNIT_ASSERT_MESSAGE("Error: Generate recurrence times",(ret.size()>=0));
}

void CRecurrence_TS1::test_setRecurrenceRule()
{
	vector < CRecurrenceRule * >vRRule;
	CRecurrenceRule *pCrr = new CRecurrenceRule();  
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
	 bool ret = pCr->setRecurrenceRule(vRRule);
	 CPPUNIT_ASSERT_MESSAGE("Error: vector rule empty",(ret==false));
	 vRRule.push_back(pCrr);
	 ret = pCr->setRecurrenceRule(vRRule);
	 CPPUNIT_ASSERT_MESSAGE("Error: vector rule not empty",(ret==true));
}

void CRecurrence_TS1::test_Recurrence()
{
	vector < string >vRRule;
	string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
	vRRule.push_back(rruleset);
	CRecurrence *p = new CRecurrence(vRRule,100);
	CRecurrence q(*(p));
	CPPUNIT_ASSERT_MESSAGE("Error: getting rrule ",(p->getRrule().size() !=0));
	CPPUNIT_ASSERT_MESSAGE("Error: getting rrule ",(q.getRrule().size() !=0));
}


void CRecurrence_TS2::test_toString()
{
	vector < string >vRule;
	string rruleset = "RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10";
	vRule.push_back(rruleset);
	CRecurrence *p = new CRecurrence(vRule,100);
	vector <string> szeDateList;
	p->setEDays(szeDateList);
	szeDateList.push_back("19970714T123000Z");
	szeDateList.push_back("19980714T123000Z");
	szeDateList.push_back("19990714T123000Z");

	p->setEDays(szeDateList);
	p->setRecurId(1);
	vector < CRecurrenceRule * >vRRule;
	CRecurrenceRule *pCrr = new CRecurrenceRule();
	pCrr->setRuleType(RECURRENCE_RULE);
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
	p->setRecurrenceRule(vRRule);
	vector<string> vrules;
	string szS = "FREQ=YEARLY;UNTIL=20000131T090000Z";
	vrules.push_back(szS);
	p->setRrule(vrules);



	cout<<p->toString();
}
void CRecurrence_TS2::test_toString_NULL()
{
	vector < string >vRRule;
	string rruleset="" ;
	vRRule.push_back(rruleset);
	CRecurrence *p = new CRecurrence(vRRule,100);
	cout<<p->toString();
}

