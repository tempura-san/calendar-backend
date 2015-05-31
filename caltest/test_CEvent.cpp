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
#include "CEvent.h"
#include "test_CEvent.h"
#include "CCalendarDB.h"
#include "CCalendar.h"
#include "CMulticalendar.h"
#include "Common.h"


CPPUNIT_TEST_SUITE_REGISTRATION(CEvent_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CEvent_TS2);

using namespace std;

void CEvent_TS1::setUp()
{
	pCe = new CEvent();
}

void CEvent_TS1::tearDown()
{
	delete pCe;
}

void CEvent_TS2::setUp()
{
	pCe = new CEvent();
}

void CEvent_TS2::tearDown()
{
	delete pCe;
}


void CEvent_TS1 ::test_SetGeo_Valid()
{
	string szGeo1="Geo";
	pCe->setGeo(szGeo1);
	string szRet=pCe->getGeo();
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(szRet==szGeo1));
}

void CEvent_TS1 ::test_SetGeo_Empty()
{
	string szGeo1="";
	bool bRet=pCe->setGeo(szGeo1);
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(bRet==true));
}

void CEvent_TS1 ::test_SetTransparency_Valid()
{
	string szTransp="OPAQUE";
	pCe->setTransparency(szTransp);
	string szRet=pCe->getTransparency();
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(szRet==szTransp));
}

void CEvent_TS1 ::test_SetTransparency_invalid()
{
	string szTransp="";
	bool bRet=pCe->setTransparency(szTransp);
	CPPUNIT_ASSERT_MESSAGE("Error : setting the empty value",(bRet==false));
}

void CEvent_TS1 ::test_SetPriority_Negative()
{
	bool bRet=pCe->setPriority(-8);
	CPPUNIT_ASSERT_MESSAGE("Error : setting the negative  value",(bRet==false));
}

void CEvent_TS1 ::test_SetPriority_Valid()
{
	pCe->setPriority(8);
	int iRet=pCe->getPriority();
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(iRet==8));
}

/**
 * Tests the behaviour in case the maximum priority for an event is exceeded.
 * Expected behaviour: function returns false.
 */
void CEvent_TS1::test_SetPriority_Inrange()
{
	bool bRet = pCe->setPriority(MAX_PRIORITY + 1);
	CPPUNIT_ASSERT_MESSAGE(
			"Unexpectedly succeeded setting priority greater than MAX_PRIORITY",
			(bRet == false));
}

void CEvent_TS1::test_setCreatedTimeZero()
{
	pCe->setCreatedTime(0);
	int stRet=pCe->getCreatedTime();
	CPPUNIT_ASSERT_MESSAGE("Error : setting the zero value",(stRet==0));
}



void CEvent_TS1::test_setCreatedTimeValid()
{
	pCe->setCreatedTime(80);
	int stRet=pCe->getCreatedTime();
	CPPUNIT_ASSERT_MESSAGE("Error : in  getting time",(stRet==80));
}

void CEvent_TS1::test_setDateStampZero()
{
	pCe->setDateStamp(0);
	int stRet=pCe->getDateStamp();
	CPPUNIT_ASSERT_MESSAGE("Error : setting 0 time",(stRet==0));
}

void CEvent_TS1::test_setDateStampValid()
{
	pCe->setDateStamp(80);
	int stRet=pCe->getDateStamp();
	CPPUNIT_ASSERT_MESSAGE("Error : in  getting time",(stRet==80));
}

void CEvent_TS1::test_setOrganizer_Null()
{
	bool bRet;
	COrganizer * pOrganizer=NULL;
	bRet=pCe->setOrganizer(pOrganizer);
	CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value",(bRet==false));
}
void CEvent_TS1::test_setOrganizer_Valid()
{
	COrganizer *pRet;
	COrganizer * pOrganizer=new COrganizer();
	pCe->setOrganizer(pOrganizer);
	pRet=pCe->getOrganizer();
	CPPUNIT_ASSERT_MESSAGE("Error :in returning",!(pRet==NULL));
}

void CEvent_TS1::test_setAttendees_Null()
{
	vector <CAttendee *> vAttendees;
	vector <CAttendee *> vRecattendees;
	bool ret = pCe->setAttendees(vAttendees);
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(ret==false));
}

void CEvent_TS1::test_setAttendees_Valid()
{
	vector<CAttendee *> vAttendees;
	vector<CAttendee *> vRecattendees;
	CAttendee *pAttendee1=new CAttendee();
	CAttendee *pAttendee2=new CAttendee();
	vAttendees.push_back(pAttendee1);
	vAttendees.push_back(pAttendee2);
	pCe->setAttendees(vAttendees);
	vRecattendees=pCe->getAttendees();
	CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value",(vRecattendees.size()>0));
}

void CEvent_TS1::test_setAttachments_Null()
{
	vector <string> vAttachments;
	bool ret = pCe->setAttachments(vAttachments);
	CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value",(ret==false));
}
void CEvent_TS1::test_setAttachments_Valid()
{
	vector<string> vAttachments;
	vector<string> vRecattachments;
	string szAttachment1="one";
	string szAttachment2="two";
	vAttachments.push_back(szAttachment1);
	vAttachments.push_back(szAttachment2);
	pCe->setAttachments(vAttachments);
	vRecattachments=pCe->getAttachments();
	CPPUNIT_ASSERT_MESSAGE("Error :setting valid value",(vRecattachments.size()>0));
}

void CEvent_TS1::test_setSequence_Valid()
{
	bool ret = pCe->setSequence(9);
	CPPUNIT_ASSERT_MESSAGE("Error :setting valid value",(ret==true));
}

void CEvent_TS1::test_setUid_Valid()
{
	bool ret = pCe->setUid(9);
	CPPUNIT_ASSERT_MESSAGE("Error :setting valid value",(ret==true));
}

void CEvent_TS1::test_setUrl_Valid()
{
	string szUrl="iuiu";
	bool ret = pCe->setUrl(szUrl);
	CPPUNIT_ASSERT_MESSAGE("Error :setting valid value",(ret==true));
}

void CEvent_TS1::test_setUrl_Empty()
{
	bool bRet;
	string szUrl="";
	bRet=pCe->setUrl(szUrl);
	CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value",(bRet==true));
}

void CEvent_TS1::test_setCategories_Valid()
{
	string szCate="iuiu";
	bool ret = pCe->setCategories(szCate);
	CPPUNIT_ASSERT_MESSAGE("Error :setting valid value",(ret==true));
}

void CEvent_TS1::test_setCategories_Empty()
{
	string szUrl="";
	bool szRet=pCe->setCategories(szUrl);
	CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value",(szRet==true));
}


void CEvent_TS1::test_setComments_Valid()
{
	string  szCate="iuiu";
	bool ret = pCe->setComments(szCate);
	CPPUNIT_ASSERT_MESSAGE("Error :setting valid value",(ret==true));
}

void CEvent_TS1::test_setComments_Empty()
{
	string szCate="";
	bool bRet=pCe->setComments(szCate);
	CPPUNIT_ASSERT_MESSAGE("Error :setting Empty  value",(bRet==true));
}

void CEvent_TS1::test_setContact_Valid()
{
	string szCont="iuiu";
	bool ret = pCe->setContact(szCont);

	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
}

void CEvent_TS1::test_setContact_Empty()
{
	string szCont="";
	bool bRet=pCe->setContact(szCont);
	CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value",(bRet==true));
}

void CEvent_TS1::test_setRelated_Valid()
{
	string szCont="r";
	bool ret = pCe->setRelated(szCont);
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
}

void CEvent_TS1::test_setRelated_Empty()
{
	string szCont="";
	bool ret =pCe->setRelated(szCont);
	CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value",(ret==true));
}

void CEvent_TS1::test_setResources_Valid()
{
	string  szCont="r";
	bool ret = pCe->setResources(szCont);
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
}
void CEvent_TS1::test_setResources_Empty()
{
	string szCont="";
	bool bRet=pCe->setResources(szCont);
	CPPUNIT_ASSERT_MESSAGE("Error :setting Empty value",(bRet==true));
}

void CEvent_TS1::test_setXProperties_Valid()
{

	vector <CProperties*> xprop;
	vector <CProperties*> retxprop;
	CProperties* prop = new CProperties();
	prop->setPropName("X_ABC");
	PropType d;
	d.szString = "test";
	prop->setPropValue(d);
	xprop.push_back(prop);

	pCe->setXProperties(xprop);
	retxprop=pCe->getXProperties();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(retxprop.size()==1));
}


void CEvent_TS1 :: test_validLocation()
{
	string szL ="hyderabad";
	bool ret = pCe->setLocation(szL);
	string szRet = pCe->getLocation();
	CPPUNIT_ASSERT_MESSAGE("Error: trying to set valid type",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("failed",(szRet==szL));
}

void CEvent_TS1 :: test_validSummary()
{
	string szS="abcdefgh";
	bool ret = pCe->setSummary(szS);
	string szRet = pCe->getSummary();
	CPPUNIT_ASSERT_MESSAGE("Error: trying to set valid type",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Summary",(szRet==szS));
}

void CEvent_TS1 :: test_emptySummary()
{
	bool bRet =pCe->setSummary("");
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set empty Summary",(bRet==false));
}


/* ValidModifiedTimeTestCase */
void CEvent_TS1 :: test_validModifiedTime()
{
	bool ret = pCe->setLastModified(10045678);
	CPPUNIT_ASSERT_MESSAGE("Error: trying to set valid type",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Modified Time",(pCe->getLastModified()==10045678));
}



/* StatusTestCase */
void CEvent_TS1 :: test_Status()
{
	bool ret = pCe->setStatus(1);
	CPPUNIT_ASSERT_MESSAGE("Error: trying to set valid type",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error: conflict in Status",(pCe->getStatus()==1));
}



/* TypeTestCase */
void CEvent_TS1 :: test_Type()
{
	bool ret = pCe->setType(2);
	CPPUNIT_ASSERT_MESSAGE("Error: conflict in id",pCe->getType()==2);
	CPPUNIT_ASSERT_MESSAGE("Error: trying to set valid type",(ret==true));
}


/* IDTestCase */
void CEvent_TS1 :: test_Id()
{
	string set = "somestring";
	bool ret = pCe->setId(set);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set valid ID",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error: conflict in id",(pCe->getId()==set));
}

/* DescriptionTestCase */
void CEvent_TS1 :: test_Description()
{
	string  szDes = "mails";
	bool ret = pCe->setDescription(szDes);
	CPPUNIT_ASSERT_MESSAGE("Error: trying to set invalid description",(ret==true));
}

/* EmptyDescriptionTestCase*/
void CEvent_TS1 :: test_emptyDescription()
{
	string szDesc ="";
	bool bRet =pCe->setDescription(szDesc);
	CPPUNIT_ASSERT_MESSAGE("Error: Trying to set empty Description",(bRet==false));
}

/* ValidDateStartTestCase */
void CEvent_TS1 :: test_validDateStart()
{
	if(pCe->setDateStart(10045678))
		CPPUNIT_ASSERT_MESSAGE("Error: Failed to set event start time",(pCe->getDateStart()==10045678));
}

/* ValidDateEndTestCase */
void CEvent_TS1 :: test_validDateEnd()
{
	if(pCe->setDateEnd(10045678))
		CPPUNIT_ASSERT_MESSAGE("Error: Failed to set event end time",(pCe->getDateEnd()==10045678));
}

void CEvent_TS1 :: test_validCEventCopyConstructor()
{
	CEvent event(*pCe);
	CPPUNIT_ASSERT_MESSAGE ("Error: test_validCEventCopyConstructor", strcmp((pCe->getGeo().c_str()), (event.getGeo().c_str()))==0); 
	CPPUNIT_ASSERT_MESSAGE ("Error: test_validCEventCopyConstructor", strcmp((pCe->getTransparency().c_str()), (event.getTransparency().c_str()))==0); 
	CPPUNIT_ASSERT_MESSAGE ("Error: test_validCEventCopyConstructor", (pCe->getPriority() == event.getPriority())); 
}
void CEvent_TS1  :: test_validCEventAssignementOperator()
{
	CEvent event;
	event = *pCe;
	CPPUNIT_ASSERT_MESSAGE ("Error: test_validCEventCopyConstructor", strcmp((pCe->getGeo().c_str()), (event.getGeo().c_str()))==0); 
	CPPUNIT_ASSERT_MESSAGE ("Error: test_validCEventCopyConstructor", strcmp((pCe->getTransparency().c_str()), (event.getTransparency().c_str()))==0); 
	CPPUNIT_ASSERT_MESSAGE ("Error: test_validCEventCopyConstructor", (pCe->getPriority() == event.getPriority())); 
}

void CEvent_TS2 :: test_toString()
{
	time_t start,end;
	time(&start);
	end = start+10500;
	CAlarm *pAlarm;
	pAlarm  = new CAlarm(start+10,1);


	CEvent *pEvent = new CEvent("summary", "description","location", start + 10000, end);
	CRecurrence *rec = new CRecurrence();
	pEvent->setRecurrence(rec);
	
	string  szDes ="mails";
	pEvent->setDescription(szDes);
	pEvent->setDateStart(45678);
	pEvent->setDateEnd(45678);
	pEvent->setFlags(4);
	pEvent->setType(2);
	pEvent->setSummary("Summary");
	pEvent->setLocation("LOC");
	pEvent->setLastModified(12345678);
	pEvent->setAllDay(0);
	pEvent->setRtype(1);
	pEvent->setCreatedTime(12364589);
	pEvent->setTransparency("OPAQUE");
	pEvent->setPriority(4);
	pEvent->setAlarm(pAlarm);
	cout<<pEvent->toString();

}
void CEvent_TS2 :: test_toString_NULL()
{
	time_t start,end;
	time(&start);
	end = start+10500;
	string sum="";


	CEvent *pEvent = new CEvent(sum,sum,sum, start + 10000, end);
	pEvent->setTransparency(sum);
	pEvent->setPriority(4);
	cout<<pEvent->toString();

}
void CEvent_TS2 :: test_toString_flags()
{
	time_t start,end;
	time(&start);
	end = start+10500;
	CEvent *pEvent = new CEvent("summary", "description","location", start + 10000, end);
	pEvent->setFlags(HAS_ATTENDEE);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_ATTENDEE);
	pEvent->setFlags(HAS_ORGANIZER);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_ORGANIZER);
	pEvent->setFlags(HAS_RECURRENCE);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_RECURRENCE);
	pEvent->setFlags(HAS_ALARM);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_ALARM);
	pEvent->setFlags(HAS_RECURRENCE_ALARM);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_RECURRENCE_ALARM);
	pEvent->setFlags(HAS_PARTICIPANT);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_PARTICIPANT);
	pEvent->setFlags(HAS_CATEGORIES);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_CATEGORIES);
	pEvent->setFlags(HAS_COMMENT);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_COMMENT);
	pEvent->setFlags(HAS_EXTRA);
	cout << pEvent->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pEvent->getFlags() == HAS_EXTRA);
}

