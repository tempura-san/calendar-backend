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

/** header files
 * used in this file 
 */

#include "Common.h"
#include <map>
using std::map;


/**
 * CParameters
 * Default constructor for CParameters
 */
CParameters::CParameters()
{
}

/**
 * overloaded assignment operator
 */
const CParameters & CParameters::operator=(const CParameters & right)
{
    if (&right != this)        //prevent self assignment
    {
    szParamName = right.szParamName;
    valueType = right.valueType;
    }
    return *this;
}

/**
 * CParameters copy constructor
 */
CParameters::CParameters(CParameters & ref)
{
    szParamName = ref.szParamName;
    valueType = ref.valueType;
}

/**
 * setParamName()
 */
bool CParameters::setParamName(string szParamName)
{
    this->szParamName = szParamName;
    return true;
}

/**
 * getParamname()
 */
string CParameters::getParamName()
{
    return this->szParamName;
}

/**
 * setParamValue()
 */
bool CParameters::setParamValue(ParamType valueType)
{
    this->valueType = valueType;
    return true;
}

/**
 * getParamValue()
 */
ParamType CParameters::getParamValue()
{
    return this->valueType;
}

int CParameters::getDataType(string szPropParam)
{
    if ((szPropParam == "CUTYPE") ||
    (szPropParam == "ROLE") ||
    (szPropParam == "PSTAT") || (szPropParam == "RSVP"))
    return INTEGER;

    if ((szPropParam == "MEMBER") ||
    (szPropParam == "DELEGATEES") ||
    (szPropParam == "DELEGATOR") ||
    (szPropParam == "SENTBY") ||
    (szPropParam == "CNAME") ||
    (szPropParam == "DIRPARAM") ||
    (szPropParam == "MAILTO") ||
    (szPropParam == LANGUAGE) ||
    (szPropParam == ALTER) || (szPropParam == FMTYPE))
    return STRING;
    if (szPropParam == RELTYPE)
    return INTEGER;


    /* Default type for all X -param and Prop is STRING */
    int found;
    found = szPropParam.find("X-", 0);

    if (found != (int)string::npos)
    return STRING;
    else
    return NONE_TYPE;
}


/**
 * ~CParameters
 * Default destructor for CParameters
 */
CParameters::~CParameters()
{
}


string CParameters::toString()
{
    string szRet;
    string szTemp;
    std::stringstream ss;
    ss << "Name=";
    if (szParamName.c_str()){
	szTemp= szParamName.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    szRet.append(",Type=");
    ss <<valueType.i;
    if (valueType.szString.c_str()){
	szTemp= valueType.szString.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    szRet.append(ss.str());

    szRet.append(ss.str());
    return szRet;
}
