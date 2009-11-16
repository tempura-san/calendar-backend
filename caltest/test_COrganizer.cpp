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

#include "test_COrganizer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(COrganizer_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(COrganizer_TS2);

void COrganizer_TS1::setUp()
{
	pCo= new COrganizer();
}
void COrganizer_TS1::tearDown()
{
	delete pCo;
	pCo = NULL;
}

void COrganizer_TS2::setUp()
{
	pCo= new COrganizer();
}
void COrganizer_TS2::tearDown()
{
	delete pCo;
	pCo = NULL;
}
void COrganizer_TS1::test_dummy()
{
	COrganizer p(*(pCo));
}

void COrganizer_TS2 ::test_toString()
{
	string szLa="OPAQUE";
	pCo->setLanguage(szLa);
	pCo->setSentBy(szLa);
	pCo->setXparameters(szLa);
	pCo->setCalAddress(szLa);
	pCo->setCommonName(szLa);
	pCo->setMailTo(szLa);
	pCo->setDirectoryParameter(szLa);
	cout<<pCo->toString();
	
	
}
void COrganizer_TS2 ::test_toString_NULL()
{
	string szLa="";
	pCo->setLanguage(szLa);
	pCo->setSentBy(szLa);
	pCo->setXparameters(szLa);
	pCo->setCalAddress(szLa);
	pCo->setCommonName(szLa);
	pCo->setMailTo(szLa);
	cout<<pCo->toString();
	
	
}

