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

#include "test_CTodo.h"
#include "Common.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CTodo_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CTodo_TS2);

using namespace std;

void CTodo_TS1::setUp()
{
	pCt= new CTodo();
}
void CTodo_TS1::tearDown()
{
	delete pCt;
}

void CTodo_TS2::setUp()
{
	pCt= new CTodo();
}
void CTodo_TS2::tearDown()
{
	delete pCt;
}
void CTodo_TS1 ::test_SetGeo_Valid()
{
	string  szGeo1="LLL";
	bool ret = pCt->setGeo(szGeo1);
	string szRet =pCt->getGeo();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",szRet==szGeo1);
}

void CTodo_TS1::test_SetGeo_Empty()
{
	bool szRet =pCt->setGeo("");
	CPPUNIT_ASSERT_MESSAGE("Error : setting the wrong value",(szRet==true));
}


void CTodo_TS1 ::test_SetDue_Valid()
{
	bool ret = pCt->setDue(100008);
	int stRet=pCt->getDue();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(stRet==100008));
}

void CTodo_TS1 ::test_SetCompleted_Valid()
{
	bool ret = pCt->setCompleted(100008);
	int stRet=pCt->getCompleted();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(stRet==100008));
	ret = pCt->setCompleted(-1);
	stRet=pCt->getCompleted();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==false));
}

void CTodo_TS1 ::test_SetPercentComplete_Negative()
{
	bool bRet=pCt->setPercentComplete(-8);
	CPPUNIT_ASSERT_MESSAGE("Error : setting the negative  value",(bRet==false));
}

void CTodo_TS1 ::test_SetPercentComplete_Valid()
{
	bool ret = pCt->setPercentComplete(80);
	int iRet=pCt->getPercentComplete();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(iRet==80));
}

void CTodo_TS1 ::test_SetPercentComplete_GTHundred()
{
	bool bRet=pCt->setPercentComplete(188);
	CPPUNIT_ASSERT_MESSAGE("Error :setting percentage greated than 100 value",(bRet == false));
}

void CTodo_TS1 ::test_SetPriority_Negative()
{
	bool bRet=pCt->setPriority(-8);
	CPPUNIT_ASSERT_MESSAGE("Error : setting the negative  value",(bRet==false));
}


void CTodo_TS1 ::test_SetPriority_Valid()
{
	bool ret = pCt->setPriority(8);
	int iRet=pCt->getPriority();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(iRet == 8));
}

void CTodo_TS1 ::test_SetPriority_Inrange()
{
	bool bRet=pCt->setPriority(20);
	CPPUNIT_ASSERT_MESSAGE("Error : setting the priority greated than 9",(bRet==false));
}

time_t start;
void ValidCTodoConstructorTestCase::setUp()
{
	time(&start);
	p= new CTodo("abcdefghi...",start,1);
}

void ValidCTodoConstructorTestCase::tearDown()
{
	delete p;
}

void ValidCTodoConstructorTestCase::test_validCTodoConstructor()
{
	CTodo r;
	CTodo q (*(p));
	r = *(p);
	CPPUNIT_ASSERT_MESSAGE("conflict description",(strcmp((p->getSummary().c_str()),"abcdefghi...")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict status",(p->getStatus()== 1));
	CPPUNIT_ASSERT_MESSAGE("conflict datestart",(p->getDateStart()==start));
	CPPUNIT_ASSERT_MESSAGE("conflict description",(strcmp((r.getSummary().c_str()),"abcdefghi...")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict status",(r.getStatus()== 1));
	CPPUNIT_ASSERT_MESSAGE("conflict date start",(r.getDateStart()==start));
	
	CPPUNIT_ASSERT_MESSAGE("conflict description ",(strcmp((q.getSummary().c_str()),"abcdefghi...")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict ststus",(q.getStatus()== 1));
	CPPUNIT_ASSERT_MESSAGE("conflict datestart",(q.getDateStart()==start));

}

void CTodo_TS2 ::test_toString()
{
	CTodo *pTodo = new CTodo("summary", start+1000, 0);
	CAlarm *alarm = new CAlarm(10000, 1);
	bool ret = pTodo->setAlarm(alarm);
	CRecurrence *rec = new CRecurrence();
	ret = pTodo->setRecurrence(rec);

	int duration;
        duration = pTodo->getDuration();
	string  szDes ="mails";
	pTodo->setDescription(szDes);
	pTodo->setDateStart(45678);
	pTodo->setDateEnd(45678);
	pTodo->setFlags(4);
	pTodo->setType(2);
	pTodo->setSummary("Summary");
	pTodo->setLocation("LOC");
	pTodo->setLastModified(12345678);
	pTodo->setAllDay(0);
	pTodo->setRtype(1);
	pTodo->setCreatedTime(12364589);

	string set = "qwerty";
	ret = pTodo->setId(set);
	cout<<pTodo->toString();
}
void CTodo_TS2 ::test_toString_NULL()
{
	CTodo *pTodo = new CTodo("Tostring Invalid -Todo", start+1000, 0);
	CAlarm *alarm = 0;
	bool ret = pTodo->setAlarm(alarm);
	CRecurrence *rec =0;
	ret = pTodo->setRecurrence(rec);
	cout<<pTodo->toString();

}
void CTodo_TS2 :: test_toString_flags()
{
	CTodo *pTodo = new CTodo("summary", start+1000, 0);
	pTodo->setFlags(HAS_ATTENDEE);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_ATTENDEE);
	pTodo->setFlags(HAS_ORGANIZER);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_ORGANIZER);
	pTodo->setFlags(HAS_RECURRENCE);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_RECURRENCE);
	pTodo->setFlags(HAS_ALARM);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_ALARM);
	pTodo->setFlags(HAS_RECURRENCE_ALARM);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_RECURRENCE_ALARM);
	pTodo->setFlags(HAS_PARTICIPANT);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_PARTICIPANT);
	pTodo->setFlags(HAS_CATEGORIES);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_CATEGORIES);
	pTodo->setFlags(HAS_COMMENT);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_COMMENT);
	pTodo->setFlags(HAS_EXTRA);
	cout << pTodo->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pTodo->getFlags() == HAS_EXTRA);
}

