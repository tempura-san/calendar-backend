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
#include <math.h>
#include "test_ICalConverter.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ICalConverter_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(ICalConverter_TS2);

int errorcode;
using namespace std;

void ICalConverter_TS1::setUp()
{
	iCalConv = new ICalConverter();
}

void ICalConverter_TS1::tearDown()
{
	//delete iCalConv;
	iCalConv = NULL;
}

static bool 
stringReplaceAll (
	string &toChange,
	const char *changeThis,
	const char *toThis)
{
    bool retval = false;
    size_t subStart;
    const string orig (changeThis);

    while ((subStart = toChange.find (changeThis)) != string::npos) {
	retval = true;
	toChange.replace (subStart, orig.length(), toThis);
    }

    return retval;
}
void ICalConverter_TS2::setUp()
{
	iCalConv = new ICalConverter();
}

void ICalConverter_TS2::tearDown()
{
	//delete iCalConv;
	iCalConv = NULL;
}

void ICalConverter_TS1 ::test_localToIcalVcal_ValidEvent()  //All entries are valid
{

         string sLoc;
         CAlarm *pAlarm;
	 time_t start,end,until;
         time(&start);
	 int errorcode = -1;

         end = start+10500;
         until=start+500000;

         CEvent *pEvent = new CEvent("» ¼ ½ ¾ ¿ À Á Â Ã", "description",
                                    "location", start + 10000, end);

         pEvent->setTransparency("OPAQUE");
         pEvent->setPriority(4);
	 pEvent->setCreatedTime(time(NULL));
	 pEvent->setLastModified(time(NULL));

	 pEvent->setCategories("Public Holiday");
	 pEvent->setSequence(1);
	 pEvent->setUrl("http://url.com");
	 pEvent->setGeo("132.23:-112.62");
	 pEvent->setContact("JoneSmith");
	 pEvent->setRelated("deloyoite");
	 pEvent->setClas("PUBLIC");
	 pEvent->setComments("having a restricted public holiday");
	 pEvent->setTzid("EET");
	 pEvent->setRelated("SIBLING");

	 int res;
	 res = pEvent->setStatus(FINAL_STATUS);

	/*Attachments*/
	 vector < string > vAttachments;
	 vAttachments.push_back ("EventAttach1");
	 pEvent->setAttachments (vAttachments);
	
	 /*Alarm*/
	 pAlarm  = new CAlarm(start + 100,1);
         pEvent->setAlarm(pAlarm);

	 /*Recurrence*/
         CRecurrence *pRecur = new CRecurrence();

         vector <string> rules;
         rules.clear();
         string s1("FREQ=");
         string s2(";UNTIL=");
         string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
         string rrule;
         struct icaltimetype uical;
	 int repeattype = 3;

         uical = icaltime_from_timet(until, 0);
         string ustring = icaltime_as_ical_string(uical);
         rrule = s1 + test[repeattype - 1] + s2 + ustring;
         rules.push_back(rrule);

         pEvent->setRtype(repeattype);
         pEvent->setUntil(until);
	 pRecur->setRrule(rules);
	 
	 /* Rdates and Edates,Exrules*/
	 vector <string> szrDateList;
	 szrDateList.push_back("19970714T123000Z");
	 szrDateList.push_back("19980714T123000Z");
	 szrDateList.push_back("19990714T123000Z");
	 pRecur->setRDays(szrDateList);
	 vector <string> szeDateList;
	 szeDateList.push_back("19970714T123000Z");
	 szeDateList.push_back("19980714T123000Z");
	 szeDateList.push_back("19990714T123000Z");
	 pRecur->setEDays(szeDateList);
	 vector<string> vErules;
	 string szS = "FREQ=YEARLY;UNTIL=20000131T090000Z";
	 vErules.push_back("FREQ=WEEKLY;COUNT=1;INTERVAL=1;BYDAY=TU,TH");
	 pRecur->setErule(vErules);

	 pEvent->setRecurrence(pRecur);

	 pEvent->setAllDay(1);

	 vector <CAttendee *> vAttends;

	 CAttendee *pAt1=new CAttendee();
	 pAt1->setMember("Member1");
	 pAt1->setCalendarUserType(ROOM);
         pAt1->setRole(NON_PARTICIPANT);
         pAt1->setParticipationStatus(IN_PROCESS);
	 pAt1->setDelegatees("Someone");
	 pAt1->setDelegator("Someone");
         pAt1->setCommonName("common name");
	 pAt1->setDirectoryParameter("ftp://DIR param");
         pAt1->setSentBy("sent by");
	 pAt1->setLanguage("English");
	 pAt1->setRSVP(1);

	 CAttendee *pAt2=new CAttendee();
	 pAt2->setCalendarUserType(GROUP);
	 pAt2->setRole(OPT_PARTICIPANT);
	 pAt2->setParticipationStatus(NEEDS_ACTION);
	 pAt2->setRSVP(0);
	 
	 CAttendee *pAt3=new CAttendee();
	 pAt3->setCalendarUserType(RESOURCE);
	 pAt3->setParticipationStatus(DECLINED);

	 CAttendee *pAt4=new CAttendee();
	 pAt4->setCalendarUserType(INDIVIDUAL);
	 pAt4->setParticipationStatus(DELEGATED);

	 CAttendee *pAt5=new CAttendee();
	 pAt5->setCalendarUserType(UNKNOWN_USER);
	 pAt5->setParticipationStatus(COMPLETED);

	 vAttends.push_back(pAt1);
	 vAttends.push_back(pAt2);
	 vAttends.push_back(pAt3);
	 vAttends.push_back(pAt4);
	 vAttends.push_back(pAt5);
	 pEvent->setAttendees(vAttends);

	 COrganizer *pOrga= new COrganizer();
	 pOrga->setLanguage("English");
	 pOrga->setSentBy("Myself");
	 pOrga->setCommonName("Something");
	 pOrga->setXparameters("WhatisThis");
	 pOrga->setCalAddress("!!!!!!!!!");
	 pOrga->setDirectoryParameter("Home");
	 pEvent->setOrganizer(pOrga);

	 pEvent->setType(E_EVENT);

	 CProperties* pProp = new CProperties();
	 vector < CProperties* > vProp;
	 pProp->setPropName("X-ABC");
	 PropType prop;
	 prop.szString = "XX";
	 pProp->setPropValue(prop);
	 vProp.push_back(pProp);
	 pEvent->setXProperties(vProp);

	 vector < CParameters* > vCat;
	 vector < CParameters* > vRes;
	 vector < CParameters* > vCon;
	 vector < CParameters* > vLoc;
	 vector < CParameters* > vSum;
	 vector < CParameters* > vDes;
	 vector < CParameters* > vCom;
	 vector < CParameters* > vRel;
	 vector < CParameters* > vPar;
	 vector < CParameters* > vAtt;

	 CParameters* pLang = new CParameters();
	 pLang->setParamName(LANGUAGE);
	 ParamType lang;
	 lang.szString = "ENG";
	 pLang->setParamValue(lang);
	 vCat.push_back(pLang);
	 vRes.push_back(pLang);
	 vCon.push_back(pLang);
	 vLoc.push_back(pLang);
	 vSum.push_back(pLang);
	 vDes.push_back(pLang);
	 vCom.push_back(pLang);
         
	 CParameters* pAlter = new CParameters();
	 pAlter->setParamName(ALTER);
	 ParamType alter;
	 alter.szString = "alternate rep";
	 pAlter->setParamValue(alter);
	 vRes.push_back(pAlter);
	 vCon.push_back(pAlter);
	 vLoc.push_back(pAlter);
	 vSum.push_back(pAlter);
	 vDes.push_back(pAlter);
	 vCom.push_back(pAlter);

         CParameters* pRel = new CParameters();
	 pRel->setParamName(RELTYPE);
	 ParamType rel;
	 rel.i = 0;
	 pRel->setParamValue(rel);
	 vRel.push_back(pRel);
	 
         CParameters* pPar = new CParameters();
	 pPar->setParamName("X-PARAM");
	 ParamType x;
	 x.szString = "paramval";
	 pPar->setParamValue(x);
	 vPar.push_back(pPar);

	 CParameters* pRel1 = new CParameters();
	 pPar->setParamName(RELTYPE);
	 ParamType rel1;
	 rel1.i = 1;
	 pRel1->setParamValue(rel1);
	 //vRel.push_back(pRel1);

	 /* */
	 CParameters* pRel2 = new CParameters();
	 pPar->setParamName(RELTYPE);
	 ParamType rel2;
	 rel2.i = 2;
	 pRel2->setParamValue(rel2);
	 //vRel.push_back(pRel2);
	
	 CParameters *pAtta = new CParameters();
	 pAtta->setParamName (FMTYPE);
	 ParamType Atta;
	 Atta.szString = "fmtype";
	 pAtta->setParamValue (Atta);
	 vAtt.push_back (pAtta);

	 map <string,vector<CParameters*> > hashmap;

	 hashmap.insert(pair<string,vector<CParameters*> >(CATEGORIES , vCat));
	 hashmap.insert(pair<string,vector<CParameters*> >(RESOURCES , vRes));
	 hashmap.insert(pair<string,vector<CParameters*> >(CONTACT , vCon));
	 hashmap.insert(pair<string,vector<CParameters*> >(LOCATION , vLoc));
	 hashmap.insert(pair<string,vector<CParameters*> >(SUMMARY , vSum));
	 hashmap.insert(pair<string,vector<CParameters*> >(DESCRIPTION , vDes));
	 hashmap.insert(pair<string,vector<CParameters*> >(COMMENT , vCom));
	 hashmap.insert(pair<string,vector<CParameters*> >(RELATEDTO , vRel));
	 hashmap.insert(pair<string,vector<CParameters*> >("X-PARAM" , vPar));
	 hashmap.insert(pair<string,vector<CParameters*> >(ATTACH , vAtt));
	 pEvent->setHashMap(hashmap);

	 string strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(NEEDSACTION_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());	
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(COMPLETED_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(INPROCESS_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(CANCELLED_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(TENTATIVE_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(DRAFT_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(FINAL_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(SENT_STATUS);	 
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(DECLINED_STATUS);	 
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(DELEGATED_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 pEvent->setStatus(ACCEPTED_STATUS);
	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,VCAL_TYPE,errorcode);
	 printf("\n%s\n",strIcal.c_str());
	 CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);

	 icaltimetype dtstartStd;
	 strIcal = iCalConv->exportTimeZone(7516800,"Europe/Helsinki",ICAL_TYPE);
	 printf("Export TimeZone ICal %s ",strIcal.c_str());
	 strIcal = iCalConv->exportTimeZone(4492800,"America/Sao_Paulo",ICAL_TYPE);
	 printf("Export TimeZone ICal %s ",strIcal.c_str());
	 strIcal = iCalConv->exportTimeZone(7516800,"Europe/Helsinki",VCAL_TYPE);
	 printf("Export TimeZone VCal %s ",strIcal.c_str());
	 strIcal = iCalConv->calculateMonthdayFromRule(7516800,dtstartStd,"FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU","EET");
	 printf("Calculate Month %s ",strIcal.c_str());

}

void ICalConverter_TS1 ::test_localToIcalVcal_InvalidEvent1() // null pointer
{
	string strIcal;
	CEvent *pEvent = NULL;

	strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : ",strIcal.empty()== true);
}


void ICalConverter_TS1 ::test_localToIcalVcal_ValidToDo()  //All entries are valid
{
	CTodo *pTodo = new CTodo;
        string sLoc;
        CAlarm *pAlarm;
	time_t start,end,until;
        time(&start);

        end = start+10500;
        until=start+500000;

	pTodo->setType(E_TODO);
        pTodo->setSummary("Valid Case setting proper data values to TODO");
        pTodo->setDescription("Valid Case setting proper data values to TODO");
        pTodo->setLocation("Valid Case setting proper data values to TODO");

	pTodo->setPriority(3);
	pTodo->setDue(time(NULL));
	pTodo->setPercentComplete(39);
	pTodo->setDateStamp(time(NULL));
	pTodo->setCreatedTime(time(NULL));
	pTodo->setLastModified(time(NULL));

	pTodo->setCategories("Public Holiday");
	pTodo->setSequence(1);
	pTodo->setUrl("http://url.com");
	pTodo->setGeo("132.23:-112.62");
	pTodo->setContact("JoneSmith");
	pTodo->setRelated("deloyoite");
	pTodo->setClas("PRIVATE");
	pTodo->setComments("having a restricted public holiday");
	pTodo->setStatus(COMPLETED_STATUS);

	pAlarm  = new CAlarm(start + 1850,1);
	pTodo->setAlarm(pAlarm);

	vector <CAttendee *> vAttends;
	CAttendee *pAt1=new CAttendee();
	pAt1->setMember("Member1");
	pAt1->setRole(CHAIR);
	pAt1->setParticipationStatus(TENTATIVE);
	pAt1->setDelegatees("Someone");
	pAt1->setDelegator("Someone");
	vAttends.push_back(pAt1);
	pTodo->setAttendees(vAttends);

	COrganizer *pOrga= new COrganizer();
	pOrga->setLanguage("English");
	pOrga->setSentBy("Myself");
	pOrga->setCommonName("Something");
	pOrga->setXparameters("WhatisThis");
	pOrga->setCalAddress("!!!!!!!!!");
	pOrga->setDirectoryParameter("Home");
	pTodo->setOrganizer(pOrga);

	pTodo->setType(E_TODO);

	CProperties* pProp = new CProperties();
	vector < CProperties* > vProp;
	pProp->setPropName("X-ABC");
	PropType prop;
	prop.szString = "XX";
	pProp->setPropValue(prop);
	vProp.push_back(pProp);
	pTodo->setXProperties(vProp);

	vector < CParameters* > vCat;
	vector < CParameters* > vRes;
	vector < CParameters* > vCon;
	vector < CParameters* > vLoc;
	vector < CParameters* > vSum;
	vector < CParameters* > vDes;
	vector < CParameters* > vCom;
	vector < CParameters* > vRel;
	vector < CParameters* > vPar;

	CParameters* pLang = new CParameters();
	pLang->setParamName(LANGUAGE);
	ParamType lang;
	lang.szString = "ENG";
	pLang->setParamValue(lang);

	vCat.push_back(pLang);
	vRes.push_back(pLang);
	vCon.push_back(pLang);
	vLoc.push_back(pLang);
	vSum.push_back(pLang);
	vDes.push_back(pLang);
	vCom.push_back(pLang);

	CParameters* pAlter = new CParameters();
	pAlter->setParamName(ALTER);
	ParamType alter;
	alter.szString = "alternate rep";
	pAlter->setParamValue(alter);

	vRes.push_back(pAlter);
	vCon.push_back(pAlter);
	vLoc.push_back(pAlter);
	vSum.push_back(pAlter);
	vDes.push_back(pAlter);
	vCom.push_back(pAlter);

	CParameters* pRel = new CParameters();
	pRel->setParamName(RELTYPE);
	ParamType rel;
	rel.i = 2;
	pRel->setParamValue(rel);
	vRel.push_back(pRel);

	CParameters* pPar = new CParameters();
	pPar->setParamName("X-PARAM");
	ParamType x;
	x.szString = "paramval";
	pPar->setParamValue(x);
	vPar.push_back(pPar);

	map <string,vector<CParameters*> > hashmap;
	hashmap.insert(pair<string,vector<CParameters*> >(CATEGORIES , vCat));
	hashmap.insert(pair<string,vector<CParameters*> >(RESOURCES , vRes));
	hashmap.insert(pair<string,vector<CParameters*> >(CONTACT , vCon));
	hashmap.insert(pair<string,vector<CParameters*> >(LOCATION , vLoc));
	hashmap.insert(pair<string,vector<CParameters*> >(SUMMARY , vSum));
	hashmap.insert(pair<string,vector<CParameters*> >(DESCRIPTION , vDes));
	hashmap.insert(pair<string,vector<CParameters*> >(COMMENT , vCom));
	hashmap.insert(pair<string,vector<CParameters*> >(RELATEDTO , vRel));
	hashmap.insert(pair<string,vector<CParameters*> >("X-PARAM" , vPar));

	pTodo->setHashMap(hashmap);

	string strIcal = iCalConv->localToIcalVcal((CComponent *) pTodo,ICAL_TYPE,errorcode);
	printf("\n%s\n",strIcal.c_str());
	CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);
}

void ICalConverter_TS1 ::test_localToIcalVcal_InValidToDo2() 
{
	string strIcal;
	CTodo *pTodo = NULL;

	strIcal = iCalConv->localToIcalVcal((CComponent *) pTodo,ICAL_TYPE,errorcode);
	printf("\n%s\n",strIcal.c_str());
	CPPUNIT_ASSERT_MESSAGE("Error return value : ",strIcal.empty()== true);
}

void ICalConverter_TS1 ::test_localToIcalVcal_ValidJournal() // All entries are valid
{
	string strIcal;
	CJournal *pJournal = new CJournal;
	pJournal->setType(E_JOURNAL);
        pJournal->setDateStart(time(NULL));
	pJournal->setSummary("Valid Case setting proper data values to TODO");
        pJournal->setDescription("Valid Case setting proper data values to TODO");
        pJournal->setLocation("Valid Case setting proper data values to TODO");
	pJournal->setDateStamp(time(NULL));
	pJournal->setCreatedTime(time(NULL));
	pJournal->setLastModified(time(NULL));
	pJournal->setCategories("Public Holiday");
	pJournal->setSequence(1);
	pJournal->setUrl("http://url.com");
	pJournal->setContact("JoneSmith");
	pJournal->setRelated("deloyoite");
	pJournal->setClas("CONFIDENTIAL");
	pJournal->setComments("having a restricted public holiday");

	vector <CAttendee *> vAttends;
	CAttendee *pAt1=new CAttendee();
	pAt1->setMember("Member1");
	pAt1->setRole(REQ_PARTICIPANT);
	pAt1->setParticipationStatus(ACCEPTED);
	pAt1->setDelegatees("Someone");
	pAt1->setDelegator("Someone");
	vAttends.push_back(pAt1);

	pJournal->setAttendees(vAttends);

	COrganizer *pOrga= new COrganizer();
	pOrga->setLanguage("English");
	pOrga->setSentBy("Myself");
	pOrga->setCommonName("Something");
	pOrga->setXparameters("WhatisThis");
	pOrga->setCalAddress("!!!!!!!!!");
	pOrga->setDirectoryParameter("Home");
	pJournal->setOrganizer(pOrga);

	CProperties* pProp = new CProperties();
	vector < CProperties* > vProp;
	pProp->setPropName("X-ABC");
	PropType prop;
	prop.szString = "XX";
	pProp->setPropValue(prop);
	vProp.push_back(pProp);
	pJournal->setXProperties(vProp);

	vector < CParameters* > vCat;
	vector < CParameters* > vRes;
	vector < CParameters* > vCon;
	vector < CParameters* > vLoc;
	vector < CParameters* > vSum;
	vector < CParameters* > vDes;
	vector < CParameters* > vCom;
	vector < CParameters* > vRel;
	vector < CParameters* > vPar;

	CParameters* pLang = new CParameters();
	pLang->setParamName(LANGUAGE);
	ParamType lang;
	lang.szString = "ENG";
	pLang->setParamValue(lang);

	vCat.push_back(pLang);
	vRes.push_back(pLang);
	vCon.push_back(pLang);
	vLoc.push_back(pLang);
	vSum.push_back(pLang);
	vDes.push_back(pLang);
	vCom.push_back(pLang);

	CParameters* pAlter = new CParameters();
	pAlter->setParamName(ALTER);
	ParamType alter;
	alter.szString = "alternate rep";
	pAlter->setParamValue(alter);

	vRes.push_back(pAlter);
	vCon.push_back(pAlter);
	vLoc.push_back(pAlter);
	vSum.push_back(pAlter);
	vDes.push_back(pAlter);
	vCom.push_back(pAlter);

	CParameters* pRel = new CParameters();
	pRel->setParamName(RELTYPE);
	ParamType rel;
	rel.i = 1;
	pRel->setParamValue(rel);
	vRel.push_back(pRel);

	CParameters* pPar = new CParameters();
	pPar->setParamName("X-PARAM");
	ParamType x;
	x.szString = "paramval";
	pPar->setParamValue(x);
	vPar.push_back(pPar);

	map <string,vector<CParameters*> > hashmap;
	hashmap.insert(pair<string,vector<CParameters*> >(CATEGORIES , vCat));
	hashmap.insert(pair<string,vector<CParameters*> >(RESOURCES , vRes));
	hashmap.insert(pair<string,vector<CParameters*> >(CONTACT , vCon));
	hashmap.insert(pair<string,vector<CParameters*> >(LOCATION , vLoc));
	hashmap.insert(pair<string,vector<CParameters*> >(SUMMARY , vSum));
	hashmap.insert(pair<string,vector<CParameters*> >(DESCRIPTION , vDes));
	hashmap.insert(pair<string,vector<CParameters*> >(COMMENT , vCom));
	hashmap.insert(pair<string,vector<CParameters*> >(RELATEDTO , vRel));
	hashmap.insert(pair<string,vector<CParameters*> >("X-PARAM" , vPar));
	pJournal->setHashMap(hashmap);

	strIcal = iCalConv->localToIcalVcal((CComponent *) pJournal,ICAL_TYPE,errorcode);
	printf("\n%s\n",strIcal.c_str());
	CPPUNIT_ASSERT_MESSAGE("Error return value : ",(strIcal.empty()==false));

}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidCompleteEvent_VCAL() // All supported details of the event
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";


	string Vcal = "BEGIN:VCALENDAR\nPRODID:-//Codenomicon//NONSGML DEFENSICS vCalendar//EN\nVERSION:1.0\nTZ:-02\nDAYLIGHT:TRUE;-01;20080330T010000Z;20081026T010000Z;EET;EEST\nGEO:65.0000,25.0000\nBEGIN:VEVENT\nDTSTART:20080401T140000Z\nDTEND:20080401T150000Z\nDCREATED:20080618T104407Z\nLAST-MODIFIED:20080618T120348Z\nRRULE:D1 20150907T090000\nUID:DEFENSICS-1213786203.123\nSUMMARY:vCalendar\nDESCRIPTION:Codenomicon vCalendar test event.\nLOCATION:Oulu, Finland\nATTENDEE;RSVP=NO;STATUS=ACCEPTED:MAILTO:John Doe <john@example.com>\nATTENDEE;RSVP=YES;STATUS=NEEDS ACTION:MAILTO:Jane Doe <jane@example.com>\nATTACH;VALUE=URL:http://127.0.0.1/vCalendar-test-suite\nCLASS:PUBLIC\nCATEGORIES:APPOINTMENT;EDUCATION\nSEQUENCE:2\nAALARM:20080606T044500Z;PT5M;2;a.mp3\nAALARM;TYPE=X-EPOCSOUND:20080606T044500Z;;;\nDALARM:20080618T084500Z;;1;beep!\nMALARM:20080618T084500Z;PT1H;2;john@example.com;Codenomicon vCalendar test\nX-ORGANIZER:MAILTO:user@example.com\nX-EPOCAGENDAENTRYTYPE:APPOINTMENT\nX-METHOD:NONE\nX-SYMBIAN-LUID:264\nPRIORITY:2\nRESOURCES.CATERING\nEND:VEVENT\nEND:VCALENDAR";
	pComp_t = iCalConv->icalVcalToLocal(Vcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];
	
	string Vcal1 = "BEGIN:VCALENDAR\nPRODID:-//Codenomicon//NONSGML DEFENSICS vCalendar//EN\nVERSION:1.0\nTZ:+02\nDAYLIGHT:TRUE;+03;20080330T010000Z;20081026T010000Z;EET;EEST\nGEO:65.0000,25.0000\nBEGIN:VEVENT\nDTSTART:20080401T140000Z\nDTEND:20080401T150000Z\nDCREATED:20080618T104407Z\nLAST-MODIFIED:20080618T120348Z\nRRULE:D1 20150907T090000\nUID:DEFENSICS-1213786203.123\nSUMMARY:vCalendar\nDESCRIPTION:Codenomicon vCalendar test event.\nLOCATION:Oulu, Finland\nATTENDEE;RSVP=NO;STATUS=ACCEPTED:MAILTO:John Doe <john@example.com>\nATTENDEE;RSVP=YES;STATUS=NEEDS ACTION:MAILTO:Jane Doe <jane@example.com>\nATTACH;VALUE=URL:http://127.0.0.1/vCalendar-test-suite\nCLASS:PUBLIC\nCATEGORIES:APPOINTMENT;EDUCATION\nSEQUENCE:2\nAALARM:20080606T044500Z;PT5M;2;a.mp3\nAALARM;TYPE=X-EPOCSOUND:20080606T044500Z;;;\nDALARM:20080618T084500Z;;1;beep!\nMALARM:20080618T084500Z;PT1H;2;john@example.com;Codenomicon vCalendar test\nX-ORGANIZER:MAILTO:user@example.com\nX-EPOCAGENDAENTRYTYPE:APPOINTMENT\nX-METHOD:NONE\nX-SYMBIAN-LUID:264\nPRIORITY:2\nRESOURCES.CATERING\nEND:VEVENT\nEND:VCALENDAR";
	pComp_t = iCalConv->icalVcalToLocal(Vcal1,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	string strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nDTSTART:20080619T114105Z\nDTEND:20080718T114105Z\nRRULE:W2 TU TH\nRDATE:20080621T114105Z\nEXRULE:W1 TH\nEXDATE:20080625T114105Z\nALARM;TYPE=WAVE;VALUE=URL:20080619T104105Z\nAALARM;TYPE=WAVE;VALUE=CONTENT-ID:20080619T104105;PT30M;4\nEND:VEVENT\nBEGIN:VTIMEZONE\nTZID:/mozilla.org/20070129_1/America/Dawson\nX-LIC-LOCATION:America/Dawson\nBEGIN:DAYLIGHT\nTZOFFSETFROM:-0800\nTZOFFSETTO:-0700\nTZNAME:PDT\nDTSTART:19700308T020000\nRRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=2SU;BYMONTH=3\nEND:DAYLIGHT\nBEGIN:STANDARD\nTZOFFSETFROM:-0700\nTZOFFSETTO:-0800\nTZNAME:PST\nDTSTART:19701101T020000\nRRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=1SU;BYMONTH=11\nEND:STANDARD\nRELATED-TO;RELTYPE=SIBLING:<19960401-080045-4000F192713@host.com>\nEND:VTIMEZONE"; 
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	
	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pEvent_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pEvent_t->getSequence()==2);

	delete pEvent_t;

	strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY;LANGUAGE=fr-CA:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:DELEGATED\nLOCATION;ALTREP=\"http://xyzcorp.com/conf-rooms/f123.vcf\":Conference Room - F123, Bldg. 002\nDTSTART:20080619T114105Z\nDTEND:20080718T114105Z\nRRULE:D2 #10\nRDATE:20080621T114105Z\nEXRULE:W1 #1\nEXDATE:20080625T114105Z\nAALARM;TYPE=WAVE;VALUE=CONTENT-ID:20080619T104105;PT15M;4\nRELATED-TO;RELTYPE=PARENT:<19960401-080045-4000F192713@host.com>\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));

	delete pEvent_t;

	strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nRELATED-TO;RELTYPE=7:<19960401-080045-4000F192713@host.com>\nSTATUS:DECLINED\nLOCATION;ALTREP=\"http://xyzcorp.com/conf-rooms/f123.vcf\":Conference Room - F123, Bldg. 002\nDTSTART:20080619T114105Z\nDTEND:20080718T114105Z\nRRULE:D2 #10\nRDATE:20080621T114105Z\nEXRULE:W1 #1\nEXDATE:20080625T114105Z\nAALARM;TYPE=WAVE;VALUE=CONTENT-ID:20080619T104105;PT30M;4\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));

	delete pEvent_t;

	strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:SENT\nLOCATION;ALTREP=\"http://xyzcorp.com/conf-rooms/f123.vcf\":Conference Room - F123, Bldg. 002\nDTSTART:20080619T114105Z\nDTEND:20080718T114105Z\nRRULE:D2 #10\nRDATE:20080621T114105Z\nEXRULE:W1 #1\nEXDATE:20080625T114105Z\nAALARM;TYPE=WAVE;VALUE=CONTENT-ID:20080619T104105;PT5M;4\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));

	delete pEvent_t;

	strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:CONFIRMED\nLOCATION;ALTREP=\"http://xyzcorp.com/conf-rooms/f123.vcf\":Conference Room - F123, Bldg. 002\nDTSTART:20080619T114105Z\nDTEND:20080718T114105Z\nRRULE:D2 #10\nRDATE:20080621T114105Z\nEXRULE:W1 #1\nEXDATE:20080625T114105Z\nAALARM;TYPE=WAVE;VALUE=CONTENT-ID:20080619T104105;PT1H;4\nRELATED-TO;RELTYPE=CHILD:<19960401-080045-4000F192713@host.com>\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));

	delete pEvent_t;

	strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:CONFIRMED\nLOCATION;ALTREP=\"http://xyzcorp.com/conf-rooms/f123.vcf\":Conference Room - F123, Bldg. 002\nDTSTART:20080619T114105Z\nDTEND:20080718T114105Z\nRRULE:D2 #10\nRDATE:20080621T114105Z\nEXRULE:W1 #1\nEXDATE:20080625T114105Z\nAALARM;TYPE=WAVE;VALUE=CONTENT-ID:20080619T104105;PT3H;4\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));

	delete pEvent_t;

}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidEvent_Component()  //All valid component parameters of entry
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VEVENT\nSUMMARY:summary\nSTATUS:CONFIRMED\nSTATUS:TENTATIVE\nLOCATION:hyd\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080102\nDESCRIPTION:This is 2008 year\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));

	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));

	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pEvent_t->getDescription() == "This is 2008 year");

	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pEvent_t->getStatus()== CONFIRMED_STATUS);
}

void ICalConverter_TS1 ::test_icalVcalToLocal_EventWithAttendees()  //A event with more than 1 attendee
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t ;
	vector <CAttendee *> pAttenList;
	
	string strIcal="BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nSTATUS:NEEDS-ACTION\nATTENDEE;LANGUAGE=en;SENT-BY=\"MAILTO:entby@host.com\";CUTYPE=INDIVIDUAL;RSVP=TRUE;PARTSTAT=DELEGATED;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nATTENDEE;CUTYPE=RESOURCE;RSVP=FALSE;PARTSTAT=TENTATIVE;DELEGATED-FROM=\"MAILTO:immud@host3.com\":MAILTO:ildoit@host1.com\nEND:VEVENT";

	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	/*checking atendee*/
	pAttenList = pEvent_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee1",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));

	CPPUNIT_ASSERT_MESSAGE("Error return value : delegated from in Attendee2",
			(pAttenList[1]->getDelegator()== "MAILTO:immud@host3.com"));
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidEvent_ComponentDetails()  //A event with componentdetail parameters
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t ;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";
	string temp;
	
	/*add check for attachments,Resources*/	
	string strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTART:20080618T114105Z\nDTEND:20080618T124105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PUBLIC\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nCATEGORIES;LANGUAGE=en:RestrictedHoliday\nTRANSP:TRANSPARENT\nCOMMENT;LANGUAGE=en:This ia new year comment\nURL:http://abc.com/pub/calendars/jsmith/mytime.ics\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pEvent_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pEvent_t->getSequence()==2);
	temp=pEvent_t->getContact();
	stringReplaceAll (temp, " 2C", ",");
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",temp == szContact);

	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pEvent_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pEvent_t->getClas() == "PUBLIC");
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pEvent_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pEvent_t->getCategories() == "RestrictedHoliday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : transparency",
			pEvent_t->getTransparency() == "TRANSPARENT");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pEvent_t->getComments() == "This ia new year comment");
}

void ICalConverter_TS1 ::test_icalVcalToLocal_EventWithOrg() //A event with organizer
{
	CEvent *pEvent_t = 0;		
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t;

	string strIcal="BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nORGANIZER;CN=JohnSmith;DIR=\"ldap://host.com:6666/o=3DDC%20Associates,c=3DUS\?\?(cn=3DJohn%20Smith)\";SENT-BY=\"MAILTO:jane_doe@host.com\";LANGUAGE=en:Germany\nATTACH;FMTTYPE=application/binary:ftp://domain.com/pub/docs/a.doc\nEND:VEVENT";
	string temp;
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	/*checking organizer details*/
	pOrg_t = pEvent_t->getOrganizer();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			pOrg_t->getCommonName()=="JohnSmith");
			temp=pOrg_t->getDirectoryParameter();
			stringReplaceAll (temp, ";ENCODING=QUOTED-PRINTABLE:",":");
			stringReplaceAll (temp, "=2C",",");
			
	CPPUNIT_ASSERT_MESSAGE("Error return value :dir param in Organizer",pOrg_t->getDirectoryParameter() == "ldap://host.com:6666/o=3DDC%20Associates\\,c=3DUS\?\?(cn=3DJohn%20Smith)");
	CPPUNIT_ASSERT_MESSAGE("Error return value : sent by in Organizer",
			pOrg_t->getSentBy()=="MAILTO:jane_doe@host.com");

	/*need to check lang */
	CPPUNIT_ASSERT_MESSAGE("Error return value : language in Organizer",
			pOrg_t->getLanguage()=="en");

}

void ICalConverter_TS1 ::test_icalVcalToLocal_InvalidEvent2() // Special characters in txt
{
	vector <CComponent *>pComp_t;
	CEvent *pEvent_t = NULL;

	string strIcal="BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:|@=C2=A3$!*#=C2=A4%&/()}][{@=C2=A3=C2=A3=C3=B6=C3=B5\nLOCATION:Bangalore\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = (CEvent *)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary special characters",
			(strcmp((pEvent_t->getSummary()).c_str(),"|@£$!*#¤%&/()}][{@££öõ")==0));
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidToDo()  //All entries are valid
{
	CTodo *pTodo_t = NULL;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";

	string strIcal = "BEGIN:VTODO\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY;LANGUAGE=en:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:CONFIRMED\nSTATUS:NEEDS-ACTION\nLOCATION;LANGUAGE=fr-CA:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PRIVATE\nATTENDEE;CUTYPE=UNKNOWN;RSVP=TRUE;PARTSTAT=COMPLETED;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDUE:20080618T114105Z\nCATEGORIES;LANGUAGE=en:Restricted Holiday\nPERCENT-COMPLETE:39\nDESCRIPTION;LANGUAGE=fr-CA:This is 2008 year\nURL:http://abc.com/pub/calendars/jsmith/mytime.ics\nCOMMENT:This ia new year comment\nATTACH;FMTTYPE=image/basic\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20080618T074105Z\nDURATION:PT10M\nREPEAT:4\nACTION:AUDIO\nATTACH;FMTTYPE=audio/basic\nEND:VALARM\nX-LIC-LOCATION;X-LIC-PARAM=xparam:Europe/Podgorica\nRELATED-TO;RELTYPE=SIBLING:<19960401-080045-4000F192713@host.com>\nRESOURCES;LANGUAGE=fr:rx-51\nEND:VTODO";
	string temp;
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pTodo_t = (CTodo *)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary ",
			(strcmp((pTodo_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pTodo_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pTodo_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pTodo_t->getOrganizer();
	
	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));

	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pTodo_t->getStatus()== CONFIRMED_STATUS);
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pTodo_t->getLocation()).c_str(),"hyd")==0));
	temp=pTodo_t->getContact();
	stringReplaceAll(temp," 2C",",");

	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",temp== szContact);

	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pTodo_t->getGeo() == "37.386;-122.083");

	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pTodo_t->getClas() == "PRIVATE");

	/*checking atendee*/
	pAttenList = pTodo_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));

	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pTodo_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");

	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pTodo_t->getCategories() == "Restricted Holiday");

	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pTodo_t->getDescription() == "This is 2008 year");

	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pTodo_t->getComments() == "This ia new year comment");

	CPPUNIT_ASSERT_MESSAGE("Error return value : percent complete ",
			pTodo_t->getPercentComplete() == 39);
}

void ICalConverter_TS1 ::test_icalVcalToLocal_InValidToDo1()  
{
	CTodo *pTodo_t = NULL;
	vector <CComponent *>pComp_t;

	string strIcal="BEGIN:VTODO\nPRIORITY:3\nDUE:20080618T114105Z\nSUMMARY;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:|@=C2=A3$!*#=C2=A4%&/()}][{@=C2=A3=C2=A3=C3=B6=C3=B5\nEND:VTODO";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pTodo_t = (CTodo *)pComp_t[0];
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary special characters pTodo",
			(strcmp((pTodo_t->getSummary()).c_str(),"|@£$!*#¤%&/()}][{@££öõ")==0));
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidJournal() // All entries are valid
{
	CJournal *pJournal_t = NULL;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";

	string strIcal = "BEGIN:VJOURNAL\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY;LANGUAGE=en:summary\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:CONFIRMED\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nCLASS:CONFIDENTIACONFIDENTIALL\nATTENDEE;RSVP=FALSE;PARTSTAT=NEEDS-ACTION;MEMBER=\"MAILTO:DEV-GROUP@host2.com\";SENT-BY=\"MAILTO:send@host.com\":\"MAILTO:sray@host.com\";LANGUAGE=en:MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nCATEGORIES;LANGUAGE=en:Restricted Holiday\nDESCRIPTION;LANGUAGE=fr:This is 2008 year\nURL:http://abc.com/pub/calendars/jsmith/mytime.ics\nATTACH;FMTTYPE=image/basic:fmtype\nCOMMENT:This ia new year comment\nX-LIC-LOCATION;X-LOC=xparam:Europe/Podgorica\nEND:VJOURNAL";
	string temp;
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pJournal_t = (CJournal *)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary ",
			(strcmp((pJournal_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pJournal_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pJournal_t->getOrganizer();

	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));

	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pJournal_t->getStatus()== CONFIRMED_STATUS);
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pJournal_t->getLocation()).c_str(),"hyd")==0));

	temp=pJournal_t->getContact();
	stringReplaceAll(temp," 2C",",");

	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",temp == szContact);
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pJournal_t->getClas() == "PUBLIC");

	/*checking atendee*/
	pAttenList = pJournal_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));

	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pJournal_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pJournal_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pJournal_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pJournal_t->getComments() == "This ia new year comment");
}

void ICalConverter_TS1 ::test_icalVcalToLocal_InValidJournal1()  
{
	CJournal *pJournal_t = NULL;
	vector <CComponent *>pComp_t;

	

	string strIcal="BEGIN:VJOURNAL\nSUMMARY;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:|@=C2=A3$!*#=C2=A4%&/()}][{@=C2=A3=C2=A3=C3=B6=C3=B5\nLOCATION:Bangalore\nDESCRIPTION;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:|@=C2=A3$!*#=C2=A4%&/()}][{@=C2=A3=C2=A3=C3=B6=C3=B5\nEND:VJOURNAL";

	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pJournal_t = (CJournal *)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary special characters pJournal",
			(strcmp((pJournal_t->getSummary()).c_str(),"|@£$!*#¤%&/()}][{@££öõ")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : Description special characters pJournal",
			(strcmp((pJournal_t->getDescription()).c_str(),"|@£$!*#¤%&/()}][{@££öõ")==0));
}

void ICalConverter_TS1 ::test_icalVcalToLocal_InValidFormat()  
{
	vector <CComponent *>pComp_t;

	string strIcal="BEGIN:ABCDEF\nSUMMARY;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:|@=C2=A3$!*#=C2=A4%&/()}][{@=C2=A3=C2=A3=C3=B6=C3=B5\nLOCATION:Bangalore\nDESCRIPTION;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:|@=C2=A3$!*#=C2=A4%&/()}][{@=C2=A3=C2=A3=C3=B6=C3=B5\nEND:ABCDEF";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : InValidFormat",(pComp_t.size()==0));

}

void ICalConverter_TS1 ::test_icalVcalToLocal_InValidFormat2()  
{
	vector <CComponent *>pComp_t;

	string strIcal="DESCRIPTION;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:|@=C2=A3$!*#=C2=A4%&/()}][{@=C2=A3=C2=A3=C3=B6=C3=B5\nEND:VJOURNAL";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : InValidFormat",(pComp_t.size()==0));
}

void ICalConverter_TS1 ::test_icalVcalToLocal_InValidFormat3()  
{
	vector <CComponent *>pComp_t;
	string strIcal="";

	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : InValidFormat empty string",(pComp_t.size()==0));
}

void ICalConverter_TS1 ::test_icalVcalToLocal_EventWithVcalData()  //A event with more than 1 attendee
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t ;
	vector <CAttendee *> pAttenList;
	
	string strIcal="BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nDCREATED:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=ORGANIZER;RSVP=YES:MAILTO:joecool@host2.com\nTRANSP:0\nSTATUS:SENT\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	/*checking atendee*/
	pAttenList = pEvent_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Mailto in Attendee",
			(pAttenList[0]->getMailTo()== "MAILTO:joecool@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : role in Attendee",
			(pAttenList[0]->getRole()==ORGANIZER_ROLE ));
	CPPUNIT_ASSERT_MESSAGE("Error return value : rsvp in Attendee",
			(pAttenList[0]->getRSVP()== 1 ));
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",
			(pEvent_t->getStatus()== SENT_STATUS ));
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : transp",
			(pEvent_t->getTransparency()== OPAQUE ));
}

void ICalConverter_TS1 ::test_icalVcalToLocal_TodoWithVcalData()  //A event with more than 1 attendee
{
	CTodo *pTodo_t = 0;
	vector <CComponent *>pComp_t ;
	vector <CAttendee *> pAttenList;
	
	string strIcal="BEGIN:VTODO\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nDCREATED:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=ORGANIZER;RSVP=NO:MAILTO:joecool@host2.com\nTRANSP:>1\nSTATUS:SENT\nEND:VTODO";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pTodo_t = new CTodo();
	pTodo_t = (CTodo*)pComp_t[0];
	
	/*checking atendee*/
	pAttenList = pTodo_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Mailto in Attendee",
			(pAttenList[0]->getMailTo()== "MAILTO:joecool@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : role in Attendee",
			(pAttenList[0]->getRole()==ORGANIZER_ROLE ));
	CPPUNIT_ASSERT_MESSAGE("Error return value : rsvp in Attendee",
			(pAttenList[0]->getRSVP()== 0 ));
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",
			(pTodo_t->getStatus()== SENT_STATUS ));
}

void ICalConverter_TS1 ::test_icalVcalToLocal_JournalWithVcalData()  //A event with more than 1 attendee
{
	CJournal *pJournal_t = 0;
	vector <CComponent *>pComp_t ;
	vector <CAttendee *> pAttenList;

	string strIcal="BEGIN:VJOURNAL\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nDCREATED:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=DELEGATE;RSVP=NO:MAILTO:joecool@host2.com\nTRANSP:>1\nSTATUS:DECLINED\nEND:VJOURNAL";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pJournal_t = new CJournal();
	pJournal_t = (CJournal*)pComp_t[0];

	/*checking atendee*/
	pAttenList = pJournal_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Mailto in Attendee",
			(pAttenList[0]->getMailTo()== "MAILTO:joecool@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : role in Attendee",
			(pAttenList[0]->getRole()==DELEGATE_ROLE ));
	CPPUNIT_ASSERT_MESSAGE("Error return value : rsvp in Attendee",
			(pAttenList[0]->getRSVP()== 0 ));
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",
			(pJournal_t->getStatus()== DECLINED_STATUS ));
}

void ICalConverter_TS1 ::test_localToIcalVcal_ValidEventWithVcalData()  //All entries are valid
{
	string sLoc;
	time_t start,end,until;
	time(&start);
	int errorcode = -1;

	end = start+10500;
	until=start+500000;

	CEvent *pEvent = new CEvent("summary", "description",
			"location", start + 10000, end);

	pEvent->setTransparency("OPAQUE");
	pEvent->setPriority(4);
	pEvent->setCreatedTime(time(NULL));
	pEvent->setLastModified(time(NULL));

	pEvent->setCategories("Public Holiday");
	pEvent->setSequence(1);
	pEvent->setUrl("http://url.com");
	pEvent->setGeo("132.23:-112.62");
	pEvent->setContact("JoneSmith");
	pEvent->setRelated("deloyoite");
	pEvent->setClas("PUBLIC");
	pEvent->setComments("having a restricted public holiday");
	pEvent->setStatus(SENT_STATUS);
	pEvent->setAllDay(0);

	/*alarm*/
	CAlarm *pAlarm  = new CAlarm(start + 100,1);
	pEvent->setAlarm(pAlarm);

	/*recurrence*/
	CRecurrence *pRecur = new CRecurrence();
	vector <string> rules;
	rules.clear();
	string s1("FREQ=");
	string s2(";UNTIL=");
	string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
	string rrule;
	struct icaltimetype uical;
	int repeattype = 2;

	uical = icaltime_from_timet(until, 0);
	string ustring = icaltime_as_ical_string(uical);
	rrule = s1 + test[repeattype - 1] + s2 + ustring;
	rules.push_back(rrule);
	pEvent->setRtype(repeattype);
	pEvent->setUntil(until);
	pRecur->setRrule(rules);
	pEvent->setRecurrence(pRecur);

	pEvent->setAllDay(0);

	/*attendee*/
	vector <CAttendee *> vAttends;
	CAttendee *pAt1=new CAttendee();
	pAt1->setMember("Member1");
	pAt1->setRole(CHAIR);
	pAt1->setParticipationStatus(TENTATIVE);
	pAt1->setDelegatees("Someone");
	pAt1->setDelegator("Someone");
	pAt1->setMailTo("one@MAIL.COM");
	pAt1->setRole(DELEGATE_ROLE);
	pAt1->setRSVP(1);
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

	pEvent->setType(E_EVENT);

	string strIcal = iCalConv->localToIcalVcal((CComponent *) pEvent,VCAL_TYPE,errorcode);
	printf("\n%s\n",strIcal.c_str());
	CPPUNIT_ASSERT_MESSAGE("Error return value :",strIcal.c_str()!=NULL);
}

void ICalConverter_TS2 ::test_validICalRRule() 
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VEVENT\nSUMMARY:summary\nSTATUS:CONFIRMED\nLOCATION:hyd\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080120\nDESCRIPTION:This is 2008 year\nRRULE:FREQ=WEEKLY;COUNT=5\nRRULE:FREQ=DAILY;COUNT=10\nRDATE:20080101T101010Z\nEXRULE:FREQ=WEEKLY;COUNT=2\nEXDATE:20080111T101010Z\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pEvent_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pEvent_t->getStatus()== CONFIRMED_STATUS);
	delete pEvent_t;


	strIcal = "BEGIN:VEVENT\nSUMMARY:summary\nSTATUS:CONFIRMED\nLOCATION:hyd\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080120\nDESCRIPTION:This is 2008 year\nRRULE:FREQ=WEEKLY;COUNT=2\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pEvent_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pEvent_t->getStatus()== CONFIRMED_STATUS);

	strIcal = "BEGIN:VTodo\nSUMMARY:summary\nSTATUS:CONFIRMED\nLOCATION:hyd\nDESCRIPTION:This is 2008 year\n;RRULE:FREQ=YEARLY;UNTIL=20390429T085000Z\nRRULE:FREQ=DAILY;COUNT=10\nRRULE:FREQ=WEEKLY;COUNT=2\nATTENDEE;PARTSTAT=IN-PROCESS;CUTYPE=ROOM;ROLE=NON-PARTICIPANT;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080620\nCATEGORIES:Restricted Holiday\nDESCRIPTION:This is 2008 year\nTRANSP:TRANSPARENT\nCOMMENT:This ia new year comment\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20080618T074105Z\nDURATION:PT10M\nREPEAT:3\nACTION:AUDIO\nEND:VALARM\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CTodo *pTodo_t = new CTodo();
	pTodo_t = (CTodo*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pTodo_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pTodo_t->getLocation()).c_str(),"hyd")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pTodo_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pTodo_t->getStatus()== CONFIRMED_STATUS);
}

void ICalConverter_TS2 :: test_validICalRRule_monthly() 
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VEVENT\nSUMMARY:Monthly Recurrence\nSTATUS:CONFIRMED\nLOCATION:hyd\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080120\nDESCRIPTION:This is 2008 year\nRRULE:FREQ=MONTHLY;COUNT=10\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"Monthly Recurrence")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));
	CRecurrence *pRecur = pEvent_t->getRecurrence();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Recurrence object",pRecur != NULL); 
	CPPUNIT_ASSERT_MESSAGE("Error return value : Recurrence type",pRecur->getRtype() == E_MONTHLY); 
	delete pEvent_t;
}

void ICalConverter_TS2 :: test_validICalRRule_complex() 
{

	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VEVENT\nSUMMARY:Monthly Recurrence\nSTATUS:CONFIRMED\nLOCATION:hyd\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080120\nDESCRIPTION:This is 2008 year\nRRULE:FREQ=MONTHLY;BYDAY=MO,TU,WE,TH,FR;BYSETPOS=-1\nEND:VEVENT";

	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"Monthly Recurrence")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));
	CRecurrence *pRecur = pEvent_t->getRecurrence();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Recurrence object",pRecur != NULL); 
	CPPUNIT_ASSERT_MESSAGE("Error return value : Recurrence type",pRecur->getRtype() == E_COMPLEX); 
	delete pEvent_t;

}

void ICalConverter_TS2 ::test_validVCalRRule()

{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VEVENT\nSUMMARY:summary\nSTATUS:CONFIRMED\nLOCATION:hyd\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080120\nDESCRIPTION:This is 2008 year\nRRULE:D #10\nRDATE:20080101T101010Z\nRULE:W1 #2\nEXDATE:20080111T101010Z\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,VCAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pEvent_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pEvent_t->getStatus()== CONFIRMED_STATUS);
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidToDo1()  //All entries are valid
{
	CTodo *pTodo_t = NULL;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";

	string strIcal = "BEGIN:VTODO\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:COMPLETED\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PRIVATE\nATTENDEE;PARTSTAT=ACCEPTED;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nCATEGORIES:Restricted Holiday\nPERCENT-COMPLETE:39\nDESCRIPTION:This is 2008 year\nURL:http://abc.com/pub/calendars/jsmith/mytime.ics\nCOMMENT;LANGUAGE=fr:This ia new year comment\nATTACH;FMTTYPE=application/binary:ftp://domain.com/pub/docs/a.doc\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20080618T074105Z\nDURATION:PT250M\nREPEAT:1\nACTION:AUDIO\nEND:VALARM\nX-LIC-LOCATION:Europe/Podgorica\nRELATED-TO;RELTYPE=CHILD:<19960401-080045-4000F192713@host.com>\nEND:VTODO";
	string temp;
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pTodo_t = (CTodo *)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary ",
			(strcmp((pTodo_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pTodo_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pTodo_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pTodo_t->getOrganizer();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));

	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pTodo_t->getStatus()== COMPLETED_STATUS);
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pTodo_t->getLocation()).c_str(),"hyd")==0));
	
	temp=pTodo_t->getContact();
	stringReplaceAll(temp," 2C",",");
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",temp== szContact);
	
	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pTodo_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pTodo_t->getClas() == "PRIVATE");

	/*checking atendee*/
	pAttenList = pTodo_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pTodo_t->getRelated() == "<19960401-080045-4000F192713@host.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pTodo_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pTodo_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pTodo_t->getComments() == "This ia new year comment");
	CPPUNIT_ASSERT_MESSAGE("Error return value : percent complete ",
			pTodo_t->getPercentComplete() == 39);
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidToDo2()  //All entries are valid
{
	CTodo *pTodo_t = NULL;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";

	string strIcal = "BEGIN:VTODO\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:FINAL\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PRIVATE\nATTENDEE;PARTSTAT=DECLINED;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDUE:20080618T114105Z\nCATEGORIES:Restricted Holiday\nPERCENT-COMPLETE:100\nDESCRIPTION:This is 2008 year\nURL:http://abc.com/pub/calendars/jsmith/mytime.ics\nCOMMENT:This ia new year comment\nATTACH;FMTTYPE=image/basic\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20080618T074105Z\nDURATION:PT850M\nREPEAT:2\nACTION:AUDIO\nEND:VALARM\nX-LIC-LOCATION:Europe/Podgorica\nRELATED-TO;RELTYPE=CHILD:<19960401-080045-4000F192713@host.com>\nEND:VTODO";
	string temp;
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pTodo_t = (CTodo *)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary ",
			(strcmp((pTodo_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pTodo_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pTodo_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pTodo_t->getOrganizer();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pTodo_t->getStatus()== FINAL_STATUS);
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pTodo_t->getLocation()).c_str(),"hyd")==0));

	temp=pTodo_t->getContact();
	stringReplaceAll(temp," 2C",",");
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",temp== szContact);

	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pTodo_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pTodo_t->getClas() == "PRIVATE");

	/*checking atendee*/
	pAttenList = pTodo_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pTodo_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pTodo_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pTodo_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pTodo_t->getComments() == "This ia new year comment");
	CPPUNIT_ASSERT_MESSAGE("Error return value : percent complete ",
			pTodo_t->getPercentComplete() == 100);
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidToDo3()  //All entries are valid
{
	CTodo *pTodo_t = NULL;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";
	
	string strIcal = "BEGIN:VTODO\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:DRAFT\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PRIVATE\nATTENDEE;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDUE:20080618T114105Z\nCATEGORIES:Restricted Holiday\nPERCENT-COMPLETE:39\nDESCRIPTION:This is 2008 year\nURL:http://abc.com/pub/calendars/jsmith/mytime.ics\nCOMMENT:This ia new year comment\nATTACH;FMTTYPE=image/basic\nBEGIN:VALARM\nTRIGGER:-P2D\nDURATION:PT10M\nREPEAT:3\nACTION:AUDIO\nATTACH;FMTTYPE=application/audio\nEND:VALARM\nX-LIC-LOCATION:Europe/Podgorica\nRELATED-TO;RELTYPE=CHILD:<19960401-080045-4000F192713@host.com>\nEND:VTODO";
	string temp;
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pTodo_t = (CTodo *)pComp_t[0];
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary ",
			(strcmp((pTodo_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pTodo_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pTodo_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pTodo_t->getOrganizer();
	
	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pTodo_t->getStatus()== DRAFT_STATUS);
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pTodo_t->getLocation()).c_str(),"hyd")==0));
	temp=pTodo_t->getContact();
	stringReplaceAll(temp," 2C",",");
	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",temp== szContact);

	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pTodo_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pTodo_t->getClas() == "PRIVATE");

	/*checking atendee*/
	pAttenList = pTodo_t->getAttendees();
	
	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pTodo_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pTodo_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pTodo_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pTodo_t->getComments() == "This ia new year comment");
	CPPUNIT_ASSERT_MESSAGE("Error return value : percent complete ",
			pTodo_t->getPercentComplete() == 39);
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidToDo4()  //All entries are valid
{
	CTodo *pTodo_t = NULL;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";

	string strIcal = "BEGIN:VTODO\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:IN-PROCESS\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PRIVATE\nATTENDEE;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDUE:20080618T114105Z\nCATEGORIES:Restricted Holiday\nPERCENT-COMPLETE:39\nDESCRIPTION:This is 2008 year\nURL:http://abc.com/pub/calendars/jsmith/mytime.ics\nCOMMENT:This ia new year comment\nATTACH;FMTTYPE=image/basic\nBEGIN:VALARM\nTRIGGER:-PT3650M\nDURATION:PT15M\nREPEAT:3\nACTION:AUDIO\nEND:VALARM\nX-LIC-LOCATION:Europe/Podgorica\nRELATED-TO;RELTYPE=CHILD:<19960401-080045-4000F192713@host.com>\nEND:VTODO";
	string temp;
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pTodo_t = (CTodo *)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary ",
			(strcmp((pTodo_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pTodo_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pTodo_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pTodo_t->getOrganizer();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pTodo_t->getLocation()).c_str(),"hyd")==0));

	temp=pTodo_t->getContact();
	stringReplaceAll(temp," 2C",",");
	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",temp== szContact);

	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pTodo_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pTodo_t->getClas() == "PRIVATE");

	/*checking atendee*/
	pAttenList = pTodo_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pTodo_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pTodo_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pTodo_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pTodo_t->getComments() == "This ia new year comment");
	CPPUNIT_ASSERT_MESSAGE("Error return value : percent complete ",
			pTodo_t->getPercentComplete() == 39);
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidToDo5()  //All entries are valid
{
	CTodo *pTodo_t = NULL;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";

	string strIcal = "BEGIN:VTODO\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:CANCELLED\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PRIVATE\nATTENDEE;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDUE:20090401T184105Z\nCATEGORIES:Restricted Holiday\nPERCENT-COMPLETE:39\nDESCRIPTION:This is 2008 year\nURL:http://abc.com/pub/calendars/jsmith/mytime.ics\nCOMMENT:This ia new year comment\nATTACH;FMTTYPE=image/basic\nBEGIN:VALARM\nTRIGGER:-PT850M\nDURATION:PT15M\nREPEAT:3\nACTION:AUDIO\nEND:VALARM\nX-LIC-LOCATION:Europe/Podgorica\nRELATED-TO;RELTYPE=CHILD:<19960401-080045-4000F192713@host.com>\nEND:VTODO";

	string temp;
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pTodo_t = (CTodo *)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary ",
			(strcmp((pTodo_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pTodo_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pTodo_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pTodo_t->getOrganizer();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pTodo_t->getStatus()== CANCELLED_STATUS);
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pTodo_t->getLocation()).c_str(),"hyd")==0));
	
	temp=pTodo_t->getContact();
	stringReplaceAll(temp," 2C",",");
	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",temp== szContact);

	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pTodo_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pTodo_t->getClas() == "PRIVATE");
	/*checking atendee*/
	pAttenList = pTodo_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pTodo_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pTodo_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pTodo_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pTodo_t->getComments() == "This ia new year comment");
	CPPUNIT_ASSERT_MESSAGE("Error return value : percent complete ",
			pTodo_t->getPercentComplete() == 39);
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidCompleteEvent11() // All supported details of the event

{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";
	string temp;

	string strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:CONFIRMED\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:CONFIDENTIAL\nATTENDEE;CUTYPE=ROOM;ROLE=CHAIR;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080620\nCATEGORIES:Restricted Holiday\nDESCRIPTION:This is 2008 year\nTRANSP:TRANSPARENT\nCOMMENT:This ia new year comment\nBEGIN:VALARM\nTRIGGER:PT0M\nDURATION:PT2700M\nREPEAT:3\nACTION:AUDIO\nEND:VALARM\nRELATED-TO;RELTYPE=CHILD:<19960401-080045-4000F192713@host.com>\nEND:VEVENT";

	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pEvent_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pEvent_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pEvent_t->getOrganizer();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));

	temp=pEvent_t->getContact();

	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pEvent_t->getStatus()== CONFIRMED_STATUS);
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));
	
	stringReplaceAll (temp, " 2C", ",");

	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",(szContact.compare(temp)==0));

	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pEvent_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pEvent_t->getClas() == "CONFIDENTIAL");

	/*checking atendee*/
	pAttenList = pEvent_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pEvent_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pEvent_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pEvent_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : transparency",
			pEvent_t->getTransparency() == "TRANSPARENT");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pEvent_t->getComments() == "This ia new year comment");

	delete pEvent_t;
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidCompleteEvent12() // All supported details of the event
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";
	string temp;

	string strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\nSTATUS:CONFIRMED\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PUBLIC\nATTENDEE;CUTYPE=USER_NONE;ROLE=NON-PARTICIPANT;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080620\nCATEGORIES:Restricted Holiday\nDESCRIPTION:This is 2008 year\nTRANSP:TRANSPARENT\nCOMMENT:This ia new year comment\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20080618T074105Z\nDURATION:PT50M\nREPEAT:4\nACTION:AUDIO\nEND:VALARM\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pEvent_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pEvent_t->getSequence()==2);

	/*checking organizer details*/
	pOrg_t = pEvent_t->getOrganizer();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));
	temp=pEvent_t->getContact();

	CPPUNIT_ASSERT_MESSAGE("Error return value : status",pEvent_t->getStatus()== CONFIRMED_STATUS);
	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));
	
	stringReplaceAll (temp, " 2C", ",");
	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",(szContact.compare(temp)==0));

	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pEvent_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pEvent_t->getClas() == "PUBLIC");

	/*checking atendee*/
	pAttenList = pEvent_t->getAttendees();
	
	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pEvent_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pEvent_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pEvent_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : transparency",
			pEvent_t->getTransparency() == "TRANSPARENT");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pEvent_t->getComments() == "This ia new year comment");
	delete pEvent_t;
}

void ICalConverter_TS1 ::test_icalVcalToLocal_ValidCompleteEvent13() // All supported details of the event
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;
	COrganizer *pOrg_t = 0;
	vector <CAttendee *> pAttenList;
	string szContact = "Jim Dolittle, ABC Industries, +1-919-555-1234";
	string temp;

	string strIcal = "BEGIN:VEVENT\nCREATED:20080618T114105Z\nLAST-MODIFIED:20080618T114105Z\nDTSTAMP:20080618T114105Z\nSUMMARY:summary\nPRIORITY:2\nSEQUENCE:2\nORGANIZER;CN=JohnSmith:MAILTO:jsmith@host1.com\n;SENT-BY=\"MAILTO:jane_doe@host.com\";LANGUAGE=en:Germany\n;STATUS:CONFIRMED\nLOCATION:hyd\nCONTACT:Jim Dolittle\\, ABC Industries\\, +1-919-555-1234\nGEO:37.386013;-122.082932\nCLASS:PUBLIC\nATTENDEE;CUTYPE=UNKNOWN_USER;ROLE=OPT-PARTICIPANT;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nRELATED-TO:<jsmith.part7.19960817T083000.xyzMail@host3.com>\nDTSTART;VALUE=DATE:20080101\nDTEND;VALUE=DATE:20080620\nCATEGORIES:Restricted Holiday\nDESCRIPTION:This is 2008 year\nTRANSP:TRANSPARENT\nCOMMENT:This ia new year comment\nBEGIN:VALARM\nTRIGGER:-PT7200T\nDURATION:PT30M\nREPEAT:3\nACTION:AUDIO\nEND:VALARM\nEND:VEVENT";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"summary")==0));
	CPPUNIT_ASSERT_MESSAGE("Error return value : priority",pEvent_t->getPriority()==2);
	CPPUNIT_ASSERT_MESSAGE("Error return value : sequence",pEvent_t->getSequence()==2);
	
	/*checking organizer details*/
	pOrg_t = pEvent_t->getOrganizer();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Common Name in Organizer",
			(strcmp((pOrg_t->getCommonName()).c_str(),"JohnSmith")==0));
	
	temp=pEvent_t->getContact();

	CPPUNIT_ASSERT_MESSAGE("Error return value : location",
			(strcmp((pEvent_t->getLocation()).c_str(),"hyd")==0));
	
	stringReplaceAll (temp, " 2C", ",");
	CPPUNIT_ASSERT_MESSAGE("Error return value : contact",(szContact.compare(temp)==0));
	
	/*check the precesion*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Geo",
			pEvent_t->getGeo() == "37.386;-122.083");
	CPPUNIT_ASSERT_MESSAGE("Error return value : class",
			pEvent_t->getClas() == "PUBLIC");
	/*checking atendee*/
	pAttenList = pEvent_t->getAttendees();

	/*MAIL TO parameter has to be verified*/
	CPPUNIT_ASSERT_MESSAGE("Error return value : Member in Attendee",
			(pAttenList[0]->getMember()== "MAILTO:DEV-GROUP@host2.com"));
	CPPUNIT_ASSERT_MESSAGE("Error return value : Related to",
			pEvent_t->getRelated() == "<jsmith.part7.19960817T083000.xyzMail@host3.com>");
	CPPUNIT_ASSERT_MESSAGE("Error return value : categories",
			pEvent_t->getCategories() == "Restricted Holiday");
	CPPUNIT_ASSERT_MESSAGE("Error return value : description",
			pEvent_t->getDescription() == "This is 2008 year");
	CPPUNIT_ASSERT_MESSAGE("Error return value : transparency",
			pEvent_t->getTransparency() == "TRANSPARENT");
	CPPUNIT_ASSERT_MESSAGE("Error return value : comment",
			pEvent_t->getComments() == "This ia new year comment");

	delete pEvent_t;
}

void ICalConverter_TS1 ::test_icalVcalToLocal_InValidToDo11()  
{
	vector <CComponent *>pComp_t;
	string strIcal="END:VTODO";
}

void ICalConverter_TS1 ::test_icalVcalToLocal_InValidToDo12()  

{
	vector <CComponent *>pComp_t;
	string strIcal="ENDw:VTODO";
}

void ICalConverter_TS1 :: test_icalVcalToLocal_ivnalidates()
{
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VEVENT\nSUMMARY:summary\nSTATUS:CONFIRMED\nSTATUS:TENTATIVE\nLOCATION:hyd\nDTSTART=\nDTEND;VALUE=DATE:20080102\nDESCRIPTION:This is 2008 year\nEND:VEVENT";

	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : importing invalid date formats",pComp_t.size() == 0);
}

void ICalConverter_TS1 :: test_icalVcalToLocal_empty()
{
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VEVENT\n\nEND:VEVENT";

	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : importing invalid date formats",pComp_t.size() == 0);
}

void ICalConverter_TS2 :: test_validICalAlarm_durations_5min() 
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VCALENDAR\nVERSION:1.0\nBEGIN:VEVENT\nCREATED:20190704T110750Z\nSUMMARY:5min before\nUID:1\nDTSTART:20190704T113000Z\nDTEND:20190704T123000Z\nSEQUENCE:0\nLAST-MODIFIED:20190704T110750Z\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20190704T112500Z\nDURATION:PT2M\nREPEAT:1\nACTION:AUDIO\nEND:VALARM\nURL:pub/calendars/jsmith/mytime.ics\nEND:VEVENT\nEND:VCALENDAR";
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : to Local",pComp_t.size() > 0); 
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"5min before")==0));
	CAlarm *pAlarm = pEvent_t->getAlarm();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm object",pAlarm != NULL); 
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm duration",pAlarm->getDuration() == E_AM_5MIN); 
	delete pEvent_t;
}

void ICalConverter_TS2 :: test_validICalAlarm_durations_15min() 
{

	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VCALENDAR\nVERSION:1.0\nBEGIN:VEVENT\nCREATED:20190704T110815Z\nSUMMARY:15 min before alarm\nUID:2\nDTSTART:20190704T131000Z\nDTEND:20190704T141000Z\nSEQUENCE:0\nLAST-MODIFIED:20190704T110815Z\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20190704T125500Z\nDURATION:PT3M\nREPEAT:1\nACTION:AUDIO\nEND:VALARM\nEND:VEVENT\nEND:VCALENDAR";
	      
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : to Local",pComp_t.size() > 0); 
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"15 min before alarm")==0));
	CAlarm *pAlarm = pEvent_t->getAlarm();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm object",pAlarm != NULL); 
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm duration",pAlarm->getDuration() == E_AM_15MIN); 
	delete pEvent_t;

}

void ICalConverter_TS2 :: test_validICalAlarm_durations_30min() 
{

	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VCALENDAR\nVERSION:1.0\nBEGIN:VEVENT\nCREATED:20190704T110839Z\nSUMMARY:30 min before alarm\nUID:3\nDTSTART:20190704T181000Z\nDTEND:20190704T191000Z\nSEQUENCE:0\nLAST-MODIFIED:20190704T110839Z\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20190704T174000Z\nDURATION:PT4M\nREPEAT:1\nACTION:AUDIO\nEND:VALARM\nEND:VEVENT\nEND:VCALENDAR";
	      
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : to Local",pComp_t.size() > 0); 
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"30 min before alarm")==0));
	CAlarm *pAlarm = pEvent_t->getAlarm();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm object",pAlarm != NULL); 
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm duration",pAlarm->getDuration() == E_AM_30MIN); 
	delete pEvent_t;

}

void ICalConverter_TS2 :: test_validICalAlarm_durations_3hr() 
{

	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VCALENDAR\nVERSION:1.0\nBEGIN:VEVENT\nCREATED:20190704T110936Z\nSUMMARY:3hr before\nUID:4\nDTSTART:20190704T221000Z\nDTEND:20190704T231000Z\nSEQUENCE:0\nLAST-MODIFIED:20190704T110936Z\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20190704T191000Z\nDURATION:PT6M\nREPEAT:1\nACTION:AUDIO\nEND:VALARM\nEND:VEVENT\nEND:VCALENDAR";      
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : to Local",pComp_t.size() > 0); 
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"3hr before")==0));
	CAlarm *pAlarm = pEvent_t->getAlarm();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm object",pAlarm != NULL); 
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm duration",pAlarm->getDuration() == E_AM_3HR); 
	delete pEvent_t;

}

void ICalConverter_TS2 :: test_validICalAlarm_durations_0min() 
{

	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VCALENDAR\nVERSION:1.0\nBEGIN:VEVENT\nCREATED:20190704T114614Z\nSUMMARY:0 min before\nUID:1\nDTSTART:20190704T115000Z\nDTEND:20190704T125000Z\nSEQUENCE:0\nLAST-MODIFIED:20190704T114614Z\nBEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20190704T115000Z\nDURATION:PT1M\nREPEAT:1\nACTION:AUDIO\nEND:VALARM\nEND:VEVENT\nEND:VCALENDAR";

	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : to Local",pComp_t.size() > 0); 
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"0 min before")==0));
	CAlarm *pAlarm = pEvent_t->getAlarm();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm object",pAlarm != NULL); 
	CPPUNIT_ASSERT_MESSAGE("Error return value : Alarm duration",pAlarm->getDuration() == E_AM_ETIME); 
	delete pEvent_t;
}

void ICalConverter_TS2 :: test_validICalRrule_exceed_datelimit() 
{

	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

	string strIcal = "BEGIN:VCALENDAR\nVERSION:1.0\nBEGIN:VEVENT\nCREATED:20090704T115609Z\nSUMMARY:repeat forever\nUID:2\nDTSTART:20090704T000000\nDTEND:20090705T000000\nATTACH:\nLAST-MODIFIED:20090704T115609Z\nRRULE:FREQ=DAILY;UNTIL=20560708T235900Z\nEND:VEVENT\nEND:VCALENDAR";
	
	pComp_t = iCalConv->icalVcalToLocal(strIcal,ICAL_TYPE,errorcode);
	CPPUNIT_ASSERT_MESSAGE("Error return value : to Local",pComp_t.size() > 0); 
	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];

	CPPUNIT_ASSERT_MESSAGE("Error return value : Summary pEvent",
			(strcmp((pEvent_t->getSummary()).c_str(),"repeat forever")==0));
	CRecurrence *pRecur = pEvent_t->getRecurrence();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Recurrence object",pRecur != NULL); 
	CPPUNIT_ASSERT_MESSAGE("Error return value : Recurrence type",pRecur->getRtype() == E_DAILY); 
	delete pEvent_t;
}

void ICalConverter_TS1 :: test_getCurrentTimeZone()
{
	string timeZone;
	timeZone = iCalConv->getCurrentTimeZone();
	CPPUNIT_ASSERT_MESSAGE("Error return value : Current Time Zone",timeZone.length() != 0); 
	
	
}

void ICalConverter_TS1::test_icalVcalToLocal_aalarm()
{
	CEvent *pEvent_t = 0;
	vector <CComponent *>pComp_t;

    string Vcal = 
            "BEGIN:VCALENDAR\n"
            "VERSION:1.0\n"
            "BEGIN:VEVENT\n"
            "DTSTART:20091231T000000\n"
            "DTEND:20100101T000000\n"
            "AALARM:20091230T200000;;0;\n"
            "X-EPOCAGENDAENTRYTYPE:EVENT\n"
            "SUMMARY:Test\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";


	pComp_t = iCalConv->icalVcalToLocal(Vcal,VCAL_TYPE,errorcode);

    CPPUNIT_ASSERT(pComp_t.size() == 1);
    CAlarm *pAlarm = pComp_t[0]->getAlarm();

    CPPUNIT_ASSERT(pAlarm != 0);
    CPPUNIT_ASSERT(pAlarm->getDuration() == E_AM_DAYBEFORE);
    CPPUNIT_ASSERT(pAlarm->getTimeBefore() == 14400);


    string Vcal2 = 
            "BEGIN:VCALENDAR\n"
            "VERSION:1.0\n"
            "BEGIN:VEVENT\n"
            "DTSTART:20091231T000000\n"
            "DTEND:20100101T000000\n"
            "AALARM:20071228T200000;;0;\n"
            "X-EPOCAGENDAENTRYTYPE:EVENT\n"
            "SUMMARY:Test\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";


	pComp_t = iCalConv->icalVcalToLocal(Vcal2,VCAL_TYPE,errorcode);

    CPPUNIT_ASSERT(pComp_t.size() == 1);
    pAlarm = pComp_t[0]->getAlarm();

    CPPUNIT_ASSERT(pAlarm == 0);

/*	pEvent_t = new CEvent();
	pEvent_t = (CEvent*)pComp_t[0];*/
}
