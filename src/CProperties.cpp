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

/**
 *  header files 
 */

#include "Common.h"
#include <sstream>

/**
 * CProperties
 * Default constructor for CProperties
 */
CProperties::CProperties()
{
    /** since it is union  
     * it is suficient if we 
     * initialize the biggest value */
 valueType.i = 0;
}

/**
 * overloaded assignment operator
 */
const CProperties & CProperties::operator=(const CProperties & right)
{
    if (&right != this)        //prevent self assignment
    {
	szPropName = right.szPropName;
	valueType = right.valueType;
    }
    return *this;
}

/**
 * CProperties copy constructor
 */
CProperties::CProperties(CProperties & ref)
{
    szPropName = ref.szPropName;
    valueType = ref.valueType;
}

/**
 * setPropName()
 */
bool CProperties::setPropName(string szPropName)
{
    this->szPropName = szPropName;
    return true;
}

/**
 * getPropname()
 */
string CProperties::getPropName()
{
    return this->szPropName;
}

/**
 * setPropValue()
 */
bool CProperties::setPropValue(PropType valueType)
{
    this->valueType = valueType;
    return true;
}

/**
 * getPropValue()
 */
PropType CProperties::getPropValue()
{
    return this->valueType;
}

int CProperties::getDataType(string szProp)
{
    if (szProp == DESCRIPTION)
    return STRING;
    if (szProp == SUMMARY)
    return STRING;

    /* Default type for all X -param and Prop is STRING */
    int found;
    found = szProp.find("X-", 0);

    if (found != (int)string::npos)
    return STRING;
    else
    return NONE_TYPE;
}

CProperties::~CProperties()
{
}

string CProperties::toString()
{
    string szRet;
    string szTemp;
    std::stringstream ss;
    ss << "PropName=";
    if ((szPropName).c_str()){
	szTemp= szPropName.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    szRet.append(",Type=");
    ss <<valueType.i;
    if ((valueType.szString).c_str()){
	szTemp= valueType.szString.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    szRet.append(ss.str());
    return szRet;
}

