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

#include "test_CComponent.h"
#include "CComponent.h"
#include "CAlarm.h"
#include "CRecurrence.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CComponent_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CComponent_TS2);

/* ValidLocationTestCase*/
void CComponent_TS1 :: setUp()
{
	pCc = new CComponent();
}
void CComponent_TS1 :: tearDown()
{
	delete pCc;
}

void CComponent_TS2 :: setUp()
{
	pCc = new CComponent();
}
void CComponent_TS2 :: tearDown()
{
	delete pCc;
}
void CComponent_TS1 :: test_setAlarm()
{
	CAlarm *alarm = new CAlarm(10000, 1);
	alarm->setAttach ("AlarmAttach");
	bool ret = pCc->setAlarm(alarm);
	CPPUNIT_ASSERT_MESSAGE("Error: setAlarm", ret == true);
}
void CComponent_TS1 :: test_getAlarmProperties()
{
	bool ret;
	ret = pCc->getAlarmProperties();
	CPPUNIT_ASSERT_MESSAGE("Error: getAlarmProperties", ret == false);
	

}
void CComponent_TS1 :: test_getDuration()
{
	int duration = pCc->getDuration();
	CPPUNIT_ASSERT_MESSAGE("Error: getDuration", duration == 0);
	CAlarm *alarm = new CAlarm(10000, 1);
	bool ret = pCc->setAlarm(alarm);
	duration = pCc->getDuration();
	CPPUNIT_ASSERT_MESSAGE("Error: getDuration", duration == 1);
	pCc->removeAlarm();
	ret = pCc->setAlarm(NULL);
	CPPUNIT_ASSERT_MESSAGE("Error: getDuration", ret == false);

}
void CComponent_TS1 :: test_setRecurrence()
{
	CPPUNIT_ASSERT_MESSAGE("Error: getRecurrence", pCc->getRecurrence() == NULL);
	CRecurrence *rec = new CRecurrence();
	bool ret = pCc->setRecurrence(rec);
	CPPUNIT_ASSERT_MESSAGE("Error: setRecurrence", ret == true);
	CPPUNIT_ASSERT_MESSAGE("Error: getRecurrence", pCc->getRecurrence() != NULL);

	ret = pCc->setRtype(1);
	CPPUNIT_ASSERT_MESSAGE("Error: setRtype", ret == true);
	CPPUNIT_ASSERT_MESSAGE("Error: getRtype", pCc->getRtype() == 1);
	pCc->removeRecurrence();

	ret = pCc->setRecurrence(NULL);
	CPPUNIT_ASSERT_MESSAGE("Error: setRecurrence", ret == false);
}
void CComponent_TS1 :: test_copyConstructor()
{
	CAlarm *alarm = new CAlarm(10000, 1);
	bool ret = pCc->setAlarm(alarm);
	CRecurrence *rec = new CRecurrence();
	ret = pCc->setRecurrence(rec);
	CComponent comp(*pCc);
	CComponent comp1 = *pCc;
	CPPUNIT_ASSERT_MESSAGE("Error: copy constructor", comp.getAlarm() != NULL);
	CPPUNIT_ASSERT_MESSAGE("Error: copy constructor", comp1.getAlarm() != NULL);
}
void CComponent_TS1 :: test_validLocation()
{
	string szLoc = "hyderabad";
	pCc->setLocation(szLoc);
	string szRet= pCc->getLocation();
	CPPUNIT_ASSERT_MESSAGE("failed",szRet =="hyderabad");
}
void CComponent_TS1:: test_validSummary()
{
	string szSummary ="abcdefg";
	pCc->setSummary(szSummary);
	string szRet = pCc->getSummary();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Summary",szRet =="abcdefg");
}
void CComponent_TS1:: test_emptySummary()
{
	string szSum = "";
	bool bRet =pCc->setSummary(szSum);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set empty Summary",bRet == false);
}
void CComponent_TS1 :: test_validModifiedTime()
{
	if(pCc->setLastModified(45678))
		CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Modified Time",pCc->getLastModified()==45678);
}

void CComponent_TS1 :: test_Status()
{
	if(pCc->setStatus(1))
		CPPUNIT_ASSERT_MESSAGE("Error: conflict in Status",pCc->getStatus()==1);
}
void CComponent_TS1 :: test_Type()
{
	int f=0;
	pCc->setType(2);
	if((pCc->getType()==1)||(pCc->getType()==2)||(pCc->getType()==3))
	{
		f=1;
	}
	CPPUNIT_ASSERT_MESSAGE("Error: trying to set invalid type",f==1);
}

void CComponent_TS1 :: test_Id()
{
	string set = "qwerty";
	pCc->setId(set);
	CPPUNIT_ASSERT_MESSAGE("Error: conflict in id",(pCc->getId()==set));
}

void CComponent_TS1 :: test_Description()
{
	string  szDes ="mails";
	pCc->setDescription(szDes);
	string szRet = pCc->getDescription();
	CPPUNIT_ASSERT_MESSAGE("Error: trying to set invalid description",szRet == "mails");
}
void CComponent_TS1 :: test_emptyDescription()
{
	string szS = "";
	bool bRet =pCc->setDescription(szS);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set empty Description",bRet==true);
}
void CComponent_TS1 :: test_validDateStart()
{
	if(pCc->setDateStart(45678))
		CPPUNIT_ASSERT_MESSAGE("Error: Failed to set event start time",pCc->getDateStart()==45678);
}
/* ValidDateEndTestCase */
void CComponent_TS1 :: test_validDateEnd()
{

	if(pCc->setDateEnd(45678))
		CPPUNIT_ASSERT_MESSAGE("Error: Failed to set event end time",pCc->getDateEnd()==45678);
}

void CComponent_TS2 :: test_toString()
{
	CAlarm *alarm = new CAlarm(10000, 1);
	bool ret = pCc->setAlarm(alarm);
	CRecurrence *rec = new CRecurrence();
	ret = pCc->setRecurrence(rec);

	int duration ;
	duration = pCc->getDuration();
	string  szDes ="mails";
	pCc->setDescription(szDes);
	pCc->setDateStart(45678);
	pCc->setDateEnd(45678);
	pCc->setFlags(4);
	pCc->setType(2);
	pCc->setSummary("Summary");
	pCc->setLocation("LOC");
	pCc->setLastModified(12345678);
	pCc->setAllDay(0);
	pCc->setRtype(1);
	pCc->setCreatedTime(12364589);

	string set = "qwerty";
	ret = pCc->setId(set);
	cout<<pCc->toString();
}
void CComponent_TS2 :: test_toString_flags()
{
	pCc->setFlags(HAS_ATTENDEE);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_ATTENDEE);
	pCc->setFlags(HAS_ORGANIZER);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_ORGANIZER);
	pCc->setFlags(HAS_RECURRENCE);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_RECURRENCE);
	pCc->setFlags(HAS_ALARM);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_ALARM);
	pCc->setFlags(HAS_RECURRENCE_ALARM);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_RECURRENCE_ALARM);
	pCc->setFlags(HAS_PARTICIPANT);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_PARTICIPANT);
	pCc->setFlags(HAS_CATEGORIES);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_CATEGORIES);
	pCc->setFlags(HAS_COMMENT);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_COMMENT);
	pCc->setFlags(HAS_EXTRA);
	cout << pCc->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pCc->getFlags() == HAS_EXTRA);
}
void CComponent_TS2 :: test_toString_NULL()
{
	CAlarm *alarm = new CAlarm(10000, 1);
	bool ret = pCc->setAlarm(alarm);
	string  szDes ="";
	pCc->setDescription(szDes);
	pCc->setDateStart(45678);
	pCc->setDateEnd(45678);
	pCc->setSummary("");
	pCc->setLocation("");

	string set = "";
	ret = pCc->setId(set);
	cout<<pCc->toString();
}

void CComponent_TS1 :: test_getRecurrenceProperties_invalid()
{
	bool ret;
	ret = pCc->getRecurrenceProperties();	
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to get Recurrence properties for non recuurent event",ret==false);

}

void CComponent_TS1 :: test_generateInstanceTimes_invalid()
{
	vector < time_t > ret;
	ret = pCc->generateInstanceTimes(-1,-1);	
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to geinerate instances for  non recuurent event",ret.size() == 0);
}

void CComponent_TS1 :: test_getInstanceNumber_invalid()
{
	int ret;
	ret = pCc->getInstanceNumber(-1,-1);	
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to get instance number for non recurrent event",ret == 0);
}
void CComponent_TS1 :: test_retrieveRecurreceRuleObject_invalid()
{
	vector < CRecurrenceRule * > ret;
	ret = pCc->retrieveRecurreceRuleObject();	
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to retrieve recurrence rule object for non recurrent event",ret.size() == 0);
}
void CComponent_TS1 :: test_retrieveRecurreceRuleObject()
{
	vector < CRecurrenceRule * > ret;
	ret = pCc->retrieveRecurreceRuleObject();	
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to retrieve recurrence rule object ",ret.size() >= 0);
}
void CComponent_TS1 :: test_updateUntilInDB()
{
	int pErrorCode;
	pCc->updateUntilInDB(time (NULL),pErrorCode);	
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to update until in DB ",pErrorCode == CALENDAR_OPERATION_SUCCESSFUL);
}
