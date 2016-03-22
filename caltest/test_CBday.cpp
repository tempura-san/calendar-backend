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

#include "CAlarm.h"
#include "CRecurrence.h"

#include "test_CBday.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CBday_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CBday_TS2);

void CBday_TS1::setUp()
{
    // set up new event on 19720926T020714
    bd = new CBdayEvent("ID12345", "John Doe", 86321234);
}

void CBday_TS1::tearDown()
{
    delete bd;
}

void CBday_TS2::setUp()
{
    // set up new event on 19720926T020714
    bd = new CBdayEvent("ID12345", "John Doe", 86321234);
}

void CBday_TS2::tearDown()
{
    delete bd;
}

void CBday_TS1::test_setEBookUid()
{
    CPPUNIT_ASSERT_MESSAGE("Could not set valid UID",
                           bd->setEBookUid("UID42") == true);

    CPPUNIT_ASSERT_MESSAGE("Unexpected UID returned",
                           bd->getEBookUid() == "UID42");
}

void CBday_TS1::test_invalidsetEBookUid()
{
    CPPUNIT_ASSERT_MESSAGE("Unexpected success in setting empty UID",
                           bd->setEBookUid("") == false);
}

void CBday_TS1::test_setFirstName()
{
    CPPUNIT_ASSERT_MESSAGE("Could not set valid name",
                           bd->setFirstName("Jane Doe") == true);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected name returned",
                                 string("Jane Doe"), bd->getFirstName());
}

void CBday_TS1::test_invalidsetFirstName()
{
    CPPUNIT_ASSERT_MESSAGE("Unexpected success in setting empty name",
                           bd->setFirstName("") == false);
}

void CBday_TS1::test_setBirthDate()
{
    CPPUNIT_ASSERT_MESSAGE("Could not set valid birthday",
                           bd->setBirthDate(86321234) == true);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected birthday returned",
                                 bd->getBirthDate(), 86321234);
}

void CBday_TS1::test_invalidsetBirthDate()
{
    CPPUNIT_ASSERT_MESSAGE("Unexpected success in setting invalid birthday",
                           bd->setBirthDate(-1) == false);
}

void CBday_TS1::test_paramConstructor()
{
    CBdayEvent *cbd = new CBdayEvent("ID12345", "John Doe", 86321234);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected ID returned",
                                 string("ID12345"), cbd->getId());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected name returned",
                                 string("John Doe"), cbd->getFirstName());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected date returned",
                                 86321234, cbd->getBirthDate());

    delete cbd;
}

void CBday_TS1::test_defaultConstructor()
{
    CBdayEvent *cbd = new CBdayEvent;
    delete cbd;
}

void CBday_TS2::test_toString()
{
    bd->setAlarm(new CAlarm(10000, 1));
    bd->setRecurrence(new CRecurrence());
    bd->setDescription("Some description");
    bd->setDateStart(45678);
    bd->setDateEnd(45678);
    bd->setFlags(4);
    bd->setType(2);
    bd->setSummary("Some summary");
    bd->setLocation("Some location");
    bd->setLastModified(12345678);
    bd->setAllDay(0);
    bd->setRtype(1);
    bd->setCreatedTime(12364589);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected representation returned",
                                 string("ID=ID12345,CalendarId=0,Summary=Some summary,"
                                        "Description=Some description,Location=Some location,"
                                        "TimeZone=,UId=,Type=2,Flags=4,Status=-1,Start date=45678,"
                                        "End date=45678,Last modified=12345678,Created=12364589,"
                                        "Until=-1,All day=0,Alarm=Attach=,Trigger=10000,Repeat=1,"
                                        "Duration=1,MinutesBefore=0,Rec=Recurrence type=1,"
                                        "Recurrence ID=-1"), bd->toString());
}
void CBday_TS2::test_toString_NULL()
{
    CBdayEvent *cbd = new CBdayEvent("", "", 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected representation returned",
                                 string("ID=,CalendarId=0,Summary=,Description=,Location=,TimeZone=,"
                                        "UId=,Type=4,Flags=-1,Status=-1,Start date=0,End date=-1,"
                                        "Last modified=-1,Created=-1,Until=-1,All day=0,Alarm=NULL,"
                                        "Rec=NULL"), cbd->toString());
    delete cbd;
}
