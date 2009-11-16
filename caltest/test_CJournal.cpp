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
#include "test_CJournal.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CJournal_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CJournal_TS2);

void ValidCJournalConstructorTestCase::setUp()
{
	p= new CJournal("abcdefghi...");
}

void ValidCJournalConstructorTestCase::tearDown()
{
	delete p;
}


void CJournal_TS2::setUp()
{
	p= new CJournal;
}

void CJournal_TS2::tearDown()
{
	delete p;
}

void ValidCJournalConstructorTestCase::test_validCJournalConstructor()
{
	CJournal q(*(p));
	CJournal r;
	r=*(p);
	CPPUNIT_ASSERT_MESSAGE("conflict description",(strcmp((p->getSummary().c_str()),"abcdefghi...")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict description",(strcmp((q.getSummary().c_str()),"abcdefghi...")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict description",(strcmp((r.getSummary().c_str()),"abcdefghi...")==0));
}

void CJournal_TS2 ::test_toString()
{

	CJournal *pJournal = new CJournal("summary");
	CAlarm *alarm = new CAlarm(10000, 1);
	bool ret = pJournal->setAlarm(alarm);
	CRecurrence *rec = new CRecurrence();
	pJournal->setRecurrence(rec);

	int duration;
        duration = pJournal->getDuration();
	string  szDes ="mails";
	pJournal->setDescription(szDes);
	pJournal->setDateStart(45678);
	pJournal->setDateEnd(45678);
	pJournal->setFlags(4);
	pJournal->setType(2);
	pJournal->setSummary("Summary");
	pJournal->setLocation("LOC");
	pJournal->setLastModified(12345678);
	pJournal->setAllDay(0);
	pJournal->setRtype(1);
	pJournal->setCreatedTime(12364589);

	string set = "qwerty";
	ret = pJournal->setId(set);
	cout<<pJournal->toString();
}
void CJournal_TS2 ::test_toString_NULL()
{

	string sum="";
	CJournal *pJournal = new CJournal(sum);
	cout<<pJournal->toString();
}

void CJournal_TS2 :: test_toString_flags()
{
	CJournal *pJournal = new CJournal("summary");
	pJournal->setFlags(HAS_ATTENDEE);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_ATTENDEE);
	pJournal->setFlags(HAS_ORGANIZER);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_ORGANIZER);
	pJournal->setFlags(HAS_RECURRENCE);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_RECURRENCE);
	pJournal->setFlags(HAS_ALARM);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_ALARM);
	pJournal->setFlags(HAS_RECURRENCE_ALARM);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_RECURRENCE_ALARM);
	pJournal->setFlags(HAS_PARTICIPANT);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_PARTICIPANT);
	pJournal->setFlags(HAS_CATEGORIES);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_CATEGORIES);
	pJournal->setFlags(HAS_COMMENT);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_COMMENT);
	pJournal->setFlags(HAS_EXTRA);
	cout << pJournal->toString();
	CPPUNIT_ASSERT_MESSAGE("Error: Failed to get flags",pJournal->getFlags() == HAS_EXTRA);
}
