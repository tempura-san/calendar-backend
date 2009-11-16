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

#include "CComponent.h"
#include "CBdayEvent.h"
#include "Common.h"
/**
 * CBdayEvent
 * Parametrized constructor
 */
CBdayEvent::CBdayEvent(string szUId, string szSummary, int iDateStart):CComponent(szUId, szSummary,
       iDateStart)
{

}


/**
 * CBdayEvent
 * Default constructor
 */
CBdayEvent::CBdayEvent()
{

}

/**
 * ~CBdayEvent
 * Default destructor
 */
CBdayEvent::~CBdayEvent()
{

}

/**
 * setEBookUid
 * Function to set Ebook UId
 *
 * @param sUid Ebook UID
 *
 * @return boolean (TRUE/FALSE)
 */
bool CBdayEvent::setEBookUid(string szEbookUid)
{
    if (szEbookUid.empty())
    return false;

    return setGUid(szEbookUid);
}


/**
 * setFirstName
 * Function to set first name
 *
 * @param sName First Name
 *
 * @return boolean (TRUE/FALSE)
 */
bool CBdayEvent::setFirstName(string szName)
{
    return setSummary(szName);
}


/**
 * setBirthDate
 * Function to set birth date
 *
 * @param int Birth date
 *
 * @return boolean (TRUE/FALSE)
 */
bool CBdayEvent::setBirthDate(int iDate)
{
    if (iDate < 0)
    return false;

    return setDateStart(iDate);
}


/**
 * getEBookUid
 * Function to get Ebook UId
 *
 * @return string Ebook Uid
 */
string CBdayEvent::getEBookUid()
{
    return getGUid();
}


/**
 * getFirstName
 * Function to get First Name
 *
 * @return boolean (TRUE/FALSE)
 */
string CBdayEvent::getFirstName()
{
    return getSummary();
}


/**
 * getBirthDate
 * Function to get Birth date
 *
 * @return integer Birth date
 */
int CBdayEvent::getBirthDate()
{
    return getDateStart();
}

string CBdayEvent::toString( )
{
    string szRet;
    string szTemp;
    CAlarm *pAlarm;
    CRecurrence *cRec;
    std::stringstream ss;
    ss << "ID=";
    if (getId().c_str()){
	szTemp= getId().substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;

    ss << ",CalendarId=";
    ss << getCalendarId();

    ss << ",summary=";
    if (getSummary ().c_str()){
	szTemp= getSummary().substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;
    ss << ",description=";
    if (getDescription().c_str()){
	szTemp= getDescription().substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;
    ss << ",location=";
    if (getLocation().c_str()){
	szTemp= getLocation().substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;
    ss << ",TimeZone=";
    if (getTzid().c_str()){
	szTemp= getTzid().substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;
    ss << ",UId=";
    if (getGUid().c_str()){

	szTemp= getGUid().substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;


    ss << ",Type=";
    ss << getType() ;
    ss << ",Flags=";
    ss << getFlags();

    ss << ",Status=";
    ss << getStatus();
    ss << ",Start date=";
    ss << getDateStart ();
    ss << ",End date=";
    ss << getDateEnd();
    ss << ",Last modified=";
    ss << getLastModified();
    ss << ",created=";
    ss << getCreatedTime();
    ss << ",until=";
    ss << getUntil();
    ss <<",All day=";
    ss << getAllDay();
    pAlarm=getAlarm();
    if(pAlarm)
	ss << pAlarm->toString();
    else
	ss << ",Alarm=NULL" ;
    cRec=getRecurrence();
    if(cRec)
	ss << cRec->toString();
    else
	ss << ",Rec=NULL" ;

    szRet.append(ss.str());
    return szRet;
}
