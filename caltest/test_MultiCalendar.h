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
#include <sqlite3.h>
#include <sys/time.h>
#include <ctime>


#include "CMulticalendar.h"
#include "CCalendarDB.h"
//#include "CalendarErrors.h"
#include "CBdayEvent.h"
using namespace std;


class CMulticalendar_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CMulticalendar_TS1 );

	
	CPPUNIT_TEST(test_getCalendar_NoCalendar);
	CPPUNIT_TEST(test_addEvent_NoCalendar);
	CPPUNIT_TEST(test_modifyEvent_NoCalendar);
	CPPUNIT_TEST(test_addTodo_NoCalendar);
	CPPUNIT_TEST(test_modifyTodo_NoCalendar);
	CPPUNIT_TEST(test_addJournal_NoCalendar);
	CPPUNIT_TEST(test_modifyJournal_NoCalendar);
	CPPUNIT_TEST(test_addBday_NoCalendar);
	CPPUNIT_TEST(test_modifyBday_NoCalendar);
	CPPUNIT_TEST(test_deleteEvent_NoCalendar);
	CPPUNIT_TEST(test_deleteJournal_NoCalendar);
	CPPUNIT_TEST(test_deleteTodo_NoCalendar);
	CPPUNIT_TEST(test_deleteBday_NoCalendar);
	CPPUNIT_TEST(test_getBirthdayCalendar);
	CPPUNIT_TEST(test_getCalendarByType_NoSyncCalendar);
	CPPUNIT_TEST(test_getCalendarByName_NoCalendars);
	CPPUNIT_TEST(test_getSynchronizedCalendar);
	CPPUNIT_TEST(test_getDefaultCalendar);
	CPPUNIT_TEST(test_getSynchronizedCalendarName);
	CPPUNIT_TEST(test_getDefaultCalendarName);

	CPPUNIT_TEST(test_addCalendar_Default);

	CPPUNIT_TEST(test_addCalendar);
	CPPUNIT_TEST(test_addCalendar_Invalid);
	CPPUNIT_TEST(test_addCalendar1);
	CPPUNIT_TEST(test_addCalendar2);
	
	CPPUNIT_TEST(test_addCalendar_AllParameters);
	CPPUNIT_TEST(test_noVisibleCal);
	CPPUNIT_TEST(test_addCalendar_NULL);
	
	CPPUNIT_TEST(test_addCalendar_AlreadyExist);
	
	CPPUNIT_TEST(test_getCalendarByType);
	CPPUNIT_TEST(test_getCalendarByType1);
	CPPUNIT_TEST(test_getCalendarByType2);
	CPPUNIT_TEST(test_getCalendarById);
	CPPUNIT_TEST(test_getCalendarByName);
	CPPUNIT_TEST(test_getNoofCalendars);
	CPPUNIT_TEST(test_getNoteCount);
	CPPUNIT_TEST(test_getTodoCount);
	CPPUNIT_TEST(test_getEventCount);
 
	
	CPPUNIT_TEST(test_addEvent);
	
	CPPUNIT_TEST(test_checkForNextPrev_noprev);
	CPPUNIT_TEST(test_getCalendarIdForComponent);
	CPPUNIT_TEST(test_getBirthDayEventCount);
	CPPUNIT_TEST(test_addEvent1);
	CPPUNIT_TEST(test_getEventCount1);
	CPPUNIT_TEST(test_modifyEvent1);
	CPPUNIT_TEST(test_modifyEvent);
 
	CPPUNIT_TEST(test_addTodo);
	CPPUNIT_TEST(test_getTodoCount1);
	CPPUNIT_TEST(test_modifyTodo);
	CPPUNIT_TEST(test_getAllEventsTodos);
	CPPUNIT_TEST(test_addJournal);
	CPPUNIT_TEST(test_getNoteCount1);
	CPPUNIT_TEST(test_modifyJournal);
	CPPUNIT_TEST(test_getComponents);
	CPPUNIT_TEST(test_addBday);
	CPPUNIT_TEST(test_getBirthDayEventCount1);
	CPPUNIT_TEST(test_modifyBirthDay);
	
	CPPUNIT_TEST(test_deleteBirthdaysForCalendar);
	CPPUNIT_TEST(test_delBirthdayCalendar);
	CPPUNIT_TEST(test_modifyCalendar);
	CPPUNIT_TEST(test_modifyCalendar_NULL);
	CPPUNIT_TEST(test_getListCalFromMc);
	CPPUNIT_TEST(test_getNumOfUndoneTasks);
	CPPUNIT_TEST(test_getNumOfAllDayEventsForToday);
	CPPUNIT_TEST(test_getTodaysAllDay);
	CPPUNIT_TEST(test_CtCap);
	CPPUNIT_TEST(test_deleteEvent);
	CPPUNIT_TEST(test_deleteTodo);
	CPPUNIT_TEST(test_deleteJournal);
	CPPUNIT_TEST(test_deleteBirthDay);
	CPPUNIT_TEST(test_importIcsFileData);
	CPPUNIT_TEST(test_getICSFileInfo);

	CPPUNIT_TEST(test_getLatestComponent);
	CPPUNIT_TEST(test_moveFileFromSkel);
	CPPUNIT_TEST(test_checkFileExists);
	CPPUNIT_TEST(test_getComponentCount);
	CPPUNIT_TEST(test_localBdayEventFromRemoteId);
	CPPUNIT_TEST(test_exportICSFileInfoProgressive);
	CPPUNIT_TEST(test_limited);
	
	CPPUNIT_TEST(test_deleteBeyond);
	CPPUNIT_TEST(test_automaticDeletion);
	CPPUNIT_TEST(test_getCalendarByIdInvalid);
	CPPUNIT_TEST(test_addEvent_NULL);
	CPPUNIT_TEST(test_modifyEvent_NULL);
	CPPUNIT_TEST(test_addTodo_NULL);
	CPPUNIT_TEST(test_modifyTodo_NULL);
	CPPUNIT_TEST(test_addJournal_NULL);
	CPPUNIT_TEST(test_modifyJournal_NULL);
	CPPUNIT_TEST(test_addBday_NULL);
	CPPUNIT_TEST(test_modifyBday_NULL);
	CPPUNIT_TEST(test_getPrevNextComponent);
	CPPUNIT_TEST(test_getRecurrenceProperties);
	CPPUNIT_TEST(test_printCurrentTime);
	CPPUNIT_TEST(test_addComponents);
	CPPUNIT_TEST(test_modifyComponents);
	CPPUNIT_TEST(test_getComponentsAllCalendars_Events);
	CPPUNIT_TEST(test_getComponents);
	CPPUNIT_TEST(test_deleteComponents);
	CPPUNIT_TEST(test_getAlarmProperties);
	CPPUNIT_TEST(test_checkForNextPrev);
	CPPUNIT_TEST(test_checkExistenceOfFutureRepeatingEvent);
	CPPUNIT_TEST(test_deleteCalendar);
	CPPUNIT_TEST(test_deleteCalendar_NULL);
	CPPUNIT_TEST_SUITE_END();

	public:
		CMulticalendar* multi;
		void setUp();
		void tearDown();
		void test_addCalendar_Default();
		void test_addCalendar();
		void test_addCalendar1();
		void test_addCalendar2();
		void test_getCalendarByType();
		void test_getCalendarByType1();
		void test_getCalendarByType2();
		void test_getCalendarById();
		void test_getCalendarByName();
		void test_getNoofCalendars();
		void test_getNoteCount();
		void test_getTodoCount();
		void test_getEventCount();
		void test_modifyCalendar();
		void test_addEvent();
		void test_checkForNextPrev_noprev();
		void test_addEvent1();
		void test_getRecurrenceProperties();
		void test_modifyEvent();
		void test_modifyEvent1();
		void test_addTodo();
		void test_modifyTodo();
		void test_addJournal();
		void test_modifyJournal();
		void test_getNoteCount1();
		void test_getTodoCount1();
		void test_getEventCount1();
		void test_deleteBeyond();
		void test_automaticDeletion();
		void test_deleteEvent();
		void test_deleteTodo();
		void test_deleteJournal();
		void test_deleteBirthDay();
		void test_modifyBirthDay();
		void test_addBday();	
		void test_getCalendarByIdInvalid();
		void test_deleteCalendar();
		void test_getComponents();
		void test_getAllCalendars();
		void test_getListCalFromMc();
		void test_importIcsFileData();
		void test_getLatestComponent();
		void test_getAlarmProperties();
		void test_getNumOfUndoneTasks();
		void test_getNumOfAllDayEventsForToday();
		void test_getTodaysAllDay();
		void test_CtCap();
		void test_getICSFileInfo();
		

		void test_getComponentsAllCalendars_Events();
		void test_addCalendar_AllParameters();
		void test_addCalendar_NULL();
		void test_addCalendar_AlreadyExist();
		void test_addEvent_NoCalendar();
		void test_addEvent_NULL();
		void test_modifyEvent_NoCalendar();
		void test_modifyEvent_NULL();
		void test_addTodo_NoCalendar();
		void test_addTodo_NULL();
		void test_modifyTodo_NoCalendar();
		void test_modifyTodo_NULL();
		void test_addJournal_NoCalendar();
		void test_addJournal_NULL();
		void test_modifyJournal_NoCalendar();
		void test_modifyJournal_NULL();
		void test_getBirthdayCalendar();
		void test_getSynchronizedCalendar();
		void test_getDefaultCalendar();
		void test_getSynchronizedCalendarName();
		void test_getDefaultCalendarName();
		void test_addCalendar_Invalid();
		void test_getBirthDayEventCount();
		void test_getBirthDayEventCount1();
		void test_deleteEvent_NULL();
		void test_deleteTodo_NULL();
		void test_deleteJournal_NULL();
		void test_deleteBirthdaysForCalendar();
		void test_getPrevNextComponent();
		void test_getCalendarIdForComponent();
		void test_getAllEventsTodos();
		void test_getCalendarByType_NoSyncCalendar();
		void test_getCalendarByName_NoCalendars();
		void test_deleteEvent_NoCalendar();
		void test_deleteJournal_NoCalendar();
		void test_deleteTodo_NoCalendar();
		void test_deleteBday_NoCalendar();
		void test_addBday_NoCalendar();
		void test_modifyBday_NoCalendar();
		void test_addBday_NULL();
		void test_modifyBday_NULL();
		void test_getCalendar_NoCalendar();
		
		void test_modifyCalendar_NULL();
		void test_deleteCalendar_NULL();
		void test_printCurrentTime();
		void test_addComponents();
		void test_modifyComponents();
		void test_deleteComponents();
		void test_delBirthdayCalendar();
		void test_noVisibleCal();
		void test_moveFileFromSkel();
		void test_checkFileExists();
		void test_getComponentCount();
		void test_checkForNextPrev();
		void test_checkExistenceOfFutureRepeatingEvent();
		void test_localBdayEventFromRemoteId();
		void test_exportICSFileInfoProgressive();
		void test_limited();

		
};
class CMulticalendar_TS2 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CMulticalendar_TS2 );
	CPPUNIT_TEST(test_addCalendar_Default_DB);
	CPPUNIT_TEST_SUITE_END();
	public:
		CMulticalendar* multi1;
		CCalendarDB *pCdb1;
		void setUp();
		void tearDown();
		void test_addCalendar_Default_DB();
	};
class CMulticalendar_TS3 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CMulticalendar_TS3 );
	CPPUNIT_TEST(test_AllEmptyget);
	CPPUNIT_TEST_SUITE_END();
	public:
	CMulticalendar* multi;
	void setUp();
	void tearDown();
	void test_AllEmptyget();
};

