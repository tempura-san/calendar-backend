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
#include "test_MultiCalendar.h"
#include "Common.h"
#include "CalendarErrors.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CMulticalendar_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CMulticalendar_TS2);
CPPUNIT_TEST_SUITE_REGISTRATION(CMulticalendar_TS3);
/* GetComponentsTestCase*/
void CMulticalendar_TS1 :: setUp()
{
    multi = CMulticalendar::MCInstance();
}
void CMulticalendar_TS1 :: tearDown()
{
    //multi->MCdestroy();
}
void CMulticalendar_TS2 :: setUp()
{
    multi1 = CMulticalendar::MCInstance();
    pCdb1 = CCalendarDB::Instance();
}
void CMulticalendar_TS2 :: tearDown()
{
    pCdb1 = NULL;
    //multi1->MCdestroy();
}
void CMulticalendar_TS3 :: setUp()
{
    multi = CMulticalendar::MCInstance();
}
void CMulticalendar_TS3 :: tearDown()
{
    //multi->MCdestroy();
}


int icalid;
int InValidId = 999;
int error_code;
void CMulticalendar_TS1 :: test_addCalendar_Default()
{

    CCalendar *pC = new CCalendar("TestMulticalendar", COLOUR_RED, 1, 1, BIRTHDAY_CALENDAR, "SomeTune.xyz", "Version-1.0");
    icalid = multi->addCalendar(pC, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Add Calendar", (icalid > 0));
}


void CMulticalendar_TS1 :: test_addCalendar()
{
    /*In the following test case the first instance for the assertion of ical should be set
     * to >0 as the rollback is done after this step.
     * */
    CCalendar *pC = new CCalendar("TestMulticalendar-11", COLOUR_RED, 1, 1, LOCAL_CALENDAR, "SomeTune.xyz", "Version-1.0");

    int icalid3 = multi->addCalendar(pC, error_code);

    CPPUNIT_ASSERT_MESSAGE("Error: Add Calendar ", (icalid3 > 0));
}

void CMulticalendar_TS1 :: test_getCalendarByType_NoSyncCalendar()
{
    CCalendar *cal = multi->getCalendarByType(DEFAULT_SYNC, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getCalendarByType -default sync Calendar", (cal == NULL));
}
void CMulticalendar_TS1 :: test_getCalendarByType()
{
    CCalendar *cal = multi->getCalendarByType(LOCAL_CALENDAR, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getCalendarByType 0 Calendar", (cal != NULL));
}

void CMulticalendar_TS1 :: test_addCalendar1()
{
    CCalendar *pC = new CCalendar("TestMulticalendarBday", COLOUR_RED, 1, 1, BIRTHDAY_CALENDAR, "SomeTune.xyz", "Version-1.0");
    int icalid1 = multi->addCalendar(pC, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Add Calendar", (icalid1 > 0));
}

void CMulticalendar_TS1 :: test_getCalendarByType1()
{
    CCalendar *cal = multi->getCalendarByType(BIRTHDAY_CALENDAR, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getCalendarByType 1 Calendar", (cal != NULL));
}

void CMulticalendar_TS1 :: test_addCalendar2()
{
    CCalendar *pC = new CCalendar("TestMulticalendar2", COLOUR_RED, 1, 1, SYNC_CALENDAR, "SomeTune.xyz", "Version-1.0");
    int icalid2 = multi->addCalendar(pC, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Add Calendar", (icalid2 > 0));
}

void CMulticalendar_TS1 :: test_addCalendar_Invalid()
{
    /**The foolowing test case will be adding an existing
     * calendar the database and as expected it should fail
     * */
    CCalendar *pC = new CCalendar("TestMulticalendar", COLOUR_RED, 1, 1, BIRTHDAY_CALENDAR, "SomeTune.xyz", "Version-1.0");
    int icalid4 = multi->addCalendar(pC, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Add Calendar", (icalid4 <= 0));
}
void CMulticalendar_TS1 :: test_getCalendarByType2()
{
    CCalendar *cal = multi->getCalendarByType(SYNC_CALENDAR, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getCalendarByType 2 Calendar", (cal != NULL));
}

void CMulticalendar_TS1 :: test_getCalendarById()
{
    multi->getCalendarById(InValidId, error_code);
    CCalendar *cal = multi->getCalendarById(icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get Calendar by id", (cal != NULL));
}

void CMulticalendar_TS1 :: test_getCalendarByName_NoCalendars()
{
    CCalendar *cal = multi->getCalendarByName("TestMulticalendar", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get Calendar by id; no calendars in db", (cal == NULL));
}
void CMulticalendar_TS1 :: test_getCalendarByName()
{
    CCalendar *cal = multi->getCalendarByName("TestMulticalendar", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get Calendar by id", (cal != NULL));
    cal = multi->getCalendarByName("CALENDAR", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get Calendar by id", (cal == NULL));
}

void CMulticalendar_TS1 :: test_getNoofCalendars()
{
    int cal = multi->getNoofCalendars();
    CPPUNIT_ASSERT_MESSAGE("Error: get no of Calendars", (cal > 1));
}

void CMulticalendar_TS1 :: test_getNoteCount()
{
    multi->getNoteCount(InValidId, error_code);
    int cal = multi->getNoteCount(icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get no of notes", (cal == 0));
}

void CMulticalendar_TS1 :: test_getTodoCount()
{
    multi->getTodoCount(InValidId, error_code);
    int cal = multi->getTodoCount(icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get no of todos", (cal == 0));
}
void CMulticalendar_TS1 :: test_getEventCount()
{
    multi->getEventCount(InValidId, error_code);
    int cal = multi->getEventCount(icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get no of events", (cal == 0));
}
void CMulticalendar_TS1 :: test_getBirthDayEventCount()
{
    int pErrorCode = -1;
    CCalendar *cal = multi->getCalendarByType(BIRTHDAY_CALENDAR, pErrorCode);
    int calid = cal->getCalendarId();
    multi->getBdayEventCount(InValidId, error_code);
    int ret = multi->getBdayEventCount(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get no of events", (ret == 0));
}

void CMulticalendar_TS1 :: test_modifyCalendar()
{
    CCalendar *cal = multi->getCalendarByName("TestMulticalendar", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: calendar empty", cal != NULL);
    cal->setCalendarColor(COLOUR_BLUE);

    bool ret = multi->modifyCalendar(cal->getCalendarId() , "TestMulticalendar-edited", (CalendarColour)3, 0, 1, LOCAL_CALENDAR, "mp3", "Version-1.1", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify calendar", (ret == true));
}

void CMulticalendar_TS1 :: test_modifyCalendar_NULL()
{
    bool ret = multi->modifyCalendar(NULL, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify calendar", (ret == false));
}

string event_id;
void CMulticalendar_TS1 :: test_addEvent()
{
    string sLoc;
    CAlarm *pAlarm;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 50000;
    CEvent *pEvent = new CEvent("summary", "description",
                                "location", start + 10000, end);
    pAlarm  = new CAlarm(start + 10, 1);
    pEvent->setAlarm(pAlarm);
    CRecurrence *pRecur = new CRecurrence();
    vector <string> rules;
    rules.clear();
    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
    string rrule;

    vector <string> szrDateList;
    szrDateList.push_back("19970714T123000Z");
    szrDateList.push_back("19980714T123000Z");
    szrDateList.push_back("19990714T123000Z");
    pRecur->setRDays(szrDateList);
    vector <string> szeDateList;
    szeDateList.push_back("19970914T123000Z");
    szeDateList.push_back("19980814T123000Z");
    szeDateList.push_back("19990414T123000Z");
    pRecur->setEDays(szeDateList);
    vector<string> vErules;
    string szS = "FREQ=YEARLY;UNTIL=20000131T090000Z";
    vErules.push_back("FREQ=WEEKLY;COUNT=1;INTERVAL=1;BYDAY=TU,TH");
    pRecur->setErule(vErules);




    struct icaltimetype uical;
    int repeattype = 3;
    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pEvent->setAllDay(0);
    multi->addEvent(pEvent, InValidId, error_code);
    bool ret = multi->addEvent(pEvent, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));
    event_id = pEvent->getId();



}
void CMulticalendar_TS1 :: test_checkForNextPrev_noprev()
{
    int pErrorCode;
    bool res = multi->checkForNextPrev(time(NULL) - 10, event_id, 1, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: getting prev event", (res == 0));

}
void CMulticalendar_TS1 :: test_getCalendarIdForComponent()
{

    int retCalId = multi->getCalendarIdForComponent(event_id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting calendar id for component", (retCalId == icalid));
}

void CMulticalendar_TS1 :: test_getAllEventsTodos()
{
    time_t start, end;
    time(&start);
    end = start + 10500;
    multi->getAllEventsTodos(InValidId, start - 100, end + 100, 2, 0, error_code);
    vector<CComponent *> list = multi->getAllEventsTodos(icalid, start - 100, end + 100, 2, 0, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting all events and todos", list.size() > 0);
}
void CMulticalendar_TS1 :: test_addEvent1()
{
    string sLoc;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 50000;
    CEvent *pEvent = new CEvent("summary", "description",
                                "location", start + 10000, end);
    pEvent->setAllDay(1);
    multi->addEvent(pEvent, InValidId, error_code);
    int eventid = multi->addEvent(pEvent, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding all day", (eventid >= 0));
}
void CMulticalendar_TS1 :: test_getRecurrenceProperties()
{
    string sLoc;
    CAlarm *pAlarm;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 50000;
    CEvent *pEvent = new CEvent("summary-new", "description",
                                "location", start + 10000, end);
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
    int repeattype = 3;
    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pEvent->setAllDay(0);
    int eventid = multi->addEvent(pEvent, icalid, error_code);
    string id = pEvent->getId();
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (eventid >= 0));
    bool ret = pEvent->getRecurrenceProperties();
    CPPUNIT_ASSERT_MESSAGE("Error: getting getRecurrenceProperties", (ret == true));
    ret = multi->deleteEvent(icalid, id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: delete Event", (ret == true));
}

void CMulticalendar_TS1 :: test_modifyEvent1()
{
    string sLoc;
    CAlarm *pAlarm;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 50000;
    CEvent *pEvent = new CEvent("summary-new", "description",
                                "location", start + 10000, end);
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
    int repeattype = 3;
    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pEvent->setAllDay(0);
    multi->addEvent(pEvent, icalid, error_code);
    string eventid = pEvent->getId();
    pEvent->setSummary("Its a modified summary");
    multi->modifyEvent(pEvent, InValidId, error_code);
    bool ret = multi->modifyEvent(pEvent, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:modifying event", (ret == true));
    multi->deleteEvent(InValidId, eventid, error_code);
    bool ret1 = multi->deleteEvent(icalid, eventid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting event", (ret1 == true));
}

void CMulticalendar_TS1 :: test_modifyEvent()
{
    CCalendar *cal = multi->getCalendarById(icalid, error_code);
    CEvent *event = cal->getEvent(event_id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting event", (event != NULL));
    event->setAllDay(1);
    bool ret = multi->modifyEvent(event, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying event", (ret == true));
}

string todo_id;
void CMulticalendar_TS1 :: test_addTodo()
{
    time_t start;
    time(&start);
    CTodo *pTodo = new CTodo("summary", start + 1000, 0);
    multi->addTodo(pTodo, InValidId, error_code);
    bool ret = multi->addTodo(pTodo, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == true));
    todo_id = pTodo->getId();
}

void CMulticalendar_TS1 :: test_modifyTodo()
{
    CCalendar *cal = multi->getCalendarById(icalid, error_code);
    CTodo *pTodo = cal->getTodo(todo_id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting todo", (pTodo != NULL));
    pTodo->setStatus(1);

    multi->modifyTodo(pTodo, InValidId, error_code);
    bool ret = multi->modifyTodo(pTodo, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying todo", (ret == true));
}

string journal_id;
void CMulticalendar_TS1 :: test_addJournal()
{
    CJournal *pJournal = new CJournal("summary");

    multi->addJournal(pJournal, InValidId, error_code);
    bool ret = multi->addJournal(pJournal, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding journal", (ret == true));
    journal_id = pJournal->getId();
}

void CMulticalendar_TS1 :: test_modifyJournal()
{
    CCalendar *cal = multi->getCalendarById(icalid, error_code);
    CJournal *pJournal = cal->getJournal(journal_id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting journal", (pJournal != NULL));
    pJournal->setSummary("New Summary Modified");
    multi->modifyJournal(pJournal, InValidId, error_code);
    bool ret = multi->modifyJournal(pJournal, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modifying journal", (ret == true));
}

CBdayEvent bday;
void CMulticalendar_TS1::test_addBday()
{
    bool bRet;
    time_t utc_date;
    time_t until;

    utc_date = time(NULL);
    until = time(NULL) + (time_t) 86400 * 28 * 365;
    CBdayEvent *pBday = new CBdayEvent("abcder", "noname", utc_date);
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
    bRet = multi->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (bRet == true));
    bday = *pBday;
}

void CMulticalendar_TS1::test_modifyBirthDay()
{
    bool bRet;
    time_t utc_date;
    time_t until;

    utc_date = time(NULL);
    until = utc_date + (time_t) 86400 * 28 * 365;
    CBdayEvent *pBday = new CBdayEvent("abcder-new", "noname", utc_date + 86400);
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

    bRet = multi->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday ", (bRet == true));
    CAlarm *pAlarm = 0;
    pAlarm = new CAlarm(utc_date + 5000, 7);
    pBday->setAlarm(pAlarm);
    until = utc_date + (time_t) 86400 * 30 * 365;
    pBday->setUntil(until);
    bRet = multi->modifyBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: modify birthday 1", (bRet == true));
}

void CMulticalendar_TS1::test_deleteBirthDay()
{
    bool bRet = multi->deleteBirthDay(bday.getId(), error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleting birthday", (bRet == true));
}

void CMulticalendar_TS1 :: test_getNoteCount1()
{
    multi->getNoteCount(InValidId, error_code);
    int cal = multi->getNoteCount(icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get no of notes", (cal == 1));
}

void CMulticalendar_TS1 :: test_getTodoCount1()
{
    multi->getTodoCount(InValidId, error_code);
    int cal = multi->getTodoCount(icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get no of todos", (cal == 1));
}
void CMulticalendar_TS1 :: test_getEventCount1()
{
    multi->getEventCount(InValidId, error_code);
    int cal = multi->getEventCount(icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get no of events", (cal >= 1));
}
void CMulticalendar_TS1 :: test_getBirthDayEventCount1()
{
    int pErrorCode = -1;
    CCalendar *cal = multi->getCalendarByType(BIRTHDAY_CALENDAR, pErrorCode);
    int calid = cal->getCalendarId();
    multi->getBdayEventCount(InValidId, error_code);
    int ret = multi->getBdayEventCount(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get no of events", (ret >= 0));
}


void CMulticalendar_TS1 :: test_getComponents()
{
    CEvent *pEvent = NULL, *pEvent1 = NULL;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 50000;
    pEvent1 = new CEvent("EventMulti1", "description",
                         "location", start + 10000, end);
    multi->addEvent(pEvent, icalid, error_code);
    vector < CComponent * > ret1 =  multi->getComponents(10000, 1, -1, -1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get all components items from invalid calendar id", (ret1.size() == 0));
    vector < CComponent * > ret2 =  multi->getComponents(10000, 1, -1, -1, 5, 0, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get all components items limit from invalid calendar id", (ret2.size() == 0));
    vector < CComponent * > ret3 =  multi->getComponents(icalid, 1, -1, -1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get all components items", (ret3.size() > 0));
    vector < CComponent * > ret4 =  multi->getComponents(icalid, 1, -1, -1, 5, 0, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get all components items limit", (ret4.size() > 0));
}

void CMulticalendar_TS1 :: test_getListCalFromMc()
{
    vector <CCalendar *> cals = multi->getListCalFromMc();
    CPPUNIT_ASSERT_MESSAGE("Error: get calendars list", (cals.size() > 0));
    multi->releaseListCalendars(cals);
}

void CMulticalendar_TS1 :: test_deleteEvent()
{
    bool ret = multi->deleteEvent(icalid, event_id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting event", (ret == true));
}

void CMulticalendar_TS1 :: test_deleteTodo()
{
    multi->deleteTodo(InValidId, todo_id, error_code);
    bool ret = multi->deleteTodo(icalid, todo_id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting todo", (ret == true));
}

void CMulticalendar_TS1 :: test_deleteJournal()
{
    multi->deleteJournal(InValidId, journal_id, error_code);
    bool ret = multi->deleteJournal(icalid, journal_id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:deleting journal", (ret == true));
}

void  CMulticalendar_TS1 :: test_getCalendarByIdInvalid()
{
    CCalendar *pCaltest = multi->getCalendarById(-1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error : getting wrong calendar instance ", (pCaltest == NULL));
}

void CMulticalendar_TS1 :: test_deleteCalendar()
{
    CCalendar *pC = new CCalendar("TestMulticalendarBday11", COLOUR_RED, 0, 1, BIRTHDAY_CALENDAR, "SomeTune1.xyz", "Version-1.0");
    icalid = multi->addCalendar(pC, error_code);
    bool ret = multi->deleteCalendar(pC->getCalendarId(), error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: delete calendar", ret == true);
}
void CMulticalendar_TS1 :: test_deleteCalendar_NULL()
{
    bool ret = multi->deleteCalendar(-1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: delete calendar null", ret == false);
}

void CMulticalendar_TS1 :: test_importIcsFileData()
{
    string id ;
    int events;
    int todos;
    int journals;
    bool ret = multi->importIcsFileData("Invalid.ics", 8, id, todos, events, journals, error_code);
    ret = multi->importIcsFileData("ical.ics", 8, id, todos, events, journals, error_code);
    bool retv = multi->exportCalendarEntries(8, "exportical.ics", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error:importIcsFileData ", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error : exporting calendar entries error ", (retv == true));
}

void CMulticalendar_TS1 :: test_deleteBeyond()
{
    string sLoc;
    time_t start, end;
    time(&start);
    end = start - 80000;
    icalid = 8;
    CEvent *pEvent = new CEvent("summary", "description",
                                "location", start - 86400, end);
    pEvent->setAllDay(1);
    int eventid = multi->addEvent(pEvent, icalid, error_code);
    CTodo *pTodo = new CTodo("summary", start - 86400, 1);
    bool retv = multi->addTodo(pTodo, icalid, error_code);
    CJournal *pJournal = new CJournal("summary");
    journal_id = multi->addJournal(pJournal, icalid, error_code);
    multi->deleteEventBeyond(InValidId, start - 20000, error_code);
    multi->deleteTodosBeyond(InValidId, start - 20000, error_code);
    multi->deleteJournalsBeyond(InValidId, start - 20000, error_code);
    bool ret = multi->deleteEventBeyond(icalid, start - 20000, error_code);
    bool ret1 = multi->deleteTodosBeyond(icalid, start - 20000, error_code);
    bool ret2 = multi->deleteJournalsBeyond(icalid, start - 20000, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleting event", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: deleting todo", (ret1 == true));
    CPPUNIT_ASSERT_MESSAGE("Error: deleting journals", (ret2 == true));
    ret = multi->deleteEventBeyond(icalid, start - 20000, error_code);
    ret1 = multi->deleteTodosBeyond(icalid, start - 20000, error_code);
    ret2 = multi->deleteJournalsBeyond(icalid, start - 20000, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: check if deleted deleting event", (ret == false));
    CPPUNIT_ASSERT_MESSAGE("Error: check if deleted deleting todo", (ret == false));
    CPPUNIT_ASSERT_MESSAGE("Error: check if deleted deleting journals", (ret1 == false));

    pEvent = new CEvent("summary", "description",
                        "location", start - 86400, end);
    eventid = multi->addEvent(pEvent, icalid, error_code);
    pTodo = new CTodo("summary", start - 86400, 1);
    retv = multi->addTodo(pTodo, icalid, error_code);
    ret = multi->deleteEventAndTodoBeyond(icalid, start - 20000, 1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleting event and todo (events)", (ret == true));
    ret = multi->deleteEventAndTodoBeyond(icalid, start - 20000, 2, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleting event and todo", (ret == true));
    ret = multi->deleteEventAndTodoBeyond(icalid, start - 20000, 3, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: deleting event and todo", (ret == false));
}

void CMulticalendar_TS1 :: test_automaticDeletion()
{
    time_t start, end;
    int ilimit = 0;
    bool ret = false;
    time(&start);
    end = start - 89000;
    multi->enableAutomaticDeletion(365, 5, ilimit, error_code);
    CEvent *pEvent = new CEvent("Multiday", "description",
                                "location", start - 90000, start - 60000);
    pEvent->setAllDay(1);
    int eventid = multi->addEvent(pEvent, icalid, error_code);

    CEvent *pEvent1 = new CEvent("AutoDeletion1", "description",
                                 "location", start - 86400 * 2, start - 86400 * 2 + 3000);
    eventid = multi->addEvent(pEvent1, icalid, error_code);

    multi->enableAutomaticDeletion(2, 5, ilimit, error_code);
    CEvent *pEvent5 = new CEvent("iE1", "description",
                                 "location", start - 90000, start - 60000);
    pEvent->setAllDay(1);
    eventid = multi->addEvent(pEvent5, icalid, error_code);

    CEvent *pEvent6 = new CEvent("AutoDeletion5", "description",
                                 "location", start - 86400 * 2, start - 86400 * 2 + 3000);
    eventid = multi->addEvent(pEvent6, icalid, error_code);
    multi->enableAutomaticDeletion(2, 1, ilimit, error_code);

    CEvent *pEvent2 = new CEvent("AutoDeletion", "description",
                                 "location", start - 86400 * 3, start + 25000);
    eventid = multi->addEvent(pEvent2, icalid, error_code);

    CTodo *pTodo = new CTodo("summary", start - 90000, 1);
    todo_id = multi->addTodo(pTodo, icalid, error_code);
    CJournal *pJournal = new CJournal("summary");
    journal_id = multi->addJournal(pJournal, icalid, error_code);

    CEvent *pEvent3 = new CEvent("AutoDel-AllDay", "description",
                                 "location", start - 210100, end);
    time_t until = 23400001;
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
    pEvent3->setAllDay(1);
    pEvent3->setRtype(repeattype);
    pEvent3->setUntil(until);
    pRecur->setRrule(rules);
    pEvent3->setRecurrence(pRecur);
    multi->addEvent(pEvent3, icalid, error_code);

    CEvent *pEvent4 = new CEvent("AutoDel-AllDay-Rec", "description",
                                 "location", start - 21560, end);
    until = time_t(2340000);
    CRecurrence *pRecur1 = new CRecurrence();
    rules.clear();
    repeattype = 4;
    uical = icaltime_from_timet(until, 0);
    ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent4->setAllDay(1);
    pEvent4->setRtype(repeattype);
    pEvent4->setUntil(until);
    pRecur1->setRrule(rules);
    pEvent4->setRecurrence(pRecur1);
    multi->addEvent(pEvent4, icalid, error_code);


    ret = multi->enableAutomaticDeletion(1, 4, ilimit, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: automatic deletion", (ret == true));
    ret = multi->enableAutomaticDeletion(1, 50, ilimit, error_code);
}

void CMulticalendar_TS1 :: test_getLatestComponent()
{
    string sLoc;
    time_t start, end, until;
    time(&start);
    int offset = 0;
    end = start + 7200;
    until = start + 86400 * 365 * 15;
    CEvent *pEvent = new CEvent("GetLatestComponent", "description",
                                "location", start + 360, end);
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
    pEvent->setAllDay(0);
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    multi->addEvent(pEvent, icalid, error_code);

    CTodo *pTodo = new CTodo("summary", start + 86400, 0);
    multi->addTodo(pTodo, icalid, error_code);


    CEvent *eventr = (CEvent *) multi->getLatestComponent(E_EVENT, start, offset, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getLatestComponent", (eventr != NULL));
    CTodo *pTodo1 = (CTodo *)multi->getLatestComponent(E_TODO, start, offset, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getLatestComponent", (pTodo1 != NULL));
    multi->getLatestComponent(3, start, offset, error_code);

}

void CMulticalendar_TS1 :: test_getAlarmProperties()
{
    CAlarm *pAlarm;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 234500;
    CEvent *pEvent = new CEvent("Rec-Alarm Event SetNextAlarm", "description",
                                "location", start + 10000, end);
    pAlarm  = new CAlarm(start + 10, 1);
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
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pEvent->setAllDay(0);
    bool ret = multi->addEvent(pEvent, icalid, error_code);

    start = time_get_time();
    struct tm *pTime;
    pTime = localtime(&start);
    pTime->tm_hour = 0;
    pTime->tm_min = 0;
    pTime->tm_sec = 0;
    start = time_mktime(pTime, 0);
    start = start + 86400;
    until = start + 234500;

    end = start + 86399;
    CEvent *pEvent1 = new CEvent("AllDay Alarm", "description",
                                 "location", start, end);

    pEvent1->setRtype(repeattype);
    pEvent1->setUntil(until);
    pEvent1->setRecurrence(pRecur);

    CAlarm *pAlarm1  = new CAlarm(start - 200, 7);
    pEvent1->setAlarm(pAlarm1);
    pEvent1->setAllDay(1);
    pEvent1->setFlags(5);
    multi->addEvent(pEvent1, icalid, error_code);

    CTodo *pTodo = new CTodo("TodoSetNextAlarm", start, 0);
    CAlarm *pAlarm2  = new CAlarm(start + 2000, 7);
    pTodo->setAlarm(pAlarm2);
    pTodo->setFlags(3);
    pTodo->setUntil(until);
    multi->addTodo(pTodo, icalid, error_code);


    time_t utc_date = time(NULL);
    until = time(NULL) + (time_t) 86400 * 28 * 365;
    CBdayEvent *pBday = new CBdayEvent("Bday", "NewBday", utc_date);
    rules.clear();
    repeattype = 5;
    uical = icaltime_from_timet(until, 0);
    ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pBday->setRtype(5);
    pBday->setUntil(1393200);
    pRecur->setRrule(rules);
    pBday->setRecurrence(pRecur);
    bool bRet = multi->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (bRet == true));




    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));
    bool ret1 = pEvent->getAlarmProperties();
    multi->setNextAlarm(InValidId, pEvent->getId(), 1, error_code);
    bool ret2 = multi->setNextAlarm(icalid, pEvent->getId(), 1, error_code);
    bool ret3 = multi->setNextAlarm(icalid, pTodo->getId(), 2, error_code);
    bool ret4 = multi->setNextAlarm(icalid, pEvent1->getId(), 1, error_code);
    bool ret5 = multi->setNextAlarm(icalid, pBday->getId(), 4, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getLatestComponent alarm properties", (ret1 == true));
    CPPUNIT_ASSERT_MESSAGE("Error: setNextAlarm", (ret2 == true));
    CPPUNIT_ASSERT_MESSAGE("Error: setNextAlarm Todo", (ret3 == true));
    CPPUNIT_ASSERT_MESSAGE("Error: setNextAlarm Allday", (ret4 == true));
    CPPUNIT_ASSERT_MESSAGE("Error: setNextAlarm BDay", (ret5 == true));
}

void CMulticalendar_TS1 :: test_getNumOfAllDayEventsForToday()
{
    string sLoc;
    time_t start, end;
    time(&start);
    end = start + 500;
    CEvent *pEvent = new CEvent("summaryA", "description",
                                "location", start, end);
    pEvent->setAllDay(1);
    multi->addEvent(pEvent, icalid, error_code);
    int ret = multi->getNumOfAllDayEventsForToday(error_code);

    CPPUNIT_ASSERT_MESSAGE("Error: getNumOfAllDayEventsForToday", (ret > 0));
}

void CMulticalendar_TS1 :: test_getNumOfUndoneTasks()
{
    time_t start;
    time(&start);
    int calid = icalid;
    CTodo *pTodo = new CTodo("UndoneTask", start, 0);
    pTodo->setStatus(0);
    multi->addTodo(pTodo, icalid, error_code);
    int ret = multi->getNumOfUndoneTasks(&calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getNumOfUndoneTasks", (ret > 0));
}

void CMulticalendar_TS1 :: test_getTodaysAllDay()
{
    string sLoc;
    time_t start = time_get_time();
    time_t end;
    struct tm *pTime;
    pTime = localtime(&start);
    pTime->tm_hour = 0;
    pTime->tm_min = 0;
    pTime->tm_sec = 0;
    start = time_mktime(pTime, 0);
    end = start + 86399;
    CEvent *pEvent = new CEvent("AllDay", "description",
                                "location", start, end);

    pEvent->setAllDay(1);
    multi->addEvent(pEvent, icalid, error_code);
    vector <CComponent *> ret = multi->getTodaysAllDay(error_code);

    CPPUNIT_ASSERT_MESSAGE("Error: getNumOfAllDayEventsForToday", (ret.size() > 0));
}

void CMulticalendar_TS1 :: test_CtCap()
{
    string ctcap = multi->getCTCapXML();
    CPPUNIT_ASSERT_MESSAGE("Error:getCTCapXML", (ctcap.size() > 0));
    multi->setApplicationName("Some name");
    CPPUNIT_ASSERT_MESSAGE("Error: getApplicationName", multi->getApplicationName() == string("Some name"));
}

void CMulticalendar_TS1 :: test_getICSFileInfo()
{
    unsigned int iEventCount;
    unsigned int iTodoCount;
    unsigned int iJournalCount;
    int pErrorCode;
    bool ret = multi->getICSFileInfo("ical.ics", iEventCount, iTodoCount, iJournalCount , pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error:getICSFileInfo", (ret == true));
    ret = multi->getICSFileInfo("file:///home/parivesh/MyDocs/ical1.ics", iEventCount, iTodoCount, iJournalCount , pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error:getICSFileInfo", (ret == false));
    ret = multi->getICSFileInfo("ical2.ics", iEventCount, iTodoCount, iJournalCount , pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error:getICSFileInfo", (ret == false));
    ret = multi->getICSFileInfo("ical3.ics", iEventCount, iTodoCount, iJournalCount , pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error:getICSFileInfo", (ret == false));
    ret = multi->getICSFileInfo("ical4.ics", iEventCount, iTodoCount, iJournalCount , pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error:getICSFileInfo", (ret == false));
    ret = multi->getICSFileInfo("Invalid2.ics", iEventCount, iTodoCount, iJournalCount , pErrorCode);
    ret = multi->getICSFileInfo("Invalid3.ics", iEventCount, iTodoCount, iJournalCount , pErrorCode);
}

void CMulticalendar_TS1 :: test_getComponentsAllCalendars_Events()
{
    int pErrorCode;
    time_t start = time_get_time();
    time_t end = start + 1800;
    CEvent *pEvent = new CEvent("Recurring Event", "description",
                                "location", start + 10000, end);
    CAlarm *pAlarm  = new CAlarm(start + 10, 1);
    pEvent->setAlarm(pAlarm);
    vector<int> vCalId;
    vector<CComponent *> compList = multi->getComponentsAllCalendars(-1, -1, 100, 0, vCalId, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: getComponentsAllCalendars", (compList.size() >= 0));

}

void CMulticalendar_TS1 :: test_getComponentsAllCalendars_Events2()
{
    int pErrorCode;
    time_t start = time_get_time();
    time_t end = start + 1800;
    CEvent *pEvent = new CEvent("Recurring Event", "description",
                                "location", start + 10000, end);
    CAlarm *pAlarm  = new CAlarm(start + 10, 1);
    pEvent->setAlarm(pAlarm);
    vector<int> vCalId;
    vector<CComponent *> compList;

    CPPUNIT_ASSERT(multi->getComponentsAllCalendars(-1, -1, 100, 0, compList) == CALENDAR_OPERATION_SUCCESSFUL);
    CPPUNIT_ASSERT(compList.size() >= 0);

    CPPUNIT_ASSERT(multi->getComponentsAllCalendars(-1, -1, 100, 100, compList) == CALENDAR_FETCH_NOITEMS);

    CPPUNIT_ASSERT(compList.size() == 0);
}
void CMulticalendar_TS1 :: test_addCalendar_AllParameters()
{
    int pErrorCode;
    CCalendar *retcal = multi->addCalendar("Calendar_One", COLOUR_RED, 0, 1, LOCAL_CALENDAR, "Tune", "1.0", pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: addCalendar With all parameters", (retcal != NULL));

}
void CMulticalendar_TS1 :: test_addCalendar_NULL()
{
    int pErrorCode;
    int ret = multi->addCalendar(NULL, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: addCalendar_NULL", (!(ret > 0)));
}
void CMulticalendar_TS1 :: test_addCalendar_AlreadyExist()
{
    int pErrorCode;

    CCalendar *pC = new CCalendar("TestMulticalendar", COLOUR_RED, 1, 1, LOCAL_CALENDAR, "SomeTune.xyz", "Version-1.0");
    int ret = multi->addCalendar(pC, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: Add Calendar", (!(ret > 0)));
}
void CMulticalendar_TS1 :: test_addEvent_NULL()
{
    int pErrorCode;
    bool ret = multi->addEvent(NULL, icalid, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_addEvent_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_addEvent_NoCalendar()
{
    int pErrorCode;
    time_t start, end;
    time(&start);
    end = start + 10500;

    CEvent *pEvent =  new CEvent("Event100", "Desc1000", "Loc1000", start, end);
    bool ret = multi->addEvent(pEvent, 1000,  pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_addEvent_NoCalendar", (ret == false));
}
void CMulticalendar_TS1 :: test_modifyEvent_NULL()
{
    int pErrorCode;
    bool ret = multi->modifyEvent(NULL, icalid, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyEvent_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_modifyEvent_NoCalendar()
{
    int pErrorCode;
    CEvent *pEvent =  new CEvent("Event100", "Desc1000", "Loc1000", 100, 100);
    bool ret = multi->modifyEvent(pEvent, 1000, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyEvent_NoCalendar", (ret == false));
}

void CMulticalendar_TS1 :: test_deleteEvent_NULL()
{
    int pErrorCode;
    bool ret = multi->deleteEvent(icalid, "", pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteEvent_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_deleteTodo_NULL()
{
    int pErrorCode;
    bool ret = multi->deleteTodo(icalid, "", pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteTodo_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_deleteJournal_NULL()
{
    int pErrorCode;
    bool ret = multi->deleteJournal(icalid, "", pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteJournal_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_addTodo_NULL()
{
    int pErrorCode;
    bool ret = multi->addTodo(NULL, icalid, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_addTodo_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_addTodo_NoCalendar()
{
    int pErrorCode;
    time_t start;
    time(&start);

    CTodo *pTodo =  new CTodo("Todo100", start, 1);
    bool ret = multi->addTodo(pTodo, 1000,  pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_addTodo_NoCalendar", (ret == false));
}
void CMulticalendar_TS1 :: test_modifyTodo_NULL()
{
    int pErrorCode;
    bool ret = multi->modifyTodo(NULL, icalid, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyTodo_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_modifyTodo_NoCalendar()
{
    int pErrorCode;
    CTodo *pTodo =  new CTodo("Todo100", 100, 1);
    bool ret = multi->modifyTodo(pTodo, 1000, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyEvent_NoCalendar", (ret == false));
}
void CMulticalendar_TS1 :: test_addJournal_NULL()
{
    int pErrorCode;
    bool ret = multi->addJournal(NULL, icalid, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_addJournal_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_addJournal_NoCalendar()
{
    int pErrorCode;
    CJournal *pJournal = new CJournal("J123");
    bool ret = multi->addJournal(pJournal, 1000,  pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_addJournal_NoCalendar", (ret == false));
}
void CMulticalendar_TS1 :: test_modifyJournal_NULL()
{
    int pErrorCode;
    bool ret = multi->modifyJournal(NULL, icalid, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyTodo_NULL", (ret == false));
}
void CMulticalendar_TS1 :: test_modifyJournal_NoCalendar()
{
    int pErrorCode;
    CJournal *pJournal = new CJournal("J123");
    bool ret = multi->modifyJournal(pJournal, 1000, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyJournal_NoCalendar", (ret == false));
}
void CMulticalendar_TS1 :: test_deleteEvent_NoCalendar()
{
    int pErrorCode;
    bool ret = multi->deleteEvent(1000, "",  pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteEvent_NoCalendar", (ret == false));
}
void CMulticalendar_TS1 :: test_deleteJournal_NoCalendar()
{
    int pErrorCode;
    bool ret = multi->deleteJournal(1000, "", pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteJournal_NoCalendar", (ret == false));
}
void CMulticalendar_TS1 :: test_deleteTodo_NoCalendar()
{
    int pErrorCode;
    bool ret = multi->deleteTodo(1000, "", pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteTodo_NoCalendar", (ret == false));
}

void CMulticalendar_TS1 :: test_addBday_NoCalendar()
{
    bool bRet;
    CBdayEvent *pBday = new CBdayEvent("bday", "noname", time(NULL));
    bRet = multi->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_addBday_NoCalendar", (bRet == false));
}
void CMulticalendar_TS1 :: test_modifyBday_NoCalendar()
{
    bool bRet;
    bRet = multi->addBirthDay(NULL, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyBday_NoCalendar", (bRet == false));
    CBdayEvent *pBday = new CBdayEvent("bday", "noname", time(NULL));
    bRet = multi->modifyBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyBday_NoCalendar", (bRet == false));
}
void CMulticalendar_TS1 :: test_deleteBday_NoCalendar()
{
    bool bRet;
    bRet = multi->deleteBirthDay("ade", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteBday_NoCalendar", (bRet == false));
}
void CMulticalendar_TS1 :: test_addBday_NULL()
{
    bool bRet;
    bRet = multi->addBirthDay(NULL, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_addBday_NULL", (bRet == false));
}
void CMulticalendar_TS1 :: test_modifyBday_NULL()
{
    bool bRet;
    bRet = multi->modifyBirthDay(NULL, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_modifyBday_NULL", (bRet == false));
}

void CMulticalendar_TS1 :: test_getBirthdayCalendar()
{
    int pErrorCode = -1;
    CCalendar *pCal =  new CCalendar("smart calendar", COLOUR_RED,
                                     0, 1, BIRTHDAY_CALENDAR, "", "1.0");
    icalid = multi->addCalendar(pCal, pErrorCode);
    CCalendar *cal = multi->getBirthdayCalendar();
    CPPUNIT_ASSERT_MESSAGE("Error: getBirthdayCalendar", (cal != NULL));
}

void CMulticalendar_TS1 :: test_getSynchronizedCalendar()
{
    int pErrorCode = -1;

    CCalendar *pCal =  new CCalendar("!!Synchronized", COLOUR_RED,
                                     0, 1, DEFAULT_SYNC, "", "1.0");
    multi->addCalendar(pCal, pErrorCode);

    CCalendar *cal = multi->getSynchronizedCalendar();
    CPPUNIT_ASSERT_MESSAGE("Error: getSynchronizedCalendar", (cal != NULL));
}
void CMulticalendar_TS1 :: test_getDefaultCalendar()
{
    int pErrorCode = -1;
    CCalendar *pCal =  new CCalendar("!!Private", COLOUR_GREEN,
                                     0, 1, DEFAULT_PRIVATE, "", "1.0");
    multi->addCalendar(pCal, pErrorCode);

    CCalendar *cal = multi->getDefaultCalendar();
    CPPUNIT_ASSERT_MESSAGE("Error: getDefaultCalendar", (cal != NULL));
}
void CMulticalendar_TS1 :: test_getSynchronizedCalendarName()
{
    string name = multi->getSynchronizedCalendarName();
    CPPUNIT_ASSERT_MESSAGE("Error: getSynchronizedCalendarName", (strcmp(name.c_str(), "!!Synchronized") == 0));

}
void CMulticalendar_TS1 :: test_getDefaultCalendarName()
{
    string name = multi->getDefaultCalendarName();
    CPPUNIT_ASSERT_MESSAGE("Error: getDefaultCalendarName", (strcmp(name.c_str(), "!!Private") == 0));
}
void CMulticalendar_TS1 :: test_deleteBirthdaysForCalendar()
{
    bool bRet;
    time_t utc_date;
    time_t until;

    utc_date = time(NULL);
    until = time(NULL) + (time_t) 86400 * 28 * 365;
    CBdayEvent *pBday = new CBdayEvent("abcder", "noname", utc_date);
    bRet = multi->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (bRet == true));
    CCalendar *cal = multi->getCalendarByType(BIRTHDAY_CALENDAR, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getCalendarByType", (cal != NULL));
    int calid = cal->getCalendarId();
    CPPUNIT_ASSERT_MESSAGE("Error: getCalendarId", calid > 0);
    bool ret = multi->deleteBirthdaysForCalendar(calid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteBirthdaysForCalendar", (ret == true));
    ret = multi->deleteBirthdaysForCalendar(-1, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: test_deleteBirthdaysForCalendar", (ret == false));

}
void CMulticalendar_TS1 :: test_getPrevNextComponent()
{
    time_t start;
    time_t instance_time = 0;
    int ilimit;
    int error_code;
    time(&start);
    multi->enableAutomaticDeletion(1, 50, ilimit, error_code);
    CCalendar *pC = new CCalendar("Calendar-Two", COLOUR_RED, 1, 1, LOCAL_CALENDAR, "SomeTune.xyz", "Version-1.0");
    icalid = multi->addCalendar(pC, error_code);
    delete pC;
    pC = 0;

    CPPUNIT_ASSERT_MESSAGE("Error: addCalendar", icalid > 0);

    CEvent *pEvent1 = new CEvent("event1", "description",
                                 "location", start + 1000, start + 1000 + 2000);
    string Id1 = pEvent1->getId();
    CEvent *pEvent2 = new CEvent("event2", "description",
                                 "location", start + 1120, start + 1120 + 2000);
    CEvent *pEvent3 = new CEvent("event3", "description",
                                 "location", start + 1300, start + 1300 + 2000);
    string Id2;
    multi->addEvent(pEvent1, icalid, error_code);
    multi->addEvent(pEvent2, icalid, error_code);
    multi->addEvent(pEvent3, icalid, error_code);

    Id2 = pEvent2->getId();
    delete pEvent1;
    delete pEvent2;
    delete pEvent3;


    CComponent *comp = multi->getPrevNextComponent(Id2, start + 1120, true, icalid, instance_time, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getPrevNextComponent", comp != NULL);
    CComponent *compNext = multi->getPrevNextComponent(Id2, start + 1120, false, icalid, instance_time, error_code);

    CAlarm *pAlarm;
    time_t end, until;
    end = start + 3600;
    until = -1;
    CEvent *pEvent = new CEvent("summaryCount", "description",
                                "location", start + 1060, end);
    pAlarm  = new CAlarm(start + 10, 1);
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
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pEvent->setAllDay(0);
    bool ret = multi->addEvent(pEvent, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));
    event_id = pEvent->getId();
    compNext = multi->getPrevNextComponent(event_id, start + 1060, false, icalid, instance_time, error_code);

    end = start + 3600;
    until = -1;
    CEvent *pEvent6 = new CEvent("summaryCount", "description",
                                 "location", start + 900, end);
    rules.clear();
    repeattype = 4;
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent6->setRtype(repeattype);
    pEvent6->setUntil(until);
    pRecur->setRrule(rules);
    pEvent6->setRecurrence(pRecur);
    pEvent6->setAllDay(0);
    ret = multi->addEvent(pEvent6, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));
    comp = multi->getPrevNextComponent(Id1, start + 1000, true, icalid, instance_time, error_code);


    start = start + 86400 * 5;
    pEvent1 = new CEvent("event11", "description",
                         "location", start + 1000, start + 1000 + 2000);
    Id1 = pEvent1->getId();
    pEvent2 = new CEvent("event2", "description",
                         "location", start + 3000 + 1120, start + 3000 + 1120 + 2000);
    pEvent3 = new CEvent("event3", "description",
                         "location", start + 7000 + 1300, start + 7000 + 1300 + 2000);
    rules.clear();
    repeattype = 1;
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent1->setRtype(repeattype);
    pEvent1->setUntil(start + 86400 * 10);
    pRecur->setRrule(rules);
    pEvent1->setRecurrence(pRecur);
    pEvent1->setAllDay(0);

    rules.clear();
    repeattype = 1;
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent2->setRtype(repeattype);
    pEvent2->setUntil(start + 86400 * 10);
    pRecur->setRrule(rules);
    pEvent2->setRecurrence(pRecur);
    pEvent->setAllDay(0);

    rules.clear();
    repeattype = 1;
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent3->setRtype(repeattype);
    pEvent3->setUntil(start + 86400 * 10);
    pRecur->setRrule(rules);
    pEvent3->setRecurrence(pRecur);
    pEvent3->setAllDay(0);


    multi->addEvent(pEvent1, icalid, error_code);
    multi->addEvent(pEvent2, icalid, error_code);
    multi->addEvent(pEvent3, icalid, error_code);

    Id2.clear();
    Id2 = pEvent2->getId();
    delete pEvent1;
    delete pEvent2;
    delete pEvent3;


    multi->enableAutomaticDeletion(1, 500, ilimit, error_code);

    comp = multi->getPrevNextComponent("1111", start + 1120, true, icalid, instance_time, error_code);
    comp = multi->getPrevNextComponent(Id2, start + 1120, true, icalid, instance_time, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getPrevNextComponent", comp != NULL);
    compNext = multi->getPrevNextComponent(Id2, start + 1120, false, icalid, instance_time, error_code);




}

void CMulticalendar_TS1 ::test_getCalendar_NoCalendar()
{
    int error_code = -1;
    CCalendar *cal = multi->getCalendarByType(DEFAULT_SYNC, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting default sync calendar", cal == NULL);
    cal = multi->getCalendarByName("Calendar", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting calendar by name", cal == NULL);
    cal = multi->getCalendarById(10, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getting calendar by id", cal == NULL);
}

void CMulticalendar_TS1 :: test_delBirthdayCalendar()
{

    CCalendar *cal11 = multi->getBirthdayCalendar();
    CPPUNIT_ASSERT_MESSAGE("Error: getBirthdayCalendar", (cal11 != NULL));
    int id = cal11->getCalendarId();
    bool ret = multi->deleteCalendar(id, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: delete calendar", ret == true);
}
void CMulticalendar_TS1 :: test_printCurrentTime()
{
    multi->printCurrentTime();
}

vector <string> duplicate;
void CMulticalendar_TS1 :: test_addComponents()
{

    vector<CComponent *> compList;
    vector <string> dupl;
    int pErrorCode = -1;
    time_t start, end;
    time(&start);
    end = start + 150000;
    multi->addComponents(compList, 300, dupl, pErrorCode);
    CTodo *pTodo = new CTodo("summary", start + 1000, 0);
    bool ret = multi->addTodo(pTodo, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == true));
    compList.push_back(pTodo);
    todo_id = pTodo->getId();
    duplicate.push_back(todo_id);
    CTodo *pTodo1 = new CTodo("summaryZ", start + 1000, 0);
    ret = multi->addTodo(pTodo1, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == true));
    compList.push_back(pTodo1);
    todo_id = pTodo1->getId();
    duplicate.push_back(todo_id);

    multi->addComponents(compList, InValidId, duplicate, pErrorCode);
    multi->addComponents(compList, icalid, duplicate, pErrorCode);


}

void CMulticalendar_TS1 :: test_modifyComponents()
{

    vector<CComponent *> compList;
    int pErrorCode = -1;
    time_t start, end;
    time(&start);
    end = start + 150000;
    multi->modifyComponents(compList, 300, pErrorCode);
    CTodo *pTodo = new CTodo("summaryQ", start + 1002, 0);
    bool ret = multi->addTodo(pTodo, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == true));
    compList.push_back(pTodo);
    CTodo *pTodo1 = new CTodo("summaryZX", start + 100, 0);
    ret = multi->addTodo(pTodo1, icalid, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding todo", (ret == true));
    compList.push_back(pTodo1);
    multi->modifyComponents(compList, InValidId, pErrorCode);
    multi->modifyComponents(compList, icalid, pErrorCode);


}

void CMulticalendar_TS1 :: test_deleteComponents()
{
    vector<CComponent *> compList;
    int pErrorCode = -1;
    vector <string> dup;
    multi->deleteComponents(dup, 300, pErrorCode);
    multi->deleteComponents(duplicate, icalid, pErrorCode);

}

void CMulticalendar_TS1 :: test_noVisibleCal()
{
    int pErrorCode = -1;
    int count;
    count = multi->getVisibleCalendarCount(pErrorCode) ;

}
void CMulticalendar_TS1 :: test_moveFileFromSkel()
{
    string szoldpath = "Invalid.ics";
    string sznewpath = "x.ics";
    bool ret = false;
    ret = multi->moveFileFromSkel(szoldpath, sznewpath);
    CPPUNIT_ASSERT_MESSAGE("Error: Unable to copy the file ", (ret == true));
}

void CMulticalendar_TS1 :: test_checkFileExists()
{
    string szInvalid = "Invalid.ics";
    bool ret = false;

    ret = multi->checkFileExists(szInvalid);
    CPPUNIT_ASSERT_MESSAGE("Error: DB found", (ret == true));
}

void CMulticalendar_TS1 :: test_getComponentCount()
{
    int pErrorCode;
    CAlarm *pAlarm;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 50000;
    CEvent *pEvent = new CEvent("summaryCount", "description",
                                "location", start + 10000, end);
    pAlarm  = new CAlarm(start + 10, 1);
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
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);
    pEvent->setAllDay(0);
    bool ret = multi->addEvent(pEvent, 8, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));
    event_id = pEvent->getId();
    int iret = multi->getComponentCount(pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: retreiving component count ", (iret >= 1));
}
void CMulticalendar_TS1 :: test_checkForNextPrev()
{
    int pErrorCode;
    time_t start, end, until;
    time(&start);
    end = start + 10500;
    until = start + 50000;
    CEvent *pEvent = new CEvent("RepsummaryCount", "description",
                                "location", start + 10000, end);
    CEvent *pEvent1 = new CEvent("NosummaryCount", "description",
                                 "location", start, end);
    CEvent *pEvent2 = new CEvent("ComplexsummaryCount", "description",
                                 "location", start, end);
    CRecurrence *pRecur = new CRecurrence();
    CRecurrence *pRecur1 = new CRecurrence();
    vector <string> rules, rules1;
    rules.clear();
    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
    string rrule, rrule1;
    struct icaltimetype uical, uical1;
    int repeattype = 3;
    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent->setRtype(repeattype);
    pEvent->setUntil(until);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);

    uical1 = icaltime_from_timet(-1, 0);
    string ustring1 = icaltime_as_ical_string(uical);
    rrule1 = s1 + test[4 - 1] + s2 + ustring1;
    rules1.push_back(rrule1);
    pEvent2->setUntil(-1);
    pEvent2->setRtype(4);
    pRecur1->setRrule(rules1);
    pEvent2->setRecurrence(pRecur1);

    bool ret = multi->addEvent(pEvent, 8, error_code);
    bool ret1 = multi->addEvent(pEvent1, 8, error_code);
    bool ret2 = multi->addEvent(pEvent2, 8, error_code);

    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret1 == true));
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret2 == true));


    string  id = pEvent->getId();
    string  id1 = pEvent1->getId();
    string  id2 = pEvent2->getId();

    bool res = multi->checkForNextPrev(until - 1000, id, 1, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: getting prev/next event", (res == 1));
    res = multi->checkForNextPrev(until - 2000, id, 0, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: getting prev/next event", (res == 1));
    bool res1 = multi->checkForNextPrev(start, id1, 1, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: getting prev/next event", (res1 == 1));
    res1 = multi->checkForNextPrev(start, id1, 0, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: getting prev/next event", (res1 == 1));
}
void CMulticalendar_TS1 :: test_checkExistenceOfFutureRepeatingEvent()
{

    int pErrorCode;
    time_t start, end, until;
    time(&start);
    end = start;

    CEvent *pEvent = new CEvent("Complexount", "description",
                                "location", start, end);

    CRecurrence *pRecur = new CRecurrence();
    vector <string> rules;
    rules.clear();
    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[6] =  { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
    string rrule;
    struct icaltimetype uical;
    int repeattype = 3;
    uical = icaltime_from_timet(-1, 0);
    string ustring = icaltime_as_ical_string(uical);
    rrule = s1 + test[repeattype - 1] + s2 + ustring;
    rules.push_back(rrule);
    pEvent->setRtype(repeattype);
    pEvent->setUntil(-1);
    pRecur->setRrule(rules);
    pEvent->setRecurrence(pRecur);

    bool ret = multi->addEvent(pEvent, 8, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: adding event with alarm and recurrence", (ret == true));
    string  id = pEvent->getId();

    bool res = multi->checkExistenceOfFutureRepeatingEvent(start - 1, id, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: getting prev/next event", (res == 1));

}
void CMulticalendar_TS1 :: test_localBdayEventFromRemoteId()
{
    bool bRet;
    time_t utc_date;
    time_t until;
    int pErrorCode;

    utc_date = time(NULL);
    until = time(NULL) + (time_t) 86400 * 28 * 365;
    CBdayEvent *pBday = new CBdayEvent("abcder", "noname", utc_date + 500);
    CAlarm *pAlarm = new CAlarm(utc_date + 200, 1);
    pAlarm->setTrigger(utc_date + 120);
    pAlarm->setRepeat(3);
    pBday->setAlarm(pAlarm);
    CRecurrence *pRecur = new CRecurrence();
    vector <string> rules;
    vector <string> Erules;
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
    string Erule = s1 + test[3] + s2 + ustring;
    Erules.push_back(Erule);
    pRecur->setErule(Erules);

    vector <string> szrDateList;
    szrDateList.push_back("19970714T123000Z");
    szrDateList.push_back("19980714T123000Z");
    szrDateList.push_back("19990714T123000Z");
    pRecur->setRDays(szrDateList);
    vector <string> szeDateList;
    szeDateList.push_back("19970914T123000Z");
    szeDateList.push_back("19980814T123000Z");
    szeDateList.push_back("19990414T123000Z");
    pRecur->setEDays(szeDateList);

    pBday->setRecurrence(pRecur);
    bRet = multi->addBirthDay(pBday, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (bRet == true));
    bday = *pBday;
    string szExId = "abcder";
    multi->localBdayEventFromRemoteId(szExId, pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: add birthday", (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL));
}


void CMulticalendar_TS1 :: test_exportICSFileInfoProgressive()
{
    string file = "importProg.ics";
    string Id;
    int event;
    int task;
    int note;
    unsigned int events;
    unsigned int tasks;
    unsigned int notes;
    int pErrorCode;
    bool read_over;
    bool ret = multi->getICSFileInfoProgressive(file, events, tasks, notes, pErrorCode, &read_over, 30);
    CPPUNIT_ASSERT_MESSAGE("Error: exportICS FILE info Progressive", ret == true);
    ret = multi->getICSFileInfoProgressive("file:///home/parivesh/MyDocs/Invalid.ics", events, tasks, notes, pErrorCode, &read_over, 30);
    ret = multi->getICSFileInfoProgressive("Invalid1.ics", events, tasks, notes, pErrorCode, &read_over, 30);
    CPPUNIT_ASSERT_MESSAGE("Error: exportICS FILE info Progressive", ret == false);
    ret = multi->getICSFileInfoProgressive("Invalid2.ics", events, tasks, notes, pErrorCode, &read_over, 30);
    ret = multi->getICSFileInfoProgressive("Invalid3.ics", events, tasks, notes, pErrorCode, &read_over, 30);
    ret = multi->importIcsFileDataProgressive("file:///home/parivesh/MyDocs/importProg1.ics", 8, Id, task, event, note, pErrorCode, &read_over, 30);
    ret = multi->importIcsFileDataProgressive("file:///home/parivesh/MyDocs/importProg2.ics", 8, Id, task, event, note, pErrorCode, &read_over, 30);
    ret = multi->importIcsFileDataProgressive("importProg.ics", 8, Id, task, event, note, pErrorCode, &read_over, 30);
    CPPUNIT_ASSERT_MESSAGE("Error: exportICS FILE info Progressive", ret = true);
}
void CMulticalendar_TS1 :: test_limited()
{
    int iStDate = 1;
    int iEndDate = 214356783;
    int iLimit = 100;
    int iOffset = 0;
    vector <int> vCalid;
    int pErrorCode;
    vector <string> id;
    string szTemp;
    multi->getComponentsWithLimitedData(iStDate, iEndDate, iLimit, iOffset, vCalid, pErrorCode);
    multi->getRequiredDates(time_get_time(), 10, 10, iStDate, iEndDate, id, pErrorCode);
    multi->getDateRange(time_get_time(), 100, true, szTemp, pErrorCode);
    multi->clearCalendarContents(1, pErrorCode);
}




void CMulticalendar_TS2 :: test_addCalendar_Default_DB()
{
    bool ret = pCdb1->closeDB();
    CCalendar *pC = new CCalendar("TestMulticalendarQE", COLOUR_RED, 1, 1, BIRTHDAY_CALENDAR, "SomeTune.xyz", "Version-1.0");
    CPPUNIT_ASSERT_MESSAGE("Error: CloseDB", (ret == true));
    icalid = multi1->addCalendar(pC, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: Add Calendar", (icalid <= 0));

}

void CMulticalendar_TS3 :: test_AllEmptyget()
{
    int iStDate = 1;
    int iEndDate = 214356783;
    int iLimit = 100;
    int iOffset = 0;
    vector <int> vCalid;
    int pErrorCode;
    vector <string> id;
    string szTemp;
    multi->getComponentsWithLimitedData(iStDate, iEndDate, iLimit, iOffset, vCalid, pErrorCode);
    CCalendar *cal = multi->getCalendarByType(LOCAL_CALENDAR, error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getCalendarByType 0 Calendar", (cal == NULL));
    string name = multi->getDefaultCalendarName();
    name = multi->getSynchronizedCalendarName();
    cal = multi->getBirthdayCalendar();
    cal = multi->getSynchronizedCalendar();
    cal = multi->getDefaultCalendar();
    vector <CComponent *> ret = multi->getTodaysAllDay(error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: getNumOfAllDayEventsForToday", (ret.size() == 0));
    int iret = multi->getComponentCount(pErrorCode);
    CPPUNIT_ASSERT_MESSAGE("Error: retreiving component count ", (iret == 0));
    multi->getVisibleCalendarCount(pErrorCode) ;
    cal = multi->getCalendarByName("TM", error_code);
    CPPUNIT_ASSERT_MESSAGE("Error: get Calendar by id; no calendars in db", (cal == NULL));
    multi->getSystemTimeShift();



}


