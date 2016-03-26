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

#include "CParticipant.h"
#include "Common.h"

bool CParticipant::setLanguage(string szLanguage)
{
    this->szLanguage = szLanguage;
    return true;
}

string CParticipant::getLanguage()
{
    return szLanguage;
}

bool CParticipant::setSentBy(string szSentBy)
{
    // must start with "mailto:" or be empty
    if((szSentBy.find("mailto:") == 0) || (szSentBy.empty())) {
        this->szSentBy = szSentBy;
        return true;
    }
    else {
        return false;
    }
}

string CParticipant::getSentBy()
{
    return szSentBy;
}

bool CParticipant::setCommonName(string szCommonName)
{
    this->szCommonName = szCommonName;
    return true;
}

void CParticipant::setMailTo(string szMailTo)
{
    this->szMailTo = szMailTo;
}

string CParticipant::getMailTo()
{
    return this->szMailTo;
}

bool CParticipant::setXparameters(string szCXparameter)
{
    this->szCXparameter = szCXparameter;
    return true;
}

bool CParticipant::setCalAddress(string szCalAddress)
{
    this->szCalAddress = szCalAddress;
    return true;
}

bool CParticipant::setDirectoryParameter(string szDirectoryParameter)
{
    this->szDirectoryParameter = szDirectoryParameter;
    return true;
}

string CParticipant::getDirectoryParameter()
{
    return szDirectoryParameter;
}

string CParticipant::getCalAddress()
{
    return szCalAddress;
}

string CParticipant::getXparameters()
{
    return szCXparameter;
}

string CParticipant::getCommonName()
{
    return szCommonName;
}

CParticipant::CParticipant(CParticipant &Cparef)
{
    szSentBy = Cparef.szSentBy;
    szCommonName = Cparef.szCommonName;
    szDirectoryParameter = Cparef.szDirectoryParameter;
    szLanguage = Cparef.szLanguage;
    szCalAddress = Cparef.szCalAddress;
    szCXparameter = Cparef.szCXparameter;
    szMailTo = Cparef.szMailTo;
}

const CParticipant &CParticipant::operator=(const CParticipant &right)
{
    // check for self assignment
    if(&right != this) {
        szSentBy = right.szSentBy;
        szCommonName = right.szCommonName;
        szDirectoryParameter = right.szDirectoryParameter;
        szLanguage = right.szLanguage;
        szCalAddress = right.szCalAddress;
        szCXparameter = right.szCXparameter;
        szMailTo = right.szMailTo;
    }

    return *this;
}

CParticipant::CParticipant()
{
}

CParticipant::CParticipant(string szSentBy, string szCommonName, string szDirectoryParameter, string szLanguage, string szCalAddress, string szCXparameter, string szMailTo): szSentBy(szSentBy), szCommonName(szCommonName),
    szDirectoryParameter(szDirectoryParameter), szLanguage(szLanguage),
    szCalAddress(szCalAddress), szCXparameter(szCXparameter),
    szMailTo(szMailTo)
{
}

CParticipant::~CParticipant()
{
}

string CParticipant::toString()
{
    std::stringstream ss;

    ss << "SENT-BY=";

    if(!szSentBy.empty()) {
        ss << szSentBy.substr(0, 100);
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",CN=";

    if(!szCommonName.empty()) {
        ss << szCommonName.substr(0, 100);
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",DIR=";

    if(!szDirectoryParameter.empty()) {
        ss << szDirectoryParameter.substr(0, 100);
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",LANGUAGE=";

    if(!szLanguage.empty()) {
        ss << szLanguage.substr(0, 100);
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",CAL-ADDR=";

    if(!szCalAddress.empty()) {
        ss << szCalAddress.substr(0, 100);
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",X-PARAM=";

    if(!szCXparameter.empty()) {
        ss << szCXparameter.substr(0, 100);
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",MAIL-TO=";

    if(!szMailTo.empty()) {
        ss << szMailTo.substr(0, 100);
    }
    else {
        ss << NULL_STRING;
    }

    return ss.str();
}

