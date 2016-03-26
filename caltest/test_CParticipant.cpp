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
#include <string.h>
#include <iostream>

#include "CParticipant.h"
#include "CComponentDetails.h"

#include "test_CParticipant.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CParticipant_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CParticipant_TS2);

void CParticipant_TS1::setUp()
{
    pCp = new CParticipant();
}

void CParticipant_TS1::tearDown()
{
    delete pCp;
    pCp = NULL;
}

void CParticipant_TS2::setUp()
{
    pCp = new CParticipant();
}

void CParticipant_TS2::tearDown()
{
    delete pCp;
    pCp = NULL;
}

void CParticipant_TS1::test_SetLanguage_Valid()
{
    string szTest = "OPAQUE";
    bool ret = pCp->setLanguage(szTest);
    string szRet = pCp->getLanguage();

    CPPUNIT_ASSERT_MESSAGE("Set LANGUAGE value", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Get LANGUAGE value", (szRet == szTest));
}

void CParticipant_TS1::test_SetSentBy_Valid()
{
    string szTest = "mailto:mail@example.com";
    bool ret = pCp->setSentBy(szTest);
    string szRet = pCp->getSentBy();

    CPPUNIT_ASSERT_MESSAGE("Set SENT-BY value", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Get SENT-BY value", (szRet == szTest));
}

void CParticipant_TS1::test_SetSentBy_Empty()
{
    bool ret = pCp->setSentBy("");
    CPPUNIT_ASSERT_MESSAGE("Set empty SENT-BY value", (ret == true));
}

void CParticipant_TS1::test_SetXParameters_Valid()
{
    string szTest = "OPAQUE";
    bool ret = pCp->setXparameters(szTest);
    string szRet = pCp->getXparameters();

    CPPUNIT_ASSERT_MESSAGE("Set X-PARAM value", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Get X-PARAM value", (szRet == szTest));
}

void CParticipant_TS1::test_SetXParameters_Empty()
{
    bool ret = pCp->setXparameters("");
    CPPUNIT_ASSERT_MESSAGE("Set empty X-PARAM value", (ret == true));
}

void CParticipant_TS1::test_SetDirectoryParameter_Valid()
{
    string szTest = "OPAQUE";
    bool ret = pCp->setDirectoryParameter(szTest);
    string szRet = pCp->getDirectoryParameter();

    CPPUNIT_ASSERT_MESSAGE("Set DIR value", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Get DIR value", (szRet == szTest));
}

void CParticipant_TS1::test_SetDirectoryParameter_Empty()
{
    bool ret = pCp->setDirectoryParameter("");
    CPPUNIT_ASSERT_MESSAGE("Set empty DIR value", (ret == true));
}

void CParticipant_TS1::test_SetCalAddress_Valid()
{
    string szTest = "hey";
    bool ret = pCp->setCalAddress(szTest);
    string szRet = pCp->getCalAddress();

    CPPUNIT_ASSERT_MESSAGE("Set CAL-ADDRESS value", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Get CAL-ADDRESS value", (szRet == szTest));
}

void CParticipant_TS1::test_SetCommonName_Valid()
{
    string szTest = "OPAQUE";
    bool ret = pCp->setCommonName(szTest);
    string szRet = pCp->getCommonName();

    CPPUNIT_ASSERT_MESSAGE("Set CN value", (ret == true));
    CPPUNIT_ASSERT_MESSAGE("Get CN value", (szRet == szTest));
}

void CParticipant_TS1::test_MailTo()
{
    string szTest = "mailto:mail@example.com";
    pCp->setMailTo(szTest);
    string szRet = pCp->getMailTo();

    CPPUNIT_ASSERT_MESSAGE("Get MAIL-TO value", (szRet == szTest));
}

void ValidCParticipantConstructorTestCase::setUp()
{
    p = new CParticipant("mailto:sender@example.com", "commonname", "directory", "language", "caladdress", "X-NAME:TEST", "mailto:recipient@example.com");
}

void ValidCParticipantConstructorTestCase::tearDown()
{
    delete p;
}

void ValidCParticipantConstructorTestCase::test_validCParticipantConstructor()
{
    // test constructor
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Constructor: conflict LANGUAGE value", string("language"), p->getLanguage());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Constructor: conflict CN value", string("commonname"), p->getCommonName());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Constructor: conflict CAL-ADDRESS value", string("caladdress"), p->getCalAddress());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Constructor: conflict SENT-BY value", string("mailto:sender@example.com"), p->getSentBy());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Constructor: conflict X-PARAM value", string("X-NAME:TEST"), p->getXparameters());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Constructor: conflict DIR value", string("directory"), p->getDirectoryParameter());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Constructor: conflict MAIL-TO value", string("mailto:recipient@example.com"), p->getMailTo());

    // test assignment operator
    CParticipant r;
    r = *(p);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment: conflict LANGUAGE value", string("language"), p->getLanguage());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment: conflict CN value", string("commonname"), p->getCommonName());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment: conflict CAL-ADDRESS value", string("caladdress"), p->getCalAddress());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment: conflict SENT-BY value", string("mailto:sender@example.com"), p->getSentBy());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment: conflict X-PARAM value", string("X-NAME:TEST"), p->getXparameters());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment: conflict DIR value", string("directory"), p->getDirectoryParameter());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Assignment: conflict MAIL-TO value", string("mailto:recipient@example.com"), p->getMailTo());

    // test copy constructor
    CParticipant q1(*(p));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy: conflict LANGUAGE value", string("language"), p->getLanguage());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy: conflict CN value", string("commonname"), p->getCommonName());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy: conflict CAL-ADDRESS value", string("caladdress"), p->getCalAddress());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy: conflict SENT-BY value", string("mailto:sender@example.com"), p->getSentBy());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy: conflict X-PARAM value", string("X-NAME:TEST"), p->getXparameters());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy: conflict DIR value", string("directory"), p->getDirectoryParameter());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy: conflict MAIL-TO value", string("mailto:recipient@example.com"), p->getMailTo());
}

void CParticipant_TS2 ::test_toString()
{
    pCp->setLanguage("en");
    pCp->setSentBy("mailto:jane_doe@example.com");
    pCp->setXparameters("X-NAME:TEST");
    pCp->setCalAddress("mailto:jane_doe@example.com");
    pCp->setCommonName("Jane Doe");
    pCp->setMailTo("mailto:jane_doe@example.com");
    pCp->setDirectoryParameter("ldap://example.com");

    string result = "SENT-BY=mailto:jane_doe@example.com,CN=Jane Doe,DIR=ldap://example.com,LANGUAGE=en,CAL-ADDR=mailto:jane_doe@example.com,X-PARAM=X-NAME:TEST,MAIL-TO=mailto:jane_doe@example.com";
    CPPUNIT_ASSERT_EQUAL(result, pCp->toString());
}

void CParticipant_TS2 ::test_toString_NULL()
{
    string szLa = "";
    pCp->setLanguage("");
    pCp->setSentBy("");
    pCp->setXparameters("");
    pCp->setCalAddress("");
    pCp->setCommonName("");
    pCp->setMailTo("");

    string result = "SENT-BY=NULL,CN=NULL,DIR=NULL,LANGUAGE=NULL,CAL-ADDR=NULL,X-PARAM=NULL,MAIL-TO=NULL";
    CPPUNIT_ASSERT_EQUAL(result, pCp->toString());
}
