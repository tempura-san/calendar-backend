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

#include "CJournal.h"
#include "Common.h"

/**
 * CJournal
 * Parameterized constructor for CJournal class
 */
CJournal::CJournal(string szDescription):CComponentDetails(szDescription)
{
    // All other members will be updated in base class
    this->setType(E_JOURNAL);
}

/**
 * copy constructor 
 */

CJournal::CJournal(CJournal & ref):CComponentDetails(ref)
{
}

/**
 * overloaded assignment 
 * operator
 */
CJournal & CJournal::operator=(CJournal & right)
{
    if (&right != this) {
    CComponentDetails::operator=(right);
    }
    return *this;
}

/**
 * CJournal
 * This is the default constructor of journal class 
 */
CJournal::CJournal():CComponentDetails()
{
    this->setType(E_JOURNAL);
}

/**
 * ~CJournal
 * This is the default destructor of journal class
 */
CJournal::~CJournal()
{
}

string CJournal::toString()

{
    CAlarm *pAlarm;
    CRecurrence *cRec;
    string szRet;
    string szTemp;
    time_t temp = 0;
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
    if (getSummary().c_str()){
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
    ss << ",Type=";
    ss << getType() ;
    ss << getFlags();
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
    ss << ",UId=";
    if (getGUid().c_str()){
	szTemp= getGUid().substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }
    else
	ss << NULL_STRING;

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

    pAlarm=getAlarm();
    if(pAlarm)
	ss << pAlarm->toString();
    else
	ss << ",Alarm=NULL" ;
    cRec=getRecurrence();
    if(cRec)
	ss << cRec->toString();
    else
	ss << ",Rec=NULL";

    szRet.append(ss.str());
    return szRet;
}
