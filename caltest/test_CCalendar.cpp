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
#include <clockd/libtime.h>
#include <iostream>
#include <string.h>
#include "test_CCalendar.h"
#include "CalendarErrors.h"
#include "Common.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CCalendar_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CCalendar_TS2);
CPPUNIT_TEST_SUITE_REGISTRATION(CCalendar_TS3);
/* GetComponentsTestCase*/

void CCalendar_TS1 :: setUp()
{
    multi = CMulticalendar::MCInstance();
}

void CCalendar_TS1 :: tearDown()
{
}

void CCalendar_TS2 :: setUp()
{
    multi = CMulticalendar::MCInstance();

}

void CCalendar_TS2 :: tearDown()
{
}
void CCalendar_TS3 :: setUp()
{
    multi = CMulticalendar::MCInstance();
}

void CCalendar_TS3 :: tearDown()
{
}

int calid, bday_calid;
string eventid;
CCalendar *calendar, *bdayCal;
time_t current_time;

void CCalendar_TS1 :: test_addCalendarsetup()
{
    int error_code = -1;
    calendar = new CCalendar("TestCalendar", COLOUR_RED, 0, 1, LOCAL_CALENDAR, "Music Tune.yz", "Version-1.0");
    calid = multi->addCalendar(calendar, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: addCalendar", calendar != NULL);
    bdayCal = new CCalendar("BdayCalendar", COLOUR_RED, 0, 1, BIRTHDAY_CALENDAR, "Music Tune.yz", "Version-1.0");
    bday_calid = multi->addCalendar(bdayCal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: addCalendar- Birthday", bdayCal != NULL);
}

void CCalendar_TS1 :: test_INVaddEvent()
{
    int error_code = -1;
    CEvent *pEvent = NULL;
    bool ret = calendar->addEvent(pEvent, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Invalid event", (ret == false));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for empty event", (error_code == CALENDAR_INVALID_ARG_ERROR));
}

void CCalendar_TS1 :: test_INVaddEvent1()
{
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start - 10500;
    CEvent *pEvent =  new CEvent("summary", "description", "location", start + 10000, end);
    bool ret = calendar->addEvent(pEvent, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Invalid event start time > end time", (ret == false));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for empty event", (error_code == CALENDAR_INVALID_ARG_ERROR));
}

void CCalendar_TS1 :: test_addEvent_1()
{
    time_t start, end;
    time(&start);
    end = start + 50000;
    int error_code = -1;
    CEvent *pEvent = new CEvent("event_summary_without", "description",
                                "location", start + 10000, end);
    vector < string > vAttachments;
    vAttachments.push_back("EventAttach1");
    pEvent->setAttachments(vAttachments);
    CPPUNIT_ASSERT_MESSAGE("Error: addEvent_1", pEvent != NULL);
    bool ret = calendar->addEvent(pEvent, error_code);
    eventid = pEvent->getId();
    CPPUNIT_ASSERT_MESSAGE("Error: adding event without alarm and recurrence", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
void CCalendar_TS1 :: test_getComponents_wo_recurrence()
{
    int error_code = -1;
    vector < CComponent * > ret = calendar->getComponents(E_EVENT, -1, -1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getComponents without recurrence events", (ret.size() > 0));

}
void CCalendar_TS1 :: test_modifyEvent_1()
{
    int error_code = -1;

    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CEvent *event = cal->getEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting event", (event != NULL));
    event->setAllDay(0);
    bool ret = calendar->modifyEvent(event, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying event", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
void CCalendar_TS1 :: test_modifyEvent_invisible()
{
    int error_code = -1;
    /* modify calendar*/
    calendar->setCalendarShown(0);
    multi->modifyCalendar(calendar, error_code);
    /*add alarm for event*/
    CEvent *event = calendar->getEvent(eventid, error_code);
    CAlarm *pAlarm = new CAlarm(time(NULL) + 100, 1);
    event->setAlarm(pAlarm);
    bool ret = calendar->modifyEvent(event, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify event by adding alarm for invisible cal", (ret == true));
    calendar->setCalendarShown(1);
    multi->modifyCalendar(calendar, error_code);
}


void CCalendar_TS1 :: test_modifyEvent_1All()
{

    int error_code = -1;
    time_t start, until;
    time(&start);
    until = start + 10000;

    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CEvent *pEvent = cal->getEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting event", (pEvent != NULL));
    CAlarm *pAlarm  = new CAlarm(start + 1000, 1);
    pEvent->setAlarm(pAlarm);
    CRecurrence *pRecur = new CRecurrence();
    vector <string> rules;
    vector <string> rdates;
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
    string rdate("20090702T020000");
    rdates.push_back(rdate);
    pRecur->setRDays(rdates);
    pEvent->setRecurrence(pRecur);
    pEvent->setAllDay(0);

    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);

    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);

    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));

    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    pEvent->setHashMap(hashmap);

    bool ret = calendar->setCalendarShown(1);
    CPPUNIT_ASSERT_MESSAGE("Error: set shown", ret == true);
    ret = calendar->modifyEvent(pEvent, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying event", (ret == true));

}
CEvent mpEvent;
void CCalendar_TS1 :: test_addEvent()
{
    string sLoc;
    CAlarm *pAlarm;
    int error_code = -1;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 50000;
    CEvent *pEvent = new CEvent("summary_1", "description",
                                "location", start + 5000, end);
    mpEvent = *pEvent;
    vector < string > vAttachments;
    vAttachments.push_back("EventAttach1");
    pEvent->setAttachments(vAttachments);
    pEvent->setTransparency("OPAQUE");
    pEvent->setPriority(4);
    //pAlarm  = new CAlarm(start + 1000,1);
    pAlarm  = new CAlarm(start - 3600, 1);
    pAlarm->setAttach("sAlarmAttach");
    pEvent->setAlarm(pAlarm);
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


    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pEvent->setAllDay(0);
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pEvent->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pEvent->setOrganizer(pOrga);



    CProperties *pProp = new CProperties();
    vector < CProperties * > vProp;
    pProp->setPropName("X-ABC");
    PropType prop;
    prop.szString = "XX";
    pProp->setPropValue(prop);
    vProp.push_back(pProp);
    pEvent->setXProperties(vProp);

    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);

    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);
    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));
    hashmap.insert(pair<string, vector<CParameters *> >(RESOURCES , vRes));
    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    pEvent->setHashMap(hashmap);
    calendar->setCalendarId(calid) ;

    bool ret = calendar->setCalendarShown(0);
    CPPUNIT_ASSERT_MESSAGE("Error: set calendar shown", ret == false);
    ret = calendar->addEvent(pEvent, error_code);
    eventid = pEvent->getId();
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

}
void CCalendar_TS1 :: test_addEventDuplicate()
{

    int error_code = -1;
    bool ret = calendar->addEvent(&mpEvent, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));

}
void CCalendar_TS1 :: test_modifyEvent()
{
    int error_code = -1;

    current_time = time_get_time();
    time_set_time(current_time + 100);

    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CEvent *event = cal->getEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting event", (event != NULL));
    event->setAllDay(0);
    bool ret = calendar->modifyEvent(event, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying event", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_modifyEventAll()
{
    int error_code = -1;
    time_t until;
    time(&until);
    until = until + 50000;
    current_time = time_get_time();
    time_set_time(current_time + 100);

    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CEvent *event = cal->getEvent(eventid, error_code);
    event->setLastModified(time(NULL) + 100);
    CPPUNIT_ASSERT_MESSAGE("Error: getting event", (event != NULL));
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

    event->setRtype(repeattype);
    event->setUntil(until);
    pRecur->setRrule(rules);
    event->setRecurrence(pRecur);
    event->setAllDay(0);
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    event->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    event->setOrganizer(pOrga);

    CProperties *pProp = new CProperties();
    vector < CProperties * > vProp;
    pProp->setPropName("X-ABC");
    PropType prop;
    prop.szString = "XX";
    pProp->setPropValue(prop);
    vProp.push_back(pProp);
    event->setXProperties(vProp);

    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);
    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);
    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));
    hashmap.insert(pair<string, vector<CParameters *> >(RESOURCES , vRes));
    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    event->setHashMap(hashmap);

    bool ret = calendar->setCalendarShown(0);
    CPPUNIT_ASSERT_MESSAGE("Error: set calendar shown", ret == false);
    ret = calendar->modifyEvent(event, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying event", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

string todoid;
CTodo mpTodo;
void CCalendar_TS1 :: test_addTodo_1()
{
    int error_code = -1;
    time_t start;
    time(&start);
    CTodo *pTodo = new CTodo("todo_without", start + 1000, 0);
    bool ret = calendar->addTodo(pTodo, error_code);
    todoid = pTodo->getId();
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_addTodo_invalid_arg()
{
    int error_code = -1;
    CTodo *pTodo = NULL;
    bool ret = calendar->addTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == false));
}
void CCalendar_TS1 :: test_modifyTodo_1()
{
    int error_code = -1;
    vector < string > vAttachments;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CTodo *pTodo = cal->getTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));
    pTodo->setStatus(1);
    vAttachments.push_back("TodoAttach1");
    pTodo->setAttachments(vAttachments);
    bool ret = cal->modifyTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
void CCalendar_TS1 :: test_modifyTodo_invalid_arg()
{
    int error_code = -1;
    CTodo *pTodo = NULL;
    bool ret = calendar->modifyTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify todo", (ret == false));
}
void CCalendar_TS1 :: test_modifyTodo_1All()
{
    int error_code = -1;
    time_t until;
    time(&until);
    until = until + 50000;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CTodo *pTodo = cal->getTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));
    pTodo->setLastModified(time(NULL) + 100);

    CAlarm *pAlarm = new CAlarm(time(NULL) + 100, 1);
    pAlarm->setAttach("TodoAttach");
    pTodo->setAlarm(pAlarm);
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
    pTodo->setRtype(repeattype);
    pTodo->setUntil(until);
    pRecur->setRrule(rules);
    pTodo->setRecurrence(pRecur);
    pTodo->setAllDay(0);
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pTodo->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    pTodo->setOrganizer(pOrga);

    CProperties *pProp = new CProperties();
    vector < CProperties * > vProp;
    pProp->setPropName("X-ABC");
    PropType prop;
    prop.szString = "XX";
    pProp->setPropValue(prop);
    vProp.push_back(pProp);
    pTodo->setXProperties(vProp);
    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);
    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);
    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));
    hashmap.insert(pair<string, vector<CParameters *> >(RESOURCES , vRes));
    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    pTodo->setHashMap(hashmap);

    bool ret = cal->setCalendarShown(0);
    CPPUNIT_ASSERT_MESSAGE("Error: set calendar shown", ret == false);
    ret = cal->modifyTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}


void CCalendar_TS1 :: test_addTodo()
{
    int error_code = -1;
    time_t start, until;
    vector < string > vAttachments;
    time(&start);
    until = start + 10000;
    CTodo *pTodo = new CTodo("todo-summary", start + 1000, start + 50000);
    CAlarm *pAlarm  = new CAlarm(start + 10, 1);
    pTodo->setAlarm(pAlarm);
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
    pTodo->setRtype(repeattype);
    pTodo->setUntil(until);
    pRecur->setRrule(rules);
    pTodo->setRecurrence(pRecur);
    pTodo->setAllDay(0);

    vAttachments.push_back("TodoAttach1");
    pTodo->setAttachments(vAttachments);

    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pTodo->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    pTodo->setOrganizer(pOrga);
    pTodo->setType(E_TODO);

    CProperties *pProp = new CProperties();
    vector < CProperties * > vProp;
    pProp->setPropName("X-INT-TYPE");
    PropType prop;
    prop.i = 10;
    pProp->setPropValue(prop);
    vProp.push_back(pProp);
    pTodo->setXProperties(vProp);
    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);
    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);
    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));
    hashmap.insert(pair<string, vector<CParameters *> >(RESOURCES , vRes));
    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    pTodo->setHashMap(hashmap);


    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));
    CCalendar *cal = NULL;
    cal = multi->getCalendarById(calid, error_code);

    bool ret = calendar->setCalendarShown(0);
    CPPUNIT_ASSERT_MESSAGE("Error: set calendar shown", ret == false);
    ret = calendar->addTodo(pTodo, error_code);
    todoid = pTodo->getId();
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
void CCalendar_TS1 :: test_addTodoDuplicate()
{
    int error_code = -1;
    bool ret = calendar->addTodo(&mpTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == true));
}
void CCalendar_TS1 :: test_modifyTodo()
{
    int error_code = -1;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CTodo *pTodo = cal->getTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));
    pTodo->setStatus(1);
    bool ret = cal->modifyTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_modifyTodoAll()
{
    int error_code = -1;
    time_t until;
    time(&until);
    until = until + 50000;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CTodo *pTodo = cal->getTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));
    pTodo->setLastModified(time(NULL) + 100);
    CAlarm *pAlarm = new CAlarm(time(NULL) + 100, 1);
    pTodo->setAlarm(pAlarm);
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
    pTodo->setRtype(repeattype);
    pTodo->setUntil(until);
    pRecur->setRrule(rules);
    pTodo->setRecurrence(pRecur);
    pTodo->setAllDay(0);
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pTodo->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    pTodo->setOrganizer(pOrga);


    CProperties *pProp = new CProperties();
    vector < CProperties * > vProp;
    pProp->setPropName("X-ABC");
    PropType prop;
    prop.szString = "XX";
    pProp->setPropValue(prop);
    vProp.push_back(pProp);
    pTodo->setXProperties(vProp);
    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);
    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);
    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));
    hashmap.insert(pair<string, vector<CParameters *> >(RESOURCES , vRes));
    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    pTodo->setHashMap(hashmap);

    bool ret = cal->setCalendarShown(0);
    CPPUNIT_ASSERT_MESSAGE("Error: set calendar shown", ret == false);
    ret = cal->modifyTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
string journalid;
void CCalendar_TS1 :: test_addJournal_1()
{
    int error_code = -1;
    CJournal *pJournal = new CJournal("journal_without");

    bool retv = calendar->addJournal(pJournal, error_code);
    journalid = pJournal->getId();
    CPPUNIT_ASSERT_MESSAGE("Error: adding journal", (retv == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_addJournal_invalid_arg()
{
    int error_code = -1;
    CJournal *pJournal = NULL;
    bool ret = calendar->addJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding journal", (ret == false));
}
void CCalendar_TS1 :: test_modifyJournal_1()
{
    int error_code = -1;
    vector < string > vAttachments;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CJournal *pJournal = cal->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting journal", (pJournal != NULL));
    pJournal->setSummary("New Summary Modified");
    vAttachments.push_back("JourAttach1");
    pJournal->setAttachments(vAttachments);
    bool ret = cal->modifyJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_modifyJournal_invalid_arg()
{
    int error_code = -1;
    CJournal *pJournal = NULL;
    bool ret = calendar->modifyJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == false));
}

void CCalendar_TS1 :: test_modifyJournal_1All()
{
    int error_code = -1;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CJournal *pJournal = cal->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting journal", (pJournal != NULL));
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pJournal->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    pJournal->setOrganizer(pOrga);
    pJournal->setSummary("New Summary Modified");

    CProperties *pProp = new CProperties();
    vector < CProperties * > vProp;
    pProp->setPropName("X-ABC");
    PropType prop;
    prop.szString = "XX";
    pProp->setPropValue(prop);
    vProp.push_back(pProp);
    pJournal->setXProperties(vProp);
    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);
    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);
    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));
    hashmap.insert(pair<string, vector<CParameters *> >(RESOURCES , vRes));
    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    pJournal->setHashMap(hashmap);


    bool ret = cal->setCalendarShown(0);
    CPPUNIT_ASSERT_MESSAGE("Error: set calendar shown", ret == false);
    ret = cal->modifyJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
void CCalendar_TS1 :: test_addJournal()
{
    int error_code = -1;
    CJournal *pJournal = new CJournal("journal_with");
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pJournal->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    pJournal->setOrganizer(pOrga);

    CProperties *pProp = new CProperties();
    vector < CProperties * > vProp;
    pProp->setPropName("X-ABC");
    PropType prop;
    prop.szString = "XX";
    pProp->setPropValue(prop);
    vProp.push_back(pProp);
    pJournal->setXProperties(vProp);
    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);
    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);
    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));
    hashmap.insert(pair<string, vector<CParameters *> >(RESOURCES , vRes));
    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    pJournal->setHashMap(hashmap);


    bool ret = calendar->setCalendarShown(0);
    CPPUNIT_ASSERT_MESSAGE("Error: set calendar shown", ret == false);
    ret = calendar->addJournal(pJournal, error_code);
    journalid = pJournal->getId();
    CPPUNIT_ASSERT_MESSAGE("Error: adding journal", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_modifyJournal()
{
    int error_code = -1;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CJournal *pJournal = cal->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting journal", (pJournal != NULL));
    pJournal->setSummary("New Summary Modified");
    bool ret = cal->modifyJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
void CCalendar_TS1 :: test_modifyJournalAll()
{
    int error_code = -1;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CJournal *pJournal = cal->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting journal", (pJournal != NULL));
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pJournal->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    pJournal->setOrganizer(pOrga);
    pJournal->setSummary("New Summary Modified");

    CProperties *pProp = new CProperties();
    vector < CProperties * > vProp;
    pProp->setPropName("X-ABC");
    PropType prop;
    prop.szString = "XX";
    pProp->setPropValue(prop);
    vProp.push_back(pProp);
    pJournal->setXProperties(vProp);
    vector < CParameters * > vCat;
    vector < CParameters * > vRes;
    vector < CParameters * > vCon;
    vector < CParameters * > vLoc;
    vector < CParameters * > vSum;
    vector < CParameters * > vDes;
    vector < CParameters * > vCom;
    vector < CParameters * > vRel;
    vector < CParameters * > vPar;

    CParameters *pLang = new CParameters();
    pLang->setParamName(LANGUAGE);
    ParamType lang;
    lang.szString = "ENG";
    pLang->setParamValue(lang);
    /*insertCATEGORIES*/
    vCat.push_back(pLang);
    CParameters *pAlter = new CParameters();
    pAlter->setParamName(ALTER);
    ParamType alter;
    alter.szString = "alternate rep";
    pAlter->setParamValue(alter);
    /*insertRESOURCES(contacts),LOCATION,SUMMARY,DESCRIPTION,COMMENTSi*/
    vRes.push_back(pAlter);
    CParameters *pRel = new CParameters();
    pRel->setParamName(RELATEDTO);
    ParamType rel;
    rel.i = 2;
    pRel->setParamValue(rel);
    /*insertRELATEDTO*/
    vRel.push_back(pRel);
    CParameters *pPar = new CParameters();
    pPar->setParamName("X-PARAM");
    ParamType x;
    x.szString = "paramval";
    pPar->setParamValue(x);
    /*insertxparam*/
    vPar.push_back(pPar);

    map <string, vector<CParameters *> > hashmap;
    hashmap.insert(pair<string, vector<CParameters *> >(CATEGORIES , vCat));
    hashmap.insert(pair<string, vector<CParameters *> >(RESOURCES , vRes));
    hashmap.insert(pair<string, vector<CParameters *> >(RELATEDTO , vRel));
    hashmap.insert(pair<string, vector<CParameters *> >("X-PARAM" , vPar));
    pJournal->setHashMap(hashmap);

    bool ret = cal->setCalendarShown(0);
    CPPUNIT_ASSERT_MESSAGE("Error: set calendar shown", ret == false);
    ret = cal->modifyJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1::test_addBday_1()
{
    int error_code = -1;
    CBdayEvent *pBday = new CBdayEvent("abcder-without", "noname1", time(NULL));
    bool bRet = bdayCal->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (bRet == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
void CCalendar_TS1::test_addBday_invalid_arg()
{
    int error_code = -1;
    CBdayEvent *pBday = NULL;
    bool bRet = bdayCal->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday with Invalid pointer", (bRet == false));
}
void CCalendar_TS1::test_addBday()
{
    bool bRet;
    time_t utc_date;
    time_t until;

    int error_code = -1;
    utc_date = time(NULL);
    until = time(NULL) + (time_t) 86400 * 28 * 365;
    CBdayEvent *pBday = new CBdayEvent("abcder", "noname2", utc_date + 10000);
    CAlarm *pAlarm = new CAlarm(time(NULL) + 100, 1);
    pBday->setAlarm(pAlarm);
    CRecurrence *pRecur = new CRecurrence();
    vector <string> rules;
    rules.clear();
    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
    string rrule;
    struct icaltimetype uical;
    int repeattype = 5;
    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pBday->setRtype(5);
    pBday->setUntil(until);
    pRecur->setRrule(rules);
    pBday->setRecurrence(pRecur);

    bRet = bdayCal->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (bRet == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1::test_modifyBirthDay()
{
    bool bRet;
    time_t utc_date;
    time_t until;
    int error_code = -1;

    utc_date = time(NULL);
    CAlarm *pAlarm = 0;
    pAlarm = new CAlarm(time(NULL) + 100, 7);
    until = utc_date + (time_t) 86400 * 29 * 365;
    CBdayEvent *pBday = new CBdayEvent("abcder1", "ModifiedBday", utc_date);
    CRecurrence *pRecur = new CRecurrence();
    vector <string> rules;
    rules.clear();
    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
    string rrule;
    struct icaltimetype uical;
    int repeattype = 4;
    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pBday->setRtype(5);
    pBday->setUntil(until);
    pRecur->setRrule(rules);
    pBday->setRecurrence(pRecur);
    bRet = bdayCal->modifyBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify birthday", (bRet == true));
}
void CCalendar_TS1::test_modifyBday_addalarm_invisiblecal()
{
    bool bRet;
    time_t utc_date;
    int error_code = -1;

    /*modify Bday calendar*/
    bdayCal->setCalendarShown(false);
    multi->modifyCalendar(bdayCal, error_code);
    /*add Bday*/
    utc_date = time(NULL);
    CBdayEvent *pBday = new CBdayEvent("AddingAlarm to invisible", "AddingAlarm to invisible", utc_date + 10000);
    bdayCal->addBirthDay(pBday, error_code);
    /*modify added Bday for adding alarm*/
    CAlarm *pAlarm = new CAlarm(time(NULL) + 100, 1);
    pBday->setAlarm(pAlarm);
    bRet = bdayCal->modifyBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify birthday invisible calendar", (bRet == true));
    bdayCal->setCalendarShown(true);
    multi->modifyCalendar(bdayCal, error_code);
}
void CCalendar_TS1::test_modifyBday_modifyalarm()
{
    bool bRet;
    time_t utc_date;
    int error_code = -1;
    CAlarm *pAlarm1 = NULL;
    /*add Bday*/
    utc_date = time(NULL);
    CBdayEvent *pBday = new CBdayEvent("ModifyingAlarm for Bday", "ModifyingAlarm for Bday", utc_date + 10000);
    CAlarm *pAlarm = new CAlarm(time(NULL) + 100, 1);
    pBday->setAlarm(pAlarm);
    bdayCal->addBirthDay(pBday, error_code);
    string bid = pBday->getId();
    delete pAlarm;
    pAlarm = 0;

    /*modify Bday calendar*/
    bdayCal->setCalendarShown(false);
    multi->modifyCalendar(bdayCal, error_code);

    /*modify addedBday for modifying alarm*/
    pAlarm = pBday->getAlarm();
    pAlarm->setTrigger(time(NULL) + 200);
    pBday->setAlarm(pAlarm);
    bRet = bdayCal->modifyBirthDay((CBdayEvent *)pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify birthday Alarm for invisible calendar", (bRet == true));

    bdayCal->setCalendarShown(true);
    multi->modifyCalendar(bdayCal, error_code);
    pAlarm1 = pBday->getAlarm();
    pAlarm1->setTrigger(time(NULL) + 300);
    pBday->setAlarm(pAlarm1);
    bRet = bdayCal->modifyBirthDay((CBdayEvent *)pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify birthday Alarm for visible calendar", (bRet == true));

}
void CCalendar_TS1::test_modifyBday_invalid_arg()
{
    int error_code = -1;
    CBdayEvent *pBday = NULL;
    bool bRet = bdayCal->modifyBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify birthday with Invalid pointer", (bRet == false));
}
void CCalendar_TS1::test_deleteBirthDay()
{
    time_t utc_date, until;
    utc_date = time(NULL);
    int error_code = -1;
    utc_date = time(NULL);
    until = time(NULL) + (time_t) 86400 * 28 * 365;
    CBdayEvent *pBday = new CBdayEvent("22", "noname3", utc_date + 10000);
    CAlarm *pAlarm = new CAlarm(time(NULL) + 100, 1);
    pBday->setAlarm(pAlarm);
    CRecurrence *pRecur = new CRecurrence();
    vector <string> rules;
    rules.clear();
    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
    string rrule;
    struct icaltimetype uical;
    int repeattype = 5;
    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pBday->setRtype(5);
    pBday->setUntil(until);
    pRecur->setRrule(rules);
    pBday->setRecurrence(pRecur);

    bool bRet = bdayCal->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (bRet == true));


    //int error_code = -1;
    bRet = calendar->deleteBirthDay("22", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleting birthday", (bRet == true));
}

void CCalendar_TS1 :: test_deleteEvent()
{
    int error_code = -1;
    bool ret = calendar->deleteEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting event", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_deleteTodo()
{
    int error_code = -1;
    bool ret = calendar->deleteTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_deleteJournal()
{
    int error_code = -1;
    bool ret = calendar->deleteJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting journal", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_getDetailedEventList()
{
    int error_code = -1;
    vector < CEvent * > event = calendar->getDetailedEventList(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getDetailedEventList", (event.size() > 0));
}

void CCalendar_TS1 :: test_getTodoListDetailed()
{
    int error_code = -1;
    vector < CTodo * > todo = calendar->getTodoListDetailed(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getTodoListDetailed", (todo.size() > 0));
}

void CCalendar_TS1 :: test_getJournalListDetailed()
{
    int error_code = -1;
    vector < CJournal * > journal = calendar->getJournalListDetailed(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getTodoListDetailed", (journal.size() > 0));
}

void CCalendar_TS1 :: test_modifyEvent_invalid_arg()
{
    int error_code = -1;
    CEvent *pEvent = NULL;
    bool ret = calendar->modifyEvent(pEvent, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying event", (ret == false));
}
void CCalendar_TS1 :: test_modifyEvent1()
{
    string sLoc;
    int error_code = -1;
    CAlarm *pAlarm;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + (86400 * 100);
    CEvent *pEvent = new CEvent("summary1", "description",
                                "location", start + 10000, end);
    pEvent->setTransparency("OPAQUE");
    pEvent->setPriority(4);
    pAlarm  = new CAlarm(start, 1);
    pEvent->setAlarm(pAlarm);
    CRecurrence *pRecur = new CRecurrence();
    vector <string> rules;
    rules.clear();
    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
    string rrule;
    struct icaltimetype uical;
    int repeattype = 1;
    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pEvent->setFlags(HAS_RECURRENCE_ALARM);
    pEvent->setAllDay(0);
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(REQ_PARTICIPANT);
    pAt1->setParticipationStatus(ACCEPTED);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setSentBy("sent by");
    pAt1->setLanguage("English");
    pAt1->setRSVP(1);
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pEvent->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    pEvent->setOrganizer(pOrga);

    bool retv;
    retv = calendar->addEvent(pEvent, error_code);
    pEvent->setSummary("Modify");
    pOrga->setLanguage("Modified-English");
    pOrga->setSentBy("Modified-Myself");
    pOrga->setCommonName("Modified-Something");
    pOrga->setXparameters("Modified-WhatisThis");
    pOrga->setCalAddress("Modified-!!!!!!!!!");
    pOrga->setDirectoryParameter("Modified-Home");
    pEvent->setOrganizer(pOrga);
    bool ret = calendar->modifyEvent(pEvent, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify event", (ret == true));
}

void CCalendar_TS1 :: test_modifyTodo1()
{
    time_t start;
    time(&start);
    int error_code = -1;
    CTodo *pTodo = new CTodo("summary", start + 1000, 0);
    bool retv ;
    retv = calendar->addTodo(pTodo, error_code);
    pTodo->setStatus(1);
    bool ret = calendar->modifyTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:modify todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_modifyJournal1()
{
    CJournal *pJournal = new CJournal("summary");
    int error_code = -1;
    bool retv ;
    retv = calendar->addJournal(pJournal, error_code);
    pJournal->setSummary("MODIFIED");
    bool ret = calendar->modifyJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}

void CCalendar_TS1 :: test_exportCalendar()
{
    CCalendar *calendar1;
    int error_code = -1;
    CBdayEvent *pBday1 = new CBdayEvent("testbd", "noname", time(NULL));
    calendar1 = new CCalendar("TestBdayX", COLOUR_RED, 1, 1, BIRTHDAY_CALENDAR, "Music Tune1.xyz", "Version-1.0");
    multi->addCalendar(calendar1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: addCalendar", calendar1 != NULL);
    bool ret = calendar1->addBirthDay(pBday1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (ret == true));
    ret = calendar1->exportCalendarEntries("exportcal.ics", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Exporting Calendar", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

}

void CCalendar_TS1 :: test_getAllAddItems()
{
    time_t start;
    time(&start);
    int error_code = -1;
    vector < CComponent * > ret =  calendar->getAllAddItems(start - 20000, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get all added items", (ret.size() > 0));
}
void CCalendar_TS1 :: test_getAllModifiedItems()
{
    int error_code = -1;
    CTodo *pTodo = calendar->getTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));
    CEvent *event = calendar->getEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting event", (event != NULL));
    CJournal *pJournal = calendar->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting journal", (pJournal != NULL));


    sleep(4);
    pTodo->setStatus(1);
    bool ret = calendar->modifyTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: in modifying toso", (ret == true));
    event->setAllDay(0);
    ret = calendar->modifyEvent(event, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying event", (ret == true));
    pJournal->setSummary("New Summary Modified");
    ret = calendar->modifyJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == true));

    CTodo *pTodo1 = calendar->getTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting modified todo", (pTodo1 != NULL));
    CEvent *event1 = calendar->getEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting modified todo", (pTodo1 != NULL));
    CJournal *pJournal1 = calendar->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting Modified journal", (pJournal1 != NULL));
    vector < CComponent * > retList =  calendar->getAllModifiedItems((pTodo1->getLastModified() - 2) , error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get all modified items", (retList.size() >= 0));
}

void CCalendar_TS1 :: test_getAllDeletedItems()
{
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start + 10500;
    CEvent *pEvent =  new CEvent("event_1", "description", "location", start + 10000, end);
    bool ret = calendar->addEvent(pEvent, error_code);
    eventid = pEvent->getId();
    ret = calendar->deleteEvent(eventid, error_code);
    vector < string  > del_items =  calendar->getAllDeletedItems(start - 10000, 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get all deleted items", (del_items.size() >= 0));
    del_items =  calendar->getAllDeletedItems(start + 10000, 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get all deleted items", (del_items.size() == 0));
    time_set_time(current_time);
}

void CCalendar_TS1 :: test_setNextAlarm()
{
    int error_code = -1;
    bool ret = calendar->setNextAlarm(eventid, 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Setting Next Alarm", (ret == true));
}

void CCalendar_TS1 :: test_setNextAlarm_invalid()
{
    int error_code = -1;
    string id = "4567";
    bool ret = calendar->setNextAlarm(id, 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Setting Next Alarm for inavlid eventId", (ret == false));
    ret = calendar->setNextAlarm(id, 2, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Setting Next Alarm for inavlid TodoId", (ret == false));
}
void CCalendar_TS1 :: test_getComponents()
{
    int error_code = -1;
    vector < CComponent * > ret = calendar->getComponents(E_EVENT, -1, -1, 100, 0, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getComponents", (ret.size() > 0));
}

void CCalendar_TS1 :: test_getComponents1()
{
    int error_code = -1;

    vector < CComponent * > ret = calendar->getComponents(E_EVENT, -1, -1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getComponents", (ret.size() > 0));
}

void CCalendar_TS1 :: test_getComponents2()
{
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start;
    vector < CComponent * > ret = calendar->getComponents(E_EVENT, start - 100000, end + 100000, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getComponents", (ret.size() > 0));
}


void CCalendar_TS1 :: test_operator()
{
    CCalendar *cal = new CCalendar("TestCal", COLOUR_RED, 0, 1, LOCAL_CALENDAR, "Music Tune.xyz", "Version-1.0");
    CCalendar cal1(*(cal));
    CCalendar cal2;
    cal2 = *(cal);
}

void CCalendar_TS1 :: test_getAllAddedEvents()
{
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start + 10500;
    CEvent *pEvent =  new CEvent("event_1", "description", "location", start + 10000, end);
    bool ret = calendar->addEvent(pEvent, error_code);
    eventid = pEvent->getId();


    CPPUNIT_ASSERT_MESSAGE("Error: Invalid event start time > end time", (ret == true));

    vector<CEvent *> eList = calendar->getAllAddedEvents(start - 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllAddedEvents", eList.size() > 0);
}
void CCalendar_TS1 :: test_getAllAddedTodos()
{
    int error_code = -1;
    time_t start;
    time(&start);
    CTodo *pTodo = new CTodo("todo_1", start + 1000, 0);
    vector < string > vAttachments;
    vAttachments.push_back("TodoAttach1");
    pTodo->setAttachments(vAttachments);
    bool ret = calendar->addTodo(pTodo, error_code);
    todoid = pTodo->getId();


    CPPUNIT_ASSERT_MESSAGE("Error: Invalid event start time > end time", (ret == true));

    vector<CTodo *> eList = calendar->getAllAddedTodos(start - 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllAddedTodos", eList.size() > 0);
}
void CCalendar_TS1 :: test_getAllAddedJournals()
{
    int error_code = -1;
    time_t start;
    time(&start);
    CJournal *pJournal = new CJournal("journal_1");
    bool ret = calendar->addJournal(pJournal, error_code);
    journalid = pJournal->getId();


    CPPUNIT_ASSERT_MESSAGE("Error: Invalid event start time > end time", (ret == true));

    vector<CJournal *> eList = calendar->getAllAddedJournals(start - 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllAddedJournals", eList.size() > 0);
}


void CCalendar_TS1 :: test_getAllModifiedEvents()
{
    int error_code = -1;

    CEvent *event = calendar->getEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting event", (event != NULL));

    sleep(3);
    event->setAllDay(0);
    bool ret = calendar->modifyEvent(event, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying event", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    CEvent *event1 = calendar->getEvent(eventid, error_code);
    vector<CEvent *> eList = calendar->getAllModifiedEvents((event1->getLastModified() - 2), error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllAddedEvents", eList.size() >= 0);

}
void CCalendar_TS1 :: test_getAllModifiedTodos()
{
    int error_code = -1;
    CTodo *pTodo = calendar->getTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));

    sleep(3);
    pTodo->setStatus(1);
    bool ret = calendar->modifyTodo(pTodo, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    CTodo *pTodo1 = calendar->getTodo(todoid, error_code);
    vector<CTodo *> eList = calendar->getAllModifiedTodos((pTodo1->getLastModified() - 2), error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllModifiedTodos", eList.size() >=  0);

}
void CCalendar_TS1 :: test_getAllModifiedJournals()
{
    int error_code = -1;
    vector<CJournal *> eList;

    CJournal *pJournal = calendar->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting journal", (pJournal != NULL));

    sleep(3);
    pJournal->setSummary("New Summary Modified");
    bool ret = calendar->modifyJournal(pJournal, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    CJournal *pJournal1 = calendar->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting Modified journal", (pJournal1 != NULL));

    eList = calendar->getAllModifiedJournals((pJournal1->getLastModified() - 2), error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllModifiedJournals", eList.size() >= 0);
}
void CCalendar_TS1 :: test_getAllDeletedEvents()
{
    int error_code = -1;
    current_time = time_get_time();
    time_set_time(current_time + 10);
    bool ret = calendar->deleteEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting event", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    vector<string> eList = calendar->getAllDeletedEvents(current_time + 5, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllDeletedEvents", eList.size() == 0);
    time_set_time(current_time);

}
void CCalendar_TS1 :: test_getAllDeletedTodos()
{
    int error_code = -1;
    current_time = time_get_time();
    time_set_time(current_time + 10);
    bool ret = calendar->deleteTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    vector<string> eList = calendar->getAllDeletedTodos(current_time + 5, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllDeletedTodoss", eList.size() == 0);
    time_set_time(current_time);
}
void CCalendar_TS1 :: test_getAllDeletedJournals()
{
    int error_code = -1;
    current_time = time_get_time();
    time_set_time(current_time + 10);
    bool ret = calendar->deleteJournal(todoid, error_code);

    CPPUNIT_ASSERT_MESSAGE("Error:deleting todo", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    vector<string> eList = calendar->getAllDeletedJournals(current_time + 5, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllDeletedJournals", eList.size() == 0);
    time_set_time(current_time);
}

void CCalendar_TS1 :: test_registerAlarmForVisibleCalendar()
{
    /* in this case, there are no events with invalid/zero value for cookie. So register
     * function will return true without actually creating a cookie for the existing event
     * */
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start + 10500;
    CEvent *pEvent =  new CEvent("event_alarm_1", "description", "location", start + 10000, end);
    CAlarm *alarm = new CAlarm(start + 99970, 1);
    pEvent->setAlarm(alarm);
    bool ret = calendar->addEvent(pEvent, error_code);
    ret = calendar->registerAlarmForVisibleCalendar(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: registerAlarmForVisibleCalendar", (ret == true));
    ret = bdayCal->registerAlarmForVisibleCalendar(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: registerAlarmForVisibleCalendar- Birthday", (ret == true));
}
void CCalendar_TS1 :: test_destroyExistingValidAlarms()
{
    int error_code = -1;
    bool ret = calendar->destroyExistingValidAlarms(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_destroyExistingValidAlarms", (ret == true));
}
void CCalendar_TS1 :: test_destroyExistingValidAlarms_bdayCal()
{
    int error_code = -1;
    bool ret = bdayCal->destroyExistingValidAlarms(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_destroyExistingValidAlarms", (ret == true));
}

void CCalendar_TS1 :: test_checkDuplicateEntryEvent()
{
    int error_code = -1;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CEvent *pEvent = cal->getEvent(eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting eevnt", (pEvent != NULL));
    bool ret = cal->checkDuplicateEntry((CComponent *)pEvent, 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: checkDuplicateEntryEvent", (ret == true));
}
void CCalendar_TS1 :: test_checkDuplicateEntryTodo()
{
    int error_code = -1;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CTodo *pTodo = cal->getTodo(todoid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));
    bool ret = cal->checkDuplicateEntry((CComponent *)pTodo, 2, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: checkDuplicateEntryTodo", (ret == true));
}
void CCalendar_TS1 :: test_checkDuplicateEntryJournal()
{
    int error_code = -1;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
    CJournal *pJournal = cal->getJournal(journalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting journal", (pJournal != NULL));
    bool ret = cal->checkDuplicateEntry((CComponent *)pJournal, 3, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: checkDuplicateEntryjournal", (ret == true));
}

void CCalendar_TS1 :: test_deleteComponent()
{
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start + 10500;
    CCalendar *cal = multi->getCalendarById(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned for success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    CEvent *pEvent =  new CEvent("Event_del", "description", "location", start + 10000, end);
    bool ret = cal->addEvent(pEvent, error_code);
    string eventid = pEvent->getId();
    cal->deleteComponent(eventid,  error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleteComponent_Event", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    CTodo *pTodo =  new CTodo("Todo_del", start, end);
    ret = cal->addTodo(pTodo, error_code);
    string todoid = pTodo->getId();
    cal->deleteComponent(todoid,  error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleteComponent_Todo", (error_code == CALENDAR_OPERATION_SUCCESSFUL));

    CJournal *pJournal =  new CJournal("Journal_del");
    ret = cal->addJournal(pJournal, error_code);
    string jounralid = pJournal->getId();
    cal->deleteComponent(journalid,  error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleteComponent_Journal", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}



void  CCalendar_TS1 :: test_getAllEventsTodosInvalid()
{
    int error_code = -1;
    int offset = 0;
    time_t start, end;
    time(&start);
    end = start + 10500;
    vector<CComponent *> list = calendar->getAllEventsTodos(start - 100, end + 100, 10, offset, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllEventsTodosInvalid", list.size() == 0);
    offset = 0;
    list = calendar->getAllEventsTodos(-1, -1, 10, offset, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllEventsTodosInvalid", list.size() == 0);
}
void  CCalendar_TS1 :: test_getAllEventsTodos()
{
    int error_code = -1;
    int offset = 0;
    time_t start, end;
    time(&start);
    end = start + 10500;
    vector<CComponent *> list = calendar->getAllEventsTodos(start - 100, end + 100, 10, offset, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllEventsTodos", list.size() > 0);
    offset = 0;
    list = calendar->getAllEventsTodos(-1, -1, 10, offset, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getAllEventsTodos", list.size() > 0);
}
void CCalendar_TS1 :: test_copyConstrutor()
{
    CCalendar *cal = new CCalendar();
    bool ret = cal->setCalendarName("ABC_Calendar");
    CPPUNIT_ASSERT_MESSAGE("Error: setting calendar name", ret == true);
    CCalendar cal1(*cal);
    CCalendar cal2 = *cal;
    CPPUNIT_ASSERT_MESSAGE("Error: copy constructor", cal->getCalendarName() == cal1.getCalendarName());
    CPPUNIT_ASSERT_MESSAGE("Error: assiginment operator", cal->getCalendarName() == cal2.getCalendarName());
    delete cal;
}
void CCalendar_TS1 :: test_constructor()
{
    CCalendar *cal = new CCalendar(100, "Cal100", COLOUR_RED, 0, 1, LOCAL_CALENDAR, "");
    CPPUNIT_ASSERT_MESSAGE("Error: constructor", cal->getProfileType() == SYNC_METHOD_NONE);
}

void CCalendar_TS1:: test_getEntry()
{
    int error_code = -1;
    CEvent *event = (CEvent *)calendar->getEntry(eventid, 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get event entry", event != NULL);
    CPPUNIT_ASSERT_MESSAGE("Error: get event entry summary", event->getSummary() == string("summary_1"));
    CTodo *todo = (CTodo *)calendar->getEntry(todoid, 2, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:get todo entry", todo != NULL);
    CPPUNIT_ASSERT_MESSAGE("Error: get todo entry summary", todo->getSummary() == string("todo-summary"));
    CJournal *journal  = (CJournal *)calendar->getEntry(journalid, 3, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:get journal entry", journal != NULL);
    CPPUNIT_ASSERT_MESSAGE("Error: get journal entry summary", journal->getSummary() == string("New Summary Modified"));
}

void CCalendar_TS1:: test_getEntry_invalid_type()
{
    int error_code = -1;
    calendar->getEntry(journalid, 4, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:get entry with Invalid Type ", error_code == CALENDAR_INVALID_ARG_ERROR);
}
void CCalendar_TS1 :: test_registerAlarmForVisibleCalendarForAlarmWithCookieZero()
{
    /* in this case, there is atleast one event (the one added in this test case)
     * that the registe API will update by creating cookie
     * */
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start + 10500;
    CEvent *pEvent =  new CEvent("event_alarm_1_AlarmWithCookieZero", "description", "location", start + 10000, end);
    CAlarm *alarm = new CAlarm(start + 99970, 1);
    pEvent->setAlarm(alarm);
    bool ret = calendar->addEvent(pEvent, error_code);
    calendar->setCalendarShown(false);
    multi->modifyCalendar(calendar, error_code);
    ret = calendar->registerAlarmForVisibleCalendar(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: registerAlarmForVisibleCalendar", (ret == true));
}
void CCalendar_TS1::test_getBirthdayEvent()
{
    int error_code1 = -1;
    string bdayid;
    CBdayEvent *pBday = new CBdayEvent("BDAY", "noname", time(NULL));
    bool bRet = bdayCal->addBirthDay(pBday, error_code1);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (bRet == true));
    bdayid = pBday->getId();
    CEvent *pb = bdayCal->getBirthDayEvent(bdayid, error_code1);
    CPPUNIT_ASSERT_MESSAGE("Error: getting Bday event", (pb != NULL));
}
void CCalendar_TS1 :: test_getDetailedBDayList()
{
    int error_code = -1;
    vector < CEvent * > event = calendar->getDetailedBdayList(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getDetailedEventList", (event.size() >= 0));
}
void CCalendar_TS1 :: test_checkIfMultiDayEvent()
{
    vector <time_t> vTime;
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start + 86400 * 3;
    CEvent *pEvent = new CEvent("MultiDay", "description",
                                "location", start + 10000, end);
    calendar->addEvent(pEvent, error_code);
    calendar->checkIfMultiDayEvent(pEvent, vTime);
    CPPUNIT_ASSERT_MESSAGE("Error:checkIfMultipleDayEvent", (vTime.size() >= 1));
}

void CCalendar_TS1 :: test_checkIfMultiDayEvent_equaldates()
{
    vector <time_t> vTime;
    int error_code = -1;
    time_t start, end;
    time(&start);
    end = start;
    CEvent *pEvent = new CEvent("MultiDay With EqualDates", "description",
                                "location", start, end);
    calendar->addEvent(pEvent, error_code);
    calendar->checkIfMultiDayEvent(pEvent, vTime);
    CPPUNIT_ASSERT_MESSAGE("Error:checkIfMultipleDayEvent with equal dates ", (vTime.size() >= 1));

}
void CCalendar_TS1 :: test_checkIfMultiDayEvent_zerodates()
{
    vector <time_t> vTime;
    int error_code = -1;
    time_t start, end;
    start = 0;
    end = start;
    CEvent *pEvent = new CEvent("MultiDay With EqualDates", "description",
                                "location", start, end);
    calendar->addEvent(pEvent, error_code);
    calendar->checkIfMultiDayEvent(pEvent, vTime);
    CPPUNIT_ASSERT_MESSAGE("Error:checkIfMultipleDayEvent with zero dates ", (vTime.size() == 0));

}
void CCalendar_TS1 :: test_removeFromIds()
{
    string Ids = "1,2,3,5,9,88,45,68,97,74";
    string remIds = "2,45";
    string szret;
    CCalendar *pCal = NULL;
    szret = pCal->removeFromIds(remIds, Ids);
    printf("After Removal : %s ", szret.c_str());
}
void CCalendar_TS1 :: test_appendToIds()
{
    string Ids = "1,11,111,1111,11111,111111,1111111,";
    string szIds;
    CCalendar *pCal;
    pCal->appendToIds(Ids, szIds);
    CPPUNIT_ASSERT_MESSAGE("Error:getJournals", szIds == ",1,11,111,1111,11111,111111,1111111,,");
}
void CCalendar_TS2 :: test_toString()
{
    CCalendar *cal;
    int error_code = -1;
    cal = new CCalendar("TestCalendarTS2", COLOUR_RED, 0, 1, LOCAL_CALENDAR, "Music", "Version-1.0");
    int calidS2 = multi->addCalendar(cal, error_code);
    cout << calidS2;
    cout << cal->toString();
}
void CCalendar_TS2 :: test_getEvents()
{
    int Id = 2;
    int error_code = -1;
    time_t start, end, until;
    CCalendar *pCal = 0;
    pCal = multi->getCalendarById(Id, error_code);

    vector < string > vAttachments;
    vAttachments.push_back("Attach1");
    /*Todo having alarm*/
    time(&start);
    CTodo *pTodo = new CTodo("todo with alarm to test_getEvents", start + 1000, 0);
    CAlarm *pAlarm = new CAlarm(time(NULL) + 100, 1);
    pAlarm->setAttach("AlarmAtt");
    pTodo->setAlarm(pAlarm);
    pTodo->setAttachments(vAttachments);
    pCal->addTodo(pTodo, error_code);
    delete pTodo;
    pTodo = NULL;

    /*normal event*/
    end = start + 50000;
    CEvent *pEvent = new CEvent("normal event to test_getEvents", "description",
                                "location", start + 10000, end);
    pCal->addEvent(pEvent, error_code);
    delete pEvent;
    pEvent = NULL;

    /*event having all types of recuurence rules ( rrule,rdays,exrule,exdates ), Attachment*/
    end = start;
    until = start + 50000;
    pEvent = new CEvent("Event with all rrules", "description",
                        "location", start , end);
    pEvent->setAttachments(vAttachments);
    pEvent->setAlarm(pAlarm);
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
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pCal->addEvent(pEvent, error_code);
    delete pEvent;
    pEvent = NULL;
    CJournal *pJournal = new CJournal("journalx");
    vector <CAttendee *> vAttends;
    CAttendee *pAt1 = new CAttendee();
    pAt1->setMember("Member1");
    pAt1->setRole(CHAIR);
    pAt1->setParticipationStatus(TENTATIVE);
    pAt1->setDelegatees("Someone");
    pAt1->setDelegator("Someone");
    pAt1->setMailTo("MailTo1");
    vAttends.push_back(pAt1);
    pJournal->setAttendees(vAttends);
    COrganizer *pOrga = new COrganizer();
    pOrga->setLanguage("English");
    pOrga->setSentBy("Myself");
    pOrga->setCommonName("Something");
    pOrga->setXparameters("WhatisThis");
    pOrga->setCalAddress("!!!!!!!!!");
    pOrga->setDirectoryParameter("Home");
    pJournal->setOrganizer(pOrga);

    pCal->addJournal(pJournal, error_code);

    vector <CEvent *> vEvent;
    vector <CTodo *> vTodo;
    vector <CJournal *> vJournal;
    vEvent = pCal->getEvents(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getEvents", (vEvent.size() >= 0));
    vTodo = pCal->getTodos(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getTodos", (vTodo.size() >= 0));
    vJournal = pCal->getJournals(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getJournals", (vJournal.size() >= 0));

}
void CCalendar_TS1 :: test_addXProperties_invalid_arg()
{
    int error_code = -1;
    vector < CProperties * > xprop_list;
    bool bRet = calendar->addXProperties(xprop_list, eventid,
                                         E_ADD, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add Xprop with Invalid pointer", (bRet == false));
}
void CCalendar_TS1 :: test_addParameters_invalid_arg()
{
    int error_code = -1;
    map < string, vector <CParameters *> > hashmap;
    bool bRet = calendar->addParameters(hashmap, eventid,
                                        E_ADD, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add Xparam with Invalid pointer", (bRet == false));
}
void CCalendar_TS1 :: test_modifyComponents()
{
    int error_code = -1;
    vector<CComponent *> compList;
    CJournal *pJournal = new CJournal("journal for testing modify components");
    compList.push_back(pJournal);
    calendar->modifyComponents(compList, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Code returned modifyComponents success", (error_code == CALENDAR_OPERATION_SUCCESSFUL));
}
void CCalendar_TS3 :: test_AllEmptyget()
{
    int error_code = -1;
    CCalendar *cal;
    cal = new CCalendar("TestCalendarTS3", COLOUR_RED, 0, 1, LOCAL_CALENDAR, "Music", "Version-1.0");
    int calidS2 = multi->addCalendar(cal, error_code);
    cout << calidS2;


    int Id = 1;
    string id;
    CCalendar *pCal = 0;
    pCal = multi->getCalendarById(Id, error_code);
    vector <CEvent *> vEvent;
    vector <CTodo *> vTodo;
    vector <CJournal *> vJournal;
    vector < CComponent * > ret;
    ret = pCal->getComponents(E_EVENT, -1, -1, 100, 0, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getComponents", (vEvent.size() == 0));
    vEvent = pCal->getEvents(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getEvents", (vEvent.size() == 0));
    vTodo = pCal->getTodos(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getTodos", (vTodo.size() == 0));
    vJournal = pCal->getJournals(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:getJournals", (vJournal.size() == 0));
    CTodo *pTodo = pCal->getTodo(id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting Todo", (pTodo == NULL));
    CEvent *event = pCal->getEvent(id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting event", (event == NULL));
    CEvent *pb = pCal->getBirthDayEvent(id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting Bday event", (pb == NULL));
}

