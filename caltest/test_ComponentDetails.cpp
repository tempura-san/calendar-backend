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
#include <map>
#include "test_ComponentDetails.h"
#include "CComponentDetails.h"
#include "CCalendar.h"
#include "CMulticalendar.h"
#include "Common.h"


CPPUNIT_TEST_SUITE_REGISTRATION(ComponentDetails_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(ComponentDetails_TS2);
using namespace std;


void ComponentDetails_TS1::setUp()
{
	pCd= new CComponentDetails();	

	multi = CMulticalendar::MCInstance();
	
}
void ComponentDetails_TS1::tearDown()
{
	delete pCd;
}

void ComponentDetails_TS2::setUp()
{
	pCd= new CComponentDetails();	

	multi = CMulticalendar::MCInstance();
}
void ComponentDetails_TS2::tearDown()
{
	delete pCd;
}

void ComponentDetails_TS1::test_setCreatedTimeValid()
{
	pCd->setCreatedTime(80);
	int stRet=pCd->getCreatedTime();
	CPPUNIT_ASSERT_MESSAGE("Error : in  getting time",(stRet==80));
}

void ComponentDetails_TS1::test_setDateStampZero()
{
	bool ret = pCd->setDateStamp(-1);
	CPPUNIT_ASSERT_MESSAGE("Error :setting improper value",(ret==false));
}

void ComponentDetails_TS1::test_setDateStampValid()
{
	bool ret = pCd->setDateStamp(80);
	int stRet=pCd->getDateStamp();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : in  getting time",(stRet==80));
}

void ComponentDetails_TS1::test_setOrganizer_Null()
{
	bool bRet=pCd->setOrganizer(NULL);
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(bRet==false));
}

void ComponentDetails_TS1::test_setOrganizer_Valid()
{
	COrganizer *pRet;
	
	COrganizer * pOrganizer=new COrganizer();
	
	bool ret = pCd->setOrganizer(pOrganizer);
	pRet=pCd->getOrganizer();
	
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :in returning",(pRet!=NULL));
}

void ComponentDetails_TS1::test_setAttendees_Valid()
{
	vector<CAttendee *> vAttendees;
	vector<CAttendee *> vRecattendees;
	
	CAttendee *pAttendee1=new CAttendee();
	CAttendee *pAttendee2=new CAttendee();
	
	vAttendees.push_back(pAttendee1);
	vAttendees.push_back(pAttendee2);
	
	bool ret = pCd->setAttendees(vAttendees);
	
	vRecattendees=pCd->getAttendees();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting  value",(vRecattendees.size()>0));
}

void ComponentDetails_TS1::test_setHashMap_Valid()
{
	 map< string, vector < CParameters* > > hashmap;
	 map< string, vector < CParameters* > > rethashmap;
	 
	 CParameters *pParam = new CParameters();
	 vector <CParameters *> vParam;
	 
	 pParam->setParamName(LANGUAGE);
	 ParamType t;
	 t.szString = "eng";
	 pParam->setParamValue(t);
	 vParam.push_back(pParam);

	 hashmap.insert(pair<string, vector < CParameters* > >(SUMMARY,vParam));
	 bool ret = pCd->setHashMap(hashmap);
	 
	 rethashmap = pCd->getHashMap();
	 CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	 CPPUNIT_ASSERT_MESSAGE("Error :setting  value",(rethashmap.size() == 1));



}
void ComponentDetails_TS1::test_setAttachments_Valid()
{
	vector<string> vAttachments;
	vector<string> vRecattachments;
	
	string szAttachment1="one";
	string szAttachment2="two";
	vAttachments.push_back(szAttachment1);
	vAttachments.push_back(szAttachment2);

	bool ret = pCd->setAttachments(vAttachments);
	vRecattachments=pCd->getAttachments();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(vRecattachments.size() != 0));
}

void ComponentDetails_TS1::test_setSequence_Valid()
{
	bool ret = pCd->setSequence(9);
	int iRet=pCd->getSequence();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(iRet==9));
}

void ComponentDetails_TS1::test_setSequence_Invalid()
{
	bool bRet = pCd->setSequence(-9);
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(bRet==false));
}

void ComponentDetails_TS1::test_setUid_Valid()
{
	bool ret = pCd->setUid(9);
	int iRet=pCd->getUid();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(iRet==9));
}

void ComponentDetails_TS1::test_setUid_Invalid()
{
	bool bRet =pCd->setUid(-9);//negative
	CPPUNIT_ASSERT_MESSAGE("Error :setting negative value",(bRet==false));
}

void ComponentDetails_TS1::test_setUrl_Valid()
{
	string szUrl="value string";
	bool ret = pCd->setUrl(szUrl);
	string szRet=pCd->getUrl();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(szRet==szUrl));
}

void ComponentDetails_TS1::test_setUrl_Empty()
{
	string szUrl="";
	bool bRet=pCd->setUrl(szUrl);
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(bRet==true));
}

void ComponentDetails_TS1::test_setCategories_Valid()
{
	string szCate="value string";
	bool ret = pCd->setCategories(szCate);
	string szRet=pCd->getCategories();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",szRet==szCate);
}

void ComponentDetails_TS1::test_setCategories_Empty()
{
	string szUrl="";
	bool bRet=pCd->setCategories(szUrl);
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(bRet==true));
}

void ComponentDetails_TS1::test_setComments_Valid()
{
	string  szCate="value string";
	bool ret = pCd->setComments(szCate);
	string szRet=pCd->getComments();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(szRet==szCate));
}

void ComponentDetails_TS1::test_setComments_Empty()
{
	string szCate="";
	bool bRet=pCd->setComments(szCate);
	CPPUNIT_ASSERT_MESSAGE("Error :setting Empty  value",(bRet==true));
}

void ComponentDetails_TS1::test_setContact_Valid()
{
	string szCont="value string";
	bool ret = pCd->setContact(szCont);
	string szRet=pCd->getContact();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(szRet==szCont));
}

void ComponentDetails_TS1::test_setContact_Empty()
{
	string szCont="";
	bool bRet=pCd->setContact(szCont);
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(bRet==true));
}

void ComponentDetails_TS1::test_setRelated_Valid()
{
	string szCont="r";
	bool ret = pCd->setRelated(szCont);
	string szRet=pCd->getRelated();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",szRet==szCont);
}

void ComponentDetails_TS1::test_setRelated_Empty()
{
	string szCont="";
	bool ret =pCd->setRelated(szCont);
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(ret==true));
}

void ComponentDetails_TS1::test_setResources_Valid()
{
	string  szCont="r";
	bool ret = pCd->setResources(szCont);
	string szRet=pCd->getResources();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(szRet==szCont));
}

void ComponentDetails_TS1::test_setResources_Empty()
{
	string szCont="";
	bool bRet=pCd->setResources(szCont);
	CPPUNIT_ASSERT_MESSAGE("Error :setting Empty value",(bRet==true));
}

void ComponentDetails_TS1::test_setXProperties_Valid()
{
	vector<CProperties *> vProp;
	vector<CProperties *> vRetProp;
	
	CProperties *pProp1=new CProperties();
	
	vProp.push_back(pProp1);
	
	bool ret = pCd->setXProperties(vProp);
	
	vRetProp = pCd->getXProperties();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :setting  value",(vRetProp.size()==1));
}


void ComponentDetails_TS1::test_setClas_Valid()
{
	string url="value string";
	bool ret = pCd->setClas(url);
	string szRet=pCd->getClas();
	CPPUNIT_ASSERT_MESSAGE("Error :setting proper value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error :getting proper value",szRet=="value string");
}

void ComponentDetails_TS1::test_setClas_Empty()
{
	bool bRet;
	string  url="";
	bRet=pCd->setClas(url);
	CPPUNIT_ASSERT_MESSAGE("Error :setting empty value",(bRet==true));
}

void ComponentDetails_TS1::test_retrieveOrganizerDetails()
{
	 time_t start,end,until;
         time(&start);
	 int error_code = -1;
         end = start+10500;
         until=start+50000;
         CEvent *pEvent = new CEvent("summary for new", "description",
                                    "location", start + 10000, end);
         pEvent->setTransparency("OPAQUE");
         pEvent->setPriority(4);
	 CAttendee *pAt1=new CAttendee();
	 vector <CAttendee *> vAttends;
	 pAt1->setMember("Member1");
	 pAt1->setRole(CHAIR);
	 pAt1->setParticipationStatus(TENTATIVE);
	 pAt1->setDelegatees("Someone");
	 pAt1->setDelegator("Someone");
	 vAttends.push_back(pAt1);
	 pEvent->setAttendees(vAttends);
	 COrganizer *pOrga= new COrganizer();
	 pOrga->setLanguage("English");
	 pOrga->setSentBy("Myself");
	 pOrga->setCommonName("Something");
	 pOrga->setXparameters("WhatisThis");
	 pOrga->setCalAddress("!!!!!!!!!");
	 pOrga->setDirectoryParameter("Home");
	 pEvent->setOrganizer(pOrga);
	 calendar=new CCalendar("SampleTestCalendar1",COLOUR_RED,0,1,LOCAL_CALENDAR, "Music Tune.xyz","Version-1.0");
	 multi->addCalendar(calendar, error_code);
         calendar->addEvent(pEvent,error_code);
	 string eventid = pEvent->getId();
	 pCd->setId(eventid);
	 COrganizer *recover = pCd->retrieveOrganizerDetails();
	 CPPUNIT_ASSERT_MESSAGE("Error :Not empty value",(recover!=NULL));
	 calendar->deleteEvent(eventid, error_code);
}

void ComponentDetails_TS1::test_retrieveAttendeeDetails()
{
	 time_t start,end,until;
         time(&start);
	 int error_code = -1;
         end = start+10500;
         until=start+50000;
         CEvent *pEvent = new CEvent("summary for new", "description",
                                    "location", start + 10000, end);
         pEvent->setTransparency("OPAQUE");
         pEvent->setPriority(4);
	 CAttendee *pAt1=new CAttendee();
	 vector <CAttendee *> vAttends;
	 pAt1->setMember("Member1");
	 pAt1->setRole(CHAIR);
	 pAt1->setParticipationStatus(TENTATIVE);
	 pAt1->setDelegatees("Someone");
	 pAt1->setDelegator("Someone");
	 vAttends.push_back(pAt1);
	 pEvent->setAttendees(vAttends);
	 calendar=multi->getCalendarByName("SampleTestCalendar1", error_code);
         calendar->addEvent(pEvent,error_code);
	 string eventid = pEvent->getId();
	 pCd->setId(eventid);
	 vector <CAttendee *> recover = pCd->retrieveAttendeeDetails();
	 CPPUNIT_ASSERT_MESSAGE("Error :Not empty value",(recover.size()>0));
	 calendar->deleteEvent(eventid, error_code);
}

void ComponentDetails_TS1::test_retrieveXPropertyDetails()
{
	 time_t start,end,until;
         time(&start);
	 int error_code = -1;
         end = start+10500;
         until=start+50000;
         CEvent *pEvent = new CEvent("summary for new", "description",
                                    "location", start + 10000, end);
         pEvent->setTransparency("OPAQUE");
         pEvent->setPriority(4);
	
	 CProperties *pProp = new CProperties();
	 vector <CProperties *> vProps;
	 pProp->setPropName("X-XYZ");
	 PropType t;
	 t.szString = "XYZ";
	 pProp->setPropValue(t);
	 vProps.push_back(pProp);
	 pEvent->setXProperties(vProps);
	 calendar=multi->getCalendarByName("SampleTestCalendar1", error_code);
         calendar->addEvent(pEvent,error_code);
	 string eventid = pEvent->getId();
	 pCd->setId(eventid);
	 vector <CProperties *> recover = pCd->retrieveXPropertyDetails();
	 CPPUNIT_ASSERT_MESSAGE("Error :Not empty value",(recover.size()>0));
	 calendar->deleteEvent(eventid, error_code);
}

void ComponentDetails_TS1::test_retrieveParameterDetails()
{
	 time_t start,end,until;
         time(&start);
	 int error_code = -1;
         end = start+10500;
         until=start+50000;
         CEvent *pEvent = new CEvent("summary for new", "description",
                                    "location", start + 10000, end);
         pEvent->setTransparency("OPAQUE");
         pEvent->setPriority(4);
	
	 map< string, vector < CParameters* > > hashmap;
	 map< string, vector < CParameters* > > rethashmap;
	 CParameters *pParam = new CParameters();
	 vector <CParameters *> vParam;
	 pParam->setParamName(LANGUAGE);
	 ParamType t;
	 t.szString = "eng";
	 pParam->setParamValue(t);
	 vParam.push_back(pParam);

	 hashmap.insert(pair<string, vector < CParameters* > >(SUMMARY,vParam));
	 pEvent->setHashMap(hashmap);
	 
	 calendar=multi->getCalendarByName("SampleTestCalendar1", error_code);
         calendar->addEvent(pEvent,error_code);
	 string eventid = pEvent->getId();
	 pCd->setId(eventid);
	 rethashmap = pCd->retrieveParameterDetails();
	 CPPUNIT_ASSERT_MESSAGE("Error :Not empty value",(rethashmap.size() > 0));
	 calendar->deleteEvent(eventid, error_code);
}

void ComponentDetails_TS2::test_toString()
{

	CComponentDetails *comp= new CComponentDetails("Summary","Descripion","LOC",1239098130,-1);
	CAlarm *alarm = new CAlarm(10000, 1);
	bool ret = comp->setAlarm(alarm);
	CRecurrence *rec = new CRecurrence();
	ret = comp->setRecurrence(rec);
	

	comp->setDateStart(45678);
	comp->setDateEnd(45678);
	comp->setFlags(4);
	comp->setType(2);
	comp->setSummary("Summary");
	comp->setLocation("LOC");
	comp->setLastModified(12345678);
	comp->setAllDay(0);
	comp->setRtype(1);
	comp->setCreatedTime(12364589);

	string set = "qwerty";
	comp->setId(set);

	comp->setClas("someURL");
	comp->setSequence(5);
	comp->setUid(6);
	comp->setUrl("URL");
	comp->setCategories("Categories");
	comp->setComments("Comments");
	comp->setRelated("Related");
	COrganizer *pOrga= new COrganizer();
	pOrga->setLanguage("English");
	pOrga->setSentBy("Myself");
	pOrga->setCommonName("Something");
	pOrga->setXparameters("WhatisThis");
	pOrga->setCalAddress("!!!!!!!!!");
	pOrga->setDirectoryParameter("Home");
	comp->setOrganizer(pOrga);
	CAttendee *pAt1=new CAttendee();
	vector <CAttendee *> vAttends;
	pAt1->setMember("Member1");
	pAt1->setRole(CHAIR);
	pAt1->setParticipationStatus(TENTATIVE);
	pAt1->setDelegatees("Someone");
	pAt1->setDelegator("Someone");
	vAttends.push_back(pAt1);
	comp->setAttendees(vAttends);
	CProperties *pProp = new CProperties();
	vector <CProperties *> vProps;
	pProp->setPropName("X-XYZ");
	PropType t;
	t.szString = "XYZ";
	pProp->setPropValue(t);
	vProps.push_back(pProp);
	comp->setXProperties(vProps);
	vector<string> vAttachments;
	string szAttachment1="one";
	string szAttachment2="two";
	vAttachments.push_back(szAttachment1);
	vAttachments.push_back(szAttachment2);
	comp->setAttachments(vAttachments);
	cout<<comp->toString();
}

void ComponentDetails_TS2::test_toString_NULL()
{
	string sum="";	
	CComponentDetails *comp= new CComponentDetails(sum,sum,sum,1239098130,-1);
	comp->setDateStamp(80);
	comp->setClas(sum);
	cout<<comp->toString();
}
void ComponentDetails_TS2 :: test_toString_flags()
{
	CComponentDetails *comp= new CComponentDetails("Summary","Descripion","LOC",1239098130,-1);
	comp->setFlags(HAS_ATTENDEE);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_ATTENDEE);
	comp->setFlags(HAS_ORGANIZER);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_ORGANIZER);
	comp->setFlags(HAS_RECURRENCE);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_RECURRENCE);
	comp->setFlags(HAS_ALARM);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_ALARM);
	comp->setFlags(HAS_RECURRENCE_ALARM);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_RECURRENCE_ALARM);
	comp->setFlags(HAS_PARTICIPANT);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_PARTICIPANT);
	comp->setFlags(HAS_CATEGORIES);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_CATEGORIES);
	comp->setFlags(HAS_COMMENT);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_COMMENT);
	comp->setFlags(HAS_EXTRA);
	cout << comp->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",comp->getFlags() == HAS_EXTRA);
}

