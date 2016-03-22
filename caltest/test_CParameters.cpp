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

#include "test_CParameters.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(CParameters_TS1);
CPPUNIT_TEST_SUITE_REGISTRATION(CParameters_TS2);

void CParameters_TS1::setUp()
{
    pParam = new CParameters();
}
void CParameters_TS1::tearDown()
{
    delete pParam;
    pParam = NULL;
}
void CParameters_TS2::setUp()
{
    pParam = new CParameters();
}
void CParameters_TS2::tearDown()
{
    delete pParam;
    pParam = NULL;
}

void CParameters_TS1::test_setParamName_Valid()
{
    string szParamName = "LANGUAGE";
    pParam->setParamName("LANGUAGE");

    CPPUNIT_ASSERT_MESSAGE("Error :setting paramName", pParam->getParamName() == szParamName);
}
void CParameters_TS1::test_setParamValue_ValidInt()
{
    ParamType uType;
    ParamType uRetType;
    uType.i = 10;
    pParam->setParamValue(uType);
    uRetType = pParam->getParamValue();

    CPPUNIT_ASSERT_MESSAGE("Error :setting paramvalue", uRetType.i == 10);
}
void CParameters_TS1::test_setParamValue_ValidString()
{
    ParamType uType;
    ParamType uRetType;
    uType.szString = "STRING TYPE";
    pParam->setParamValue(uType);
    uRetType = pParam->getParamValue();

    CPPUNIT_ASSERT_MESSAGE("Error :setting paramvalue", uRetType.szString == "STRING TYPE");
}
void CParameters_TS1::test_getDataType_Valid()
{
    string szParam = "LANGUAGE";
    CPPUNIT_ASSERT_MESSAGE("Error :getting parameter data type", pParam->getDataType(szParam) == 0);
}
void CParameters_TS1::test_getDataType_ValidX()
{
    string szParam = "X-ABC";
    CPPUNIT_ASSERT_MESSAGE("Error :getting parameter data type", pParam->getDataType(szParam) == 0);
    CPPUNIT_ASSERT_MESSAGE("Error :getting parameter data type", pParam->getDataType("RELTYPE") == 1);
}
void CParameters_TS1::test_getDataType_InValid()
{
    string szParam = "ABC";
    CPPUNIT_ASSERT_MESSAGE("Error :getting parameter data type", pParam->getDataType(szParam) == 4);
}
void   CParameters_TS1 :: test_validCopyConstructor()
{
    CParameters param(*pParam);
    CPPUNIT_ASSERT_MESSAGE("Error: test_validCParametersCopyConstructor",
                           strcmp((pParam->getParamName().c_str()), (param.getParamName().c_str())) == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: test_validCParametersCopyConstructor",
                           (pParam->getParamValue().i == param.getParamValue().i));
}
void  CParameters_TS1 :: test_validAssignementOperator()
{
    CParameters param;
    param = *(pParam);
    CPPUNIT_ASSERT_MESSAGE("Error: test_validCParametersAssignmentConstructor",
                           strcmp((pParam->getParamName().c_str()), (param.getParamName().c_str())) == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: test_validCParametersAssignmentConstructor",
                           (pParam->getParamValue().i == param.getParamValue().i));
}

void CParameters_TS2::test_toString()
{
    pParam->setParamName("LANGUAGE");
    cout << pParam->toString();
}
void CParameters_TS2::test_toString_NULL()
{
    pParam->setParamName("");
    cout << pParam->toString();
}

