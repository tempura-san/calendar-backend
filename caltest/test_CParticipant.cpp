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
	pCp= new CParticipant();
}
void CParticipant_TS1::tearDown()
{
	delete pCp;
	pCp = NULL;
}
void CParticipant_TS2::setUp()
{
	pCp= new CParticipant();
}
void CParticipant_TS2::tearDown()
{
	delete pCp;
	pCp = NULL;
}

void CParticipant_TS1 ::test_SetLanguage_Valid()
{
	string szLa="OPAQUE";
	bool ret = pCp->setLanguage(szLa);
	string szRet=pCp->getLanguage();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(szRet==szLa));
}

void CParticipant_TS1 ::test_SetSentBy_Valid()
{
	string szSent="OPAQUE";
	bool ret = pCp->setSentBy(szSent);
	string szRet=pCp->getSentBy();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(szRet == szSent));
}

void CParticipant_TS1 ::test_SetSentBy_Empty()
{
	bool bRet=pCp->setSentBy("");
	CPPUNIT_ASSERT_MESSAGE("Error : setting the empty value",(bRet==true));
}

void CParticipant_TS1 ::test_SetXParameters_Valid()
{
	string szTransp="OPAQUE";
	bool ret = pCp->setXparameters(szTransp);
	string szRet=pCp->getXparameters();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",(szTransp==szRet));
}

void CParticipant_TS1 ::test_SetXParameters_Empty()
{
	bool bRet=pCp->setXparameters("");
	CPPUNIT_ASSERT_MESSAGE("Error : setting the NULL value",(bRet==true));
}

void CParticipant_TS1 ::test_SetDirectoryParameter_Valid()
{
	string szTransp="OPAQUE";
	bool ret = pCp->setDirectoryParameter(szTransp);
	string szRet=pCp->getDirectoryParameter();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",szTransp == szRet);
}

void CParticipant_TS1 ::test_SetDirectoryParameter_Empty()
{
	bool bRet=pCp->setDirectoryParameter("");
	CPPUNIT_ASSERT_MESSAGE("Error : setting the NULL value",(bRet==true));
}

void CParticipant_TS1 ::test_SetCalAddress_Valid()
{
	string szHey= "hey";
	bool ret = pCp->setCalAddress(szHey);
	string szRet=pCp->getCalAddress();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",szRet == szHey); 
}

void CParticipant_TS1 ::test_SetCommonName_Valid()
{
	string szCom="OPAQUE";
	bool ret = pCp->setCommonName(szCom);
	string szRet=pCp->getCommonName();
	CPPUNIT_ASSERT_MESSAGE("Error : setting value",(ret==true));
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",szRet == szCom);
}

void CParticipant_TS1 ::test_MailTo()
{
	string szCom="mailto";
	pCp->setMailTo(szCom);
	string szRet=pCp->getMailTo();
	CPPUNIT_ASSERT_MESSAGE("Error : returning the wrong value",szRet == szCom);
}

void ValidCParticipantConstructorTestCase::setUp()
{
	p= new CParticipant("sentBy","commonName","directoryParameter","language","calAddress", "cXparameter","email@email.com");
}

void ValidCParticipantConstructorTestCase::tearDown()
{
	delete p;
}

void ValidCParticipantConstructorTestCase::test_validCParticipantConstructor()
{
	CPPUNIT_ASSERT_MESSAGE("conflict language",(strcmp((p->getLanguage().c_str()),"language")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict commonname",(strcmp((p->getCommonName().c_str()),"commonName")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict calAddress",(strcmp((p->getCalAddress().c_str()),"calAddress")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict sentby",(strcmp((p->getSentBy().c_str()),"sentBy")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict cXparameter",(strcmp((p->getXparameters().c_str()),"cXparameter")==0));
	
	CPPUNIT_ASSERT_MESSAGE("conflict directoryParameter",(strcmp((p->getDirectoryParameter().c_str()),"directoryParameter")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict email",(strcmp((p->getMailTo().c_str()),"email@email.com")==0));
	
	CParticipant r;
	r= *(p);
	CPPUNIT_ASSERT_MESSAGE("conflict language",(strcmp((r.getLanguage().c_str()),"language")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict commonname",(strcmp((r.getCommonName().c_str()),"commonName")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict calAddress",(strcmp((r.getCalAddress().c_str()),"calAddress")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict sentby",(strcmp((r.getSentBy().c_str()),"sentBy")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict cXparameter",(strcmp((r.getXparameters().c_str()),"cXparameter")==0));
	
	CPPUNIT_ASSERT_MESSAGE("conflict directoryParameter",(strcmp((r.getDirectoryParameter().c_str()),"directoryParameter")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict email",(strcmp((r.getMailTo().c_str()),"email@email.com")==0));
	CParticipant q1(*(p)); 
	CPPUNIT_ASSERT_MESSAGE("conflict language",(strcmp((q1.getLanguage().c_str()),"language")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict commonname",(strcmp((q1.getCommonName().c_str()),"commonName")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict calAddress",(strcmp((q1.getCalAddress().c_str()),"calAddress")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict sentby",(strcmp((q1.getSentBy().c_str()),"sentBy")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict cXparameter",(strcmp((q1.getXparameters().c_str()),"cXparameter")==0));
	
	CPPUNIT_ASSERT_MESSAGE("conflict directoryParameter",(strcmp((q1.getDirectoryParameter().c_str()),"directoryParameter")==0));
	CPPUNIT_ASSERT_MESSAGE("conflict email",(strcmp((q1.getMailTo().c_str()),"email@email.com")==0));
}


void CParticipant_TS2 ::test_toString()
{
	string szLa="OPAQUE";
	pCp->setLanguage(szLa);
	pCp->setSentBy(szLa);
	pCp->setXparameters(szLa);
	pCp->setCalAddress(szLa);
	pCp->setCommonName(szLa);
	pCp->setMailTo(szLa);
	pCp->setDirectoryParameter(szLa);
	cout<<pCp->toString();
	
	
}
void CParticipant_TS2 ::test_toString_NULL()
{
	string szLa="";
	pCp->setLanguage(szLa);
	pCp->setSentBy(szLa);
	pCp->setXparameters(szLa);
	pCp->setCalAddress(szLa);
	pCp->setCommonName(szLa);
	pCp->setMailTo(szLa);
	cout<<pCp->toString();
	
	
}

