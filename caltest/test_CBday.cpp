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

#include "test_CBday.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CBday_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CBday_TS2);

void CBday_TS1::setUp()
{
    bd = new CBdayEvent("qw123", "qw123", 86321234);
}

void CBday_TS1::tearDown()
{
    delete bd;
}



void CBday_TS2::setUp()
{
    bd = new CBdayEvent("qw123", "qw123", 86321234);
}

void CBday_TS2::tearDown()
{
    delete bd;
}

void CBday_TS1::test_setEBookUid()
{
    bool ret = bd->setEBookUid("qw123");
    CPPUNIT_ASSERT_MESSAGE("Error: Setting id", (ret == true));
    string id = bd->getEBookUid();
    CPPUNIT_ASSERT_MESSAGE("Error: getting id", (id == "qw123"));
}

void CBday_TS1::test_invalidsetEBookUid()
{
    bool ret = bd->setEBookUid("");
    CPPUNIT_ASSERT_MESSAGE("Error: Setting id", (ret == false));
}

void CBday_TS1::test_setFirstName()
{
    bool ret = bd->setFirstName("qw123");
    CPPUNIT_ASSERT_MESSAGE("Error: Setting FirstName", (ret == true));
    string id = bd->getFirstName();
    CPPUNIT_ASSERT_MESSAGE("Error: getting FirstName", (id == "qw123"));

}

void CBday_TS1::test_invalidsetFirstName()
{
    bool ret = bd->setFirstName("");
    CPPUNIT_ASSERT_MESSAGE("Error: Setting FirstName empty", (ret == false));
}

void CBday_TS1::test_setBirthDate()
{
    bool ret = bd->setBirthDate(86321234);
    CPPUNIT_ASSERT_MESSAGE("Error: Setting BirthDate", (ret == true));
    int dt =  bd->getBirthDate();
    CPPUNIT_ASSERT_MESSAGE("Error: getting BirthDate", (dt == 86321234));
}

void CBday_TS1::test_invalidsetBirthDate()
{
    bool ret = bd->setBirthDate(-1);
    CPPUNIT_ASSERT_MESSAGE("Error: Setting BirthDate", (ret == false));
}
void CBday_TS1 :: test_defaultConstructor()
{
    CBdayEvent *cbd = new CBdayEvent;
    delete cbd;
}


void CBday_TS2::test_toString()
{
    bd = new CBdayEvent("qw1", "qw13", 86321234);
    CAlarm *alarm = new CAlarm(10000, 1);
    bool ret = bd->setAlarm(alarm);
    CRecurrence *rec = new CRecurrence();
    ret = bd->setRecurrence(rec);

    int duration = bd->getDuration();
    string  szDes = "mails";
    bd->setDescription(szDes);
    bd->setDateStart(45678);
    bd->setDateEnd(45678);
    bd->setFlags(4);
    bd->setType(2);
    bd->setSummary("Summary");
    bd->setLocation("LOC");
    bd->setLastModified(12345678);
    bd->setAllDay(0);
    bd->setRtype(1);
    bd->setCreatedTime(12364589);

    string set = "qwerty";
    ret = bd->setId(set);

    cout << bd->toString();
}
void CBday_TS2::test_toString_NULL()
{
    string qw = "", qe = "";
    bd = new CBdayEvent(qw, qe, 86321234);
    cout << bd->toString();
}

