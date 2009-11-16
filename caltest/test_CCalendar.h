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


#include "CCalendar.h"
#include "CMulticalendar.h"
#include "CBdayEvent.h"

using namespace std;


class CCalendar_TS1 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CCalendar_TS1 );
	CPPUNIT_TEST(test_addCalendarsetup);
	CPPUNIT_TEST(test_getAllEventsTodosInvalid);
	CPPUNIT_TEST(test_INVaddEvent);
	CPPUNIT_TEST(test_INVaddEvent1);
	CPPUNIT_TEST(test_addEvent_1);
	CPPUNIT_TEST(test_addTodo_1);
	CPPUNIT_TEST(test_addJournal);
	CPPUNIT_TEST(test_getAllAddItems);
	CPPUNIT_TEST(test_getComponents_wo_recurrence);
	CPPUNIT_TEST(test_modifyEvent_1);
	CPPUNIT_TEST(test_modifyEvent_invisible);
	CPPUNIT_TEST(test_modifyEvent_invalid_arg);
	CPPUNIT_TEST(test_modifyEvent_1All);
	CPPUNIT_TEST(test_addTodo_invalid_arg);
	CPPUNIT_TEST(test_modifyTodo_1);
	CPPUNIT_TEST(test_modifyTodo_invalid_arg);
	CPPUNIT_TEST(test_modifyTodo_1All);
	CPPUNIT_TEST(test_addJournal_1);
	CPPUNIT_TEST(test_modifyJournal_1);
	CPPUNIT_TEST(test_modifyJournal_invalid_arg);
	CPPUNIT_TEST(test_modifyJournal_1All);
	CPPUNIT_TEST(test_addBday_invalid_arg);
	CPPUNIT_TEST(test_addBday_1);
	CPPUNIT_TEST(test_addEvent);
	CPPUNIT_TEST(test_addTodo);
	CPPUNIT_TEST(test_addJournal_invalid_arg);
	CPPUNIT_TEST(test_addBday);
	CPPUNIT_TEST(test_getBirthdayEvent);
	CPPUNIT_TEST(test_getDetailedBDayList);
	CPPUNIT_TEST(test_getEntry);
	CPPUNIT_TEST(test_getEntry_invalid_type);
	CPPUNIT_TEST(test_addEventDuplicate);
	CPPUNIT_TEST(test_addTodoDuplicate);
	CPPUNIT_TEST(test_getAllEventsTodos);
	CPPUNIT_TEST(test_modifyEvent);
	CPPUNIT_TEST(test_modifyEventAll);
	CPPUNIT_TEST(test_modifyEvent1);
	CPPUNIT_TEST(test_modifyTodo);
	CPPUNIT_TEST(test_modifyTodoAll);
	CPPUNIT_TEST(test_modifyTodo1);
	CPPUNIT_TEST(test_getAllModifiedItems);
	CPPUNIT_TEST(test_modifyJournal);
	CPPUNIT_TEST(test_modifyJournalAll);
	CPPUNIT_TEST(test_modifyJournal1);
	CPPUNIT_TEST(test_modifyBirthDay);
	CPPUNIT_TEST(test_modifyBday_invalid_arg);
	CPPUNIT_TEST(test_modifyBday_addalarm_invisiblecal);
	CPPUNIT_TEST(test_modifyBday_modifyalarm);
	CPPUNIT_TEST(test_addXProperties_invalid_arg);
	CPPUNIT_TEST(test_addParameters_invalid_arg);
	CPPUNIT_TEST(test_modifyComponents);



	CPPUNIT_TEST(test_setNextAlarm);
	CPPUNIT_TEST(test_setNextAlarm_invalid);
	CPPUNIT_TEST(test_registerAlarmForVisibleCalendar);
	CPPUNIT_TEST(test_destroyExistingValidAlarms);
	CPPUNIT_TEST(test_destroyExistingValidAlarms_bdayCal);
	CPPUNIT_TEST(test_getComponents);
	CPPUNIT_TEST(test_getComponents1);
	CPPUNIT_TEST(test_getComponents2);
	CPPUNIT_TEST(test_exportCalendar);
	CPPUNIT_TEST(test_getDetailedEventList);
	CPPUNIT_TEST(test_getTodoListDetailed);
	CPPUNIT_TEST(test_getJournalListDetailed);
	CPPUNIT_TEST(test_checkIfMultiDayEvent);
	CPPUNIT_TEST(test_checkIfMultiDayEvent_equaldates);
	CPPUNIT_TEST(test_checkIfMultiDayEvent_zerodates);
	CPPUNIT_TEST(test_deleteEvent);
	CPPUNIT_TEST(test_deleteTodo);
	CPPUNIT_TEST(test_deleteJournal);
	CPPUNIT_TEST(test_deleteBirthDay);
	CPPUNIT_TEST(test_getAllDeletedItems);
	CPPUNIT_TEST(test_operator);

	CPPUNIT_TEST(test_getAllAddedEvents);
	CPPUNIT_TEST(test_getAllAddedTodos);
	CPPUNIT_TEST(test_getAllAddedJournals);
	CPPUNIT_TEST(test_checkDuplicateEntryEvent);
	CPPUNIT_TEST(test_checkDuplicateEntryTodo);
	CPPUNIT_TEST(test_checkDuplicateEntryJournal);
	CPPUNIT_TEST(test_getAllModifiedEvents);
	CPPUNIT_TEST(test_getAllModifiedTodos);
	CPPUNIT_TEST(test_getAllModifiedJournals);
	CPPUNIT_TEST(test_getAllDeletedEvents);
	CPPUNIT_TEST(test_getAllDeletedTodos);
	CPPUNIT_TEST(test_getAllDeletedJournals);
	CPPUNIT_TEST(test_deleteComponent);
	CPPUNIT_TEST(test_removeFromIds);
	CPPUNIT_TEST(test_appendToIds);
	CPPUNIT_TEST(test_constructor);
	CPPUNIT_TEST(test_copyConstrutor);
	
	
	CPPUNIT_TEST(test_registerAlarmForVisibleCalendarForAlarmWithCookieZero);
	CPPUNIT_TEST_SUITE_END();

	public:
		CMulticalendar* multi;
		void setUp();
		void tearDown();
		void test_addCalendarsetup();
		void test_INVaddEvent();
		void test_INVaddEvent1();
		void test_addEvent();
		void test_getComponents_wo_recurrence();
		void test_modifyEvent();
		void test_modifyEvent1();
		void test_addTodo();
		void test_addTodo_invalid_arg();
		void test_modifyTodo();
		void test_modifyTodo1();
		void test_modifyTodo_invalid_arg();
		void test_addJournal();
		void test_addJournal_invalid_arg();
		void test_getAllAddItems();
		void test_modifyJournal();
		void test_modifyJournal1();
		void test_modifyJournal_invalid_arg();
		void test_getAllModifiedItems();
		void test_getDetailedEventList();
		void test_getTodoListDetailed();
		void test_getJournalListDetailed();
		void test_getComponents();
		void test_getComponents1();
		void test_getComponents2();
		void test_deleteEvent();
		void test_deleteTodo();
		void test_deleteBirthDay();
		void test_modifyBirthDay();
		void test_modifyBday_invalid_arg();
		void test_modifyBday_addalarm_invisiblecal();
		void test_modifyBday_modifyalarm();
		void test_addBday();	
		void test_addBday_invalid_arg();
		void test_deleteJournal();
		void test_exportCalendar();
		void test_operator();
		void test_getAllDeletedItems();
		void test_setNextAlarm();
		void test_setNextAlarm_invalid();

		void test_registerAlarmForVisibleCalendar();
		void test_registerAlarmForVisibleCalendarForAlarmWithCookieZero();
		void test_destroyExistingValidAlarms();
		void test_destroyExistingValidAlarms_bdayCal();
		void test_checkDuplicateEntryEvent();
		void test_checkDuplicateEntryTodo();
		void test_checkDuplicateEntryJournal();
		void test_getAllAddedEvents();
		void test_getAllAddedTodos();
		void test_getAllAddedJournals();
		void test_getAllModifiedEvents();
		void test_getAllModifiedTodos();
		void test_getAllModifiedJournals();
		void test_getAllDeletedEvents();
		void test_getAllDeletedTodos();
		void test_getAllDeletedJournals();
		void test_deleteComponent();
		void test_getAllEventsTodosInvalid();
		void test_getAllEventsTodos();
		void test_constructor();
		void test_copyConstrutor();
		void test_addEventDuplicate();
		void test_addTodoDuplicate();
		void test_modifyEventAll();
		void test_modifyTodoAll();
		void test_addEvent_1();
		void test_modifyEvent_1();
		void test_modifyEvent_invalid_arg();
		void test_modifyEvent_invisible();
		void test_modifyEvent_1All();
		void test_addTodo_1();
		void test_modifyTodo_1();
		void test_modifyTodo_1All();
		void test_addJournal_1();
		void test_modifyJournal_1();
		void test_modifyJournal_1All();
		void test_modifyJournalAll();
		void test_getEntry();
		void test_getEntry_invalid_type();
		void test_addBday_1();
		void test_addXProperties_invalid_arg();
		void test_addParameters_invalid_arg();
		void test_modifyComponents();
		
		void test_getBirthdayEvent();
		void test_getDetailedBDayList();
		
		void test_checkIfMultiDayEvent();
		void test_checkIfMultiDayEvent_equaldates();
		void test_checkIfMultiDayEvent_zerodates();
		void test_appendToIds();
		void test_removeFromIds();
					
};

class CCalendar_TS2 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CCalendar_TS2 );
	CPPUNIT_TEST(test_toString);
	CPPUNIT_TEST(test_getEvents);
	CPPUNIT_TEST_SUITE_END();

	public:
		CMulticalendar* multi;
		void setUp();
		void tearDown();
		void test_toString();
		void test_getEvents();
};
class CCalendar_TS3 : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( CCalendar_TS3 );
	CPPUNIT_TEST(test_AllEmptyget);
	CPPUNIT_TEST_SUITE_END();

	public:
		CMulticalendar* multi;
		void setUp();
		void tearDown();
		void test_AllEmptyget();
};

		

		

