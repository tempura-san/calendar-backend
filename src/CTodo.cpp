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
 * include files
 */
#include "CTodo.h"
#include "Common.h"

/**
 * CTodo
 * Overloaded constructor for CTodo class
 */


CTodo::CTodo(string szSummary, int iTodoDue, int iStatus):CComponentDetails(szSummary, iTodoDue,
          iStatus), iDue(-1), iCompleted(-1), iPercentComplete(-1),
iPriority(-1)
{

    //All other parameters will be initialised in 
    //base clasdd ie CComponent and CComponentDetails.
    this->setType(E_TODO);
}

/**
 * ~CTodo 
 * @param none
 * @return none
 * destructor
 */
CTodo::~CTodo()
{
    CAL_DEBUG_LOG("Inside destructor for todo\n");
}

/**
 * setGeo
 * @param str  Latitude and Longitude  
 * @return bool
 *
 * This function will be used to set geo for a todo entry 
 * The geo is combination of latitude and 
 * longitude.ex: 120.98;45.64
 * returns true when geo is set correctly.
 * returns false when geo is empty.    

*/
bool CTodo::setGeo(string szGeo)
{
    this->szGeo = szGeo;
    return true;
}

/**
 * getGeo
 * @param none
 * @return geo string 
 *
 * This function will be used to extract geo for a todo entry 
 * which has been previously set using setGeo().The geo is combination of latitude and 
 * longitude.ex: 120.98;45.64
 * returns geo string


*/
string CTodo::getGeo()
{
    return this->szGeo;
}

/**
 * setDue
 * @param int time
 * @return bool
 *
 * This function will be used to set due date for a todo entry 
 * 
 * returns true when due date is set correctly.
 * returns false when due date is empty.    

*/ bool CTodo::setDue(int iDue)
{


    this->iDue = iDue;
    setDateStart(iDue);

    return true;
}

/**
 * getDue
 * @param none
 * @return due time int 
 *
 * This function will be used to due time for a todo 
 *  which has been previously set using setTodoDue()
 * returns due time 


*/
int CTodo::getDue()
{
    this->iDue = getDateStart();
    return this->iDue;
}

/**
 * setCompleted
 * @param int time
 * @return bool
 *
 * This function will be used to set completed date for a todo entry 
 * 
 * returns true when  date is set correctly.
 * returns false when  date is empty.    

*/
bool CTodo::setCompleted(int iCompleted)
{
    if (iCompleted < 0)
    return false;
    this->iCompleted = iCompleted;
    return true;


}

/**
 * getCompleted
 * @param none
 * @return completed time int 
 *
 * This function will be used to completed time for a todo 
 * which has been previously set using setTodoComp()
 * returns completed time 


*/
int CTodo::getCompleted()
{
    return this->iCompleted;
}

/**
 * setPercentComplete
 * @param integer P percentage of todo completed
 * @return bool 
 * This function will be used to set percentage of todo completed for  a todo entry 
 * returns true when percent  is set correctly.

*/

bool CTodo::setPercentComplete(int iPercentComplete)
{
    if (iPercentComplete < ZERO || iPercentComplete > HUNDRED)
    return false;
    this->iPercentComplete = iPercentComplete;
    return true;

}

/**
 * getPercentComplete
 * @param none
 * @return int percentage of todo completed.
 * This function will be used to extract percent of todo completed for a todo entry 
 * which has been previously set using setPercent().
 */
int CTodo::getPercentComplete()
{
    return this->iPercentComplete;
}

/**
 * setPriority
 * @param integer pripority 
 * @return bool 
 *
 * This function will be used to set priority of todo 
 * returns true when priority  is set correctly.
 */

bool CTodo::setPriority(int iPriority)
{
    if (iPriority >= ZERO && iPriority <= MAX_PRIORITY) {
    this->iPriority = iPriority;
    return true;
    }
    return false;


}

/**
 * getPriority
 * Function to get the priority value
 *
 * @param none
 *
 * @return int Todo priority
 */
int CTodo::getPriority()
{
    return this->iPriority;
}

/**
 * calTodo 
 * @param  none
 * @return None 
 * Default constructor
 */
CTodo::CTodo():CComponentDetails(), iDue(-1), iCompleted(-1), iPercentComplete(-1),
iPriority(-1)
{
    this->setType(E_TODO);

}



/**
 * copy Constructor 
 */
CTodo::CTodo(CTodo & ref):CComponentDetails(ref)
{

    szGeo = ref.szGeo;
    iDue = ref.iDue;
    iCompleted = ref.iCompleted;
    iPercentComplete = ref.iPercentComplete;
    iPriority = ref.iPriority;

}

/**
 * overloaded assignment operator 
 */
CTodo & CTodo::operator=(CTodo & right)
{
    //avoid self assignment 
    //
    if (&right != this) {
    CComponentDetails::operator=(right);
    szGeo = right.szGeo;
    iDue = right.iDue;
    iCompleted = right.iCompleted;
    iPercentComplete = right.iPercentComplete;
    iPriority = right.iPriority;
    }
    return *this;
}
string CTodo::toString()
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
    ss << ",UId=";
    if (getGUid().c_str()){
	szTemp= getGUid().substr(0,100);
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

    ss << ",TodoDue=";
    ss << iDue;
    ss << ",Completed=";
    ss << iCompleted;
    ss << ",Percent";
    ss << iPercentComplete;
    ss << ",Priority";
    ss << iPriority;
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

    szRet.append(ss.str());
    return szRet;
}
