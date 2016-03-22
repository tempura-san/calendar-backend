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

#include "test_CProperties.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CProperties_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CProperties_TS2);
void CProperties_TS1::setUp()
{
    pProp = new CProperties();
}
void CProperties_TS1::tearDown()
{
    delete pProp;
    pProp = NULL;
}
void CProperties_TS2::setUp()
{
    pProp = new CProperties();
}
void CProperties_TS2::tearDown()
{
    delete pProp;
    pProp = NULL;
}

void CProperties_TS1::test_copyConstructor()
{
    string szPropName = "SUMMARY";
    pProp->setPropName("SUMMARY");
    CProperties prop(*pProp);
    CPPUNIT_ASSERT_MESSAGE("Error: copy constructor", pProp->getPropName() == prop.getPropName());
}
void CProperties_TS1::test_setPropName_Valid()
{
    string szPropName = "SUMMARY";
    pProp->setPropName("SUMMARY");

    CPPUNIT_ASSERT_MESSAGE("Error :setting propName", pProp->getPropName() == szPropName);
}
void CProperties_TS1::test_setPropValue_ValidInt()
{
    PropType uType;
    PropType uRetType;
    uType.i = 10;
    pProp->setPropValue(uType);
    uRetType = pProp->getPropValue();

    CPPUNIT_ASSERT_MESSAGE("Error :setting propvalue", uRetType.i == 10);
}
void CProperties_TS1::test_setPropValue_ValidString()
{
    PropType uType;
    PropType uRetType;
    uType.szString = "STRING TYPE";
    pProp->setPropValue(uType);
    uRetType = pProp->getPropValue();

    CPPUNIT_ASSERT_MESSAGE("Error :setting propvalue", uRetType.szString == "STRING TYPE");
}
void CProperties_TS1::test_getDataType_Valid()
{
    string szProp = "SUMMARY";
    CPPUNIT_ASSERT_MESSAGE("Error :getting property data type", pProp->getDataType(szProp) == 0);
    szProp = "DESCRIPTION";
    CPPUNIT_ASSERT_MESSAGE("Error :getting property data type", pProp->getDataType(szProp) == 0);
}
void CProperties_TS1::test_getDataType_ValidX()
{
    string szProp = "X-ABC";
    CPPUNIT_ASSERT_MESSAGE("Error :getting property data type", pProp->getDataType(szProp) == 0);
}
void CProperties_TS1::test_getDataType_InValid()
{
    string szProp = "ABC";
    CPPUNIT_ASSERT_MESSAGE("Error :getting property data type", pProp->getDataType(szProp) == 4);
}

void CProperties_TS2::test_toString()
{
    pProp->setPropName("SUMMARY");
    cout << pProp->toString();

}
void CProperties_TS2::test_toString_NULL()
{
    pProp->setPropName("");

    cout << pProp->toString();
}

