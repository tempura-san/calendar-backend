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
#include "CParticipant.h"
#include "Common.h"

/**
 * setLanguage
 * @param string Language parameter
 * @return bool indicates whether operation is successful or not 
 *
 * This function will be used to set the language parameter of participant
 */
bool CParticipant::setLanguage(string szLanguage)
{
    this->szLanguage = szLanguage;
    return true;
}

/**
 * getLanguage
 * Function to get the value of language parameter
 *
 * @param none
 *
 * @return string String value
 */
string CParticipant::getLanguage()
{
    return (this->szLanguage);
}


/**
 * setSentBy
 * @param string SentBy parameter
 * @return bool indicates whether operation is successful or not 
 *
 * This function will be used to set the sentby parameter of participant
 */
bool CParticipant::setSentBy(string szSentBy)
{
    this->szSentBy = szSentBy;
    return true;
}

/**
 * getSentBy
 * @param none
 * @return string sentBy 
 * 
 * This function will be used to get the sentby of participant
 */

string CParticipant::getSentBy()
{
    return (this->szSentBy);
}


/**
 * setCommonName
 * @param string common-name parameter
 * @return bool indicates whether operation is successful or not 
 *
 * This function will be used to set the common-name parameter of participant
 */

bool CParticipant::setCommonName(string szCommonName)
{
    this->szCommonName = szCommonName;
    return true;
}

/**
 * setMailTo
 * @param string szMailTo
 * @return none 
 * Function used to set mailto parameter
 */
void CParticipant::setMailTo(string szMailTo)
{
    this->szMailTo = szMailTo;

}

/**
 * getMailTo
 * @param none
 * @return string szMailTo
 * Function to retrieve MailTo from the class
 */
string CParticipant::getMailTo()
{
    return this->szMailTo;
}

/**
 * setXparameters
 * @param string X-PARAMETER parameter
 * @return bool indicates whether operation is successful or not 
 *
 * This function will be used to set the x-parameter of participant
 */

bool CParticipant::setXparameters(string szCXparameter)
{
    this->szCXparameter = szCXparameter;
    return true;
}

/**
 * setCalAddress
 * @param string cal-Address parameter
 * @return bool indicates whether operation is successful or not 
 *
 * This function will be used to set the Cal-Address parameter of participant
 */
bool CParticipant::setCalAddress(string szCalAddress)
{
    this->szCalAddress = szCalAddress;
    return true;
}

/**
 * setDirectoryParameter
 * @param string directory name parameter
 * @return bool indicates whether operation is successful or not 
 *
 * This function will be used to set the directory parameter of participant
 */
bool CParticipant::setDirectoryParameter(string szDirectoryParameter)
{
    this->szDirectoryParameter = szDirectoryParameter;
    return true;
}

/**
 * getDirectoryParameter
 * @return string directory name parameter
 * 
 * This function will be used to get the directory parameter of participant
 */

string CParticipant::getDirectoryParameter()
{
    return (this->szDirectoryParameter);
}

/**
 * getCalAddress()
 * @param none
 * @return string cal-Address parameter
 * 
 * This function will be used to get the Cal-Address parameter of participant
 */
string CParticipant::getCalAddress()
{
    return (this->szCalAddress);
}

/**
 * getXparameters
 * @param none
 * @return string X-parameter
 * 
 * This function will be used to get the X-parameter of participant
 */
string CParticipant::getXparameters()
{
    return (this->szCXparameter);
}

/**
 * getCommonName
 * @param none
 * @return string CommonName
 * 
 * This function will be used to get the Common-name of participant
 */
string CParticipant::getCommonName()
{
    return (this->szCommonName);
}

/**
 * CParticipant copy constructor 
 *
 *
 */
CParticipant::CParticipant(CParticipant & Cparef)
{


    szSentBy = Cparef.szSentBy;
    szCommonName = Cparef.szCommonName;
    szDirectoryParameter = Cparef.szDirectoryParameter;
    szLanguage = Cparef.szLanguage;
    szCalAddress = Cparef.szCalAddress;
    szCXparameter = Cparef.szCXparameter;
    szMailTo = Cparef.szMailTo;

}

/**
 * overloaded assignment operator
 */
const CParticipant & CParticipant::operator=(const CParticipant & right)
{
    if (&right != this)        //check for self assignment
    {
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


/**
 * CParticipant
 * Default constructor for CParticipant
 */
CParticipant::CParticipant()
{
}

/**
 *CParticipant
 *parameterised constructor for CParticipant
 */
CParticipant::CParticipant(string szSentBy, string szCommonName, string szDirectoryParameter, string szLanguage, string szCalAddress, string szCXparameter, string szMailTo):szSentBy(szSentBy), szCommonName(szCommonName),
szDirectoryParameter(szDirectoryParameter), szLanguage(szLanguage),
szCalAddress(szCalAddress), szCXparameter(szCXparameter),
szMailTo(szMailTo)
{
}



/**
 * ~CParticipant
 * Default destructor for CParticipant
 */
CParticipant::~CParticipant()
{
}


string CParticipant::toString()
{
    string szRet;
    string szTemp;
    std::stringstream ss;
    ss << "SentBy=";
    if (szSentBy.c_str()){
	szTemp= szSentBy.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    ss << ",CommonName=";
    if (szCommonName.c_str()){
	szTemp= szCommonName.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	szRet.append(NULL_STRING); 
    ss << ",Directory=";
    if (szDirectoryParameter.c_str()){
	szTemp= szDirectoryParameter.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    ss << ",Lang=";
    if (szLanguage.c_str()){
	szTemp= szLanguage.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    ss << ",CalAddr=";
    if (szCalAddress.c_str()){
	szTemp= szCalAddress.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    ss << ",CXparam=";
    if (szCXparameter.c_str()){
	szTemp= szCXparameter.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;  
    ss << ",MailTo=";
    if (szMailTo.c_str()){
	szTemp= szMailTo.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;
    szRet.append(ss.str());
    return szRet;
}

