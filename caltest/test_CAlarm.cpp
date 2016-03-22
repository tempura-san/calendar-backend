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
#include <libalarm.h>

#include "test_CAlarm.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CAlarm_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CAlarm_TS2);
using namespace std;

void CAlarm_TS1::setUp()
{
    pCa = new CAlarm();
}

void CAlarm_TS1::tearDown()
{
    delete pCa;
    pCa = NULL;
}
void CAlarm_TS2::setUp()
{
    pCa1 = new CAlarm();
}

void CAlarm_TS2::tearDown()
{
    delete pCa1;
    pCa1 = NULL;
}


void CAlarm_TS1::test_setTrigger_Valid()
{
    int iTrig = 102345;
    pCa->setTrigger(iTrig);
    CPPUNIT_ASSERT_MESSAGE("Error : Failed to set trigger  ", (pCa->getTrigger() == iTrig));
}

void CAlarm_TS1 :: test_setTrigger_Invalid()
{
    pCa->setTrigger(-1);

    CPPUNIT_ASSERT_MESSAGE("Error: Trying to set invalid ID", pCa->getTrigger() != -1);
}

void CAlarm_TS1::test_setRepeat_Valid()
{
    int iRep = 5;
    pCa->setRepeat(iRep);
    CPPUNIT_ASSERT_MESSAGE("Error : Failed to set Repeat  ", (pCa->getRepeat() == iRep));
}

void CAlarm_TS1 :: test_setRepeat_Invalid()
{
    pCa->setRepeat(-10);
    CPPUNIT_ASSERT_MESSAGE("Error: Trying to set invalid Repeat negative!!", (pCa->getRepeat() != -10));
}

void CAlarm_TS1::test_setDuration_Valid()
{
    int iDur = 3;
    pCa->setDuration(iDur);
    CPPUNIT_ASSERT_MESSAGE("Error : Failed to set Duration  ", (pCa->getDuration() == iDur));
}

void CAlarm_TS1 :: test_setDuration_Invalid()
{
    pCa->setDuration(-1);
    CPPUNIT_ASSERT_MESSAGE("Error: Trying to set invalid Duration < 0!", (pCa->getDuration() != -1));
}
void CAlarm_TS1 :: test_setAction_Valid()
{
    int iAct = 1;
    pCa->setAction(iAct);
    CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Action ", (pCa->getAction() == iAct));
}

//TODO::Action has to be made a enum type. Then invalid test cases for setAction() can be added
void CAlarm_TS1::test_setCookie_Valid()
{
    vector <long> izCookieList;
    vector <long> izRetCookieList;

    cookie_t arg1 = 10000000;
    cookie_t arg2 = 20000000;
    cookie_t arg3 = 30000000;
    izCookieList.push_back(arg1);
    izCookieList.push_back(arg2);
    izCookieList.push_back(arg3);

    pCa->setCookie(izCookieList);
    izRetCookieList = pCa->getCookie();

    CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Cookies", (izCookieList.size() == izRetCookieList.size()));
    CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Cookies", (izCookieList[0] == izRetCookieList[0]));
    CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Cookies", (izCookieList[1] == izRetCookieList[1]));
    CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Cookies", (izCookieList[2] == izRetCookieList[2]));
}

void CAlarm_TS1::test_setCookie_Invalid()
{
    vector <long> izCookieList;
    bool bRet = pCa->setCookie(izCookieList);

    CPPUNIT_ASSERT_MESSAGE("Error: Trying to set invalid Cookies empty!!", (bRet == true));
}

void CAlarm_TS1 :: test_setAttach_Valid()
{
    string szS = "file://hime/user/attach.txt";
    pCa->setAttach(szS);

    string szRet = pCa->getAttach();

    CPPUNIT_ASSERT_MESSAGE("Error: Failed to set Attachment", (szS == szRet));
}

void CAlarm_TS1 :: test_setAttach_Empty()
{
    string szS = "";
    bool bRet = pCa->setAttach(szS);
    CPPUNIT_ASSERT_MESSAGE("Error: Trying to set empty!!  Attachment", (bRet == true));
}

void CAlarm_TS1 :: test_constructor()
{
    string szAttach = "qwerty";
    CAlarm *a1 = new CAlarm(10, 1, 1000, 1, szAttach);
    CAlarm a2(*(a1));

    CPPUNIT_ASSERT_MESSAGE("Error: Geting value Trigger", a2.getTrigger() == 10);
    CPPUNIT_ASSERT_MESSAGE("Error: Geting value Repeat", a2.getRepeat() == 1);
    CPPUNIT_ASSERT_MESSAGE("Error: Geting value Duration", a2.getDuration() == 1000);
    CPPUNIT_ASSERT_MESSAGE("Error: Geting value Action", a2.getAction() == 1);
    CPPUNIT_ASSERT_MESSAGE("Error: Geting value Action", a2.getAttach() == szAttach);
}

void CAlarm_TS2 :: test_toString()
{
    string szAttach = "qwerty";
    CAlarm *a1 = new CAlarm(10, 1, 1000, 1, szAttach);
    vector <long> izCookieList;
    cookie_t arg1 = 10000000;
    cookie_t arg2 = 20000000;
    cookie_t arg3 = 30000000;
    izCookieList.push_back(arg1);
    izCookieList.push_back(arg2);
    izCookieList.push_back(arg3);

    a1->setCookie(izCookieList);

    cout << a1->toString();

}
void CAlarm_TS2 :: test_toString_NULL()
{
    string szAttach = "";
    CAlarm *a1 = new CAlarm(10, 1, 1000, 1, szAttach);
    cout << a1->toString();

}

