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

#include "test_CAttendee.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CAttendee_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CAttendee_TS2);

void CAttendee_TS1::setUp()
{
    pCa = new CAttendee();
}

void CAttendee_TS1::tearDown()
{
    delete pCa;
}

void CAttendee_TS2::setUp()
{
    pCa = new CAttendee();
}

void CAttendee_TS2::tearDown()
{
    delete pCa;
}

void CAttendee_TS1::test_SetCalendarUserType_Valid()
{
    int szRet;
    pCa->setCalendarUserType((CalendarUserType)4);
    szRet = pCa->getCalendarUserType();

    CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value", (szRet == 4));
}


void CAttendee_TS1::test_SetMember_Valid()
{
    string szRet;
    string szMember = "iuiu";
    pCa->setMember(szMember);
    szRet = pCa->getMember();
    CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value", (szRet == "iuiu"));
}

void CAttendee_TS1::test_SetMember_Empty()
{
    string szMember = "";
    bool bRet = pCa->setMember(szMember);

    //Empty param allowed for this
    CPPUNIT_ASSERT_MESSAGE("Error :setting empty value", (bRet == true));
}

void CAttendee_TS1::test_SetRole_Valid()
{
    pCa->setRole(CHAIR);
    ParticipantRole szRet = pCa->getRole();
    CPPUNIT_ASSERT_MESSAGE("Error :setting proper value", (szRet == CHAIR));
}

void CAttendee_TS1::test_SetParticipationStatus_Valid()
{
    pCa->setParticipationStatus(ACCEPTED);
    ParticipantStatus szRet = pCa->getParticipationStatus();
    CPPUNIT_ASSERT_MESSAGE("Error :setting proper value", (szRet == ACCEPTED));
}

void CAttendee_TS1::test_SetDelegator_Valid()
{
    string szCalendarUserType = "iuiu";
    pCa->setDelegator(szCalendarUserType);
    string szRet = pCa->getDelegator();

    CPPUNIT_ASSERT_MESSAGE("Error :setting proper value", (szRet == szCalendarUserType));
}

void CAttendee_TS1::test_SetDelegator_Empty()
{
    string szMember = "";
    bool bRet = pCa->setDelegator(szMember);
    CPPUNIT_ASSERT_MESSAGE("Error :setting empty value", (bRet == true));
}

void CAttendee_TS1::test_SetDelegatees_Valid()
{
    string szDelegatees = "iuiu";
    pCa->setDelegatees(szDelegatees);
    string szRet = pCa->getDelegatees();
    CPPUNIT_ASSERT_MESSAGE("Error :setting NULL value", (szRet == szDelegatees));
}

void CAttendee_TS1::test_SetDelegatees_Empty()
{
    string szMember = "";
    bool bRet = pCa->setDelegatees(szMember);
    CPPUNIT_ASSERT_MESSAGE("Error :setting empty value", (bRet == true));
}

void CAttendee_TS1::test_SetRSVP_true()
{
    pCa->setRSVP(true);
    bool bRet = pCa->getRSVP();
    CPPUNIT_ASSERT_MESSAGE("Error :right params value", (bRet == true));
}

void CAttendee_TS1::test_SetRSVP_false()
{
    pCa->setRSVP(false);
    bool bRet = pCa->getRSVP();
    CPPUNIT_ASSERT_MESSAGE("Error :right params value", (bRet == false));
}

void CAttendee_TS1::test_SetLanguage_Valid()
{
    string szTransp = "OPAQUE";
    pCa->setLanguage(szTransp);
    string szRet = pCa->getLanguage();
    CPPUNIT_ASSERT_MESSAGE("Error :returning the wrong value", (szRet == szTransp));
}

void CAttendee_TS1 ::test_SetSentBy_Valid()
{
    string szTransp = "OPAQUE";
    pCa->setSentBy(szTransp);
    string szRet = pCa->getSentBy();

    CPPUNIT_ASSERT_MESSAGE("Error :returning the wrong value", (szRet == szTransp));
}

void CAttendee_TS1 ::test_SetSentBy_Empty()
{
    string szGeo1 = "";
    bool bRet = pCa->setSentBy(szGeo1);
    CPPUNIT_ASSERT_MESSAGE("Error : setting the empty value", (bRet == true));
}

void CAttendee_TS1 ::test_SetXParameters_Valid()
{
    string szTransp = "OPAQUE";
    pCa->setXparameters(szTransp);
    string szRet = pCa->getXparameters();
    CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value", (szRet == szTransp));
}

void CAttendee_TS1 ::test_SetXParameters_Empty()
{
    string szGeo1 = "";
    bool bRet = pCa->setXparameters(szGeo1);
    CPPUNIT_ASSERT_MESSAGE("Error : setting the empty value", (bRet == true));
}


void CAttendee_TS1 ::test_SetDirectoryParameter_Valid()
{
    string szTransp = "OPAQUE";
    pCa->setDirectoryParameter(szTransp);
    string szRet = pCa->getDirectoryParameter();

    CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value", (szRet == szTransp));
}

void CAttendee_TS1 ::test_SetDirectoryParameter_Empty()
{
    string szGeo1 = "";
    bool bRet = pCa->setDirectoryParameter(szGeo1);
    CPPUNIT_ASSERT_MESSAGE("Error : setting the empty value", (bRet == true));
}

void CAttendee_TS1 ::test_SetCalAddress_Valid()
{
    string szHey = "test";
    pCa->setCalAddress(szHey);
    string szRet = pCa->getCalAddress();
    CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value", (szRet == szHey));
}

void CAttendee_TS1 ::test_SetCalAddress_Empty()
{
    string szGeo1 = "";
    bool bRet = pCa->setCalAddress(szGeo1);
    CPPUNIT_ASSERT_MESSAGE("Error : setting the empty value", (bRet == true));
}

void CAttendee_TS1 ::test_SetCommonName_Valid()
{
    string szTrans = "OPAQUE";
    pCa->setCommonName(szTrans);
    string szRet = pCa->getCommonName();
    CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value", (szRet == szTrans));
}

void CAttendee_TS1 ::test_SetCommonName_Empty()
{
    string geo1 = "";
    bool bRet = pCa->setCommonName(geo1);
    CPPUNIT_ASSERT_MESSAGE("Error : setting the empty value", (bRet == false));
}

void ValidCAttendeeConstructorTestCase::setUp()
{
    p = new CAttendee();
}

void ValidCAttendeeConstructorTestCase::tearDown()
{
    delete p;
}

void ValidCAttendeeConstructorTestCase::test_validCAttendeeConstructor()
{
    CAttendee q(*(p));
    CAttendee r ;
    r = *(p);
}

void CAttendee_TS2::test_toString()
{
    string szRet;
    string szLa = "OPAQUE";
    pCa->setLanguage(szLa);
    pCa->setSentBy(szLa);
    pCa->setXparameters(szLa);
    pCa->setCalAddress(szLa);
    pCa->setCommonName(szLa);
    pCa->setMailTo(szLa);
    pCa->setDirectoryParameter(szLa);
    pCa->setCalendarUserType((CalendarUserType)4);
    pCa->setRole(CHAIR);
    pCa->setParticipationStatus(ACCEPTED);
    string szMember = "iuiu";
    pCa->setMember(szMember);
    string szTrans = "OPAQUE";
    pCa->setCommonName(szTrans);
    string szTransp = "OPAQUE";
    pCa->setXparameters(szTransp);
    string szHey = "test";
    pCa->setCalAddress(szHey);
    pCa->setRSVP(true);
    cout << pCa->toString();
}

void CAttendee_TS2::test_toString_NULL()
{
    string szRet;
    string szMember = "";
    pCa->setMember(szMember);


    string szTrans = "";
    pCa->setCommonName(szTrans);


    string szTransp = "";
    pCa->setXparameters(szTransp);


    string szHey = "";
    pCa->setCalAddress(szHey);


    pCa->setRSVP(true);
    pCa->getRSVP();

    cout << pCa->toString();
}

