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
 * Header files
 */

#include "COrganizer.h"
#include "Common.h"
/**
 * COrganizer
 * Default constructor for COrganizer class
 */
COrganizer::COrganizer(): CParticipant()
{
}

/**
 * COrganizer
 * copy constructor for Corganizer
 */
COrganizer::COrganizer(COrganizer &ref): CParticipant(ref)
{
}


/**
 * Overloaded assignment operator
 */
COrganizer &COrganizer::operator=(COrganizer &right)
{
    if(&right != this) {
        CParticipant::operator=(right);
    }

    return *this;
}


/**
 * ~COrganizer
 * destructor for COrganizer class
 */
COrganizer::~COrganizer()
{
}

string COrganizer::toString()
{
    string szRet;
    string szTemp;
    std::stringstream ss;
    ss << "SentBy=";

    if(getSentBy().c_str()) {
        szTemp = getSentBy().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",CommonName=";

    if(getCommonName().c_str()) {
        szTemp = getCommonName().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    szRet.append(",Directory=");

    if(getDirectoryParameter().c_str()) {
        szTemp = getDirectoryParameter().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",Lang=";

    if(getLanguage().c_str()) {
        szTemp = getLanguage().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",CalAddr=";

    if(getCalAddress().c_str()) {
        szTemp = getCalAddress().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",CXparam=";

    if(getXparameters().c_str()) {
        szTemp = getXparameters().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",MailTo=";

    if(getMailTo().c_str()) {
        szTemp = getMailTo().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    szRet.append(ss.str());
    return szRet;
}
