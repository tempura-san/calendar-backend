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
 *  headers used in this file */
#include "CEvent.h"
#include "COrganizer.h"
#include "CAttendee.h"
#include "Common.h"
static int MAX_PRIORITY =9 ;
/**
 * CEvent
 * This is the overloaded parameterized constructor CEvent class where minimum set of members will be intialized.
 */

CEvent::CEvent(string szSummary, string szDescription, string szLocation,
           int iDateStart, int iDateEnd):CComponentDetails(szSummary,
                                   szDescription,
                                   szLocation,
                                   iDateStart,
                                   iDateEnd),
iPriority(-1)
{
    this->setType(E_EVENT);
}

/**
 * copy constructor
 * for Cevent class
 */
CEvent::CEvent(CEvent & ref):CComponentDetails(ref)
{
    szGeo = ref.szGeo;
    szTransparency = ref.szTransparency;
    iPriority = ref.iPriority;
}

/**
 * overloaded assignment 
 * operator 
 */
const CEvent & CEvent::operator=(const CEvent & right)
{
    //check for self assignment 
    //
    if (&right != this) {
    CComponentDetails::operator=(right);
    szGeo = right.szGeo;
    szTransparency = right.szTransparency;
    iPriority = right.iPriority;

    }

    return *this;
}


/**
 * CEvent
 * @param None
 * @return None
 * 
 * this is the default constructor for CEvent class where all variables are being initialised
 */
CEvent::CEvent():CComponentDetails(), iPriority(-1)
{
    this->setType(E_EVENT);
}


/**
 * ~CEvent
 * Default destructor
 */
CEvent::~CEvent()
{

}

/**
 * setGeo
 * Function to set the value for Geo
 *
 * @param szGeo Geography value
 *
 * @return boolean (TRUE/FALSE)
 */
bool CEvent::setGeo(string szGeo)
{
    this->szGeo = szGeo;
    return true;
}

/**
 * getGeo
 * Function to get the Geo
 * 
 * @param none
 * 
 * @return geo
 */
string CEvent::getGeo()
{
    return this->szGeo;
}

/**
 * setTransparency
 * 
 * @param  transp Transparency of entry
 * 
 * @return bool (SUCCESS/FAILURE)
 *
 * This function is used to set the transparency of the entry created.
 * Generally, an entry will take up time on an individual calendar. 
 * Hence, the event will appear as an opaque interval in a search for busy time. 
 * Alternately, the event can have its Time Transparency set to "TRANSPARENT" in order to prevent
 * blocking of the event in searches for busy time.
 */
bool CEvent::setTransparency(string szTransparency)
{

    if ((szTransparency == OPAQUE) || (szTransparency == TRANSPERANCY)) {
    this->szTransparency = szTransparency;
    return true;
    }

    return false;
}

/**
 * getTransparency
 * Function to get the transparency for an entry
 * 
 * @param None
 * @return  transp Transparency of entry
 */
string CEvent::getTransparency()
{
    return this->szTransparency;
}


/**
 * setPriority
 * @param integer pripority 
 * @return bool 
 *
 * This function will be used to set priority of todo 
 * returns true when priority  is set correctly.
 */
bool CEvent::setPriority(int iPriority)
{
    if (iPriority >= 0 && iPriority <= MAX_PRIORITY) {
    this->iPriority = iPriority;
    return true;
    }
    return false;


}

/**
 * getPriority
 * Function to get the Todo priority.
 * 
 * @param none
 * 
 * @return int priority of todo .
 */
int CEvent::getPriority()
{
    return this->iPriority;
}

string CEvent::toString()
{
    CAlarm *pAlarm;
    CRecurrence *cRec;
    string szRet;
    string szTemp;
    time_t temp=0;
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
    szRet.append(",description=");
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
    szRet.append(",TimeZone");
    if (getTzid ().c_str()){
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
    ss <<getType();
    ss << ",Flags=";
    switch (getFlags()){
	case HAS_ATTENDEE:
	    ss << "Has Attendee";
	    break;
	case HAS_ORGANIZER:
	    ss << "Has Organizer";
	    break;
	case HAS_RECURRENCE:
	    ss << "Has Recurrence";
	    break;
	case HAS_ALARM:
	    ss << "Has Alarm";
	    break;
	case HAS_RECURRENCE_ALARM :
	    ss << "Has Recurrent Alarm";
	    break;
	case HAS_PARTICIPANT :
	    ss << "Has Participant";
	    break;
	case HAS_CATEGORIES :
	    ss << "Has Categories";
	    break;
	case HAS_COMMENT:
	    ss << "Has Comment ";
	    break;
	case HAS_EXTRA:
	    ss << "Has Extra ";
	    break;
	default:
	    break;
    }

    ss << ",Status=";
    ss << getStatus();
    ss << ",Start date=";
    temp = getDateStart();
    ss << ctime(&temp);
    ss << ",End date=";
    temp = getDateEnd();
    ss << ctime(&temp);
    ss << ",Last modified=";
    temp = getLastModified();
    ss << ctime(&temp);
    ss << ",created=";
    temp = getCreatedTime();
    ss << ctime(&temp);
    ss << ",until=";
    temp = getUntil();
    ss << ctime(&temp);

    ss << ",All day=";
    ss << getAllDay();
    ss << ",Geo=";
    if (szGeo.c_str()){
	szTemp= szGeo.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;
    ss << ",Transparency=";
    if (szTransparency.c_str()){
	szTemp= szTransparency.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;

    pAlarm=getAlarm();
    if(pAlarm)
	ss << pAlarm->toString();
    else
	ss << ",Alarm=NULL";

    cRec=getRecurrence();
    if(cRec)
	ss << cRec->toString();
    else
	ss << ",Rec=NULL";

    ss << ",Priority=";
    ss << iPriority;
    szRet.append(ss.str());
    return szRet;

}
