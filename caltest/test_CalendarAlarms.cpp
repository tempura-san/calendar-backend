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
#include <time.h>
#include <libalarm.h>

#include "CAlarm.h"
#include "Common.h"
#include "test_CalendarAlarms.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CalendarAlarms_TS1);

using namespace std;

void CalendarAlarms_TS1::setUp()
{
	pCm = new CAlarm();
	mc = CMulticalendar::MCInstance();
}
void CalendarAlarms_TS1::tearDown()
{
	delete pCm;
}
int icaid ;
void CalendarAlarms_TS1::test_alarminit()
{
        int error_code;
        CCalendar *pC=new CCalendar("TestCalendarAlarm",COLOUR_RED,1,1,LOCAL_CALENDAR,"SomeTune.xyz","Version-1.0");
        icaid = mc->addCalendar(pC,error_code);
}


void CalendarAlarms_TS1::test_setAlarm()
{
	cookie_t cookie;
	time_t start_time;
	time(&start_time);
	int iErrorCode;
	cookie = pCm->addAlarmEvent ( start_time + 88888888,"message", "location", start_time +99999999, start_time +99999999 + 100000000,"id",icaid, "Description", 1, 1, "UTC",iErrorCode);	
	CPPUNIT_ASSERT_MESSAGE("Error : setting alarm ",(cookie>0));
}

void CalendarAlarms_TS1::test_modifyAlarm()
{
	cookie_t cookie_new;
	cookie_t cookie;
	time_t start_time;
	time(&start_time);
	int iErrorCode;
	cookie = pCm->addAlarmEvent ( start_time + 88888888,"message", "location", start_time +99999999, start_time +99999999 + 100000000,"id",icaid, "Description", 1, 1, "UTC", iErrorCode);
	cookie_new = pCm->modifyAlarmEvent(cookie,start_time + 88888888,"messagerepeat", "locationchange", start_time +99999999, start_time +99999999 + 100000000,"id",icaid, "Description", 1, 1, "UTC", iErrorCode);
	CPPUNIT_ASSERT_MESSAGE("Error : modify alarm  ",(cookie_new>0));
	if(cookie_new>0)
		cookie=cookie_new;
}

void CalendarAlarms_TS1::test_deleteAlarm()
{
	cookie_t cookie;
	time_t start_time;
	time(&start_time);
	int iErrorCode;
	cookie = pCm->addAlarmEvent ( start_time + 88888888,"message", "location", start_time +99999999, start_time +99999999 + 100000000,"id",icaid, "Description", 1, 1, "UTC", iErrorCode);
	int ret = pCm->deleteAlarmEvent(cookie, iErrorCode);
	CPPUNIT_ASSERT_MESSAGE("Error : delete alarm  ",(ret==0));//replaced ret>0 with ret==0 as alarmd_event_del should return 0 on no error
}

void CalendarAlarms_TS1::test_deleteAlarmInvalid()
{
	int iErrorCode;
	int ret = pCm->deleteAlarmEvent(100000, iErrorCode);
	CPPUNIT_ASSERT_MESSAGE("Error : delete alarm  ",(ret<0));//replaced ret==0 with ret<0 as alarmd_event_del should return -1 on no error
}

void CalendarAlarms_TS1::test_registerAlarmForAutoDeletion()
{
	time_t start_time;
	time(&start_time);
	int iErrorCode;
	cookie_t ret = pCm->registerAlarmForAutoDeletion(start_time+10000, iErrorCode);
	CPPUNIT_ASSERT_MESSAGE("Error :Add alarm for automatic deletion ",(ret>0));
}
