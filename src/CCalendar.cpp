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

/* Headers */
#include <sstream>
#include <iomanip>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "CBdayEvent.h"
#include "Common.h"
#include "ICalConverter.h"
#include "CalendarErrors.h"
#include "CUtility.h"
#include "CCalendarProcs.h"

// Uncomment the following line to in order to generate event instances right after event is added
// #define ADD_COMPONENT_INSTANCES

static const int UPDATE_EXISTING_ALARM = 1;
static const int ADD_NEW_ALARM = 2;

static string sqlquoted_string(const std::string &s)
{
    char *sql_str = sqlite3_mprintf("%Q", s.c_str());
    string retval(sql_str);
    sqlite3_free(sql_str);

    return retval;
};

/**
 * Copy Constructor for CCalendarClass
 */
CCalendar::CCalendar(CCalendar & ref)
{
    szName = ref.szName;
    iColor = ref.iColor;
    fReadOnly = ref.fReadOnly;
    fIsVisible = ref.fIsVisible;
    iCalId = ref.iCalId;
    iCalType = ref.iCalType;
    szCalendarTune = ref.szCalendarTune;
    szCalendarVersion = ref.szCalendarVersion;
    iProfileType = ref.iProfileType;
}


/**
 * Overloaded assignment operator for CCalendar class 
 */
const CCalendar & CCalendar::operator=(const CCalendar & right)
{
    /* Check for self assignment */
    if (&right != this) {
    szName = right.szName;
    iColor = right.iColor;
    fReadOnly = right.fReadOnly;
    fIsVisible = right.fIsVisible;
    iCalId = right.iCalId;
    iCalType = right.iCalType;
    szCalendarTune = right.szCalendarTune;
    szCalendarVersion = right.szCalendarVersion;
    iProfileType = right.iProfileType;
    }
    return *this;
}

/**
 * @param SyncProfileType - Profile type of calendar
 * @return NONE
 *
 *  Function to set profile Type required from maesync point of view 
 */
void CCalendar::setProfileType(SyncProfileType iProfile)
{
    this->iProfileType = iProfile;
}


/**
 * @param NONE
 * @return SyncProfileType - Profile type of calendar
 *
 *  Function to get profile Type required from maesync point of view 
 */
SyncProfileType CCalendar::getProfileType()
{
    return (this->iProfileType);
}


	
/**
* @param int iCalid calendar Id 
* @param int stDate date start in integer format
* @param vector <CComponent*> vector of CComponent pointers
* @param int * pErrorCode error code if any
* @return vector <CComponent*> Modified vector of CComponent pointers
* 
*Function is used to obtain recursive events in particular time range 
*/
vector <CComponent *>CCalendar::getRecursiveEventsInRange(int iStDate,
                                int iEndDate,
                                vector <CComponent *> vListEntry,
                                int &pErrorCode)
{
    QueryResult *pQr = 0;
    CComponent *pEntry = 0;
    int iI_RecRanCount = 0;
    int iJ_RecRanCount = 0;
    int iK_RecRanCount = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    CCalendarDB *pDb = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    pDb = CCalendarDB::Instance();
    if (pDb == 0) {
	    CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	    pErrorCode = CALENDAR_APP_ERROR;
	    return vListEntry;
    }

    /*
     * 
     #define SELECT_MULTI_RANGE_PRCR        "select * from %s where %s = %d AND
     (%s = %d OR %s=%d) AND (%s = %d OR %s = %d)"
     *      
     */
    pQuery =
    sqlite3_mprintf(SELECT_MULTI_RANGE_PRCR, COMPONENTS_TABLE,
            ENTRY_FIELD_CALID, this->getCalendarId(),
            ENTRY_FIELD_TYPE, E_EVENT, ENTRY_FIELD_TYPE,
            E_BDAY, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,
            ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM, iStDate,
            ENTRY_FIELD_UNTIL, NULLID, ENTRY_FIELD_UNTIL);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("pQuery is : %s", pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    /* errro code can be set in the mapper function above */
    if (pQr == 0) 
	return vListEntry;
    

    /*TODO: The following piece of code can be optimised by moving it to new 
     * function and calling the new procedure from getCompoents and here as-well
     */
    for (iI_RecRanCount = 0; iI_RecRanCount < pQr->iRow; iI_RecRanCount++) {
	pEntry = new CComponent();
	ASSERTION(pEntry);
    pEntry->setCalendarId(this->getCalendarId());

	for (iJ_RecRanCount = 0; iJ_RecRanCount < pQr->iColumn; iJ_RecRanCount++) {
	    iK_RecRanCount = pQr->iColumn + (iI_RecRanCount * pQr->iColumn);

	    switch (iJ_RecRanCount) {

		case DB_COLUMN_ID1:
		    pEntry->setId(pQr-> pResult[iK_RecRanCount + iJ_RecRanCount]);
		    break;

		case DB_COLUMN_ID2:
		    break;

		case DB_COLUMN_ID3:
		    pEntry-> setType(atoi (pQr-> pResult[iK_RecRanCount + 
					    iJ_RecRanCount]));
		    break;

		case DB_COLUMN_ID4:
		    pEntry-> setFlags(atoi (pQr-> pResult[iK_RecRanCount + 
					    iJ_RecRanCount]));
		    break;

		case DB_COLUMN_ID5:
		    pEntry-> setDateStart(atoi (pQr-> pResult[iK_RecRanCount +
					    iJ_RecRanCount]));
		    break;

		case DB_COLUMN_ID6:
		    pEntry-> setDateEnd(atoi (pQr-> pResult[iK_RecRanCount +
					    iJ_RecRanCount]));
		    break;

		case DB_COLUMN_ID7:
		    if (pQr->pResult[iK_RecRanCount + iJ_RecRanCount]) {
			pEntry->setSummary((string) pQr-> pResult[iK_RecRanCount +
					    iJ_RecRanCount]);
		    }
		    break;

		case DB_COLUMN_ID8:
		    if (pQr->pResult[iK_RecRanCount + iJ_RecRanCount]) {
			pEntry->setLocation((string) pQr-> pResult[iK_RecRanCount +
					    iJ_RecRanCount]);
		    }
		    break;

		case DB_COLUMN_ID9:
		    if (pQr->pResult[iK_RecRanCount + iJ_RecRanCount]) {
			pEntry->setDescription((string) pQr-> pResult[iK_RecRanCount 
					    + iJ_RecRanCount]);
		    }
		    break;

		case DB_COLUMN_ID10:
		    pEntry-> setStatus(atoi (pQr-> pResult[iK_RecRanCount +
					    iJ_RecRanCount]));
		    break;

		case DB_COLUMN_ID11:
		    if (pQr->pResult[iK_RecRanCount + iJ_RecRanCount]) {
			pEntry->setGUid(pQr-> pResult[iK_RecRanCount +
					    iJ_RecRanCount]);
		    }
		    break;
		case DB_COLUMN_ID12:
		    pEntry-> setUntil(atoi (pQr-> pResult[iK_RecRanCount +
					    iJ_RecRanCount]));
		    break;

		case DB_COLUMN_ID13:
		    pEntry-> setAllDay(atoi (pQr-> pResult[iK_RecRanCount +
					    iJ_RecRanCount]));
		    break;

		case DB_COLUMN_ID14:
		    pEntry->setCreatedTime(atoi(pQr->pResult[iK_RecRanCount+ 
				iJ_RecRanCount]));
		    break;
		case DB_COLUMN_ID15:

		    pEntry->setLastModified(atoi(pQr->pResult[iK_RecRanCount +
					     iJ_RecRanCount]));
		    break;
		case DB_COLUMN_ID16:
		    pEntry->setTzid(pQr->pResult[iK_RecRanCount +iJ_RecRanCount]);
		    break;
		default:
		    break;
	    }

	}
	CAL_DEBUG_LOG("CCalendar %d:getRecursiveEventsInRange-> %s \n",
			    iI_RecRanCount,pEntry->toString().c_str());
	if (((pEntry->getFlags() == HAS_RECURRENCE)
		|| (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
		&& (pEntry->getType() != E_JOURNAL))
	    pEntry->getRecurrenceProperties();
	if (((pEntry->getFlags() == HAS_ALARM)
		|| (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
		&& (pEntry->getType() != E_JOURNAL))
	    pEntry->getAlarmProperties();

	/*Check if there are any instances available in
	 * given range*/
	if (pEntry->getInstanceNumber(iStDate, iEndDate))
	    vListEntry.push_back((CComponent *) pEntry);
	else {
	    delete pEntry;
	    pEntry = 0;
	}
    }


    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }

    return vListEntry;

}

/**
 * @param : stDate to do conditional fetching based on start date. -1 
 * in case of no conditions
 * @param : endDate to do conditional fetching based on end date. -1 
 * in case of no conditions
 * @param : limit limit for number of events fetched by the calenadr
 * @param int * pErrorCode error code:
 *          CALENDAR_OPERATION_SUCCESSFUL- fetching is successful and can be continued;
 *          CALENDAR_FETCH_NOITEMS- fetching is finished (no more items found in DB);
 *          other error code- fetching is failed.
 * @return : Vector for CComponent. If pErrorCode equals to CALENDAR_OPERATION_SUCCESSFUL,
 *           but vector is empty, no component (from offset to limit) meets the conditions,
 *           continue fetching
 * This function is used to get all the Events /Todos /Bdays  associated with 
 * particular Calendar progressively It is an overloaded API for get componets 
 * used to fetch Components in bursts of specified size
 */

vector<CComponent *> CCalendar::getAllEventsTodos(int iStDate,
                                   int
                                   iEndDate,
                                   int iLimit,
                                   int &iOffset,
                                   int
                                   &pErrorCode)
{
    QueryResult *pQr = 0;
    CComponent *pEntry = 0;
    int iI_CompCount = 0;
    int iJ_CompCount = 0;
    int iK_CompCount = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    vector <CComponent *>vListEntry;
    CCalendarDB *pDb = 0;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    int recCount = 0;
    bool againFlag = false;

    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return vListEntry;
    }
    int iNewStDate = 0;
    int iNewEndDate = 0;
    CMulticalendar *mc = CMulticalendar::MCInstance();
    int time_diff = time_get_time_diff (iStDate, mc->getSystemTimeZone().c_str(), "UTC");
    iNewStDate = iStDate + time_diff;
    iNewEndDate = iEndDate + time_diff;

    /* Implementation in getComponents function will be similar for getting 
     * events /todo / journals (or) fetching all components other than query 
     * part. So all the functialities are written together in a single 
     * function for better code optimization
     **/
    do{
//    query is split in to four parts
//    Non allday events non recursuve events  union
//    Tasks  and allday non allday non recursive 
//    Events non allday   recursive 
//    Events and bdays  allday recursive 
	againFlag = false;
	pQuery =
	    sqlite3_mprintf(SELECT_EVENTS_TODOS_OFFSET_LIMIT_UNION,
	    
	    COMPONENTS_TABLE, ENTRY_FIELD_CALID, this->getCalendarId(),ENTRY_FIELD_TYPE, E_EVENT,
	    ENTRY_FIELD_DTSTART, iEndDate,ENTRY_FIELD_DTEND, iStDate, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,
	    ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,
	    COMPONENTS_TABLE,ENTRY_FIELD_CALID, this->getCalendarId(),ENTRY_FIELD_TYPE, E_TODO, ENTRY_FIELD_DTSTART,
	    ENTRY_FIELD_TZOFFSET, iNewStDate,ENTRY_FIELD_DTEND, ENTRY_FIELD_TZOFFSET, iNewEndDate, ENTRY_FIELD_FLAGS,
	    HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM, 
	    COMPONENTS_TABLE, ENTRY_FIELD_CALID, iCalId,ENTRY_FIELD_TYPE, E_EVENT,
	    ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,iStDate,
	    ENTRY_FIELD_UNTIL,NULLID, ENTRY_FIELD_UNTIL,
	    COMPONENTS_TABLE, ENTRY_FIELD_CALID, iCalId,ENTRY_FIELD_TYPE, E_EVENT,ENTRY_FIELD_TYPE,
	    E_BDAY,ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM, 
	    iNewStDate, ENTRY_FIELD_UNTIL, ENTRY_FIELD_TZOFFSET,NULLID, ENTRY_FIELD_UNTIL,
	    iLimit, iOffset);


	ASSERTION(pQuery);
	CAL_DEBUG_LOG("pQuery is %s",pQuery);


	pQr = pDb->getRecords(pQuery,iSqliteError);
	pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	sqlite3_free(pQuery);
	/* error code will be set in the mapper function above */
	if (pQr == 0) {
	    if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL)
		pErrorCode = CALENDAR_FETCH_NOITEMS;
	    return vListEntry;
	}

	for (iI_CompCount = 0; iI_CompCount < pQr->iRow; iI_CompCount++) {
	    pEntry = new CComponent();
	    ASSERTION(pEntry);
        pEntry->setCalendarId(this->getCalendarId());

	    for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
		iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

		if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
		    continue;

		switch (iJ_CompCount) {

		    case DB_COLUMN_ID1:
			pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
			break;

		    case DB_COLUMN_ID2:
			//pEntry->setCalendarId(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
			break;

		    case DB_COLUMN_ID3:
			pEntry-> setType(atoi (pQr->pResult[iK_CompCount + iJ_CompCount]));
			break;

		    case DB_COLUMN_ID4:
			pEntry-> setFlags(atoi (pQr->pResult[iK_CompCount + iJ_CompCount]));
			break;

		    case DB_COLUMN_ID5:
			pEntry-> setDateStart(atoi (pQr-> pResult[iK_CompCount + 
				    iJ_CompCount]));
			break;

		    case DB_COLUMN_ID6:
			pEntry-> setDateEnd(atoi (pQr-> pResult[iK_CompCount + 
				    iJ_CompCount]));
			break;

		    case DB_COLUMN_ID7:
			if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			    pEntry->setSummary((string) pQr-> pResult[iK_CompCount +
				    iJ_CompCount]);
			}
			break;

		    case DB_COLUMN_ID8:
			if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			    pEntry->setLocation((string) pQr-> pResult[iK_CompCount +
				    iJ_CompCount]);
			}
			break;

		    case DB_COLUMN_ID9:
			if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			    pEntry->setDescription((string) pQr-> pResult[iK_CompCount +
				    iJ_CompCount]);
			}
			break;

		    case DB_COLUMN_ID10:
			pEntry->setStatus(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
			break;

		    case DB_COLUMN_ID11:
			if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			    pEntry->setGUid(pQr->pResult[iK_CompCount + iJ_CompCount]);
			}
			break;
		    case DB_COLUMN_ID12:
			pEntry->setUntil(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
			break;

		    case DB_COLUMN_ID13:
			pEntry->setAllDay(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
			break;
		    case DB_COLUMN_ID14:
			pEntry->setCreatedTime(atoi(pQr->pResult[iK_CompCount + 
				    iJ_CompCount]));
			break;
		    case DB_COLUMN_ID15:

			pEntry->setLastModified(atoi(pQr->pResult[iK_CompCount +
				    iJ_CompCount]));
			break;
		    case DB_COLUMN_ID16:
			pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
			break;
		    default:
			break;
		} 
	    }

	    CAL_DEBUG_LOG("CCalendar %d:getAllEventsTodos-> %s \n",
		    iI_CompCount,pEntry->toString().c_str());
	    CAL_DEBUG_LOG("Event id before is %s\n", pEntry->getId().c_str());
	    if (((pEntry->getFlags() == HAS_RECURRENCE) ||
			(pEntry->getFlags() == HAS_RECURRENCE_ALARM)) &&
		    (pEntry->getType() != E_JOURNAL)) {
		CAL_DEBUG_LOG("Event id is %s", pEntry->getId().c_str());
		pEntry->getRecurrenceProperties();
	    }

	    if (((pEntry->getFlags() == HAS_ALARM) ||
			(pEntry->getFlags() == HAS_RECURRENCE_ALARM)) &&
		    (pEntry->getType() != E_JOURNAL))
		pEntry->getAlarmProperties();


	    /*see if any instance is in the view range
	     *  if it is not present in view range then
	     *  remove it from the list
	     */
	    if (((pEntry->getFlags() == HAS_RECURRENCE) ||
			(pEntry->getFlags() == HAS_RECURRENCE_ALARM)) &&
		    (pEntry->getType() != E_JOURNAL)) {
		if (pEntry->getInstanceNumber(iStDate, iEndDate))
		    vListEntry.push_back(pEntry);

		else {
		    recCount++;
		    if(recCount == pQr->iRow){
		//	againFlag = true;
		//	iOffset = iOffset + iLimit;
		    }
		    delete pEntry;
		    pEntry = 0;
		}
	    } 
	    else
		vListEntry.push_back(pEntry);
	}
    }
    while(againFlag);

    CAL_DEBUG_LOG("size of list is %d", vListEntry.size());

    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    return vListEntry;
}

/**
 * @param : calid Calendar ID
 * @param : type 0 -> All entries
 *               1 -> EVENT
 *               2 -> TODO
 *               3 -> JOURNAL
 *               4 -> BDAY
 *               
 *
 * @param : stDate to do conditional fetching based on start date. -1 in 
 * case of no conditions
 * @param : endDate to do conditional fetching based on end date. -1 in 
 * case of no conditions
 * @param : limit limit for number of events fetched by the calenadr
 * @param int * pErrorCode error code:
 *          CALENDAR_OPERATION_SUCCESSFUL- fetching is successful and can be continued;
 *          CALENDAR_FETCH_NOITEMS- fetching is finished (no more items found in DB);
 *          other error code- fetching is failed.
 * @return : Vector for CComponent. If pErrorCode equals to CALENDAR_OPERATION_SUCCESSFUL,
 *           but vector is empty, no component (from offset to limit) meets the conditions,
 *           continue fetching
 * This function is used to get all the entries associated with particular 
 * component type progressively.It is an overloaded API for get componets 
 * used to fetch Components in  bursts of specified size.
 */

vector <CComponent *>CCalendar::getComponents(int iType, int iStDate,
                        int iEndDate, int iLimit,
                        int iOffset,
                        int &pErrorCode)
{
    QueryResult *pQr = 0;
    CComponent *pEntry = 0;
    int iI_CompCount = 0;
    int iJ_CompCount = 0;
    int iK_CompCount = 0;
    char *pQuery = 0;
    vector <CComponent *>vListEntry;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return vListEntry;
    }


    int iNewStDate = 0;
    int iNewEndDate = 0;
    CMulticalendar *mc = CMulticalendar::MCInstance();
    int time_diff = time_get_time_diff (iStDate, mc->getSystemTimeZone().c_str(), "UTC");
    iNewStDate = iStDate + time_diff;
    iNewEndDate = iEndDate + time_diff;
    
    /* Implementation in getComponents function will be similar for getting 
     * events /todo / journals (or) fetching all components other than query 
     * part. 
     * So all the functialities are written together in a single function for 
     * better code optimization
     **/


    CAL_DEBUG_LOG("In others querying...\n");
    if ((iStDate == NULLID) || (iEndDate == NULLID)) {
	pQuery = sqlite3_mprintf(SELECT_ALL_ENTRIES, COMPONENTS_TABLE,
		     ENTRY_FIELD_CALID, this->getCalendarId(),
		     ENTRY_FIELD_TYPE, iType, iLimit, iOffset);
    } 
    else {

	    pQuery =
		    sqlite3_mprintf(SELECT_MRCR_OFFSET_LIMIT_UNION,COMPONENTS_TABLE,ENTRY_FIELD_CALID, this->getCalendarId(),
		    ENTRY_FIELD_TYPE, iType,ENTRY_FIELD_DTSTART, iStDate,ENTRY_FIELD_DTEND, iEndDate,
		    ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,
		    
		    COMPONENTS_TABLE,ENTRY_FIELD_CALID, this->getCalendarId(), ENTRY_FIELD_TYPE, iType, ENTRY_FIELD_TYPE,
		    ENTRY_FIELD_DTSTART, ENTRY_FIELD_TZOFFSET, iNewStDate,ENTRY_FIELD_FLAGS, 
		    HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM, 
		    
		    COMPONENTS_TABLE,ENTRY_FIELD_CALID, this->getCalendarId(), ENTRY_FIELD_TYPE, iType,
		    ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,iStDate, 
		    ENTRY_FIELD_UNTIL,NULLID, ENTRY_FIELD_UNTIL,
		    
		    COMPONENTS_TABLE,ENTRY_FIELD_CALID, iCalId,ENTRY_FIELD_TYPE, iType, 
		    ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,
		    iNewStDate, ENTRY_FIELD_UNTIL, ENTRY_FIELD_TZOFFSET,NULLID, ENTRY_FIELD_UNTIL,
		    iLimit, iOffset);
    }
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("Query: %s\nStDate:%d,EndDate:%d",
		    pQuery,iStDate,iEndDate);

    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0) {
	if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL)
	    pErrorCode = CALENDAR_FETCH_NOITEMS;
	return vListEntry;
    }

    for (iI_CompCount = 0; iI_CompCount < pQr->iRow; iI_CompCount++) {
	pEntry = new CComponent();
	ASSERTION(pEntry);
    pEntry->setCalendarId(this->getCalendarId());

	for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
	    iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

	    if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
	    continue;

	   switch (iJ_CompCount) {
		
	       case DB_COLUMN_ID2:
		   break;

	       case DB_COLUMN_ID1:
		   pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
		   break;

	       case DB_COLUMN_ID3:
		   pEntry->setType(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
		   break;

	       case DB_COLUMN_ID4:
		   pEntry->setFlags(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
		   break;

	       case DB_COLUMN_ID5:
		   pEntry->setDateStart(atoi(pQr->pResult[iK_CompCount + 
			       iJ_CompCount]));
		   break;

	       case DB_COLUMN_ID6:
		   pEntry-> setDateEnd(atoi (pQr->
				pResult[iK_CompCount + iJ_CompCount]));
		   break;

	       case DB_COLUMN_ID7:
		   if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
		       pEntry->setSummary((string) pQr-> pResult[iK_CompCount +
				iJ_CompCount]);
		   }
		   break;

	       case DB_COLUMN_ID8:
		   if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
		       pEntry->setLocation((string) pQr->pResult[iK_CompCount +
				iJ_CompCount]);
		   }
		   break;

	       case DB_COLUMN_ID9:
		   if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
		       pEntry->setDescription((string) pQr->pResult[iK_CompCount +
				iJ_CompCount]);
		   }
		   break;

	       case DB_COLUMN_ID10:
		   pEntry->setStatus(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
		   break;

	       case DB_COLUMN_ID11:
		   if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
		       pEntry->setGUid(pQr->pResult[iK_CompCount + iJ_CompCount]);
		   }
		   break;
	       case DB_COLUMN_ID12:
		   pEntry-> setUntil(atoi (pQr->pResult[iK_CompCount + iJ_CompCount]));
		   break;

	       case DB_COLUMN_ID13:
		   pEntry-> setAllDay(atoi (pQr->pResult[iK_CompCount + iJ_CompCount]));
		   break;
	       case DB_COLUMN_ID14:
		   pEntry->setCreatedTime(atoi(pQr->pResult[iK_CompCount + 
				iJ_CompCount]));
		   break;
	       case DB_COLUMN_ID15:

		   pEntry->setLastModified(atoi(pQr->pResult[iK_CompCount +
				iJ_CompCount]));
		   break;
		case DB_COLUMN_ID16:
		    pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    break;
		default:
		    break;
	    } 
	   
	}

       CAL_DEBUG_LOG("CCalendar %d:getComponents-> %s \n",
			   iI_CompCount,pEntry->toString().c_str());
	CAL_DEBUG_LOG("Event id beforeis %s", pEntry->getId().c_str());
	if (((pEntry->getFlags() == HAS_RECURRENCE)
	     || (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
	    && (pEntry->getType() != E_JOURNAL)) {
	    CAL_DEBUG_LOG("Event id is %s", pEntry->getId().c_str());
	    pEntry->getRecurrenceProperties();
	}

	if (((pEntry->getFlags() == HAS_ALARM)
	     || (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
	    && (pEntry->getType() != E_JOURNAL))
	    pEntry->getAlarmProperties();


	/*see if any instance is in the view range
	 *  if it is not present in view range then
	 *  remove it from the list*/
	if (((pEntry->getFlags() == HAS_RECURRENCE)
	     || (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
	    && (pEntry->getType() != E_JOURNAL)) {

	    if (pEntry->getInstanceNumber(iStDate, iEndDate)) {
	    vListEntry.push_back(pEntry);
	    } 
	    else {
	    delete pEntry;
	    pEntry = 0;
	    }

    } 
    else
        vListEntry.push_back(pEntry);


    }



    CAL_DEBUG_LOG("size of list is %d", vListEntry.size());

    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    return vListEntry;
}



/**
 * @param : calid Calendar ID
 * @param : type 0 -> All entries
 *               1 -> EVENT
 *               2 -> TODO
 *               3 -> JOURNAL
 *               4 -> BDAY
 *               
 *
 * @param : stDate to do conditional fetching based on start date. -1 in 
 * case of no conditions
 * @param : endDate to do conditional fetching based on end date. -1 in 
 * case of no conditions
 * @param int * pErrorCode error code if any
 * @return : Vector for CComponent
 * This function is used to get all the entries associated with particular 
 * component type
 */
vector <CComponent *>CCalendar::getComponents(int iType, int iStDate,
                        int iEndDate,
                        int &pErrorCode)
{
    QueryResult *pQr = 0;
    CComponent *pEntry = 0;
    int iI_CompCount = 0;
    int iJ_CompCount = 0;
    int iK_CompCount = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    vector <CComponent *>vListEntry;
    CCalendarDB *pDb = 0;

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    pDb = CCalendarDB::Instance();

    if (pDb == 0) {
    CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
    pErrorCode = CALENDAR_APP_ERROR;
    return vListEntry;
    }


    /* Implementation in getComponents function will be similar for getting 
     * events /todo / journals (or) fetching all components other than query 
     * part. So all the functialities are written together in a single 
     * function for better code optimization
     **/
    if ((iStDate == NULLID) && (iEndDate == NULLID)) {
	    pQuery = sqlite3_mprintf(SELECT_TAB_COND_AND, COMPONENTS_TABLE,
			    ENTRY_FIELD_CALID,
			    this->getCalendarId(),
			    ENTRY_FIELD_TYPE, iType);
    } 
else {
	    if ((iType == E_EVENT) || (iType == E_BDAY))
	    {

		/*All recursive events which fall in this range needs
		 * to be reported to UI */
		CAL_DEBUG_LOG("**********************");
		CAL_DEBUG_LOG("Entering Events in Range procedure\n");
		CAL_DEBUG_LOG("**********************");
		vListEntry =
		    getRecursiveEventsInRange(iStDate, iEndDate,
				  vListEntry, pErrorCode);
	    }
	    int iNewStDate = 0;
	    int iNewEndDate = 0;
	    CMulticalendar *mc = CMulticalendar::MCInstance();
	    int time_diff = time_get_time_diff (iStDate, mc->getSystemTimeZone().c_str(), "UTC");
	    iNewStDate = iStDate + time_diff;
	    iNewEndDate = iEndDate + time_diff;

	    pQuery =
	    sqlite3_mprintf(SELECT_MULTI_RANGE_MRCR,COMPONENTS_TABLE, ENTRY_FIELD_CALID, 
			    this->getCalendarId(),ENTRY_FIELD_TYPE, iType, ENTRY_FIELD_DTSTART,
			    iStDate,ENTRY_FIELD_DTEND, iEndDate, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,
			    ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,COMPONENTS_TABLE, ENTRY_FIELD_CALID,
			    this->getCalendarId(), ENTRY_FIELD_TYPE, iType,ENTRY_FIELD_DTSTART, ENTRY_FIELD_TZOFFSET, 
			    iNewStDate, ENTRY_FIELD_DTEND, ENTRY_FIELD_TZOFFSET, iNewEndDate,ENTRY_FIELD_FLAGS,
			    HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM);
	}

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("%s is the query in CCOmponets", pQuery);

    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0) {
	pErrorCode = CALENDAR_APP_ERROR;
	return vListEntry;
    }


    for (iI_CompCount = 0; iI_CompCount < pQr->iRow; iI_CompCount++) {
	pEntry = new CComponent();
	ASSERTION(pEntry);
    pEntry->setCalendarId(this->getCalendarId());

	for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
	    iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

	    if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
	    continue;

	    switch (iJ_CompCount) {

		case DB_COLUMN_ID1:
		    pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    break;

		case DB_COLUMN_ID2:
		    break;

		case DB_COLUMN_ID3:
		    pEntry->setType(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID4:
		    pEntry->setFlags(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID5:
		    pEntry->setDateStart(atoi(pQr->
				 pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID6:
		    pEntry->setDateEnd(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID7:
		    if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			pEntry->setSummary((string) pQr->pResult[iK_CompCount +
				iJ_CompCount]);
		    }
		    break;

		case DB_COLUMN_ID8:
		    if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			pEntry->setLocation((string) pQr->pResult[iK_CompCount +
				iJ_CompCount]);
		    }

		    break;

		case DB_COLUMN_ID9:
		    if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			pEntry->setDescription((string) pQr->
				pResult[iK_CompCount + iJ_CompCount]);
		    }
		    break;

		case DB_COLUMN_ID10:
		    pEntry-> setStatus(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID11:
		    if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			pEntry->setGUid(pQr-> pResult[iK_CompCount + iJ_CompCount]);
		    }
		    break;
		case DB_COLUMN_ID12:
		    pEntry->setUntil(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID13:
		    pEntry->setAllDay(atoi(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;
		case DB_COLUMN_ID14:
		    pEntry->setCreatedTime(atoi(pQr->pResult[iK_CompCount + 
				iJ_CompCount]));
		    break;
		case DB_COLUMN_ID15:

		    pEntry->setLastModified(atoi(pQr->pResult[iK_CompCount +
					     iJ_CompCount]));
		    break;

		case DB_COLUMN_ID16:
		    pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    break;
		    
		default:
		    break;
	    }
	    

	}

	CAL_DEBUG_LOG("CCalendar %d:getComponents-> %s \n",
			    iI_CompCount,pEntry->toString().c_str());


	if (((pEntry->getFlags() == HAS_RECURRENCE) ||
	     (pEntry->getFlags() == HAS_RECURRENCE_ALARM)) &&
	    (pEntry->getType() != E_JOURNAL))
	    pEntry->getRecurrenceProperties();
	if (((pEntry->getFlags() == HAS_ALARM) ||
	     (pEntry->getFlags() == HAS_RECURRENCE_ALARM)) &&
	    (pEntry->getType() != E_JOURNAL))
	    pEntry->getAlarmProperties();
	vListEntry.push_back((CComponent *) pEntry);
	 /**/
    }


    CAL_DEBUG_LOG("size of list is %d", vListEntry.size());
    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    return vListEntry;
}

/**
 * @param int Component id
 * @param int Component Type 
 * @param int * pErrorCode error code if any
 * @return bool true/false
 * Function to set the next recurrent alarm, when alarm triggers
 */
bool CCalendar::setNextAlarm(string sComponentId,int iType, int &pErrorCode)
{

	CEvent *pEvent = 0;
	CTodo *pTodo = 0 ;
	vector <time_t> listtime;
	CAlarm alarmUtility;
	time_t dateStart = 0;
	time_t dateToday = 0;
	struct tm tmToday;
	time_t timeDiff = 0;
	CAlarm *pAlarm = 0;
	time_t instanceDateStart = 0;
	vector <long>cookie;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    CAL_DEBUG_LOG("ALARM");

	if (iType == E_TODO)
	{
		/* Tasks doesn't have recurrence so it is ok if we 
		 * remove the alarm and modify such tasks 
		 */
		pTodo = this->getTodo(sComponentId,pErrorCode);
		if(pTodo == 0){

			pErrorCode = CALENDAR_INVALID_ARG_ERROR;
			return false;
		}
			
		
		if (pTodo->getFlags() == HAS_ALARM )
		{
			pAlarm = pTodo->getAlarm();
			dateStart = pAlarm->getTrigger();
			time_get_local(&tmToday);
			tmToday.tm_hour = 0;
			tmToday.tm_min = 0;
			tmToday.tm_sec = 0;
			tmToday.tm_isdst = -1;
			dateToday = mktime(&tmToday);
			if (dateToday > dateStart)
			{
				pTodo->setFlags(-1);
				pTodo->removeAlarm();
			}
			else
			{
				int temp;
				temp =
					alarmUtility.addAlarmEvent(dateStart,
							pTodo->getSummary(),
							pTodo->getLocation(), 
							dateStart,
							dateStart,
							pTodo->getId(),
							this->getCalendarId(),
							pTodo->getDescription(),
							pTodo->getType(),
							FALSE,
							pTodo->getTzid(),
							pErrorCode);

				if (pErrorCode  == CALENDAR_OPERATION_SUCCESSFUL) {
					cookie.push_back(temp);
					pAlarm->setCookie(cookie);
					this->addAlarm(pTodo->getAlarm(), E_MODIFY, 
							pTodo->getId(),pErrorCode);
			 		CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:setNextAlarm:Add Alarm failed");
				} 
			}

		}
		/* remove the alarm component from event
		 * object and then modify the event */
		
		this->modifyTodo(pTodo, pErrorCode);
		CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:setNextAlarm:Modify Todo Alarm failed");
		/* if control reaches here it implies 
		 * everything is ok so no need to 
		 * put else condition*/
		if(pTodo) {
			delete pTodo;
			pTodo = 0;
		}
		return true ;

	}

	if (iType == E_EVENT)
		pEvent = this->getEvent(sComponentId, pErrorCode);
	else if (iType == E_BDAY)
		pEvent = this->getBirthDayEvent(sComponentId, pErrorCode);
	
	if (pEvent == 0) {
		pErrorCode = CALENDAR_INVALID_ARG_ERROR;
		return false;
	}

    if (pEvent->updateAlarmTriggerTime(&instanceDateStart) &&
        (pAlarm = pEvent->getAlarm()) != 0)
    {
		dateStart = pAlarm->getTrigger();

		timeDiff = pEvent->getDateEnd() - pEvent->getDateStart();

		int temp;
		temp =
			alarmUtility.addAlarmEvent(dateStart,
					pEvent->getSummary(),
					pEvent->getLocation(), 
					instanceDateStart,
					instanceDateStart + timeDiff,
					pEvent->getId(),
					this->getCalendarId(),
					pEvent->getDescription(),
					pEvent->getType(),
					pEvent->getAllDay(),
					pEvent->getTzid(),
					pErrorCode);

		if (pErrorCode  == CALENDAR_OPERATION_SUCCESSFUL) {
			cookie.push_back(temp);
			pAlarm->setCookie(cookie);
			this->addAlarm(pEvent->getAlarm(), E_MODIFY, 
					pEvent->getId(),pErrorCode);
			 CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:setNextAlarm:Add Alarm failed");
		} 
		CAL_DEBUG_LOG("\n Next alarm set for this event\n");
	}
	else
    {
        CAL_DEBUG_LOG("No more instances so remove alarm");
        pEvent->removeAlarm();
        this->modifyEvent(pEvent, pErrorCode);
        CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:setNextAlarm:Modify Event Alarm failed");
    }

	if(pEvent) {
		delete pEvent;
		pEvent = 0;
	}

    return true;
}



/**
 * @param ACTION Enumerated data type
 * @param CComponent pointer
 * @param Calendar id
 * @param int * pErrorCode error code if any
 * @return TRUE/FALSE
 * Function used to manage the cookies
 */
bool CCalendar::manageCookie(const ACTION iAction, CComponent * pcomp,
                 int &pErrorCode)
{
    CAlarm alarmUtility;
    vector < long> cookie;
    vector <time_t>::iterator titer;
    vector <long> newCookie;
    vector <long>::iterator citer;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    /* Extract alarm details from CComponent object */
    time_t dateStart = pcomp->getDateStart();
    time_t dateEnd = pcomp->getDateEnd();
    string sSummary = pcomp->getSummary();
    string sLocation = pcomp->getLocation();
    string szDescription = pcomp->getDescription();
    CAlarm *pAlarm = pcomp->getAlarm();
    bool bReturn = false;
    time_t instanceTime = 0;

    /* if the event has recurrence then find to which instance the trigger 
     * belongs to and calculate thei effective date start for such events  */
    if (pcomp->updateAlarmTriggerTime(&instanceTime))
    {
        switch (iAction) {
        case E_ADD:
                CAL_DEBUG_LOG("ALARM: Adding alarm");

                int temp;
                temp = alarmUtility.addAlarmEvent(pAlarm->getTrigger(),
                        sSummary, sLocation, 
                        instanceTime, instanceTime + (dateEnd - dateStart),
                        pcomp->getId(),
                        this->getCalendarId(),
                        szDescription,
                        pcomp->getType(),
                        pcomp->getAllDay(),
                        pcomp->getTzid(),
                        pErrorCode);

                if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL){
                    cookie.push_back(temp);
                    pAlarm->setCookie(cookie);
                    bReturn = true;
                } else {
                    CAL_DEBUG_LOG("%d bad Cookie, Alarm Daemon might be inactive\n", temp);
                }
                break;

        case E_MODIFY:
            CAL_DEBUG_LOG("ALARM: Modify alarm");

            cookie = this->getCookie(pcomp->getId(), pErrorCode);

            if(!cookie.empty())
                alarmUtility.deleteAlarmEvent(cookie[0],pErrorCode );

            CAL_DEBUG_LOG("Delete Alarm event returned %d ", pErrorCode);

            if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
                break;

            int ephemeralcookie;
            ephemeralcookie =
            alarmUtility.addAlarmEvent(pAlarm->getTrigger(),
                        sSummary, sLocation,
                        instanceTime, instanceTime + (dateEnd - dateStart),
                        pcomp->getId(),
                        this->getCalendarId(),
                        szDescription,
                        pcomp->getType(),
                        pcomp->getAllDay(),
                        pcomp->getTzid(),
                        pErrorCode);

            CAL_DEBUG_LOG("Add Alarm event returned %d ", pErrorCode);

            if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL){
                newCookie.push_back(ephemeralcookie);
                pAlarm->setCookie(newCookie);
                bReturn = true;
            } else{
                CAL_DEBUG_LOG("%d bad ephemeralcookie, Alarm Daemon might be inactive\n",
                              ephemeralcookie);
            }
            break;

        default:
            // do nothing 
            CAL_DEBUG_LOG("ALARM: Nothing to be done \n");
            break;
        }
    }
    else
    {
        // No mor instances -> do not set alarm
        pErrorCode = CALENDAR_ALARM_ERROR;
    }

    return bReturn;
}


/*
 * @param event,todo ID 
 * @param int * pErrorCode error code if any
 * @return list of stored cookie id of alarm
 * API to retrive the stored cookies 
 */
vector <long> CCalendar::getCookie(string iEtjId, int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    vector <long>icookie;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    pDb = CCalendarDB::Instance();

    if (pDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        pErrorCode = CALENDAR_APP_ERROR;
        return icookie;
    }

    pQuery = sqlite3_mprintf(SELECT_DOMAIN, ALARM_FIELD_COOKIEID,
                 ALARM_TABLE, ENTRY_FIELD_ID, iEtjId.c_str());
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is: %s", pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0) {
        icookie.clear();
        return icookie;
    }

    if (pQr->pResult[ONE] != 0) 
        icookie.push_back(atoi(pQr->pResult[ONE]));

    if (pQr) {
        sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    return (icookie);

}


/**
 * @param  string szTable  table name 
 * @param  string szField column name 
 * @param string  szValue value to be checked 
 * 
 */
bool CCalendar::checkFieldinTable(string szTable, string szField,
                  string szValue)
{

    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    int pErrorCode  = CALENDAR_OPERATION_SUCCESSFUL;

    pDb = CCalendarDB::Instance();

    if (pDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        pErrorCode = CALENDAR_APP_ERROR;
        return false;
    }

    pQuery = sqlite3_mprintf(SELECT_TAB_COND2, szTable.c_str(),
                 szField.c_str(), szValue.c_str());
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s", pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    
    if (pQr == 0)
	return false;

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return true;

}


/**
 * @param string table name  
 * @param entryid can be Id of event todo or Journal 
 * @param int * pErrorCode error code if any
 * @return boolean ressult of operation 
 * this function is used to know whether there is any entry 
 * present in a table for particular ID 
 * 
 */
bool CCalendar::checkEntryExist(string szTable, string sId,
                int &pErrorCode)
{

    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    
    pDb = CCalendarDB::Instance();

    if (pDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        pErrorCode = CALENDAR_APP_ERROR;
        return false;
    }

    pQuery = sqlite3_mprintf(SELECT_TAB_COND2, szTable.c_str(),
                 ENTRY_FIELD_ID, sId.c_str());
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s", pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    CAL_DEBUG_LOG("SQLITE ERROR IS %d",iSqliteError);
    /* error code will be set int he mapper function above */
    if (pQr == 0) 
	return false;
    
    if (pQr) {
	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    return true;

}



/**
 * @param ptrAlarm pointer to Alarm
 * @param sId - id of component string  
 * @param iAction  type of Action 
 * @param int * pErrorCode error code if any
 * @return bool  result of the operation 
 *
 * This function is used to Add alarm in Data base 
 */
bool CCalendar::addAlarm(CAlarm * ptrAlarm, ACTION iAction, string sId,
             int &pErrorCode)
{



    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    int iCount = 0;
    int iSqliteError = SQLITE_OK;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	
    
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if ((pCalDb == 0) || (ptrAlarm == NULL)) {
	CAL_DEBUG_LOG("invalid CalDb pointer or event pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }
    if (iAction == E_ADD)
	pQuery =
	    sqlite3_mprintf(INSERT_ALARM, ALARM_FIELD_ID,
		    ALARM_FIELD_TRIGGER, ALARM_FIELD_REPEAT,
		    ALARM_FIELD_DURATION, ALARM_FIELD_ACTION,
		    ALARM_FIELD_COOKIEID, ALARM_FIELD_ATTACH);
    else
	pQuery =
	    sqlite3_mprintf(UPDATE_ALARM, ALARM_FIELD_TRIGGER,
		    ALARM_FIELD_REPEAT, ALARM_FIELD_DURATION,
		    ALARM_FIELD_ACTION, ALARM_FIELD_COOKIEID,
		    ALARM_FIELD_ATTACH, ALARM_FIELD_ID);


    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is :%s", pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),&pPreparedStmt, 
		    &pTail);
    sqlite3_free(pQuery);

    if (iAction == E_ADD) {
	sqlite3_bind_text(pPreparedStmt, ++iCount, sId.c_str(),
              sId.length(), SQLITE_TRANSIENT);
    }

    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrAlarm->getTrigger());
    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrAlarm->getRepeat());
    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrAlarm->getDuration() + ptrAlarm->getTimeBefore() * 10);
    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrAlarm->getAction());



    string cache;

    string temp;
    stringstream out;
    out << ((ptrAlarm->getCookie())[0]);
    temp = out.str();
    cache = cache + temp;

    sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
              cache.length(), SQLITE_TRANSIENT);


    if (ptrAlarm->getAttach().length() == 0)
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else
	sqlite3_bind_text(pPreparedStmt, ++iCount,
              (ptrAlarm->getAttach()).c_str(),
              (ptrAlarm->getAttach()).length(),
              SQLITE_TRANSIENT);

    if (iAction == E_MODIFY) {
	sqlite3_bind_text(pPreparedStmt, ++iCount, sId.c_str(),
		    sId.length(), SQLITE_TRANSIENT);
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addAlarm:Modify Alarm failed to update in DB");
    } 
    else {

	    pCalDb->insertRows(pPreparedStmt,iSqliteError);
	    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addAlarm:Add Alarm failed to insert in DB");
    }
	return true;

}

// 
bool CCalendar::addCacheEntry(string sId, time_t startDate, int &pErrorCode)
{
	 sqlite3_stmt *pPreparedStmt = 0;
	 char *pQuery = 0;
	 int iCount = 0;
	 int iSqliteError = 0;
	 const char *pTail = 0;
	 pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	 if((sId.empty()) || (startDate == 0)) {
		 pErrorCode = CALENDAR_APP_ERROR;
		 return false;
	 }

	 CCalendarDB *pCalDb = CCalendarDB::Instance();
	 if (pCalDb == 0) {
		 CAL_DEBUG_LOG("invalid CalDb pointer\n");
		 pErrorCode = CALENDAR_APP_ERROR;
		 return false;
	 }

	  pQuery =
		sqlite3_mprintf(INSERT_CACHE, CACHE_START, CACHE_ID);
	  ASSERTION(pQuery);
	  
	  CAL_DEBUG_LOG("Query for adding into cache is - %s", pQuery);
	  sqlite3_prepare(pCalDb->getDb(),pQuery,strlen(pQuery),
			  &pPreparedStmt,&pTail);
	  sqlite3_free(pQuery);
	  sqlite3_bind_int(pPreparedStmt, ++iCount, startDate);
	  sqlite3_bind_text(pPreparedStmt, ++iCount, sId.c_str(),
			   	sId.length(), SQLITE_TRANSIENT);

	  pCalDb->insertRows(pPreparedStmt,iSqliteError);
	  pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);

	  CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addCacheEntry failed");

	  return true;
}

/**
 * @param ptrRecur Pointer to Recurrence
 * @param iAction   Add / Modify
 * @param sId       Event id - string 
 * @param int * pErrorCode error code if any
 *
 * @return TRUE/FALSE
 * Function to add values in to recurrence table 
 * 
 */
bool CCalendar::addRecurrence(CRecurrence * ptrRecur, 
	string sId, int &pErrorCode)
{
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    int iCount = 0;
    int iSqliteError = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	

    if ((pCalDb == 0) || (ptrRecur == NULL)) {
	CAL_DEBUG_LOG("invalid CalDb pointer or event pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    pQuery =
	sqlite3_mprintf(INSERT_RECURSIVE, RECUR_FIELD_ID,
		RECUR_RRULE, RECUR_RDATES,
		RECUR_EDATES, RECUR_EXRULE,
		RECUR_RECURID, RECUR_RTYPE);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("Query is %s", pQuery);
    sqlite3_prepare(pCalDb->getDb(),pQuery,strlen(pQuery),&pPreparedStmt,&pTail);
    sqlite3_free(pQuery);

    sqlite3_bind_text(pPreparedStmt, ++iCount, sId.c_str(),
	    sId.length(), SQLITE_TRANSIENT);


    if (ptrRecur->getRrule().size() == 0)
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else {
	string cache;
	unsigned int iter;
	for (iter = 0; iter < (ptrRecur->getRrule()).size(); iter++)
	    cache = cache + ((ptrRecur->getRrule())[iter]) + EXCLAMATION;
	sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
		cache.length(), SQLITE_TRANSIENT);
    }

    if (!(ptrRecur->getRDays()).size())
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else {
	string cache;
	unsigned int iter;
	for (iter = 0; iter < (ptrRecur->getRDays()).size(); iter++)
	    cache = cache + ((ptrRecur->getRDays())[iter])+SEMI_COLON;
	sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
		cache.length(), SQLITE_TRANSIENT);
    }
    
    if (!(ptrRecur->getEDays()).size())
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else {
	string cache;
	unsigned int iter;
	for (iter = 0; iter < (ptrRecur->getEDays()).size(); iter++)
	    cache = cache + ((ptrRecur->getEDays())[iter]) + SEMI_COLON;
	sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
		cache.length(), SQLITE_TRANSIENT);
    }

    if ((ptrRecur->getErule()).size() == 0)
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else {
	string cache;
	unsigned int iter;
	for (iter = 0; iter < (ptrRecur->getErule()).size(); iter++)
	    cache = cache + ((ptrRecur->getErule())[iter]) + EXCLAMATION;
	sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
		cache.length(), SQLITE_TRANSIENT);
    }

    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrRecur->getRecurId());
    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrRecur->getRtype());

    sqlite3_bind_text(pPreparedStmt, ++iCount, sId.c_str(),
	    sId.length(), SQLITE_TRANSIENT);
    pCalDb->insertRows(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addRecurrence:addRecurrence failed to update in DB");


    return true;

}

/**
 * @param ptrRecur Pointer to Recurrence
 * @param sId       Event id - string 
 * @param int  pErrorCode error code if any
 *
 * @return TRUE/FALSE
 * Function to modify values in to recurrence table 
 * 
 */

bool CCalendar::modifyRecurrence(CRecurrence * ptrRecur,
	string sId, int &pErrorCode)
{
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    int iCount = 0;
    int iSqliteError = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	

    if ((pCalDb == 0) || (ptrRecur == NULL)) {
	CAL_DEBUG_LOG("invalid CalDb pointer or event pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    pQuery =
	sqlite3_mprintf(UPDATE_RECURSIVE, RECUR_RRULE,
		RECUR_RDATES, RECUR_EDATES,
		RECUR_EXRULE, RECUR_RECURID,
		RECUR_RTYPE, RECUR_FIELD_ID);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("Query is %s", pQuery);
    sqlite3_prepare(pCalDb->getDb(),pQuery,strlen(pQuery),&pPreparedStmt,&pTail);
    sqlite3_free(pQuery);
    if (ptrRecur->getRrule().size() == 0)
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else {
	string cache;
	unsigned int iter;
	for (iter = 0; iter < (ptrRecur->getRrule()).size(); iter++)
	    cache = cache + ((ptrRecur->getRrule())[iter]) + EXCLAMATION;
	sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
		cache.length(), SQLITE_TRANSIENT);
    }
    
    if (!(ptrRecur->getRDays()).size())
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else {
	string cache;
	unsigned int iter;
	for (iter = 0; iter < (ptrRecur->getRDays()).size(); iter++)
	    cache = cache + ((ptrRecur->getRDays())[iter]) + SEMI_COLON;
	sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
		cache.length(), SQLITE_TRANSIENT);
    }
    
    if (!(ptrRecur->getEDays()).size())
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else {
	string cache;
	unsigned int iter;
	for (iter = 0; iter < (ptrRecur->getEDays()).size(); iter++)
	    cache = cache + ((ptrRecur->getEDays())[iter]) + SEMI_COLON;
	sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
		cache.length(), SQLITE_TRANSIENT);
    }
    if ((ptrRecur->getErule()).size() == 0)
	sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    else {
	string cache;
	unsigned int iter;
	for (iter = 0; iter < (ptrRecur->getErule()).size(); iter++)
	    cache = cache + ((ptrRecur->getErule())[iter]) + EXCLAMATION;
	sqlite3_bind_text(pPreparedStmt, ++iCount, cache.c_str(),
		cache.length(), SQLITE_TRANSIENT);

    }
    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrRecur->getRecurId());
    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrRecur->getRtype());
    sqlite3_bind_text(pPreparedStmt, ++iCount, sId.c_str(),
	    sId.length(), SQLITE_TRANSIENT);
    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyRecurrence:modifyRecurrence failed to insert in DB");

    return true;


}
/**
 * @param int * pErrorCode error code if any
 * @return detailed list of Event 
 *
 * This function is called when a Event list
 * is to be retrieved from DB it gives verbose list of all Event in DB
 * this member function is added from sync point of view 
 */
vector <CEvent *> CCalendar::getDetailedBdayList(int &pErrorCode)
{
    vector <CEvent *> vListEvent;
    vector <CEvent *> ::iterator iterEvent;
    CEvent *temp = 0;
    vector <string> vListId;
    vector <string>::iterator iterId;
    vListId = getIdList(E_BDAY, pErrorCode);
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    
    CAL_DEBUG_LOG("Bday id list size = %d \n", vListId.size());
    for (iterId = vListId.begin(); iterId != vListId.end(); iterId++) {
	temp = this->getBirthDayEvent((*iterId), pErrorCode);
	if (temp) 
	    vListEvent.push_back(temp);

    }
    vListId.clear();
    CAL_DEBUG_LOG("printing the size of the bday List: %d", vListEvent.size());
    return (vListEvent);
}


/**
 * @param int * pErrorCode error code if any
 * @return detailed list of Event 
 *
 * This function is called when a Event list
 * is to be retrieved from DB it gives verbose list of all Event in DB
 * this member function is added from sync point of view 
 * */

vector<CEvent *> CCalendar::getDetailedEventList(int &pErrorCode)
{
    vector <CEvent *>vListEvent;
    vector <CEvent *>::iterator iterEvent;
    CEvent *temp = 0;
    vector <string> vListId;
    vector <string>::iterator iterId;
    vListId = getIdList(E_EVENT, pErrorCode);
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    CAL_DEBUG_LOG("event id list = %d \n", vListId.size());
    for (iterId = vListId.begin(); iterId != vListId.end(); iterId++) {
	CAL_DEBUG_LOG("In the loop\n");
	temp = getEvent((*iterId), pErrorCode);
	if (temp) {
	    vListEvent.push_back(temp);

	}

    }
    vListId.clear();
    CAL_DEBUG_LOG("printing the size of the Event List: %d", vListEvent.size());

    return (vListEvent);

}

/**
 * @param int * pErrorCode error code if any
 * @return detailed list of Todo 
 *
 * This function is called when a Todo list
 * is to be retrieved from DB it gives verbose list of all Todo in DB
 * this member function is added from sync point of view 
 * */

vector <CTodo *>CCalendar::getTodoListDetailed(int &pErrorCode)
{

    vector <CTodo *> vListTodo;
    vector <CTodo *>::iterator iteriTodo;
    CTodo *temp = 0;
    vector <string> vListId;
    vector <string>::iterator iterId;
    vListId = getIdList(E_TODO, pErrorCode);
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    for (iterId = vListId.begin(); iterId != vListId.end(); iterId++) {
	temp = getTodo((*iterId), pErrorCode);
	if (temp)
	    vListTodo.push_back(temp);

    }
    vListId.clear();
    CAL_DEBUG_LOG("printing the size of the Todo List: %d", vListTodo.size());
    return (vListTodo);

}


/**
 * @param string - filename of the ics file 
 * @param int * pErrorCode error code if any
 * @return  status of operation 
 *  Export file format will all be in line with ICAL format
 */

int CCalendar::exportCalendarEntries(string szFileName, int &pErrorCode)
{

    /*Declarations */
    vector <CTodo*> todoList;
    vector <CJournal*> journalList;
    vector <CEvent*> eventList;
    vector <CEvent*> bdayList;
    vector <CEvent*>::iterator bdayIter; 
    vector <CEvent*>::iterator eventIter;
    vector <CTodo*>::iterator todoIter;
    vector <CJournal*>::iterator journalIter;

    unsigned int iter = 0;
    int fd = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if ((fd = open(szFileName.c_str(), O_RDWR | O_CREAT | O_NDELAY | O_TRUNC,
          S_IRUSR|S_IWUSR)) == -1) {
	CAL_DEBUG_LOG("Unable to create new file \n");
    /* some error code has to be
     * handled  here */
	pErrorCode = CALENDAR_FILE_ERROR;
	return FAILURE;
    }

    ICalConverter *pIcal = new ICalConverter();
    ASSERTION(pIcal);

    //This API makes sure that if syncing is not
    //set we are just exporting the content to file
    //format and no encoding is required
    pIcal->setSyncing(false);
    /* if the Calendar is Bday calendar 
     * then export only Bdays 
     */
    if (this ->getCalendarType() == BIRTHDAY_CALENDAR){
	bdayList = this-> getDetailedBdayList(pErrorCode);

	for ( iter = 0; (iter < bdayList.size()) && (pIcal != NULL); iter++) {
	    //the life time of printtofile is restricted
	    //to one iteration of the loop.
	    string printtoFile;
	    unsigned int n = 0;
	    printtoFile =
		    pIcal->localToIcalVcal(bdayList[iter], ICAL_TYPE, pErrorCode);
	    printtoFile = printtoFile + "\r\n";
	    n = write(fd, printtoFile.c_str(), printtoFile.size());

	    if (n != printtoFile.size()) {
		CAL_DEBUG_LOG("write error in file ");
		pErrorCode = CALENDAR_FILE_ERROR;
		/* deleting pIcal*/
		delete pIcal;
		pIcal = 0;
	    }
	}

	/* deleting the event list in successful scenario*/
	for (bdayIter = bdayList.begin(); bdayIter != bdayList.end();bdayIter++)	
	    delete (*bdayIter);
	if(pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
			close(fd);
			if(pIcal)
				delete pIcal;
			return FAILURE;
	}
    } 
    else{
	eventList = this->getDetailedEventList(pErrorCode);
	for ( iter = 0; (iter < eventList.size()) && (pIcal != NULL); iter++) {
	    string printtoFile;
	    unsigned int n = 0;
	    printtoFile =
		    pIcal->localToIcalVcal(eventList[iter], ICAL_TYPE, pErrorCode);
	    printtoFile = printtoFile + "\r\n";
	    n = write(fd, printtoFile.c_str(), printtoFile.size());

	    if (n != printtoFile.size()) {
		CAL_DEBUG_LOG("write error in file ");
		pErrorCode = CALENDAR_FILE_ERROR;
		/* deleting pIcal*/
		delete pIcal;
		pIcal = 0;
	    }
	}

	    /* deleting the event list in successful scenario*/
	for (eventIter = eventList.begin();eventIter != eventList.end();eventIter++)	
	    delete (*eventIter);
        if(pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
	    close(fd);
	    if(pIcal)
		delete pIcal;
	    return FAILURE;
	}
	
	todoList = this->getTodoListDetailed(pErrorCode);
	for ( iter = 0; (iter < todoList.size()) && (pIcal != NULL); iter++) {
	    string printtoFile;
	    unsigned int n = 0;
	    printtoFile =
		    pIcal->localToIcalVcal(todoList[iter], ICAL_TYPE, pErrorCode);
	    printtoFile = printtoFile + "\r\n";
	    n = write(fd, printtoFile.c_str(), printtoFile.size());
	    if (n != printtoFile.size()) {
		pErrorCode = CALENDAR_FILE_ERROR;
		CAL_DEBUG_LOG("write error in file ");
		/* deleting pIcal */
		delete pIcal;
		pIcal = 0;
		/* freeing the todo list */
		for (todoIter = todoList.begin(); todoIter != todoList.end()
			    ; todoIter++)	
		    delete (*todoIter);
		close(fd);
		return FAILURE;
	    }
	    /*probably we can throw some error here */

	}
	/* freeing the todo list */
	for (todoIter = todoList.begin(); todoIter != todoList.end()
		    ; todoIter++)	
	    delete (*todoIter);
        if(pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
		close(fd);
		delete pIcal;
		return FAILURE;
	}
	journalList = this->getJournalListDetailed(pErrorCode);

	for ( iter = 0; (iter < journalList.size()) && (pIcal != NULL); iter++) {
	    string printtoFile;
	    unsigned int n = 0;
	    printtoFile = pIcal->localToIcalVcal(journalList[iter], ICAL_TYPE,
			    pErrorCode);
	    printtoFile = printtoFile + "\r\n";
	    n = write(fd, printtoFile.c_str(), printtoFile.size());
	    if (n != printtoFile.size()) {
		pErrorCode = CALENDAR_FILE_ERROR;
		CAL_DEBUG_LOG("write error in file ");
		/* deleting pIcal */
		delete pIcal;
		pIcal = 0;
	    }
	}
	/* freeing the Journal list in successful scenario */
	for (journalIter = journalList.begin(); journalIter != journalList.end()
		    ; journalIter++)	
	    delete (*journalIter);
        if(pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
		close(fd);
		delete pIcal;
		return FAILURE;
	}
    }
    delete pIcal;
    /*Close the file descriptor */
    close(fd);

    return SUCCESS;
}




/**
 * getJournalListDetailed
 * @param int * pErrorCode error code if any
 * @return detailed list of Journals 
 *
 * This function is called when a journal list
 * is to be retrieved from DB it gives verbose list of all Journals in DB
 * this member function is added from sync point of view 
 * */

vector <CJournal*>CCalendar::getJournalListDetailed(int &pErrorCode)
{

    vector <CJournal*> vListJournal;
    vector <CJournal*>::iterator iterJournal;
    CJournal *temp = 0;
    vector <string> vListId;
    vector <string>::iterator iterId;
    vListId = getIdList(E_JOURNAL, pErrorCode);

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    for (iterId = vListId.begin(); iterId != vListId.end(); iterId++) {
	temp = getJournal((*iterId), pErrorCode);
	if (temp) {
	    CAL_DEBUG_LOG("adding element to journal vector\n");
	    vListJournal.push_back(temp);
	}

    }
    vListId.clear();
    CAL_DEBUG_LOG("printing the size of the Journal List: %d",
          vListJournal.size());
    return (vListJournal);


}


/**
 * addAttendeeDetails
 * @param vector list of CAttendee pointers
 * @param string represents id of the component 
 * @param  int type can be EVENT , TODO or JOURNAL.
 * @param  ACTION type can be Add / Modify.
 * @param int * pErrorCode error code if any
 * @return whether the operation is successfull or not 
 *
 * This function is called when an entry
 * is to be added in attendee table for event todo or journal
 * it adds an event(provided by CEvent pointer) into database
 */
bool CCalendar::addAttendeeDetails(vector <CAttendee*>vAttenList,
                   string sId, int iType, ACTION iAction,
                   int &pErrorCode)
{


    /* Declare a hashmap and 
     * Put all the attendee details in to this map
     * then call addParamreters 
     */


    vector <CAttendee *>::iterator iter;
    map < string, vector <CParameters*> >hashmap;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid CalDb pointer or event pointer ");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }
    /* push the attendee values in to a  map< string , vector<CParameters*> */
    for (iter = vAttenList.begin(); iter != vAttenList.end(); iter++) {
	/* for attendee insert the MailTo value in key and 
	 * seperate them by the "+" symbol
	 */
	string szTemp;
	vector <CParameters*> vParameter;

	/* for attendee the key value used is a combination 
	 * of PropertyName and MailTo*/
	string stest(ATTDEE);
	szTemp = stest + "+" + (*iter)->getMailTo();

	CParameters *pParam = 0;
	bool nextParam = false;
	for (int i = 0; i < MAILTO; i++) {

	    pParam = new CParameters();
	    ASSERTION(pParam);
	    ParamType val;
	    val.i = 0;
	    val.szString = "";
	    nextParam = false;

	    switch (i) {

		case CALENDAR_USER_TYPE:
		    val.i = (*iter)->getCalendarUserType();
		    pParam->setParamValue(val);
		    pParam->setParamName(ATCUTYPE);
		    break;

		case MEMBER:
		    if ((*iter)->getMember().empty())
			nextParam = true;
		    else {
			val.szString = (*iter)->getMember();
			pParam->setParamValue(val);
			pParam->setParamName(ATMEMBER);
		    }
		    break;
		    
		case ROLE:
		    val.i = (*iter)->getRole();
		    pParam->setParamValue(val);
		    pParam->setParamName(ATROLE);
		    break;

		case PART_STATUS:
		    val.i = (*iter)->getParticipationStatus();
		    pParam->setParamValue(val);
		    pParam->setParamName(ATPSTAT);
		    break;

		case RSVP:
		    val.i = (*iter)->getRSVP();
		    pParam->setParamValue(val);
		    pParam->setParamName(ATRSVP);
		    break;

		case DELEGATEES:
		    if ((*iter)->getDelegatees().empty())
			nextParam = true;
		    else {
			val.szString = (*iter)->getDelegatees();
			pParam->setParamValue(val);
			pParam->setParamName(ATDELEGATEES);
		    }
		    break;

		case DELEGATOR:
		    if ((*iter)->getDelegator().empty())
			nextParam = true;
		    else {
			val.szString = (*iter)->getDelegator();
			pParam->setParamValue(val);
			pParam->setParamName(ATDELEGATOR);
		    }
		    break;

		case SENTBY:
		    if ((*iter)->getSentBy().empty())
			nextParam = true;
		    else {
			val.szString = (*iter)->getSentBy();
			pParam->setParamValue(val);
			pParam->setParamName(SENT);
		    }
		    break;

		case COMMON_NAME:
		    if ((*iter)->getCommonName().empty())
			nextParam = true;
		    else {
			val.szString = (*iter)->getCommonName();
			pParam->setParamValue(val);
			pParam->setParamName(CNAME);
		    }
		    break;

		case DIR_PARAM:
		    if ((*iter)->getDirectoryParameter().empty())
			nextParam = true;
		    else {
			val.szString = (*iter)->getDirectoryParameter();
			pParam->setParamValue(val);
			pParam->setParamName(DIRPAR);
		    }
		    break;

		case ATTLANGUAGE:
		    if ((*iter)->getLanguage().empty())
			nextParam = true;
		    else {
			val.szString = (*iter)->getLanguage();
			pParam->setParamValue(val);
			pParam->setParamName(LANGUAGE);
		    }
		    break;

		case MAILTO:
		    if ((*iter)->getMailTo().empty())
			nextParam = true;
		    else {
		       val.szString = (*iter)->getMailTo();
		       pParam -> setParamValue(val);
		       pParam->setParamName(GNMAILTO);
		    }
		    break;

		default:
		    break;
	    }            /* end of the switch  statement */

	    /* so all the parameters for 
	     * a given attendeee are pushed in to 
	     * vector of parameters*/
	    if (!nextParam)
		vParameter.push_back(pParam);
	    else {
		delete pParam;
		pParam = 0;
		nextParam = false;
	    }

	} /* end of the for loop */


	/* Now we have to push the values in to a predefined hash map*/
	hashmap.insert(pair < string, vector <CParameters*> >(szTemp, vParameter));

    } /* end of outer for loop */

    /* once all attendees are pushed in to 
     * the hash map we have to call addparameters 
     * method to update in the parameters table 
     */
    addParameters(hashmap, sId, E_ADD, pErrorCode);

    return true;

}

/**
 * addXProperties()
 */
bool CCalendar::addXProperties(vector <CProperties*> vXProp, string sId,
                   ACTION iAction, int &pErrorCode)
{

    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    vector <CProperties*>::iterator iter;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	
    int iCount = 0;

    CAL_DEBUG_LOG ("*** vXProp.size() = %d", vXProp.size());
    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid CalDb pointer or event pointer ");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    if (vXProp.size() == 0) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }
    
    for (iter = vXProp.begin(); iter != vXProp.end(); iter++) {

	iCount = 0;
	if (iAction == E_ADD)
	    pQuery = sqlite3_mprintf(INSERT_XPROP, XPROP_FIELD_ID,
		    XPROP_FIELD_XPROPNAME, XPROP_FIELD_XPROPVALUE);
	ASSERTION(pQuery);
	CAL_DEBUG_LOG("query is %s", pQuery);

	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			&pPreparedStmt, &pTail);
	sqlite3_free(pQuery);

	CAL_DEBUG_LOG("ievent id = %s \n", sId.c_str());
	CAL_DEBUG_LOG("iCount= %d \n", iCount);

	//if(iAction == E_ADD)
	sqlite3_bind_text(pPreparedStmt, ++iCount, sId.c_str(),
			sId.length(), SQLITE_TRANSIENT);


	CAL_DEBUG_LOG("*** xprop name = '%s'",
			(*iter)->getPropName().c_str());
	sqlite3_bind_text(pPreparedStmt, ++iCount,
			((*iter)->getPropName().c_str()),
			(*iter)->getPropName().length(),
			SQLITE_TRANSIENT);

	PropType val;
	val = (*iter)->getPropValue();
	CAL_DEBUG_LOG("*** xprop value = '%s'", val.szString.c_str());
	
	if (((*iter)->getDataType((*iter)->getPropName())) == STRING)
	    sqlite3_bind_text(pPreparedStmt, ++iCount,
			val.szString.c_str(), val.szString.length(),SQLITE_TRANSIENT);

	if (((*iter)->getDataType((*iter)->getPropName())) == INTEGER)
	    sqlite3_bind_int(pPreparedStmt, ++iCount, val.i);


	pCalDb->insertRows(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode); 
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addXprop:addXprop failed to insert in DB");

    }

    CAL_DEBUG_LOG("before returning addXProp()\n");
    return true;

}

/**
 * addParameters()
 * @param : hashMap to store prop and param values
 * @param : sId id of the entry
 * @param : iAction action to be taken
 * @param : int pErrorCode
 * @param : clearHashMap indicator for hashMap content deletion
 * @return : bool
 * 
 * This function adds/modifies parameters of an entry .
 */
bool CCalendar::addParameters(map < string,
                  vector <CParameters*> >hashMap, string sId,
                  ACTION iAction, int &pErrorCode,
                  bool clearHashMap)
{
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    int iCount = 0;
    int iSqliteError = 0;
    map < string, vector <CParameters*> >::iterator it;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid CalDb pointer or event pointer ");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    if (hashMap.size() == 0) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }


    for (it = hashMap.begin(); it != hashMap.end(); it++) {
	string szTemp;
	string szValue;
	string szorg = (*it).first;
	size_t pos = szorg.find(PLUS);
	if (pos != string::npos) {
	    szTemp = szorg.substr(0, pos);
	    szValue = szorg.substr(pos + 1);
	} 
	else
	    szTemp = szorg;
	/* if the string contains ATTDEE 
	 * ORGZER it means MAILTO parameter is also present 
	 * so we have to seperate the mailto from the actual name of the 
	 * parameter value for other parameters 0 will be bound   */

	for (unsigned int iParamCount = 0;
			iParamCount < (*it).second.size(); iParamCount++) {

	    iCount = 0;

	    if (iAction == E_ADD)
		pQuery =
		sqlite3_mprintf(INSERT_PARAM, PARAM_FIELD_ID,
				PARAM_FIELD_PROPNAME,
				PARAM_FIELD_PROPVALUE,
				PARAM_FIELD_PARAMNAME,
				PARAM_FIELD_PARAMVALUE);

	    ASSERTION(pQuery);
	    CAL_DEBUG_LOG("query is %s", pQuery);
	    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
					    &pPreparedStmt, &pTail);

	    sqlite3_free(pQuery);

	    sqlite3_bind_text(pPreparedStmt, ++iCount, sId.c_str(),
					    sId.length(), SQLITE_TRANSIENT);

	    sqlite3_bind_text(pPreparedStmt, ++iCount, szTemp.c_str(),
					    szTemp.length(), SQLITE_TRANSIENT);
	    sqlite3_bind_text(pPreparedStmt, ++iCount, szValue.c_str(),
					    szValue.length(), SQLITE_TRANSIENT);

	    sqlite3_bind_text(pPreparedStmt, ++iCount,
					    (*it).second[iParamCount]->getParamName().
					    c_str(),
					    (*it).second[iParamCount]->getParamName().
					    length(), SQLITE_TRANSIENT);

	    ParamType val;
	    val = (*it).second[iParamCount]->getParamValue();
	    /*check type */
	    /*if string */
	    if (((*it).second[iParamCount]-> getDataType((*it).second
				    [iParamCount]->getParamName())) == STRING)
		sqlite3_bind_text(pPreparedStmt, ++iCount, val.szString.c_str(),
				val.szString.length(), SQLITE_TRANSIENT);

	    if (((*it).second[iParamCount]-> getDataType((*it).second
				    [iParamCount]->getParamName())) == INTEGER)
		sqlite3_bind_int(pPreparedStmt, ++iCount, val.i);

	    pCalDb->insertRows(pPreparedStmt,iSqliteError);
	    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode); 
	    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addParameters:addParameters failed to insert in DB");

	} /* paramlist for */
	
	if (clearHashMap) {
		for (unsigned int iParamCount = 0;
				iParamCount < (*it).second.size(); iParamCount++) 
		delete (*it).second[iParamCount];
	}
	    
    } /* map for */
#if 0
    std::vector < CParameters * >::iterator iIter;
    std::vector < CParameters * > list;
    for (it = hashMap.begin(); it != hashMap.end(); it++) {
	list.clear();
	list = (*it).second;
	    CAL_DEBUG_LOG("Freeing the parameter list");
	    for (iIter = list.begin(); iIter != list.end(); iIter++) {
		CAL_DEBUG_LOG("LOOP");
		if(*iIter) 
		    delete(*iIter);
		(*iIter) = 0;
	    }
    }
#endif
    hashMap.clear();
    
    return true;
}

/**
 * addOrganizer
 * @param ptrOrganizer COrganizer pointer
 * @param string sId Id of the component
 * @param int * pErrorCode error code if any
 * @param iType type can be Event/Todo/Journal
 * @param iAction can be add/modify  
 * @return boolean value 
 *
 * This function is called when an event entry
 * is to be added in to organizer table .
 * it adds an event(provided by CEvent pointer) into database
 */

bool CCalendar::addOrganizer(COrganizer * ptrOrganizer, string sId,
                 int iType, ACTION iAction, int &pErrorCode)
{
    map < string, vector <CParameters*> > hashmap;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    string szTemp;
    vector <CParameters*> vParameter;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    
    if (pCalDb == 0) {
        CAL_DEBUG_LOG("invalid CalDb pointer or event pointer ");
        pErrorCode = CALENDAR_APP_ERROR;
        return false;
    }
    if (ptrOrganizer == 0) {
        pErrorCode = CALENDAR_INVALID_ARG_ERROR;
        return false;
    }

    /* we have to populate the organizer values in to a map 
     * and then try to update the actual value
     */
    /* for attendee the key value used is a combination 
     * of PropertyName and MailTo
     */
    /* Venkat: why are we using the following format? */
    string stest(ORGZER);
    szTemp = stest + PLUS + ptrOrganizer->getMailTo();

    CParameters *pParam = 0;
    bool nextParam = false;

    for (int i = 0; i <= ORGLANGUAGE; i++) {
        pParam = new CParameters();
        ASSERTION(pParam);
        ParamType val;
        nextParam = false;

	switch (i) {

	    case SENTBYPARAM:
		if (ptrOrganizer->getSentBy().empty())
		    nextParam = true;
		else {
		    val.szString = ptrOrganizer->getSentBy();
		    pParam->setParamValue(val);
		    pParam->setParamName(SENT);
		}

		break;

	    case CNPARAM:
		if (ptrOrganizer->getCommonName().empty())
		    nextParam = true;
		else {
		    val.szString = ptrOrganizer->getCommonName();
		    pParam->setParamValue(val);
		    pParam->setParamName(CNAME);
		}
		break;

	    case DIRPARAM:
		if (ptrOrganizer->getDirectoryParameter().empty())
		    nextParam = true;
		else {
		    val.szString = ptrOrganizer->getDirectoryParameter();
		    pParam->setParamValue(val);
		    pParam->setParamName(DIRPAR);
		}
		break;

	    case ORGLANGUAGE:
		if (ptrOrganizer->getLanguage().empty())
		    nextParam = true;
		else {
		    val.szString = ptrOrganizer->getLanguage();
		    pParam->setParamValue(val);
		    pParam->setParamName(LANGUAGE);
		}
		break;

	    default:
		break;
	}
	if (!nextParam)
	    vParameter.push_back(pParam);
	else {
	    delete pParam;
	    pParam = 0;
	    nextParam = false;
	}
    } /* end of the for loop */


    /* Now we have to push the values in to a predefined hash map*/
    hashmap.insert(pair < string, vector <CParameters*> >(szTemp, vParameter));

    /* now we have to add map in parameters table */
    addParameters(hashmap, sId, E_ADD, pErrorCode);

    return true;
}


/** addCompDetails
 * @param sqlite3_stmt* Sqlite pPreparedStmttement
 * @parm comp* template to CEvent, CTodo, CJournal
 * @param int * pErrorCode error code if any
 *
 * returns bool true/false
 *
 * This function is used to do the prepared statement for common parameters
 */
template <class comp>
    bool CCalendar::addCompDetails(sqlite3_stmt * pStmt, comp * cDet,
                   int &pErrorCode)
{
    int iCount = ONE;
    sqlite3_bind_int(pStmt, iCount++, atoi(cDet->getId().c_str()));

    sqlite3_bind_int(pStmt, iCount++, cDet->getType());

    sqlite3_bind_text(pStmt, iCount, cDet->getClas().c_str(),
              cDet->getClas().length(), SQLITE_TRANSIENT);
    //cDet->setDateStamp(time_get_time());
    iCount = iCount + THREE ; 
    sqlite3_bind_int(pStmt, iCount, cDet->getDateStamp());
    

    sqlite3_bind_int(pStmt, ++iCount, cDet->getSequence());
    /* skipping next parameter */    
    ++iCount;
    sqlite3_bind_int(pStmt, ++iCount, NULLID);


    sqlite3_bind_text(pStmt,++iCount , cDet->getUrl().c_str(),
              (cDet->getUrl()).length(), SQLITE_TRANSIENT);


    /*
       sqlite3_bind_text(pStmt, 13, cDet->getAttachmentsments(),
       column_len(cDet->getAttachmentsments()), SQLITE_TRANSIENT);
       this has to be corrected               */
    string szTemp;
    if (cDet->getAttachments().size())
        szTemp = cDet->getAttachments()[ZERO];

    sqlite3_bind_text(pStmt, ++iCount, szTemp.c_str(), szTemp.length(),
              SQLITE_TRANSIENT);
    sqlite3_bind_text(pStmt, ++iCount, cDet->getCategories().c_str(),
              (cDet->getCategories()).length(), SQLITE_TRANSIENT);
    sqlite3_bind_text(pStmt,++iCount , cDet->getComments().c_str(),
              (cDet->getComments()).length(), SQLITE_TRANSIENT);
    sqlite3_bind_text(pStmt, ++iCount, cDet->getContact().c_str(),
              (cDet->getContact()).length(), SQLITE_TRANSIENT);
    sqlite3_bind_text(pStmt,++iCount , cDet->getRelated().c_str(),
              (cDet->getRelated()).length(), SQLITE_TRANSIENT);
    sqlite3_bind_text(pStmt, ++iCount, cDet->getResources().c_str(),
              (cDet->getResources()).length(), SQLITE_TRANSIENT);

    return true;

}


/** 
 * addEventCompDetails
 * @param CEvent* Pointer to CEvent class
 * @param int * pErrorCode error code if any
 * @return istatus of operation 
 *
 * This function is used to add event related parameters into database
 */
bool CCalendar::addEventCompDetails(CEvent * pEvent, int &pErrorCode)
{
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    int iCount = 0;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        pErrorCode = CALENDAR_APP_ERROR;
        return false;
    }

/*FOR-RECURRENCE*/
    pQuery =
    sqlite3_mprintf(INSERT_COMPONENT, COMP_FIELD_ID, COMP_FIELD_TYPE,
            COMP_FIELD_CLAS, COMP_FIELD_GEO, COMP_FIELD_PRIO,
            COMP_FIELD_DTSTAMP, COMP_FIELD_SEQ,
            COMP_FIELD_TRANSP, COMP_FIELD_UID, COMP_FIELD_URL,
            COMP_FIELD_ATTACH, COMP_FIELD_CATEGORY,
            COMP_FIELD_COMMENT, COMP_FIELD_CONTACT,
            COMP_FIELD_RELATED, COMP_FIELD_RESOURCES,
            COMP_FIELD_PERCENT, COMP_FIELD_COMPLETED);
    ASSERTION(pQuery);

    CAL_DEBUG_LOG("query is \n---8<---\n%s\n---8<---\n", pQuery);

    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);

    /* Binding common parameters */

    if (!this->addCompDetails(pPreparedStmt, pEvent, pErrorCode))
	return false;


    /* Bind Event specific parameters */
    iCount = iCount + THREE + 1;
    sqlite3_bind_text(pPreparedStmt, iCount, (pEvent->getGeo()).c_str(),
              pEvent->getGeo().length(), SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, ++iCount, pEvent->getPriority());
    iCount = iCount + THREE;
    sqlite3_bind_text(pPreparedStmt, iCount,
              (pEvent->getTransparency()).c_str(),
              (pEvent->getTransparency()).length(),
              SQLITE_TRANSIENT);


    /* Assign -1 to Todo related parameters */
    sqlite3_bind_int(pPreparedStmt, TENSEVEN, -1);
    sqlite3_bind_int(pPreparedStmt, ++iCount, -1);

    pCalDb->insertRows(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode); 
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addEventCompDetails:addEventCompDetails failed to insert in DB");
    return true;
}


/**
 * addTodoCompDetails
 * @param CTodo* Pointer to CTodo class
 * @param int * pErrorCode error code if any
 * @return bool true or false
 *
 * This function is used to add todo related parameters into database
 */
bool CCalendar::addTodoCompDetails(CTodo * ptrTodo, int &pErrorCode)
{
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    int iCount = 0;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        pErrorCode = CALENDAR_APP_ERROR;
        return false;
    }

    pQuery = 0;
    pPreparedStmt = 0;
    CAL_DEBUG_LOG("addTodoCompDetails in");
    pQuery =
    sqlite3_mprintf(INSERT_COMPONENT, COMP_FIELD_ID, COMP_FIELD_TYPE,
            COMP_FIELD_CLAS, COMP_FIELD_GEO, COMP_FIELD_PRIO,
            COMP_FIELD_DTSTAMP, COMP_FIELD_SEQ,
            COMP_FIELD_TRANSP, COMP_FIELD_UID, COMP_FIELD_URL,
            COMP_FIELD_ATTACH, COMP_FIELD_CATEGORY,
            COMP_FIELD_COMMENT, COMP_FIELD_CONTACT,
            COMP_FIELD_RELATED, COMP_FIELD_RESOURCES,
            COMP_FIELD_PERCENT, COMP_FIELD_COMPLETED);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);

    /* Binding common parameters */

    CAL_DEBUG_LOG("Before addCompDetails ");
    this->addCompDetails(pPreparedStmt, ptrTodo, pErrorCode);
    CAL_DEBUG_LOG("After addCompDetails ");

    /* Bind Todo specific parameters */
    iCount = iCount + THREE + 1 ;
    sqlite3_bind_text(pPreparedStmt, iCount++, (ptrTodo->getGeo()).c_str(),
              (ptrTodo->getGeo()).length(), SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, iCount, ptrTodo->getPriority());
    sqlite3_bind_int(pPreparedStmt, TENSEVEN, ptrTodo->getPercentComplete());

    if (ptrTodo->getStatus() == COMPLETED_STATUS) {
    /* Completed is the time at which Task
     * is marked as completed while adding itself the
     * task is completed then it is same as adding time
     */
    ptrTodo->setCompleted(time_get_time());

    }
    iCount = TENSEVEN;
    sqlite3_bind_int(pPreparedStmt, ++iCount, ptrTodo->getCompleted());

    /* Assign -1 to Event related parameters */
    sqlite3_bind_text(pPreparedStmt, EIGHT, 0, 0, SQLITE_TRANSIENT);

    pCalDb->insertRows(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode); 
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addTodoCompDetails:addTodoCompDetails failed to insert in DB");
    return true;

}

/**
 * checkDuplicateEntry()
 * @param int iType - Type of entry can be event todo or journal 
 * @param int iType - Type of entry can be event todo or journal 
 * @param int&  pErrorCode -   
 * @return bool  - Duplicate is present or not 
 *
 * Function to check whether duplicate entry exist in DB
 */
bool CCalendar::checkDuplicateEntry(CComponent * pEntry, int iType,
                    int &pErrorCode)
{
/*
 * Duplicates are checked only for events and 
 * todos. It has to be done per calendar basis 
 * 
 * For events - title , date start ,dateend  " location ,description "  
 * if only one  todos - title and date 
 *         
 */

    /*
       #define SELECT_DUPLICATE_ENTRY  "select Id from CComponents where 
       summary = \"%s\" and dateStart = "%d" ComponentType = %d and 
       CalendarId = %d"
     */
    /* 
     * if the task is identified as duplicate 
     * we need to modify the existing task instead 
     * of adding a duplicate one*/


    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        pErrorCode = CALENDAR_APP_ERROR;
        return false;
    }

    int iSqliteError = 0;
    string sId;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pEntry->getLastModified() == NULLID) {
        pEntry->setLastModified(time_get_time());
    }
	
        if (iType == E_JOURNAL) {
        //datestart is not valid for TODOs - it is the current time always
        pQuery = sqlite3_mprintf(SELECT_DUPLICATE_JOURNAL,
                pEntry->getSummary().c_str(),
                iType,
                this->getCalendarId());

    }
    else {
        // otherwise include it in the search query
        pQuery = sqlite3_mprintf(SELECT_DUPLICATE_ENTRY,
                pEntry->getSummary().c_str(),
                pEntry->getDateStart(), iType,
                this->getCalendarId());
    }
    string strQuery(pQuery);
    sqlite3_free(pQuery);

    if (iType != E_TODO) {
        // now we add extra fields 
        // location 
        if (pEntry->getLocation().empty())
            strQuery += " AND location = ''";
        else        
            strQuery += " AND location = " + sqlquoted_string(pEntry->getLocation());

        // description
        if (pEntry->getDescription().empty())
            strQuery += " AND description = ''";
        else        
            strQuery += " AND description = " + sqlquoted_string(pEntry->getDescription());

        if (iType == E_EVENT) {
        // GUID should be checked for events only - for Birthdays  it holds contact UIDs.
            if (!pEntry->getGUid().empty())
                strQuery += " AND (uid = '' or uid = " + sqlquoted_string(pEntry->getGUid()) + ")";
        }
    }

    pQuery = (char*)strQuery.c_str();
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is : %s", pQuery);
    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);


    if (pQr == 0) {
	if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL)	    
	    pErrorCode = NO_DUPLICATE_ITEM;
	return false;
    }

    sId = pQr->pResult[1];
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    /* here we are sure that duplicate of the event / todo exists 
     * in the DB we need to make sure which one to retain and 
     * which one not to */

    /* check if local event in DB is latest
     * if yes update the local event 
     * if no then external event has to be modified*/

    CAL_DEBUG_LOG(" Id of the Duplicate Entry is  %s \n", sId.c_str());
    pEntry->setId(sId);

    pQuery =
    sqlite3_mprintf(SELECT_ENTRY_RETAIN, pEntry->getLastModified(), sId.c_str());

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is : %s", pQuery);
    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0) {
	if (pErrorCode  == CALENDAR_OPERATION_SUCCESSFUL)
	    pErrorCode = EXT_ITEM_RETAINED;
	CAL_DEBUG_LOG (" External  item is Latest one  so modifying it  \n");
	return true;
    }

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    pErrorCode = LOCAL_ITEM_RETAINED;
    CAL_DEBUG_LOG("Local item is latest one  Item retained ");

    return false;


}


/*
 * TODO the below piece of code is not tested 
 * will be tested and released in WK03 
 */

/**
 * @param  perrorCode 
 * @return bool success/failure indicator 
 * Function to register Alarm for visible calendars 
 */

bool CCalendar::registerAlarmForVisibleCalendar(int &perrorCode)
{

    QueryResult *pQr = 0;
    int iI_AlarmCount = 0;
    int iJ_AlarmCount = 0;
    int iK_AlarmCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    string cache;
    string sCompId;

    CAL_DEBUG_LOG("ALARM");

    pDb = CCalendarDB::Instance();
    perrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        perrorCode = CALENDAR_APP_ERROR;
        return false;
    }

    /*
     * #define SELECT_VISIBLE_ALARM select Id from Alarm where Id in 
     * (select Id from components where calendarid = %d) AND CookieId = /"%s/"  
     *
     */
    pQuery =
    sqlite3_mprintf(SELECT_VISIBLE_ALARM, this->getCalendarId(), "0",
            time_get_time());

    ASSERTION(pQuery);
    CAL_DEBUG_LOG(" query is:  %s \n", pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,perrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0) 
	return true;

    for (iI_AlarmCount = 0; iI_AlarmCount < pQr->iRow; iI_AlarmCount++) {
	for (iJ_AlarmCount = 0; iJ_AlarmCount < pQr->iColumn; iJ_AlarmCount++) {
	    iK_AlarmCount = pQr->iColumn + (iI_AlarmCount * pQr->iColumn);
	    if (pQr->pResult[iK_AlarmCount + iJ_AlarmCount] == 0 )
		    continue;
	    switch (iJ_AlarmCount) {

		case DB_COLUMN_ID1:
		    sCompId = pQr->pResult[iK_AlarmCount + iJ_AlarmCount];
		    break;

		default:
		    break;
	    }
	}
	CComponent *pComp = 0;
	if (this -> getCalendarType() != BIRTHDAY_CALENDAR)
	    pComp = (CComponent*) this->getEvent(sCompId, perrorCode);
	else
	    pComp = (CComponent*) this->getBirthDayEvent(sCompId, perrorCode);

	if (!pComp) {
	    if (pQr) {
	        sqlite3_free_table(pQr->pResult);
	        delete pQr;
	    }
	    return false;
	}

	/* register the alarm value  here *
	 * using alarmEventAddCall and then modify the 
	 * same in Database 
	 */
	long cookie = 0;
	CAlarm *pAlarm = 0;
    CAL_DEBUG_LOG("ALARM: Updtating trigger time");

    time_t instanceTime = 0;
    if (pComp->updateAlarmTriggerTime(&instanceTime))
    {
        pAlarm = pComp->getAlarm();

        if (pAlarm) {
            cookie = pAlarm->addAlarmEvent((time_t) pAlarm->getTrigger(),
                pComp->getSummary(),
                pComp->getLocation(),
                instanceTime, instanceTime + (pComp->getDateEnd() - pComp->getDateStart()),
                sCompId,
                this->getCalendarId(),
                pComp->getDescription(),
                pComp->getType(),
                pComp->getAllDay(),
                pComp->getTzid(),
                perrorCode);

            if (perrorCode == CALENDAR_OPERATION_SUCCESSFUL) {
                vector <long> vCookie;
                vCookie.push_back(cookie);

                pAlarm->setCookie(vCookie);
            }
            else
            {
                CAL_DEBUG_LOG("%ld bad Cookie, Alarm Daemon might be inactive\n", cookie);
            }
        }
    }

	/* Updating the alarm table with dummy cookie */
	this->addAlarm(pAlarm, E_MODIFY, pComp->getId(), perrorCode);
	if(perrorCode != CALENDAR_OPERATION_SUCCESSFUL)
		CAL_DEBUG_LOG("Alarm not registered for Entry with Id: %s ",
				pComp->getId().c_str());
	if(pComp) {
		delete pComp;
		pComp = 0;
	}

    }

    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }


    return true;
}

/**
 *  addJournalCompDetails
 * @param CJournal* Pointer to CJournal class
 * @param int * pErrorCode error code if any
 * @return bool true or false
 *
 * This function is used to add journal related parameters into database
 */
bool CCalendar::addJournalCompDetails(CJournal * ptrJournal,
                      int &pErrorCode)
{
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    int iCount = 0;
    int iSqliteError = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        pErrorCode = CALENDAR_APP_ERROR;
        return false;
    }

    pQuery = 0;
    pPreparedStmt = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    pQuery =
    sqlite3_mprintf(INSERT_COMPONENT, COMP_FIELD_ID, COMP_FIELD_TYPE,
            COMP_FIELD_CLAS, COMP_FIELD_GEO, COMP_FIELD_PRIO,
            COMP_FIELD_DTSTAMP, COMP_FIELD_SEQ,
            COMP_FIELD_TRANSP, COMP_FIELD_UID, COMP_FIELD_URL,
            COMP_FIELD_ATTACH, COMP_FIELD_CATEGORY,
            COMP_FIELD_COMMENT, COMP_FIELD_CONTACT,
            COMP_FIELD_RELATED, COMP_FIELD_RESOURCES,
            COMP_FIELD_PERCENT, COMP_FIELD_COMPLETED);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);

    /* Binding common parameters */
    this->addCompDetails(pPreparedStmt, ptrJournal, pErrorCode);

    iCount = iCount + THREE + ONE;
    sqlite3_bind_text(pPreparedStmt, iCount++, 0, 0, SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, iCount , -1);
    sqlite3_bind_int(pPreparedStmt, TENSEVEN, -1);
    sqlite3_bind_int(pPreparedStmt, ++iCount, -1);

    sqlite3_bind_text(pPreparedStmt, EIGHT, 0, 0, SQLITE_TRANSIENT);

    pCalDb->insertRows(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode); 
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addJournalCompDetails:addJournalCompDetails failed to insert in DB");
    return true;
}


/**
 * addComponent
 * @param pEvent calEvent pointer
 * @param int * pErrorCode error code if any
 * @return int Add component status
 *
 * This function is called when an event entry
 * is to be added.
 * it adds an entry into Component table  
 * */
template <class ent> int 
CCalendar::addComponent(
		ent     *pEvent, 
		ACTION   iAction, 
		int     &pErrorCode)
{
    sqlite3_stmt  *pPreparedStmt = 0;
    const char    *pTail = 0;
    int            iCount = 0;
    char          *pQuery = 0;
    int            iSqliteError = 0;
    CCalendarDB   *pCalDb = CCalendarDB::Instance();
    CMulticalendar *pMc = CMulticalendar::MCInstance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    /* Fetching the system timezone and 
     * adding it to Event Object */
//printf("\n CCalendar::addComponent() \n");
    
    if ((pCalDb == 0) || (pEvent == NULL)) {
	CAL_DEBUG_LOG("invalid CalDb pointer or event pointer ");
	pErrorCode = CALENDAR_APP_ERROR;
	
	return FAILURE;
    }

    string sTzid ;
    /* if the timezone field is 
     * empty then set the local timezone
     */
    if (pEvent->getTzid().empty()){
    sTzid= pMc->getSystemTimeZone();
    pEvent->setTzid(sTzid);
    }
    pEvent->setTzOffset(time_get_time_diff(pEvent->getDateStart(), pEvent->getTzid().c_str(), "UTC")); 
    
    /*FOR-RECURRENCE*/
    if (iAction == E_ADD)
	pQuery = sqlite3_mprintf(INSERT_ENTRY, 
        ENTRY_FIELD_CALID, ENTRY_FIELD_TYPE,
		ENTRY_FIELD_FLAGS, ENTRY_FIELD_DTSTART,
		ENTRY_FIELD_DTEND, ENTRY_FIELD_SUMMARY,
		ENTRY_FIELD_LOC, ENTRY_FIELD_DESC,
		ENTRY_FIELD_STATUS, ENTRY_FIELD_UID,
		ENTRY_FIELD_UNTIL, ENTRY_FIELD_ALLDAY,
		ENTRY_FIELD_CRTIME, ENTRY_FIELD_MODTIME,
		ENTRY_FIELD_TZID,
		ENTRY_FIELD_TZOFFSET);
    else
	pQuery = sqlite3_mprintf(UPDATE_COMPONENTS, ENTRY_FIELD_CALID,
		ENTRY_FIELD_TYPE, ENTRY_FIELD_FLAGS,
		ENTRY_FIELD_DTSTART, ENTRY_FIELD_DTEND,
		ENTRY_FIELD_SUMMARY, ENTRY_FIELD_LOC,
		ENTRY_FIELD_DESC, ENTRY_FIELD_STATUS,
		ENTRY_FIELD_UID, ENTRY_FIELD_UNTIL,
		ENTRY_FIELD_ALLDAY, ENTRY_FIELD_MODTIME,
		ENTRY_FIELD_TZID,
		ENTRY_FIELD_TZOFFSET, ENTRY_FIELD_ID);

    if (pQuery == 0) {
	CAL_DEBUG_LOG ("Not enough memory?");
	pErrorCode = CALENDAR_MEMORY_ERROR;
	return FAILURE;
    }

    CAL_DEBUG_LOG("query is \n---8<---\n%s\n---8<---\n", pQuery);
    sqlite3_prepare (pCalDb->getDb(), pQuery, strlen(pQuery), 
		    &pPreparedStmt, &pTail);

    sqlite3_free(pQuery);

    CAL_DEBUG_LOG("*** pPreparedStmt = %p", pPreparedStmt);
    if (pPreparedStmt == 0) {
	CAL_DEBUG_LOG ("sqlite3_prepare() failed. No memory?");
	pErrorCode = CALENDAR_MEMORY_ERROR;
	return FAILURE;
    }


    sqlite3_bind_int(pPreparedStmt, ++iCount, this->getCalendarId());


    sqlite3_bind_int (pPreparedStmt, ++iCount, pEvent->getType());

    /*
     * If we have recurrence we need to change the flags value in event
     * HAS_RECURRENCE  3
     * HAS_ALARM       4
     * HAS_RECURRENCE_ALARM 5 defined in common.h
     */
    if ((pEvent->getRecurrence() != 0) && (pEvent->getAlarm() != 0))
	pEvent->setFlags(HAS_RECURRENCE_ALARM);
    else if ((pEvent->getRecurrence() != 0) && (pEvent->getAlarm() == 0))
	pEvent->setFlags(HAS_RECURRENCE);
    else if ((pEvent->getRecurrence() == 0) && (pEvent->getAlarm() != 0))
	pEvent->setFlags(HAS_ALARM);
    else
	pEvent->setFlags(NULLID);

    CAL_DEBUG_LOG ("Flags value obtianed is %d", pEvent->getFlags());

    sqlite3_bind_int (pPreparedStmt, ++iCount, pEvent->getFlags());
    sqlite3_bind_int (pPreparedStmt, ++iCount, pEvent->getDateStart());
    sqlite3_bind_int (pPreparedStmt, ++iCount, pEvent->getDateEnd());

    sqlite3_bind_text (pPreparedStmt, ++iCount,
		    pEvent->getSummary().c_str(), 
		    pEvent->getSummary().size(),
		    SQLITE_TRANSIENT);


    sqlite3_bind_text (pPreparedStmt, ++iCount,
		    pEvent->getLocation().c_str(),
		    pEvent->getLocation().size(),
		    SQLITE_TRANSIENT);


    sqlite3_bind_text (pPreparedStmt, ++iCount,
		    pEvent->getDescription().c_str(), 
		    pEvent->getDescription().size(),
		    SQLITE_TRANSIENT);

    sqlite3_bind_int (pPreparedStmt, ++iCount, pEvent->getStatus());

    /*FOR-RECURRENCE */
    sqlite3_bind_text(pPreparedStmt, ++iCount, 
		    pEvent->getGUid().c_str(),
		    pEvent->getGUid().size(),
		    SQLITE_TRANSIENT);

    sqlite3_bind_int (pPreparedStmt, ++iCount, pEvent->getUntil());

    /* FOR-RECURRENCE */
    // for all day event 
    // this will be 1 for all others it will be 0
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEvent->getAllDay());
    if (iAction == E_ADD) {
	    pEvent->setCreatedTime(time_get_time());
	    sqlite3_bind_int(pPreparedStmt, ++iCount,pEvent->getCreatedTime());
    }

    pEvent->setLastModified(time_get_time());
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEvent->getLastModified());

    /** adding the time zone to which event belongs */
    sqlite3_bind_text (pPreparedStmt, ++iCount, 
		    pEvent->getTzid().c_str(),
		    pEvent->getTzid().size(),
		    SQLITE_TRANSIENT);

    sqlite3_bind_int (pPreparedStmt, ++iCount, pEvent->getTzOffset());
    /* 
     * Adding data into ComponentCache table 
     */
    if (iAction == E_MODIFY) {
	   sqlite3_bind_int(pPreparedStmt, ++iCount,atoi( pEvent->getId().c_str()));
	   pCalDb->updateDB(pPreparedStmt,iSqliteError);
	   pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
       CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addComponent:addCompoent failed to update DB");
	   CAL_DEBUG_LOG("id is %s", pEvent->getId().c_str());
    }
    else {
    
        CAL_DEBUG_LOG("\n now insert rows\n");
        int id = pCalDb->insertRows(pPreparedStmt,iSqliteError);
        CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addComponent:addComponent failed to insert in DB");

        /*convert returned id to text and store it */
        static char  temp[10];
        sprintf(temp,"%d",id);
        pEvent->setId(temp);
        pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
        CAL_DEBUG_LOG("\nCCALENDAR:addComponent: result is %d from %d id is %d\n",pErrorCode,iSqliteError, id);
    }

#ifdef ADD_COMPONENT_INSTANCES
    /*  now update Instances */

    // delete old instances if any
    CAL_DEBUG_LOG("deleting instances for event %s", pEvent->getId().c_str());
    CMulticalendar::MCInstance()->dropInstances(pEvent->getId());

    // add new instances
    CAL_DEBUG_LOG("adding instances for event %s", pEvent->getId().c_str());


    // calculate recurrent instances
    vector<time_t> instances;
    pEvent->generateInstanceTimes(1, 2145830400, instances);
    CAL_DEBUG_LOG("calculated  %d instances", instances.size());

    time_t duration = 0;

    if (pEvent->getType() == E_EVENT && !pEvent->getAllDay()) 
        duration =  pEvent->getDateEnd() - pEvent->getDateStart();

    if(instances.size()){
       // add instances
       CMulticalendar::MCInstance()->addInstances(pEvent->getId(),instances,duration);
    }
    else {
       // add this event start date
       CMulticalendar::MCInstance()->addInstance(pEvent->getId(),pEvent->getDateStart(),duration);
    }
#endif
       
   // CMulticalendar::MCInstance()->commitAllChanges();
    return SUCCESS;
}


/**
 * addEvent
 * @param pEvent calEvent pointer
 * @param int * pErrorCode error code if any
 * @return bool 
 *
 * This function is called when an event entry
 * is to be added.
 * it adds an event(provided by calEvent pointer) into database
 * */
bool CCalendar::addEvent(CEvent * pEvent, int &pErrorCode)
{

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    if (!pEvent) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }

    CAL_DEBUG_LOG("calenadr ID is%d", this->getCalendarId());
    /* 
     * check to ensure that datestart 
     * should be greater than date end 
     */
    if (pEvent->getDateStart() > pEvent->getDateEnd()) {
	CAL_DEBUG_LOG
	    ("Date Start is greater than DateEnd - Illegal time values provided!");
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }
    /* check For Duplicate event and make a decision 
     * to retain the existing events or new event  
     */


    checkDuplicateEntry(pEvent, E_EVENT, pErrorCode);

    if (pErrorCode == EXT_ITEM_RETAINED) {
	/* if external item is to be retained then 
	 * modify the existing one
	 */
	modifyEvent(pEvent, pErrorCode);
	if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG ("Duplicate exist so modifying the Existing event\n");
	    pErrorCode = CALENDAR_ENTRY_DUPLICATED;
	    return true;
	} 
	else {
	    CAL_DEBUG_LOG(" Failure in modifying duplicate Event \n");
	    return false;
	}
    } else if (pErrorCode == LOCAL_ITEM_RETAINED) {
	/* Entry existing in DB is the latest one
	 * so marking the same 
	 */
	CAL_DEBUG_LOG("Calendar item is the newer one so no \
		    need to update anything in calendar returning true \n");
	pErrorCode = CALENDAR_ENTRY_DUPLICATED;
	return true;
    }

    CAL_DEBUG_LOG("No duplicates exist so adding a new Item \n");
    /* over write the error in case there is no duplicate item 
     * present. Control reaches at this point only if the 
     * Calendar entry is not duplicated 
     * 
     */
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    
    /* Add event details to component table */
    this->addComponent(pEvent, E_ADD, pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode, "CALENDAR:addEvent: addcomponent to failed");
    CAL_DEBUG_LOG("CALENDAR:addEvent: Error code is : %d ", pErrorCode);
    
    /* adding data in to componentdetails */
    addEventCompDetails(pEvent, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG("Error code is : %d, so returning from here",
		  pErrorCode);
	return false;
    }

    /* adding attendee details */
    if ((pEvent->getAttendees()).size() != 0) {
	this->addAttendeeDetails(pEvent->getAttendees(), pEvent->getId(),
		     E_EVENT, E_ADD, pErrorCode);

	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("Error code is : %d, so returning from here",
		  pErrorCode);
	    return false;
	}
    }
    /* adding organizer details */
    if (pEvent->getOrganizer()) {
	this->addOrganizer(pEvent->getOrganizer(), pEvent->getId(),
		   E_EVENT, E_ADD, pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("Error code is : %d, so returning from here",
		  pErrorCode);
	    return false;
	}
    }

    /* adding recurrence details */
    if (pEvent->getRecurrence()) {
	this->addRecurrence(pEvent->getRecurrence(),
                pEvent->getId(), pErrorCode);

	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("Error code is : %d, so returning from here",
		  pErrorCode);
	    return false;
	}
    }


    /*Add Alarm */
    CAlarm *pAlarm = 0;
    pAlarm = pEvent->getAlarm();

    if (pAlarm) {        // cookie management 
	if (!this->IsShown()) {
	    /* Donot set alarm values for non 
	     * visible calendars*/
	    CAL_DEBUG_LOG
	    ("Calendar is invisible  so not registernig the Alarm event ");
	    this->addAlarm(pAlarm, E_ADD, pEvent->getId(), pErrorCode);

	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		CAL_DEBUG_LOG("Error code is : %d, so returning from here",
		      pErrorCode);
		return false;
	    }
	} 
	else {
        if (this->manageCookie(E_ADD, pEvent, pErrorCode)) {
            /* Adding alarm for the component */
            this->addAlarm(pAlarm, E_ADD, pEvent->getId(), pErrorCode);
            if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
                CAL_DEBUG_LOG ("Error code is : %d, so returning from here",
                pErrorCode);
                return false;
            }
        }
        else if (pErrorCode == CALENDAR_ALARM_ERROR) {
            pEvent->removeAlarm();
            this->addComponent(pEvent, E_MODIFY, pErrorCode);

            CAL_DEBUG_LOG
                ("Alarm error so ignoring add alarm request and \
                adding it as normal event \n");
            pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	    }
	    else {
		CAL_DEBUG_LOG("Manage Cookie returned errror \n");
		return false;
	    }
	}
    }

    /*adding xproperties if any  */
    if ((pEvent->getXProperties()).size() != 0) {
	this->addXProperties(pEvent->getXProperties(), pEvent->getId(),
                 E_ADD, pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("Error code is : %d, so returning from here",
              pErrorCode);
	    return false;
	}
    }


    /* adding parameters */
    if ((pEvent->getHashMap()).size() != 0) {

	this->addParameters(pEvent->getHashMap(), pEvent->getId(), E_ADD,
                pErrorCode, false);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("Error code is : %d, so returning from here",
		  pErrorCode);
	    return false;
	}
    }

    return true;
}


/**
 * addTodo
 * @param ptrTodo calTodo pointer
 * @param int * pErrorCode error code if any
 * @return bool 
 *
 * This function is called when a todo entry
 * is to be added.
 * it adds a todo(provided by calTodo pointer) into database
 * */
bool CCalendar::addTodo(CTodo * ptrTodo, int &pErrorCode)
{

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (!ptrTodo) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }
    /* check for duplicates if it is duplicate task then 
     * call modifyTodo to update the created and last modified values 
     */
    checkDuplicateEntry(ptrTodo, E_TODO, pErrorCode);
    if (pErrorCode == EXT_ITEM_RETAINED) {

	if (modifyTodo(ptrTodo, pErrorCode)) {
	    CAL_DEBUG_LOG
		("Duplicate exist so modifying the created and Last modified values \n");
	    pErrorCode = CALENDAR_ENTRY_DUPLICATED;
	    return true;
	} 
	else {
	    CAL_DEBUG_LOG(" Failure in modifying duplicate todo \n");
	    pErrorCode = CALENDAR_DATABASE_ERROR;
	    return false;
	}

    } 
    else if (pErrorCode == LOCAL_ITEM_RETAINED) {

	CAL_DEBUG_LOG("Calendar item is the newer one so"
		      "need to update anything in calendar returning true \n");
	pErrorCode = CALENDAR_ENTRY_DUPLICATED;
	return true;

    }
    CAL_DEBUG_LOG(" No duplicate Todo exists so adding  new todo \n");

    /* overwriting the error code here */
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    /* adding data to components table */

    this->addComponent(ptrTodo, E_ADD, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
              pErrorCode);
	return false;
    }
    CAL_DEBUG_LOG("addComponent done");

    /* adding data to component details table */

    addTodoCompDetails(ptrTodo, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
              pErrorCode);
    return false;

    }
    CAL_DEBUG_LOG("addComponentDetails done");

    /* adding attendee details */

    if ((ptrTodo->getAttendees()).size() != 0) {
	this->addAttendeeDetails(ptrTodo->getAttendees(),
                 ptrTodo->getId(), E_TODO, E_ADD, pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
		  pErrorCode);
	    return false;
	}
    }
    CAL_DEBUG_LOG("addAttendies done");

    /* adding organizer details */

    if (ptrTodo->getOrganizer()) {
	this->addOrganizer(ptrTodo->getOrganizer(), ptrTodo->getId(),
		   E_TODO, E_ADD, pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
		  pErrorCode);
	    return false;
	}

    }

    /* adding recurrence details although recurrence is not 
     * supported for tasks in calendar other apps might need
     */
    if (ptrTodo->getRecurrence()) {
	this->addRecurrence(ptrTodo->getRecurrence(),
		    ptrTodo->getId(), pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
		  pErrorCode);
	    return false;
	}

    }
    CAL_DEBUG_LOG("addRecurrence done");

    /*Add Alarm */
    CAlarm *pAlarm = 0;
    pAlarm = ptrTodo->getAlarm();
    if (pAlarm) {
	if (!this->IsShown()) {
	    /* register alarm for only 
	     * visible calendars*/

	    CAL_DEBUG_LOG
	    ("Calendar is invisible  so not registernig the Alarm event ");
	    this->addAlarm(pAlarm, E_ADD, ptrTodo->getId(), pErrorCode);

	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
			  pErrorCode);
		return false;
	    }
	} 
	else {

	    if (this->manageCookie(E_ADD, ptrTodo, pErrorCode)) {
            this->addAlarm(pAlarm, E_ADD, ptrTodo->getId(), pErrorCode);
            if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
                CAL_DEBUG_LOG
                    ("ErrorCode is :%d ,So returning from here "
                    , pErrorCode);
                return false;
            }

	    } 
	    else if (pErrorCode == CALENDAR_ALARM_ERROR) {
            ptrTodo->removeAlarm();
            this->addComponent(ptrTodo, E_MODIFY, pErrorCode);

            CAL_DEBUG_LOG
                ("Alarm error so ignoring add alarm request and "
                "adding it as normal event \n");
            pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	    }
	    else
	    return false;
	}
    }
    CAL_DEBUG_LOG("addAlarm done");

    /*adding xproperties */
    if ((ptrTodo->getXProperties()).size() != 0) {

	this->addXProperties(ptrTodo->getXProperties(), ptrTodo->getId(),
		     E_ADD, pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
		  pErrorCode);
	    return false;
	}
    }
    CAL_DEBUG_LOG("addXProp done");
    /* adding parameters */
    if ((ptrTodo->getHashMap()).size() != 0) {

	this->addParameters(ptrTodo->getHashMap(), ptrTodo->getId(), E_ADD,
		    pErrorCode, false);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
		  pErrorCode);
	    return false;
	}
    }
    CAL_DEBUG_LOG("addParam done");

    return true;
}


/**
 * addJournal
 * @param ptrJournal calJournal pointer
 * @param int * pErrorCode error code if any
 * @return bool 
 *
 * This function is called when a journal entry
 * is to be added.
 * it adds a journal(provided by calJournal pointer) into database
 **/
bool CCalendar::addJournal(CJournal * ptrJournal, int &pErrorCode)
{

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (!ptrJournal) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }

    checkDuplicateEntry(ptrJournal, E_JOURNAL, pErrorCode);
    if (pErrorCode == EXT_ITEM_RETAINED) {

    if (modifyJournal(ptrJournal, pErrorCode)) {
        CAL_DEBUG_LOG
        ("Duplicate exist so modifying the created and Last modified values \n");
        pErrorCode = CALENDAR_ENTRY_DUPLICATED;
        return true;
    } 
    else {
        CAL_DEBUG_LOG(" Failure in modifying duplicate note \n");
        pErrorCode = CALENDAR_DATABASE_ERROR;
        return false;
    }

    } 
    else if (pErrorCode == LOCAL_ITEM_RETAINED) {

    CAL_DEBUG_LOG("Calendar item is the newer one so"
              "need to update anything in calendar returning true \n");
    pErrorCode = CALENDAR_ENTRY_DUPLICATED;
    return true;

    }
    CAL_DEBUG_LOG(" No duplicate notes exists so adding  new note \n");

    /* overwriting the error code here */
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    /* adding data to components table *
     */
    this->addComponent(ptrJournal, E_ADD, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
              pErrorCode);
	return false;
    }

    /* adding data to componentdetails table */

    addJournalCompDetails(ptrJournal, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
              pErrorCode);
	return false;
    }
    /* adding attendee details */
    if ((ptrJournal->getAttendees()).size() != 0) {
	this->addAttendeeDetails(ptrJournal->getAttendees(),
                 ptrJournal->getId(), E_JOURNAL, E_ADD,
                 pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
              pErrorCode);
	    return false;
	}

    }

    /* adding organizer details */

    if (ptrJournal->getOrganizer()) {
	this->addOrganizer(ptrJournal->getOrganizer(), ptrJournal->getId(),
               E_JOURNAL, E_ADD, pErrorCode);

	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
              pErrorCode);
	    return false;
	}
    }

    /*adding xproperties */
    if ((ptrJournal->getXProperties()).size() != 0) {

	this->addXProperties(ptrJournal->getXProperties(),
		     ptrJournal->getId(), E_ADD, pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
		  pErrorCode);
	    return false;
	}

    }
    /* adding parameters */
    if ((ptrJournal->getHashMap()).size() != 0) {

	this->addParameters(ptrJournal->getHashMap(), ptrJournal->getId(),
		    E_ADD, pErrorCode, false);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG("ErrorCode is :%d ,So returning from here ",
		  pErrorCode);
	    return false;
	}
    }
    return true;
}

/**
 * destroyExistingValidAlarms
 * @param pErrorCode - reference to error Code 
 * @return status of operation
 *
 * Function to deregister all the alarms which are 
 * already set for a  visible calendar  
 */
bool CCalendar::destroyExistingValidAlarms(int &pErrorCode)
{

    /* First get all component ID  which are mapped 
     * to this Calendar and have valid alarms in alarm table 
     * then use it for 
     */

    QueryResult *pQr = 0;
    int iI_AlarmCount = 0;
    int iJ_AlarmCount = 0;
    int iK_AlarmCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    string cache;
    string sCompId;
    vector <long> oldcookie;
    CAlarm alarmUtility;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    pDb = CCalendarDB::Instance();

    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    /*
     * #define SELECT_ACTIVE_ALARM select Id from Alarm where Id in 
     * (select Id from components where calendarid = %d) AND CookieId != /"%s/"  
     *
     */
    pQuery =
    sqlite3_mprintf(SELECT_ACTIVE_ALARM, this->getCalendarId(), "0",
            time_get_time());

    ASSERTION(pQuery);
    CAL_DEBUG_LOG(" query is:  %s \n", pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0)
	return true;

    for (iI_AlarmCount = 0; iI_AlarmCount < pQr->iRow; iI_AlarmCount++) {
        for (iJ_AlarmCount = 0; iJ_AlarmCount < pQr->iColumn; iJ_AlarmCount++) {
            iK_AlarmCount = pQr->iColumn + (iI_AlarmCount * pQr->iColumn);
            switch (iJ_AlarmCount) {

            case DB_COLUMN_ID1:
                sCompId = pQr->pResult[iK_AlarmCount + iJ_AlarmCount];
                break;

            default:
                break;

            }
        }

        /* for each such  component deregister the alarm
        * First obtain the cookie Id and deregister the 
        * alarm 
        *  */
        oldcookie = this->getCookie(sCompId, pErrorCode);

        if (oldcookie.size()) 
            alarmUtility.deleteAlarmEvent(oldcookie[0],pErrorCode);

        CComponent *pComp = 0;
        if (this -> getCalendarType() != BIRTHDAY_CALENDAR)
            pComp = this->getEvent(sCompId, pErrorCode);
        else
            pComp = this->getBirthDayEvent(sCompId, pErrorCode);

        if (!pComp) {
            if (pQr) {
                    sqlite3_free_table(pQr->pResult);
                delete pQr;
                pQr = 0;
                }
            return false;
        }
        /* register the alarm value  here *
        * using alarmEventAddCall and then modify the 
        * same in Database 
        */

        long cookie = 0;
        CAlarm *pAlarm =  pComp->getAlarm();

        if (pAlarm) {
            CAL_DEBUG_LOG("Remove alarm for event #%s '%s'", pComp->getId().c_str(), pComp->getSummary().c_str());

            vector <long> vCookie;
            vCookie.push_back(cookie);

            pAlarm->setCookie(vCookie);

            /* Updating the alarm table with dummy cookie */
            if (!this->addAlarm(pAlarm, E_MODIFY, pComp->getId(), pErrorCode)) {
                CAL_DEBUG_LOG("Alarm not registered for Entry #%s ",
                               pComp->getId().c_str());
            }
        } else {
            CAL_DEBUG_LOG("Event #%s '%s' have no alarm", 
                            pComp->getId().c_str(),
                            pComp->getSummary().c_str());
        }

        if (pComp) {
            delete pComp;
            pComp = 0;
        }
    }

    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }

    return true;
}

/**
 * deleteComponent
 * @param sId of the component to be deleted
 * @param int * pErrorCode error code if any
 * @return NONE 
 * 
 * Function deletes component from DB based on Component ID  
 */
void CCalendar::deleteComponent(string sId, int &pErrorCode)
{

    /* Check whether the component-type is Event , Todo or Journal 
     * E_EVENT
     * E_TODO
     * E_JOURNAL component type
     */

    char *pQuery = 0;
    entryType compType ;
    QueryResult *pQr = 0;
    int iSqliteError = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid CalDb pointer or event pointer ");
	pErrorCode = CALENDAR_APP_ERROR;
	return;
    }
    /*
     *  Select ComponentType from components where Id = %d;
     *  select %s from %s where %s = %d;
     */
    pQuery =
    sqlite3_mprintf(SELECT_DOMAIN, ENTRY_FIELD_TYPE, COMPONENTS_TABLE,
            ENTRY_FIELD_ID, sId.c_str());

    ASSERTION(pQuery);
    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CAL_DEBUG_LOG("query is given by  %s", pQuery);
    sqlite3_free(pQuery);

    if (pQr == 0) {
	return;
    }

    compType = (entryType) atoi(pQr->pResult[pQr->iColumn]);

    switch (compType) {

        case E_EVENT:
            this->deleteEvent(sId, pErrorCode);
            break;
        case E_TODO:
            this->deleteTodo(sId, pErrorCode);
            break;
        case E_JOURNAL:
            this->deleteJournal(sId, pErrorCode);
            break;
        default:
            CAL_DEBUG_LOG(" Invalid component type in database \n");
	    break;
    }

    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
}

/**
 * deleteEvent
 * @param sEtjId eventId to be deleted
 * @param int * pErrorCode error code if any
 * @return bool 
 *
 * This function is called when an event
 * is to be deleted.
 * it deletes an event(provided by eventId) from database
 *
 */
bool CCalendar::deleteEvent(string sEtjId, int &pErrorCode)
{
    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    vector <long> oldcookie;
    vector <long>::iterator citer;
    int iSqliteError = 0;
    
    /**************** Caching logic is commented **************** 
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    if(this->deleteCacheEntries(sEtjId, E_EVENT, pErrorCode) == false) {
	    pErrorCode = CALENDAR_APP_ERROR;
	    return false;
    }
    **************************************************************/
    
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid Caldb pointer ");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    /* update the trash table with the values */
    this->updateTrash(this->getCalendarId(), sEtjId,
              E_EVENT, time_get_time(), pErrorCode);

    oldcookie = this->getCookie(sEtjId, pErrorCode);
    CAlarm alarmUtility;
   
   /* deleting from components table will fire a DB trigger 
    * which will delete from other tables */ 
    pQuery =
    sqlite3_mprintf(DELETE_TABLE_AND, COMPONENTS_TABLE, ENTRY_FIELD_ID,
            ENTRY_FIELD_TYPE);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("delete query is %s", pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);
    sqlite3_bind_text(pPreparedStmt, DB_QUERY_INDEX1, sEtjId.c_str(), 
		    sEtjId.length(), SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, DB_QUERY_INDEX2, E_EVENT);
    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deleteEvent:deleteEvent failed to upadte COMPONENTS_TABLE in DB");
    /* its not a big deal if event is deleted and 
     * alarm still exist so not rolling back in this case 
     * as this is a corner scenario 
     */
    if(!oldcookie.empty())
        alarmUtility.deleteAlarmEvent(oldcookie[0],pErrorCode);

    return true;
}


// 
int CCalendar::getCacheTableCount(time_t inputDate, int &pErrorCode)
{
	 char *pQuery = 0;
	 QueryResult *pQr = 0;
	 int iSqliteError = 0;
	 pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	 int iCount = 0;

	 if(inputDate == 0) {
		 pErrorCode = CALENDAR_APP_ERROR;
		 return iCount;
	 }
	 
	 CCalendarDB *pDb = CCalendarDB::Instance();
	 if(pDb == 0) {
		 pErrorCode = CALENDAR_APP_ERROR;
		  CAL_DEBUG_LOG("invalid caldb pointer when getting count");
		   return false;
	 }

	 pQuery = sqlite3_mprintf(SELECT_COUNTCOMPONENT, CACHE_TABLE, 
			 CACHE_START, inputDate);

	 ASSERTION(pQuery); 
	 pQr = pDb->getRecords(pQuery,iSqliteError);
	 pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	 CAL_DEBUG_LOG("query is given by  %s", pQuery);

	 sqlite3_free(pQuery);

	 if (pQr == 0) {
		 CAL_DEBUG_LOG("Query result is null when getting count");
		 return false;
	 }

	 iCount = atoi(pQr->pResult[1]);
	 sqlite3_free_table(pQr->pResult);
	 delete pQr;
	 return iCount;
}

// 
void CCalendar::checkIfMultiDayEvent(CComponent* pEntry, 
		vector <time_t> &vStartDates)
{
	CUtility objUtility;
	time_t iStartDate = 0;
	time_t iEndDate = 0;
	int iCount = 0;

	iStartDate = objUtility.getDateFromTime(pEntry->getDateStart());
	iEndDate = objUtility.getDateFromTime(pEntry->getDateEnd());

	//TODO: Check with Addy if start date and end date 
	//can be zero in any scenario
	if((iStartDate == 0) || (iEndDate == 0)) {
		return;
	}

	if(iStartDate != iEndDate) {
		CAL_DEBUG_LOG("----Multiday event is being added---");
		for(iCount = iStartDate; iCount <= iEndDate; 
				iCount = iCount + ONEDAY) {
			vStartDates.push_back(iCount);
		CAL_DEBUG_LOG("----the pushed date is:%d---", iCount);
		}
	} else {
		vStartDates.push_back(iStartDate);
	}
}
			
	
// 
bool  CCalendar::addCacheEntries(CComponent * pEntry, int& pErrorCode)
{
	vector <time_t> vStartDates;
	int iCount = 0;
	int iLength = 0;
	time_t startTime = 0;
	time_t startDate = 0;
	string szIds;
	string szEventId;
	CUtility objUtility;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	if(pEntry == NULL) {
		pErrorCode = CALENDAR_APP_ERROR;
		return false;
	}

	//for adding non-recursive events.
	//Multiday event is treated as recursive
	//event and added into CACHE table for all
	//the days of the event
	if(!pEntry->getRecurrence()) {
		checkIfMultiDayEvent(pEntry, vStartDates);
	} else {
		pEntry->generateInstanceTimes(0, 0, vStartDates);
	}
	iLength = vStartDates.size();

	CAL_DEBUG_LOG("-------------The length is:%d----------\n", iLength);

	szEventId = pEntry->getId();
	if(szEventId.empty()) {
		pErrorCode = CALENDAR_APP_ERROR;
		return false;
	}

	for(iCount = 0; iCount < iLength; iCount++) {
		szIds.clear();
		startTime = vStartDates[iCount];
		startDate = objUtility.getDateFromTime(startTime);
		if(startDate == 0) {
			continue;
		}
		
		if(getCacheTableCount(startDate, pErrorCode) > 0) {

			szIds = getCacheEntry(startDate, pErrorCode);
			if(szIds.empty()) {
				continue;
			}
			if(appendToIds(szEventId, szIds)) {
				if(!updateCacheEntry(szIds, startDate, 
							pErrorCode)) {
					return false;
				}
			}
		} else {
			if(appendToIds(szEventId, szIds)) {
				if(!addCacheEntry(szIds, startDate, 
							pErrorCode)) {
					return false;
				}
			}
		}
	}
		
	return true;
}
/* this function returns the instance Ids
 * on which the event is present 
 */
vector<CCache*> CCalendar::getDeletedCacheTableInfo(string sCompId,int &pErrorCode)
{
	
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    CCache* pCache = 0;
    CUtility objUtility;
    vector <CCache*> vListCache;
    vector<string> IdList;
    string szTemp;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return vListCache;
    }

	/***
	 *  first make a query from DB to see if the 
	    #define SELECT_ID_LIKE "select * from %s where Id like "%%s%"
	    Id is like the current on 
	 * 
	 */
    string  szQuery (SELECT_ID_LIKE);
    szQuery = szQuery + "\"" + "%" + sCompId + "%" + "\"" ;
    CAL_DEBUG_LOG("query is %s",szQuery.c_str());
    pQr = pDb->getRecords((char*)szQuery.c_str(),iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    /* error code if any will be set 
     * in the mapper function */
    
    if (pQr == 0) 
	return vListCache;
    int iI_ColCount =0;
    int iJ_ColCount =0;
    int iK_ColCount =0;

    for (iI_ColCount = 0; iI_ColCount < pQr->iRow; iI_ColCount++) {
	    pCache = new CCache();
	    ASSERTION(pCache);
	    for (iJ_ColCount = 0; iJ_ColCount < pQr->iColumn; iJ_ColCount++) {
		    iK_ColCount = pQr->iColumn + (iI_ColCount * pQr->iColumn);

		    if ((pQr->pResult[iK_ColCount + iJ_ColCount]) == 0)
			    continue;

		    switch (iJ_ColCount) {

			    case DB_COLUMN_ID1:
				    
				    pCache->setCacheStartDate(atoi(pQr->pResult[iK_ColCount + iJ_ColCount]));
				    break;

			    case DB_COLUMN_ID2:
				    szTemp= (char*)pQr->pResult[iK_ColCount + iJ_ColCount]; 
				    IdList = objUtility.parseIds(szTemp);
				    pCache->setCacheIds(IdList);
				    IdList.clear();
				    szTemp.clear();
				    break;
			    default:
				    break;
		    }
	    }
	    vListCache.push_back(pCache);
    }


    CAL_DEBUG_LOG(" Size of list on which Id %s is present is %d " , sCompId.c_str(),vListCache.size());
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return vListCache;

}

// 
bool CCalendar::deleteCacheEntries(string szId, int eEntry, 
					int &pErrorCode)
{
	int iCount = 0;
	int iLength = 0;
	time_t startDate = 0;
	string szIds;
	string szUpdatedIds;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	if(szId.empty()) {
		pErrorCode = CALENDAR_APP_ERROR;
		return false;
	}
	/* 

	CCache 
		istartDate
		vListId
	*/

	vector <CCache*> listCache;
	listCache = getDeletedCacheTableInfo(szId,pErrorCode);
	
	iLength = listCache.size();
		
	for(iCount = 0; iCount < iLength; iCount++) {
		szUpdatedIds.clear();
		szIds = listCache[iCount]-> getCommaSeperatedIdString();
		startDate = listCache[iCount]->getCacheStartDate();
		/* remove the Id deleted event Id 
		 * from the column value  
		 */  
		szUpdatedIds = removeFromIds(szId, szIds);
		CAL_DEBUG_LOG("Original list is:%s\n", szIds.c_str());
		CAL_DEBUG_LOG("Updated Id  list is:%s\n", szUpdatedIds.c_str());

		if(szUpdatedIds.empty()) {
			if(!deleteCacheEntry(startDate, pErrorCode)) {
				return false;
			}
		} else {
			if(!updateCacheEntry(szUpdatedIds, startDate, 
						pErrorCode)) {
				return false;
			}
		}
	}

    CUtility *pUt = 0;
    pUt = CUtility::Instance();
    pUt->releaseVector<CCache>(listCache);
	return true;
}

// 
string CCalendar::removeFromIds(string szId, string szIdStrings)
{
	vector <string> vIdList;
	CUtility objUtility;
	int iCount = 0;
	int iLength = 0;
	string szOutput;
	string szSeparator = ",";

	//TODO: can optimize this logic. No need to parse
	//the strings into vector and get them again
	if((szId.empty()) || (szIdStrings.empty())) {
		return szOutput;
	}
	vIdList = objUtility.parseIds(szIdStrings);
	iLength = vIdList.size();
	if(iLength == 0) {
		return szOutput;
	}
	
	for(iCount = 0; iCount < iLength; iCount++) {
		if(vIdList[iCount] != szId) {
			szOutput.append(szSeparator);
			szOutput.append(vIdList[iCount]);
		}
	}
	if(!szOutput.empty())
	szOutput.append(szSeparator);

	return szOutput;
}

// 
bool CCalendar::updateCacheEntry(string szIds, time_t startDate,
				 int &pErrorCode)
{
	char *pQuery = 0;
	sqlite3_stmt *pPreparedStmt = 0;
	const char *pTail = 0;
	int iCount = 0;
	int iSqliteError = 0;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	CAL_DEBUG_LOG("Updating the cache entry\n");

	if((szIds.empty()) || (startDate == 0)) {
		pErrorCode = CALENDAR_APP_ERROR;
		return false;
	}
	
	CCalendarDB *pCalDb = CCalendarDB::Instance();
	 if ((startDate == 0) || (pCalDb == 0) || szIds.empty()) {
		 CAL_DEBUG_LOG("Calendar Invalid CalDb pointer");
		 pErrorCode = CALENDAR_APP_ERROR;
		 return false;
	 }

	 pQuery = sqlite3_mprintf(UPDATE_TABLE, CACHE_TABLE, 
			 CACHE_ID, CACHE_START);
	 ASSERTION(pQuery);
	 sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			  &pPreparedStmt, &pTail);
	 CAL_DEBUG_LOG("pQuery is : %s", pQuery);
	 sqlite3_free(pQuery);
	 pQuery = 0;

	 if (pPreparedStmt == 0) {
		 pErrorCode = CALENDAR_APP_ERROR;
		 return false;
	 }

	 sqlite3_bind_text(pPreparedStmt, ++iCount, szIds.c_str(), 
			 szIds.length(), SQLITE_TRANSIENT);
	 sqlite3_bind_int(pPreparedStmt, ++iCount, startDate);
	 pCalDb->updateDB(pPreparedStmt,iSqliteError);
	 pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	 return true;
}

// 
string CCalendar::getCacheEntry(time_t startDate, int &pErrorCode)
{
	 QueryResult *pQr = 0;
	 char *pQuery = 0;
	 CCalendarDB *pDb = 0;
	 int iSqliteError = 0;
	 string szIds;
	 pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	 if(startDate == 0) {
		  pErrorCode = CALENDAR_APP_ERROR;
		  return szIds;
	 }
	 
	 pDb = CCalendarDB::Instance();
	 if (pDb == 0) {
		  CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
		  pErrorCode = CALENDAR_APP_ERROR;
		  return szIds;
	 }
	 
	 pQuery =
	 sqlite3_mprintf(SELECT_TAB_COND3, CACHE_ID, CACHE_TABLE, CACHE_START,
			  startDate);
	 ASSERTION(pQuery);

	 pQr = pDb->getRecords(pQuery,iSqliteError);
	 pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	 CAL_DEBUG_LOG("pQuery is for getting cache entry is %s", pQuery);
	 sqlite3_free(pQuery);

	 if (pQr == 0) {
	   CAL_DEBUG_LOG(" Error in getting entries");
	   return szIds;
	 }

	  szIds = pQr->pResult[1];

	  sqlite3_free_table(pQr->pResult);
	  delete pQr;
	  pQr = 0;

	  CAL_DEBUG_LOG("------Ids is %s-----\n", szIds.c_str());
	  return szIds;
}

// 
bool CCalendar::appendToIds(string szId, string& szIds)
{
	string szOutput;
	string szSeparator = ",";
	string szCompare;

	if(szId.empty()) {
		return false;
	}
	szCompare.append(szSeparator);
	szCompare.append(szId);
	szCompare.append(szSeparator);

	//Append only if not there already
	if(szIds.find(szCompare) == string::npos)
	{
		if(szIds.empty()){
			szIds.append(szSeparator);
		}

		szIds.append(szId);
		szIds.append(szSeparator);
		//return true if strings have been updated 
		//else return false
		return true;
	}

	return false;
}


// 
bool CCalendar::deleteCacheEntry(time_t startDate, int &pErrorCode)
{
	char *pQuery = 0;
	sqlite3_stmt *pPreparedStmt = 0;
	const char *pTail = 0;
	int iCount = 0;
	int iSqliteError = 0;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	if(startDate == 0) {
		pErrorCode = CALENDAR_APP_ERROR;
		return false;
	}

	CCalendarDB *pCalDb = CCalendarDB::Instance();
	if(pCalDb == 0) {
		pErrorCode = CALENDAR_APP_ERROR;
		CAL_DEBUG_LOG("invalid caldb pointer when deleting cache");
		return false;
	}

	pQuery = sqlite3_mprintf(DELETE_TABLE, CACHE_TABLE, CACHE_START);
	ASSERTION(pQuery);

	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery), 
			&pPreparedStmt, &pTail);
	sqlite3_free(pQuery);

	sqlite3_bind_int(pPreparedStmt, ++iCount, startDate);
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"deleteCache failed");

	return true;
}


//TODO: Logic needs to be changed. This is for prototype purpose
//Currently removing all entries and adding new entries for every
//relevant update
bool CCalendar::modifyCacheEntry(CComponent* oldEntry, int &pErrorCode)
{
	CAL_DEBUG_LOG("In ModifyCache ");

	if(oldEntry == NULL) {
		pErrorCode = CALENDAR_APP_ERROR;
		return false;
	}
	/********************* 	commented the cache implementation ***
	if(deleteCacheEntries(oldEntry->getId(), oldEntry->getType(), 
				pErrorCode) == false) {
		return false;
	}
	if(addCacheEntries(oldEntry, pErrorCode) == false) {
		return false;
	}
	***********************************************************/

	return true;
}
	
/**
 * deleteTodo
 * @param sEtjId todoid to be deleted
 * @param int * pErrorCode error code if any
 * @return bool
 *
 * This function is called when a todo entry
 * is to be deleted.
 * it deletes a todo(provided by todoid) from database
 * */

bool CCalendar::deleteTodo(string sEtjId, int &pErrorCode)
{

    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    vector <long>oldcookie;
    vector <long>::iterator citer;
	int iSqliteError = 0;

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid caldb pointer :");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    /***************** Commented caching logic ************************ 
    if(deleteCacheEntries(sEtjId, E_TODO, pErrorCode) == false) {
	    CAL_DEBUG_LOG("unable to delete from cache");
	    pErrorCode = CALENDAR_APP_ERROR;
	    return false;
    }
    ***************************************************************/

    /*      update the trash table with the 
     *      values */
    this->updateTrash(this->getCalendarId(), sEtjId,
              E_TODO, time_get_time(), pErrorCode);
    /* old cookie value has to be fetched before deleting  Component table as
     * Db trigger associated with it will clear Alarm table too*/
    oldcookie = this->getCookie(sEtjId, pErrorCode);

    pQuery = sqlite3_mprintf(DELETE_TABLE_AND, COMPONENTS_TABLE, ENTRY_FIELD_ID,
            ENTRY_FIELD_TYPE);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);
    sqlite3_bind_text(pPreparedStmt, DB_COLUMN_ID2, sEtjId.c_str(), 
		    sEtjId.length(), SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, DB_COLUMN_ID3, E_TODO);
    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deleteTodo:deleteTodo failed to update COMPONENTS_TABLE in DB");
    CAlarm alarmUtility;
    /* its not a big deal if event is deleted and 
     * alarm still exist so not rolling back in this case 
     * as this is a corner scenario 
     */
    if(!oldcookie.empty())
        alarmUtility.deleteAlarmEvent(oldcookie[0],pErrorCode);
    return true;	

}


/**
 * deleteJournal 
 * @param iJournalId iJournalId to be deleted
 * @param int * pErrorCode error code if any
 * @return bool 
 *
 * This function is called when a journal entry
 * is to be deleted.
 * it deletes a journal(provided by journalid) from database
 **/

bool CCalendar::deleteJournal(string sJournalId, int &pErrorCode)
{

    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    int iSqliteError = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid CCalendarDB pointer ");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }
    /*      update the trash table with the 
     *      values */
    this->updateTrash(this->getCalendarId(), sJournalId,
              E_JOURNAL, time_get_time(), pErrorCode);
   /* DB trigger will ensure that data is deleted from other tables */
    pQuery =
    sqlite3_mprintf(DELETE_TABLE_AND, COMPONENTS_TABLE, ENTRY_FIELD_ID,
            ENTRY_FIELD_TYPE);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);
    sqlite3_bind_text(pPreparedStmt, DB_COLUMN_ID2, sJournalId.c_str(),
              sJournalId.length(), SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, DB_COLUMN_ID3, E_JOURNAL);

    pCalDb->updateDB(pPreparedStmt,iSqliteError); 
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deleteJournal:deleteJournal failed to update COMPONENT_TABLE iin DB");
    return true;
}



/**
 *  setCalendarName 
 *  @param strName Calendar Name to be set
 *  @return bool (SUCCESS/FAILURE)
 * 
 *  This function is called to set the name of calendar.
 *  it sets the strName as the calendar name.
 *  */

bool CCalendar::setCalendarName(string szName)
{
    this->szName = szName;
    return true;

}

/**
 * setCalendarColor 
 * @param strColor Calendar Color to be set
 * @return bool (SUCCESS/FAILURE)
 *
 * This function is called to set the color of calendar.
 * it sets the strColor as the calendar color.
 * */

bool CCalendar::setCalendarColor(CalendarColour iColor)
{
    this->iColor = iColor;
    return true;
}

/**
 *  setCalendarId 
 *  @param id Calendar id to be set
 *  @return bool (SUCCESS/FAILURE)
 * 
 *  This function is called to set the id of calendar.
 *  it sets the id as the calendarid.
 **/

bool CCalendar::setCalendarId(int iId)
{

    iCalId = iId;
    return SUCCESS;


}

/**
 *  Calendar 
 *  @param iCalid integer clendar Id 
 *  @param name represents calendar name
 *  @param color represents calendar color
 *  @param readonly represents whether calendar read only or not 
 *  @param isvisbile represents whether calendar visible/invisible
 *  @param calendarTune represents calendarTune 
 * 
 *  This is parameterized constructor for Calendar class.
 *  it initializes the name,color,id,readonly ,visible , calendartype and
 *  calendarTune  parameters
 *  of the calendar.
 *   */

CCalendar::CCalendar(int iCalId, string szName, CalendarColour iColor, 
	bool fReadOnly, bool fIsVisible, CalendarType iCalType, 
	string szCalendarTune):szName(szName), iColor(iColor), 
	fReadOnly(fReadOnly),fIsVisible(fIsVisible), iCalId(iCalId), 
	iCalType(iCalType),szCalendarTune(szCalendarTune)
{
    this->iProfileType = SYNC_METHOD_NONE;
}

/**
 *  Calendar 
 *  @param fname represents calendar name
 *  @param fcolor represents calendar color
 *  @param freadonly represents whether calendar read only or not 
 *  @param fisvisbile represents whether calendar visible/invisible
 *  @param calendarTune represents calendarTune 
 * 
 *  This is parameterized constructor for Calendar class.
 *  it initializes the name,color,readonly and visible parameters
 *  of the calendar.
 *   */

CCalendar::CCalendar(string szName, CalendarColour iColor, bool fReadOnly, 
	bool fIsVisible, CalendarType iCalType, string szCalendarTune, 
	string szCalendarVersion):szName(szName), iColor(iColor), 
        fReadOnly(fReadOnly),fIsVisible(fIsVisible), iCalId(-1), 
	iCalType(iCalType),szCalendarTune(szCalendarTune), 
	szCalendarVersion(szCalendarVersion)
{

    this->iProfileType = SYNC_METHOD_NONE;
}

/**
 *  setCalendarReadOnly 
 *  @param flag represents whether calendar is readonly 
 *  @return bool (SUCCESS/FAILURE)
 *  
 *  This function is called to set whether calendar is readonly.
 *  */

bool CCalendar::setCalendarReadOnly(bool fFlag)
{

    fReadOnly = fFlag;
    return fReadOnly;
}

/**
 * getCalendarColor 
 * @param None
 * @return returns calendar color as an Enumeration
 *  This function is called to get the calendar color.
 **/

CalendarColour CCalendar::getCalendarColor()
{
    return iColor;
}

/**
 * getCalendarName 
 * @param None
 * @return returns calendar name as a char pointer
 * 
 * This function is called to get the calendar name.
 * */
string CCalendar::getCalendarName()
{
    return szName;
}

/**
 *  getCalendarId 
 *  @param None
 *  @return returns the calendar id
 * 
 *  This function is called to get the calendar id.
 *  */
int CCalendar::getCalendarId()
{
    return iCalId;
}

/**
 * IsReadOnly 
 * @param None
 * @return returns whether calendar is readonly as a bool 
 *
 * This function is called to know whether calendar is readonly.
 * */

bool CCalendar::IsReadOnly()
{
    return fReadOnly;
}

/**
 *  IsShown 
 *  @param None
 *  @return returns the visibility of the calendar
 *  
 * This function is called to know calendar visibility .
 */
bool CCalendar::IsShown()
{
    return fIsVisible;
}

/**
 *  setCalendarShown 
 *  @param flag represents whether calendar is visible or not
 *  @return bool (SUCCESS/FAILURE)
 *  
 *  This function is called to set whether calendar is visible.
 **/
bool CCalendar::setCalendarShown(bool fFlag)
{

    fIsVisible = fFlag;
    return fIsVisible;
}

/**
 *   Calendar 
 *   @param None
 *   @return None
 *   
 *  This is default constructor of Calendar class.
 * */

CCalendar::CCalendar():iColor(COLOUR_DARKBLUE),fReadOnly(false), fIsVisible(false), iCalId(-1),
		       iCalType(LOCAL_CALENDAR)
{
    this->iProfileType = SYNC_METHOD_NONE;
}



/**
 * modifyTodo 
 * @param ptrEntry pointer to calTodo 
 * @param int * pErrorCode error code if any
 * @return bool 
 * This function is called to modify the todo entry record
 * in the database.
 * */

bool CCalendar::modifyTodo(CTodo * pEntry, int &pErrorCode)
{

    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    int iCount = 0;
    int iSqliteError = 0;
    bool bRet = false;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if ((pCalDb == 0) || (pEntry == NULL)) {
	CAL_DEBUG_LOG("Calendar Invalid CalDb pointer");
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }

    /* modify component properties */


    this->addComponent(pEntry, E_MODIFY, pErrorCode);

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode1 is :%d,so returning  from here ",
		  pErrorCode);
	return false;
    }

    /* modify recurrence parameters */

    if (pEntry->getRecurrence()) {
	bRet = checkEntryExist(RECURSIVE_TABLE, pEntry->getId(), pErrorCode);
	if ((pErrorCode == CALENDAR_OPERATION_SUCCESSFUL) && (bRet)){
	    this->modifyRecurrence(pEntry->getRecurrence(),
		    pEntry->getId(), pErrorCode);
	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		CAL_DEBUG_LOG(" Errorcode2 is :%d,so returning  from here ",
                  pErrorCode);
		return false;
	    }
	}

	else {
	    this->addRecurrence(pEntry->getRecurrence(),
		    pEntry->getId(), pErrorCode);
	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		CAL_DEBUG_LOG(" Errorcode 3is :%d,so returning  from here ",
			  pErrorCode);
		return false;
	    }

	}
    }
    /* modify Alarm Parameters */

    CAlarm *pAlarm = 0;
    pAlarm = pEntry->getAlarm();

    bool bAlarmExists = checkEntryExist(ALARM_TABLE, pEntry->getId(), pErrorCode);
    int iAlarmCondition = 0;

    if(pAlarm){
	if((this->IsShown() && bAlarmExists) ||
	    (!this->IsShown() && bAlarmExists))
		iAlarmCondition = UPDATE_EXISTING_ALARM;
	else if((this->IsShown() && !bAlarmExists) ||
		(!this->IsShown() && !bAlarmExists))
		iAlarmCondition = ADD_NEW_ALARM;

	/* (Re)Create the alarm in the system using libalarm APIs */
	if (this->IsShown() && bAlarmExists)
	    this->manageCookie(E_MODIFY, pEntry, pErrorCode);
	else if (this->IsShown() && (bAlarmExists == false))
	     this->manageCookie(E_ADD, pEntry, pErrorCode);
	
	if (pErrorCode != CALENDAR_ALARM_ERROR &&
		pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
		return false;


	if(iAlarmCondition == UPDATE_EXISTING_ALARM)
		/* create the alarm (store it to DB) */
	    this->addAlarm(pAlarm, E_MODIFY, pEntry->getId(), pErrorCode);
	else
	    this->addAlarm(pAlarm, E_ADD, pEntry->getId(), pErrorCode);

	if (pErrorCode != CALENDAR_ALARM_ERROR &&
		pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
		return false;
    } else
	/* if pEntry doesn't have alarm details then de-register the existing alarms */
	deregisterAlarm(pEntry, pErrorCode);

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG (" Errorcode4 is :%d,so returning  from here ",
			    pErrorCode);
	    return false;
    }

    /* updating componentdetails table */


    pQuery =
    sqlite3_mprintf(UPDATE_COMPONENTDETAILS, COMP_FIELD_TYPE,
            COMP_FIELD_CLAS, COMP_FIELD_GEO, COMP_FIELD_PRIO,
            COMP_FIELD_SEQ, COMP_FIELD_TRANSP, COMP_FIELD_UID,
            COMP_FIELD_URL, COMP_FIELD_ATTACH,
            COMP_FIELD_CATEGORY, COMP_FIELD_COMMENT,
            COMP_FIELD_CONTACT, COMP_FIELD_RELATED,
            COMP_FIELD_RESOURCES, COMP_FIELD_PERCENT,
            COMP_FIELD_COMPLETED, COMP_FIELD_DTSTAMP, COMP_FIELD_ID);


    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);

    CAL_DEBUG_LOG("pQuery is : %s", pQuery);
    sqlite3_free(pQuery);
    pQuery = 0;

    if (pPreparedStmt == 0) {
	CAL_DEBUG_LOG("Prepared statement is null\n");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getType());

    sqlite3_bind_text(pPreparedStmt, ++iCount, (pEntry->getClas()).c_str(),
              (pEntry->getClas()).length(), SQLITE_TRANSIENT);

    //for todo geo is not present so we are binding with 0

    sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getPriority());
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getSequence());
    //Transparency is not present for todo so binding with null
    sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getUid());

    sqlite3_bind_text(pPreparedStmt, ++iCount, (pEntry->getUrl()).c_str(),
              (pEntry->getUrl()).length(), SQLITE_TRANSIENT);
    /* TODO can support multiple attachments */
    string szTemp;
    if (pEntry->getAttachments().size())
    szTemp = pEntry->getAttachments()[0];
    sqlite3_bind_text(pPreparedStmt, ++iCount, szTemp.c_str(),
              (szTemp.length()), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getCategories()).c_str(),
              (pEntry->getCategories()).length(),
              SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getComments()).c_str(),
              (pEntry->getComments()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getContact()).c_str(),
              (pEntry->getContact()).length(), SQLITE_TRANSIENT);


    sqlite3_bind_text(pPreparedStmt, ++iCount,
              pEntry->getRelated().c_str(),
              (pEntry->getRelated()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getResources()).c_str(),
              (pEntry->getResources()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, ++iCount,
             pEntry->getPercentComplete());
    if (pEntry->getStatus() == COMPLETED_STATUS) {
    /* Completed is the time at which Task
     * is marked as completed while adding itself the
     * task is completed then it is same as adding time
     */
	pEntry->setCompleted(time_get_time());

    }

    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getCompleted());
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getDateStamp());
    sqlite3_bind_int(pPreparedStmt, ++iCount, atoi(pEntry->getId().c_str()));
    

    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyTodo:modifyTodo failed to update DB");
	/* update Attendees and Organizer */
	if ((pEntry->getAttendees()).size() != 0) {
			updateAttendeesForComponent (pEntry, pErrorCode);
			CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyTodo:modifyTodo failed to update Attendees");
	}

	if (pEntry->getOrganizer()) {
			updateOrganizerForComponent (pEntry, pErrorCode);
			CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyTodo:modifyTodo failed to upadte Organizer");
	}

	/* update xproperties and parameters */
	if ((pEntry->getXProperties()).size() != 0) {
			updateXPropertiesForComponent (pEntry, pErrorCode);
			CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyTodo:modifyTodo failed to update XProperties");
	}

	if ((pEntry->getHashMap()).size() != 0) {
			updateParametersForComponent (pEntry, pErrorCode);
			CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyTodo:modifyTodo faield to update Parameters");
	}


    return true;


}


/**
 * modifyJournal 
 * @param ptrEntry pointer to CJournal
 * @param int * pErrorCode error code if any
 * @return NONE
 * * This function is called to modify the journal entry record
 * in the database.
 **/

bool CCalendar::modifyJournal(CJournal * pEntry, int &pErrorCode)
{


    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    int iCount = 0;
    time_t stTime = 0;
	int iSqliteError = 0;

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if ((pCalDb == 0) || (pEntry == NULL)) {

	CAL_DEBUG_LOG("invalid caldb pointer ");
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }
    /* modifying component properties */

    this->addComponent(pEntry, E_MODIFY, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
		  pErrorCode);
	return false;
    }
    /* modify Component Details */

    pQuery =
    sqlite3_mprintf(UPDATE_COMPONENTDETAILS, COMP_FIELD_TYPE,
            COMP_FIELD_CLAS, COMP_FIELD_GEO, COMP_FIELD_PRIO,
            COMP_FIELD_SEQ, COMP_FIELD_TRANSP, COMP_FIELD_UID,
            COMP_FIELD_URL, COMP_FIELD_ATTACH,
            COMP_FIELD_CATEGORY, COMP_FIELD_COMMENT,
            COMP_FIELD_CONTACT, COMP_FIELD_RELATED,
            COMP_FIELD_RESOURCES, COMP_FIELD_PERCENT,
            COMP_FIELD_COMPLETED, COMP_FIELD_DTSTAMP, COMP_FIELD_ID);


    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);

    CAL_DEBUG_LOG("query is : %s", pQuery);
    sqlite3_free(pQuery);
    pQuery = 0;

    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getType());

    sqlite3_bind_text(pPreparedStmt, ++iCount, (pEntry->getClas()).c_str(),
              (pEntry->getClas()).length(), SQLITE_TRANSIENT);
    //geo is not present for journal so bind with null
    sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    stTime = time_get_time();
    pEntry->setLastModified(stTime);
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getLastModified());
    //priority not present so bind with -1
    sqlite3_bind_int(pPreparedStmt, ++iCount, -1);
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getSequence());
    //Transparency is not present so bind with null
    sqlite3_bind_text(pPreparedStmt, ++iCount, 0, 0, SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getUid());


    sqlite3_bind_text(pPreparedStmt, ++iCount, (pEntry->getUrl()).c_str(),
              (pEntry->getUrl()).length(), SQLITE_TRANSIENT);
    string szTemp;
    if (pEntry->getAttachments().size())
	szTemp = pEntry->getAttachments()[0];
    sqlite3_bind_text(pPreparedStmt, ++iCount, szTemp.c_str(),
              (szTemp.length()), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getCategories()).c_str(),
              (pEntry->getCategories()).length(),
              SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getComments()).c_str(),
              (pEntry->getComments()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getContact()).c_str(),
              (pEntry->getContact()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getRelated()).c_str(),
              (pEntry->getRelated()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getResources()).c_str(),
              (pEntry->getResources()).length(), SQLITE_TRANSIENT);


    //we donot have percent and completed for Journal so we keep it -1 
    sqlite3_bind_int(pPreparedStmt, ++iCount, -1);
    sqlite3_bind_int(pPreparedStmt, ++iCount, -1);
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getDateStamp());
    sqlite3_bind_int(pPreparedStmt, ++iCount, atoi(pEntry->getId().c_str()));
            
    
    

    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyJournal:modifyJournal faield to update in DB");
	/* updating attendee information */
    if ((pEntry->getAttendees()).size() != 0) {

	ACTION iAction = E_ADD;
	pQuery = 0;
	pQuery =
	    sqlite3_mprintf(DELETE_TABLE_AND, PARAMS_TABLE, PARAM_FIELD_ID,
		    PARAM_FIELD_PROPNAME);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
		&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, DB_COLUMN_ID2,atoi(pEntry->getId().c_str()));
	sqlite3_bind_text(pPreparedStmt, DB_COLUMN_ID3, ATTDEE, ATTDEE_LEN, 
			SQLITE_TRANSIENT);
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyJournal:modifyJournal failed to upadate Attendee");
	this->addAttendeeDetails(pEntry->getAttendees(), pEntry->getId(),
		     E_JOURNAL, iAction, pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	    CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
		  pErrorCode);
	    return false;
	}

    }
    /* updating organizer details */


    if (pEntry->getOrganizer()) {


	pQuery = 0;
	pQuery =
	    sqlite3_mprintf(DELETE_TABLE_AND, PARAMS_TABLE, PARAM_FIELD_ID,
		    PARAM_FIELD_PROPNAME);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
		&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, DB_COLUMN_ID2, atoi(pEntry->getId().c_str()));
	sqlite3_bind_text(pPreparedStmt, DB_COLUMN_ID3, ORGZER, ORGZER_LEN, 
			    SQLITE_TRANSIENT);
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyJournal:modifyJournal faield to update PARAMS_TABLE in DB");
	this->addOrganizer(pEntry->getOrganizer(), pEntry->getId(),
		   E_JOURNAL, E_ADD, pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyJournal:modifyJournal faield to add Organizer");
    }
    /* modifying xprop */
    if ((pEntry->getXProperties()).size() != 0) {
	ACTION iAction = E_ADD;
	pQuery = 0;
	pQuery =
	    sqlite3_mprintf(DELETE_TABLE, XPROP_TABLE, XPROP_FIELD_ID);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
		&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, 1, atoi(pEntry->getId().c_str()));
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyJournal:modifyJournal failed to update XPROP_TABLE in DB"); 
	this->addXProperties(pEntry->getXProperties(), pEntry->getId(),
		     iAction, pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyJournal:modifyJournal failed to add XPROP_TABLE in DB"); 
    }
    /* modifying parameters */
    if ((pEntry->getHashMap()).size() != 0) {
	ACTION iAction = E_ADD;
	/*      if (checkEntryExist(PARAMS_TABLE, pEntry->getId(),pErrorCode)) */
	pQuery = 0;
	pQuery =
	    sqlite3_mprintf(DELETE_TABLE, PARAMS_TABLE, PARAM_FIELD_ID);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
		&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, 1, atoi(pEntry->getId().c_str()));
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyJournal:modifyJournal failed to upadte PARAMS_TABLE in DB");
	this->addParameters(pEntry->getHashMap(), pEntry->getId(), iAction,
		    pErrorCode, false);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyJournal:modifyJournal failed to add PARAMS_TABLE in DB");
    }

    /* If control reaches here it means 
     * everything is succesful :-)
     */

    return true;
}

/**
 * modifyEvent 
 * @param ptrEntry pointer to calEvent
 * @param int * pErrorCode error code if any
 * @return bool 
 *
 * This function is called to modify the event entry record
 * in the database.
 * */

bool CCalendar::modifyEvent(CEvent * pEntry, int &pErrorCode)
{
    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    int iCount = 0;
    int iSqliteError = 0;
    bool bRet = 0;

    CCalendarDB *pCalDb = CCalendarDB::Instance();

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if ((pCalDb == 0) || (pEntry == NULL)) {
	CAL_DEBUG_LOG("Calendar :: invalid caldb pointer ");
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }
    /* modifying component table contents */

    this->addComponent(pEntry, E_MODIFY, pErrorCode);

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
		  pErrorCode);
	return false;
    }

    /*It is possible that recurrence can be added  for first time here 
     * so we need to check initially whether recurrence exists already or not.
     */

    if (pEntry->getRecurrence()) {
    
	bRet = checkEntryExist(RECURSIVE_TABLE, pEntry->getId(), pErrorCode); 
	if ((pErrorCode == CALENDAR_OPERATION_SUCCESSFUL) && (bRet)){
	    /* Component exists so adding the recurrence infomration */
	    this->modifyRecurrence(pEntry->getRecurrence(),
		    pEntry->getId(), pErrorCode);
	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
			  pErrorCode);
		return false;
	    }
	} 
	else {

	    this->addRecurrence(pEntry->getRecurrence(),
		    pEntry->getId(), pErrorCode);
		if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		    CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
			      pErrorCode);
		    return false;
		}

	}
    }
    /*modify  Alarm component */

    CAlarm *pAlarm = 0;
    pAlarm = pEntry->getAlarm();
    if (pAlarm) {
	bRet = checkEntryExist(ALARM_TABLE, pEntry->getId(), pErrorCode);
	if ((pErrorCode == CALENDAR_OPERATION_SUCCESSFUL) && (bRet)){
	    if (!this->IsShown()) {
		/* Donot register alarm for an invisible calendar */

		CAL_DEBUG_LOG
		    ("Calendar is invisible  so not registernig the Alarm event ");
		this->addAlarm(pAlarm, E_MODIFY, pEntry->getId(),
			   pErrorCode);
		if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		    CAL_DEBUG_LOG
		    (" Errorcode is :%d,so returning  from here ",
		     pErrorCode);
		    return false;
		}
	    } 
	    else {

        if (this->manageCookie(E_MODIFY, pEntry, pErrorCode)) {
            this->addAlarm(pAlarm, E_MODIFY, pEntry->getId(),
                pErrorCode);
            if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
            CAL_DEBUG_LOG
                (" Errorcode is :%d,so returning  from here ",
                    pErrorCode);
            return false;
            }
		} 
		else if (pErrorCode == CALENDAR_ALARM_ERROR) {
            pEntry->removeAlarm();
            this->addComponent(pEntry, E_MODIFY, pErrorCode);

            CAL_DEBUG_LOG
                ("Alarm error so ignoring add alarm request and \
                  adding it as normal event \n");
            pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

		} 
		else
		return false;
	    }
	} 
	else {
	    if (!this->IsShown()) {
		CAL_DEBUG_LOG
		    ("Calendar is invisible  so not registernig the Alarm event ");
		this->addAlarm(pAlarm, E_ADD, pEntry->getId(), pErrorCode);
		if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		    CAL_DEBUG_LOG
		    (" Errorcode is :%d,so returning  from here ",
		     pErrorCode);
		    return false;
		}
	    } 
	    else {

		if (this->manageCookie(E_ADD, pEntry, pErrorCode)) {
            this->addAlarm(pAlarm, E_ADD, pEntry->getId(),
                pErrorCode);
            if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
            CAL_DEBUG_LOG
                (" Errorcode is :%d,so returning  from here ",
                    pErrorCode);
            return false;
            }
		} 
		else if (pErrorCode == CALENDAR_ALARM_ERROR) {
            pEntry->removeAlarm();
            this->addComponent(pEntry, E_MODIFY, pErrorCode);

            CAL_DEBUG_LOG
                ("Alarm error so ignoring add alarm request and \
                  adding it as normal event \n");
            pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
		} 
		else
		return false;
	    }
	}
    }

    else {
	vector <long> oldcookie;
	vector <long>::iterator citer;
	/* deleting the pre-existing alarm */

	oldcookie = this->getCookie(pEntry->getId(), pErrorCode);
	CAlarm alarmUtility;
	if (!oldcookie.empty()){

	    alarmUtility.deleteAlarmEvent(oldcookie[0],pErrorCode);
	    pQuery =
		    sqlite3_mprintf(DELETE_TABLE, ALARM_TABLE, ALARM_FIELD_ID);
	    ASSERTION(pQuery);
	    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			    &pPreparedStmt, &pTail);
	    sqlite3_bind_int(pPreparedStmt, 1, atoi(pEntry->getId().c_str()));
	    CAL_DEBUG_LOG("%s", pQuery);
	    sqlite3_free(pQuery);
	    pQuery = 0;
	    pCalDb->updateDB(pPreparedStmt,iSqliteError);
	    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
		return false;
       }
    }

    /* Updating the component details table */

    pQuery =
    sqlite3_mprintf(UPDATE_COMPONENTDETAILS, COMP_FIELD_TYPE,
            COMP_FIELD_CLAS, COMP_FIELD_GEO, COMP_FIELD_PRIO,
            COMP_FIELD_SEQ, COMP_FIELD_TRANSP, COMP_FIELD_UID,
            COMP_FIELD_URL, COMP_FIELD_ATTACH,
            COMP_FIELD_CATEGORY, COMP_FIELD_COMMENT,
            COMP_FIELD_CONTACT, COMP_FIELD_RELATED,
            COMP_FIELD_RESOURCES, COMP_FIELD_PERCENT,
            COMP_FIELD_COMPLETED, COMP_FIELD_DTSTAMP, COMP_FIELD_ID);

    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);

    CAL_DEBUG_LOG("query is : %s", pQuery);
    sqlite3_free(pQuery);
    pQuery = 0;

    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getType());
    CAL_DEBUG_LOG("iCount: %d", iCount);

    sqlite3_bind_text(pPreparedStmt, ++iCount, (pEntry->getClas()).c_str(),
              (pEntry->getClas()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount, (pEntry->getGeo()).c_str(),
              (pEntry->getGeo()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getPriority());
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getSequence());

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getTransparency()).c_str(),
              (pEntry->getTransparency()).length(),
              SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getUid());


    sqlite3_bind_text(pPreparedStmt, ++iCount, (pEntry->getUrl()).c_str(),
              (pEntry->getUrl()).length(), SQLITE_TRANSIENT);

    string szTemp;
    if (pEntry->getAttachments().size())
	szTemp = pEntry->getAttachments()[0];
    sqlite3_bind_text(pPreparedStmt, ++iCount, szTemp.c_str(),
              (szTemp.length()), SQLITE_TRANSIENT);


    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getCategories()).c_str(),
              (pEntry->getCategories()).length(),
              SQLITE_TRANSIENT);


    sqlite3_bind_text(pPreparedStmt, ++iCount,
              pEntry->getComments().c_str(),
              (pEntry->getComments()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              pEntry->getContact().c_str(),
              (pEntry->getContact()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              pEntry->getRelated().c_str(),
              (pEntry->getRelated()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_text(pPreparedStmt, ++iCount,
              (pEntry->getResources()).c_str(),
              (pEntry->getResources()).length(), SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, ++iCount, -1);
    sqlite3_bind_int(pPreparedStmt, ++iCount, -1);
    sqlite3_bind_int(pPreparedStmt, ++iCount, pEntry->getDateStamp());
    sqlite3_bind_int(pPreparedStmt, ++iCount, atoi(pEntry->getId().c_str()));

    CAL_DEBUG_LOG("%p", pPreparedStmt);
    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to update DB");
    /* update attendee details */


    if ((pEntry->getAttendees()).size() != 0) {

	/* deleting attendees from params table */

	pQuery = 0;
	pQuery =
		sqlite3_mprintf(DELETE_TABLE_AND, PARAMS_TABLE, PARAM_FIELD_ID,
				PARAM_FIELD_PROPNAME);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, DB_COLUMN_ID2, atoi(pEntry->getId().c_str()));
	sqlite3_bind_text(pPreparedStmt, DB_COLUMN_ID3, ATTDEE, ATTDEE_LEN, 
			    SQLITE_TRANSIENT);
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to delete Attendee from PARAM_TABLE");
	/* adding attendee details */

	this->addAttendeeDetails(pEntry->getAttendees(), pEntry->getId(),
		    E_EVENT, E_ADD, pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to add Attendee from PARAM_TABLE");
    }

    /* attendee check */
    /* modifying organizer details */
    if (pEntry->getOrganizer()) {

	pQuery = 0;
	pQuery =
	    sqlite3_mprintf(DELETE_TABLE_AND, PARAMS_TABLE, PARAM_FIELD_ID,
		    PARAM_FIELD_PROPNAME);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
		&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, DB_COLUMN_ID2, atoi(pEntry->getId().c_str()));
	sqlite3_bind_text(pPreparedStmt, DB_COLUMN_ID3, ORGZER, ORGZER_LEN, 
			SQLITE_TRANSIENT);
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to update organizer details in DB");
	this->addOrganizer(pEntry->getOrganizer(), pEntry->getId(),
		   E_EVENT, E_ADD, pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to add organizer details in DB");
    }

    /* modifying xprop */
    if ((pEntry->getXProperties()).size() != 0) {
	ACTION iAction = E_ADD;
	pQuery = 0;
	pQuery =
	    sqlite3_mprintf(DELETE_TABLE, XPROP_TABLE, XPROP_FIELD_ID);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
		&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, 1, atoi(pEntry->getId().c_str()));
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to update XPROP TABLE in DB");
	this->addXProperties(pEntry->getXProperties(), pEntry->getId(),
		     iAction, pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to add XPROP in DB");

    }


    /* modifying parameters */
    if ((pEntry->getHashMap()).size() != 0) {
	ACTION iAction = E_ADD;
	pQuery = 0;
	pQuery =
	    sqlite3_mprintf(DELETE_TABLE_PROP, PARAMS_TABLE,
		    PARAM_FIELD_ID, PARAM_FIELD_PROPNAME,
		    PARAM_FIELD_PROPNAME);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
		&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, 1, atoi(pEntry->getId().c_str()));
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to update parameters ");
	this->addParameters(pEntry->getHashMap(), pEntry->getId(), iAction,
		    pErrorCode, false);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyEvent:modifyEvent failed to add parameters");
    }

    return true;

}
/**
 * ~Calendar 
 *  @param None 
 *  @return None 
 * 
 *  This is destructor for the Calendar class.
 * */


CCalendar::~CCalendar()
{

}

/** getIdList
 * @param Component type
 * @param int * pErrorCode error code if any
 * @return list of Id of existing event Todo and Journal
 * 
 * This function is used to get the elaborate details of particular entry
 */
vector <string> CCalendar::getIdList(int iCompType, int &pErrorCode)
{



    QueryResult *pQr = 0;
    int iI_IdCount = 0;
    int iJ_IdCount = 0;
    int iK_IdCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    vector <string> listId;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return listId;
	}
    pQuery =
    sqlite3_mprintf(SELECT_DOMAIN_AND, ENTRY_FIELD_ID,
            COMPONENTS_TABLE, ENTRY_FIELD_TYPE,
            iCompType, ENTRY_FIELD_CALID,
            this->getCalendarId());
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CAL_DEBUG_LOG("query is given by  %s", pQuery);
    sqlite3_free(pQuery);
    /* error code will be set int the mapper function */
    if (pQr == 0)
	return listId;


    for (iI_IdCount = 0; iI_IdCount < pQr->iRow; iI_IdCount++) {
	for (iJ_IdCount = 0; iJ_IdCount < pQr->iColumn; iJ_IdCount++) {
	    iK_IdCount = pQr->iColumn + (iI_IdCount * pQr->iColumn);
	    listId.push_back(pQr->pResult[iK_IdCount + iJ_IdCount]);
	}
    }
    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    return (listId);
}

/** 
 * getEvent
 * @param Component ID
 * @param int * pErrorCode error code if any
 * @return CEvent* Pointer to CEvent class
 * 
 * This function is used to get the elaborate details of particular entry
 */
CEvent* CCalendar::getBirthDayEvent(string sCompId, int &pErrorCode)
{
    CEvent *event = 0;
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return 0;
    }

    /*this function is responsible to retrieve the values stored in 
     * Component table and uses the set function to add values in 
     * to event object*/

    pQuery = sqlite3_mprintf( SELECT_TAB_COND_AND ,COMPONENTS_TABLE, 
		              COMP_FIELD_TYPE,E_BDAY, COMP_FIELD_ID,
			      atoi(sCompId.c_str()));        
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    /* error code will be set in the mapper function */
    if (pQr == 0) {
        return 0;
    }

   if (pQr->iRow > TWO_ROWS) {
        CAL_DEBUG_LOG("Database corruptioni, two rows found with same entry id.\n");
        pErrorCode = CALENDAR_FUNC_ERROR;
        sqlite3_free_table(pQr->pResult);
        delete pQr;
        pQr = 0;
        return 0;
    }

//    CAL_DEBUG_LOG("column number : %d", pQr->iColumn);
//    CAL_DEBUG_LOG("Inside event Details ");

    event = CCalendarProcs::createComponentFromTableRecord((const char**)(pQr->pResult + pQr->iColumn), pQr->iColumn);

    sqlite3_free_table(pQr->pResult);

    delete pQr;
    pQr = 0;
    return event;
}


/** 
 * getEvent
 * @param Component ID
 * @param int * pErrorCode error code if any
 * @return CEvent* Pointer to CEvent class
 * 
 * This function is used to get the elaborate details of particular entry
 */
CEvent *CCalendar::getEvent(string sCompId, int &pErrorCode)
{
    CEvent *event = 0;
    CAlarm *pAlarm = 0;
    CRecurrence *pRec = 0;
    QueryResult *pQr = 0;
    int iI_EventCount = 0;
    int iJ_EventCount = 0;
    int iK_EventCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CUtility *pUt = 0;
    pUt = CUtility::Instance();
    vector <long> vCookie;
    vector <string> vERule;
    vector <string> vEdate;
    vector <string> vRdate;
    vector <string> vRRule;


    if ((pDb == 0)) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return 0;
    }

    /*this function is responsible to retrieve the values stored in 
     * Component table and uses the set function to add values in 
     * to event object*/
    event = new CEvent();
    pAlarm = new CAlarm();
    pRec = new CRecurrence();
    if (!event || ! pAlarm || !pRec) {
        pErrorCode = CALENDAR_MEMORY_ERROR;
        CAL_DEBUG_LOG("Memory allocation  error");
        if (event)
            delete event;
        if (pAlarm)
            delete pAlarm;
        if (pRec)
            delete pRec;
        return 0;
    }
    event->setId(sCompId);
    pQuery = sqlite3_mprintf(SELECT_LEFT_JOIN, sCompId.c_str());
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    /* error code will be set in the mapper function */
    if (pQr == 0) {
        if (event)
            delete event;
        if (pAlarm)
            delete pAlarm;
        if (pRec)
            delete pRec;
        return 0;
    }

    if (pQr->iRow > TWO_ROWS) {
        CAL_DEBUG_LOG
            ("Database corruption, two rows found with same entry id.\n");
        pErrorCode = CALENDAR_FUNC_ERROR;
        sqlite3_free_table(pQr->pResult);
        delete pQr;
        pQr = 0;
        if (event)
            delete event;
        if (pAlarm)
            delete pAlarm;
        if (pRec)
            delete pRec;
        return 0;
    }

    CAL_DEBUG_LOG("column number : %d", pQr->iColumn);
    CAL_DEBUG_LOG("INside event Details ");
    iI_EventCount = 0;
    for (iJ_EventCount = 0; iJ_EventCount < pQr->iColumn; iJ_EventCount++) {
	iK_EventCount = pQr->iColumn + (iI_EventCount * pQr->iColumn);
	if (pQr->pResult[iK_EventCount + iJ_EventCount] == 0)
		continue;
	switch (iJ_EventCount) {
	    case DB_COLUMN_ID1:
		event->setId(pQr->pResult[iK_EventCount + iJ_EventCount]);
		break;
	    case DB_COLUMN_ID2:
		//event->setCalendarId(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID3:
		event->
		    setType(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID4:
		event-> setFlags(atoi (pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID5:
		event-> setDateStart(atoi (pQr->
			     pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID6:
		event-> setDateEnd(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID7:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setSummary(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID8:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setLocation(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID9:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setDescription(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID10:
		event->setStatus(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID11:

		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setGUid(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID12:
		event->
		    setUntil(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	   case DB_COLUMN_ID13:
		event->
		    setAllDay(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID14:
		event-> setCreatedTime(atoi (pQr->
			     pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID15:

		event->setLastModified(atoi (pQr->
			     pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID16:
		event->setTzid(pQr->pResult[iK_EventCount + iJ_EventCount]);
		break;
		/* getting tz offset is not required for UI */
	    case DB_COLUMN_ID17:
	    case DB_COLUMN_ID18:
	    case DB_COLUMN_ID19:
		break;

	    case DB_COLUMN_ID20:

		if (pQr->pResult[iK_EventCount + iJ_EventCount] != 0) {
		    event->setClas(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;


	    case DB_COLUMN_ID21:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setGeo(pQr->pResult[iK_EventCount + iJ_EventCount]);
		}
		break;


	    case DB_COLUMN_ID22:
		event->setPriority(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID23:
		event-> setDateStamp(atoi
			    (pQr-> pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID24:
		event-> setSequence(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID25:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setTransparency(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID26:
		event->
		    setUid(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
		break;


	    case DB_COLUMN_ID27:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setUrl((pQr->pResult[iK_EventCount + iJ_EventCount]));
		}
		break;

	    case DB_COLUMN_ID28:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    vector <string> vAtt;
		    vAtt.push_back((pQr->pResult[iK_EventCount + iJ_EventCount]));
		    event->setAttachments(vAtt);
		}
		break;

	    case DB_COLUMN_ID29:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setCategories(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID30:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setComments(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID31:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setContact(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID32:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setRelated(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID33:
		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    event->setResources(pQr->
			    pResult[iK_EventCount + iJ_EventCount]);
		}
		break;

	    case DB_COLUMN_ID34:
		break;

	    case DB_COLUMN_ID35:
		break;
	    case DB_COLUMN_ID36:
		break;
	    case DB_COLUMN_ID37:
		if (pQr->pResult[iK_EventCount + iJ_EventCount])
		    pAlarm->setTrigger(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID38:
		if (pQr->pResult[iK_EventCount + iJ_EventCount])
		    pAlarm->setRepeat(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID39:
        if (pQr->pResult[iK_EventCount + iJ_EventCount])
        {
            int raw_duration = atoi(pQr->pResult[iK_EventCount + iJ_EventCount]);
            CAL_DEBUG_LOG("raw_duration=%d", raw_duration);
            pAlarm->setDuration(raw_duration % 10);
            pAlarm->setTimeBefore(raw_duration / 10);
        }
		break;

	    case DB_COLUMN_ID40:
		if (pQr->pResult[iK_EventCount + iJ_EventCount])
		    pAlarm->setAction(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID41:

		if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		    vCookie.push_back(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    pAlarm->setCookie(vCookie);
		}
		break;

	    case DB_COLUMN_ID42:
		if (pQr->pResult[iK_EventCount + iJ_EventCount])
		    pAlarm->setAttach(pQr-> pResult[iK_EventCount + iJ_EventCount]);
		break;
	    case DB_COLUMN_ID43:
		break;
	    case DB_COLUMN_ID44:

		if(pQr->pResult[iK_EventCount + iJ_EventCount]){
		    vRRule = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount], EXCLAMATION);
		    pRec->setRrule(vRRule);
		}
		break;

	    case DB_COLUMN_ID45:

		if(pQr->pResult[iK_EventCount + iJ_EventCount]){
		    vRdate = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],SEMI_COLON);
		    pRec->setRDays(vRdate);
		}
		break;
	    case DB_COLUMN_ID46:

		if(pQr->pResult[iK_EventCount + iJ_EventCount]){
		    vEdate = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],SEMI_COLON);
		    pRec->setEDays(vEdate);
		}
		break;

	    case DB_COLUMN_ID47:

		if(pQr->pResult[iK_EventCount + iJ_EventCount]){
		    vERule = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],EXCLAMATION);;
		    pRec->setErule(vERule);
		}
		break;

	    case DB_COLUMN_ID48:
		if (pQr->pResult[iK_EventCount + iJ_EventCount])
		    pRec->setRecurId(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		break;

	    case DB_COLUMN_ID49:
		if (pQr->pResult[iK_EventCount + iJ_EventCount])
		    pRec->setRtype(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		break;


	    default:
		break;
	}
	


    }
    CAL_DEBUG_LOG("CCalendar %d:getEvent-> %s \n",
			iI_EventCount,event->toString().c_str());

    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    /* Flags has the following values 
     * HAS_RECURRENCE -- 3 
     * HAS_ALARM ---- 4
     * HAS_RECURRENCE_ALARM - 5 defined in common.h
     */
    if ((event->getFlags() == HAS_RECURRENCE) ||
	    (event->getFlags() == HAS_RECURRENCE_ALARM))
	event->setRecurrence (pRec);
    delete pRec;
    pRec = 0;


    if ((event->getFlags() == HAS_ALARM) ||
	    (event->getFlags() == HAS_RECURRENCE_ALARM))
	event->setAlarm(pAlarm);
    delete pAlarm;
    pAlarm =0 ;

    vector < CAttendee * >listAttendee;

    listAttendee = event->retrieveAttendeeDetails();
    COrganizer *pOrg = 0;

    pOrg = event->retrieveOrganizerDetails();
    if (listAttendee.size())
	event->setAttendees(listAttendee);
    pUt->releaseVector(listAttendee);
    if (pOrg){
        event->setOrganizer(pOrg);
        delete pOrg ;
        pOrg = 0;
    }

    /*retrieve xprop */
    vector < CProperties * >vPropList;
    vPropList = event->retrieveXPropertyDetails();
    event->setXProperties(vPropList);
    pUt->releasePropertiesVector(vPropList);
    /*retrieve params */
    map < string, vector < CParameters * > >paramMap;
    paramMap = event->retrieveParameterDetails();

    event->setHashMap(paramMap);
    paramMap.clear();
    

    return event;
}



/** 
 * getTodo
 * @param Component ID
 * @param int * pErrorCode error code if any
 * @return CTodo* Pointer to CTodo class
 * 
 * This function is used to get the elaborate details of particular entry
 */
CTodo *CCalendar::getTodo(string sCompId, int &pErrorCode)
{
    CTodo *todo = 0;
    CAlarm *pAlarm = 0;
    QueryResult *pQr = 0;
    int iI_TodoCount = 0;
    int iJ_TodoCount = 0;
    int iK_TodoCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    vector <long> vCookie;
    pDb = CCalendarDB::Instance();

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CUtility *pUt = 0;
    pUt = CUtility::Instance();



    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return 0;
    }


    /* this function is responsible to retrieve the values stored
     * in Component table and uses the set function to add values
     * in to todo object
     */
    todo = new CTodo();
    pAlarm = new CAlarm();
    ASSERTION(todo);
    todo->setId(sCompId);
    pQuery = sqlite3_mprintf(SELECT_LEFT_JOIN, sCompId.c_str());
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);


    if (pQr == 0) {
	if (todo)
	    delete todo;
	if (pAlarm)
	    delete pAlarm;
	return 0;
    }

    if (pQr->iRow > TWO_ROWS) {
	    CAL_DEBUG_LOG
		    ("Database corruption, two rows found with same entry id.\n");
	    pErrorCode = CALENDAR_FUNC_ERROR;
	    sqlite3_free_table(pQr->pResult);
	    delete pQr;
	    pQr = 0;
	    if(todo) {
	    delete todo;
	    todo = 0;
	    }
	    if (pAlarm) {
	    delete pAlarm;
	    pAlarm = 0;
	    }
	    return 0;
    }
    CAL_DEBUG_LOG("Inside get Todo");

    iI_TodoCount = 0;
    for (iJ_TodoCount = 0; iJ_TodoCount < pQr->iColumn; iJ_TodoCount++) {
	iK_TodoCount = pQr->iColumn + (iI_TodoCount * pQr->iColumn);
        if ((pQr->pResult[iK_TodoCount + iJ_TodoCount]) == 0)
	    continue;
	switch (iJ_TodoCount) {

	    case DB_COLUMN_ID1:
		todo->setId(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		break;
	    case DB_COLUMN_ID2:
		//todo->setCalendarId(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID3:
		todo->setType(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID4:
		todo->
		    setFlags(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID5:
		todo-> setDateStart(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID6:
		todo-> setDateEnd(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID7:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
		    todo->setSummary(pQr->
			    pResult[iK_TodoCount + iJ_TodoCount]);
		}
		break;

	    case DB_COLUMN_ID8:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
		    todo->setLocation(pQr->
			    pResult[iK_TodoCount + iJ_TodoCount]);
		}
		break;

	    case DB_COLUMN_ID9:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
		    todo->setDescription(pQr->
			    pResult[iK_TodoCount + iJ_TodoCount]);
		}
		break;

	    case DB_COLUMN_ID10:
		todo->
		    setStatus(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;
	    case DB_COLUMN_ID11:

		if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
		    todo->setGUid(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		}
		break;
	    case DB_COLUMN_ID12:
		todo->
		    setUntil(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID13:
		todo->
		    setAllDay(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;
	    case DB_COLUMN_ID14:
		todo-> setCreatedTime(atoi
			    (pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		break;
	    case DB_COLUMN_ID15:

		todo-> setLastModified(atoi
			    (pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID16:
		todo->setTzid(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		break;
		/* getting tz offset is not required for UI */
	    case DB_COLUMN_ID17:
		break;
	    case DB_COLUMN_ID18:
		todo->setId(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		break;

	    case DB_COLUMN_ID19:
		todo->setType(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID20:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount] != 0) {
		    todo->setClas(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		}
		break;


	    case DB_COLUMN_ID21:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
		    todo->setGeo(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		}
		break;


	    case DB_COLUMN_ID22:
		todo-> setPriority(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID23:
		todo-> setDateStamp(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID24:
		todo-> setSequence(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID25:
		break;

	    case DB_COLUMN_ID26:
		todo->setUid(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID27:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
		    todo->setUrl((pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		}
		break;

	    case DB_COLUMN_ID28:
		//  todo->setAttachments (pQr->pResult[iK+iJ]);
		break;

	    case DB_COLUMN_ID29:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    todo->setCategories(pQr->
			    pResult[iK_TodoCount + iJ_TodoCount]);
		break;

	    case DB_COLUMN_ID30:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    todo->setComments(pQr->
			    pResult[iK_TodoCount + iJ_TodoCount]);
		break;

	    case DB_COLUMN_ID31:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    todo->setContact(pQr-> pResult[iK_TodoCount + iJ_TodoCount]);
		break;

	    case DB_COLUMN_ID32:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    todo->setRelated(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		break;

	    case DB_COLUMN_ID33:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    todo->setResources(pQr-> pResult[iK_TodoCount + iJ_TodoCount]);
		break;

	    case DB_COLUMN_ID34:
		todo-> setPercentComplete(atoi
			(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID35:
		todo->setCompleted(
			atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		break;
	    case DB_COLUMN_ID37:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    pAlarm->setTrigger(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID38:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    pAlarm->setRepeat(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID39:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    pAlarm->setDuration(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID40:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    pAlarm->setAction(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		break;

	    case DB_COLUMN_ID41:

		if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
		    vCookie.push_back(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		    pAlarm->setCookie(vCookie);
		}
		break;

	    case DB_COLUMN_ID42:
		if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
		    pAlarm->setAttach(pQr-> pResult[iK_TodoCount + iJ_TodoCount]);
		break;
	    default:
		break;
	}
    }
    CAL_DEBUG_LOG("CCalendar %d:getTodo-> %s \n", iI_TodoCount,todo->toString().c_str());
    sqlite3_free_table(pQr->pResult);
    if (todo->getFlags() == HAS_ALARM||todo->getFlags() == HAS_RECURRENCE_ALARM) 
	todo->setAlarm(pAlarm);

    //pAlarm should be always deleted because todo->setAlarm creates copy of alarm
    delete pAlarm;
    pAlarm =0 ;

    /*retrieve xprop */
    vector < CProperties * >vPropList;

    vPropList = todo->retrieveXPropertyDetails();
    todo->setXProperties(vPropList);
    pUt->releasePropertiesVector(vPropList);
    /*retrieve params */
    map < string, vector < CParameters * > >paramMap;
    paramMap = todo->retrieveParameterDetails();
    todo->setHashMap(paramMap);
    paramMap.clear();

    delete pQr;
    pQr = 0;
    return todo;
}


/** getJournal
 * @param Component ID
 * @param int * pErrorCode error code if any
 * @return CJournal* Pointer to CJournal class
 * 
 * This function is used to get the elaborate details of particular entry
 */
CJournal *CCalendar::getJournal(string sCompId, int &pErrorCode)
{
    CJournal *pJournal = 0;
    QueryResult *pQr = 0;
    int iI_JourCount = 0;
    int iJ_JourCount = 0;
    int iK_JourCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CUtility *pUt = 0;
    pUt = CUtility::Instance();


    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return 0;
    }


    /*this function is responsible to retrieve the values stored in 
     * Component table and uses the set function to add values in 
     * to Journal object*/
    pJournal = new CJournal();
    ASSERTION(pJournal);
    pJournal->setId(sCompId);
    pQuery = sqlite3_mprintf(SELECT_INNER_JOIN, sCompId.c_str());
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    CAL_DEBUG_LOG(" query in getJournal is : %s", pQuery);


    if (pQr == 0) {
	if (pJournal)
	    delete pJournal;
	return 0;
    }


    if (pQr->iRow > TWO_ROWS) {
	CAL_DEBUG_LOG
		("Database corruption, two rows found with same entry id.\n");
	pErrorCode = CALENDAR_FUNC_ERROR;
	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
	if (pJournal){
	    delete pJournal;
	    pJournal = 0;
	}
	return 0;
    }

    iI_JourCount = 0;
    for (iJ_JourCount = 0; iJ_JourCount < pQr->iColumn; iJ_JourCount++) {
	iK_JourCount = pQr->iColumn + (iI_JourCount * pQr->iColumn);
	switch (iJ_JourCount) {

	    case DB_COLUMN_ID1:
		pJournal->setId(pQr->pResult[iK_JourCount + iJ_JourCount]);
		break;
	    case DB_COLUMN_ID2:
		//pJournal->setCalendarId(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID3:
		pJournal->
		    setType(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID4:
		pJournal->
		    setFlags(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID5:
		pJournal-> setDateStart(atoi
			    (pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID6:
		pJournal-> setDateEnd(atoi
			    (pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID7:
		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->setSummary(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		}
		break;

	    case DB_COLUMN_ID8:
		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->setLocation(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		}
		break;

	    case DB_COLUMN_ID9:
		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->setDescription(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		}
		break;

	    case DB_COLUMN_ID10:
		pJournal->
		    setStatus(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID11:

		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->setGUid(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		}
		break;
	    case DB_COLUMN_ID12:
		pJournal-> setUntil(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID13:
		pJournal->
		    setAllDay(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;
	    case DB_COLUMN_ID14:
		pJournal-> setCreatedTime(atoi (pQr->
			     pResult[iK_JourCount + iJ_JourCount]));
		break;
	    case DB_COLUMN_ID15:

		pJournal-> setLastModified(atoi (pQr->
			     pResult[iK_JourCount + iJ_JourCount]));
		break;
	    
	    case DB_COLUMN_ID16:
		pJournal->setTzid(pQr->pResult[iK_JourCount + iJ_JourCount]);
		break;
		/* getting tz offset is not required for UI */
	    case DB_COLUMN_ID17:
		break;
	    case DB_COLUMN_ID18:
		pJournal->setId(pQr->pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID19:
		pJournal->
		    setType(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID20:
		if (pQr->pResult[iK_JourCount + iJ_JourCount] != 0) {
		    pJournal->setClas(pQr-> pResult[iK_JourCount + iJ_JourCount]);
		}
		break;


	    case DB_COLUMN_ID21:
		break;


	    case DB_COLUMN_ID22:
		break;

	    case DB_COLUMN_ID23:
		pJournal->
		    setDateStamp(atoi
			    (pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID24:
		pJournal-> setSequence(atoi
			    (pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID25:
		break;

	    case DB_COLUMN_ID26:
		pJournal->
		    setUid(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID27:
		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->
			setUrl((pQr->pResult[iK_JourCount + iJ_JourCount]));
		}
		break;

	    case DB_COLUMN_ID28:
		//       journal->setAttachments (pQr->pResult[iK+iJ]);
		break;

	    case DB_COLUMN_ID29:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setCategories(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID30:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setComments(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID31:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setContact(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID32:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setRelated(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID33:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setResources(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID34:
		break;

	    case DB_COLUMN_ID35:
		break;

	    default:
		break;
	}
	


    }
    CAL_DEBUG_LOG("CCalendar %d::getJournal-> %s \n",
			iI_JourCount,pJournal->toString().c_str());

    /*retrieve xprop */
    vector < CProperties * >vPropList;
    vPropList = pJournal->retrieveXPropertyDetails();
    pJournal->setXProperties(vPropList);
    pUt->releasePropertiesVector(vPropList);

    /*retrieve params */
    map < string, vector < CParameters * > >paramMap;
    paramMap = pJournal->retrieveParameterDetails();
    CAL_DEBUG_LOG("HASH MAP SIZE is %d\n", paramMap.size());
    pJournal->setHashMap(paramMap);
    paramMap.clear();

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    CAL_DEBUG_LOG("returning pjournal\n");
    return pJournal;
}


/**
 * setCalendarType
 * @param id: Calendar Type to be set
 * @return bool (SUCCESS/FAILURE)
 * This function is called to set the type of calendar
 * 0 - Local calendar
 * 1 - Sync calendar
 * 2 - Smart calendar
 */
bool CCalendar::setCalendarType(CalendarType iCalType)
{
    if(this->iCalType != DEFAULT_SYNC)
        this->iCalType = iCalType;
    return SUCCESS;
}

/**
 * getCalendarType
 * @param None
 * @return: returns the calendar type
 * 
 * This function is called to get the calendar type
 */
CalendarType CCalendar::getCalendarType()
{
    return this->iCalType;
}

/**
 * getCalendarVersion
 * @param none
 * @return string calendar version number
 * Function to retrieve Calendar version
 */
string CCalendar::getCalendarVersion()
{
    return this->szCalendarVersion;
}

/**
 * setCalendarVersion
 * @param string szCaeldnarVersion
 * @return none 
 * Function to set Caelndar version
 */
void CCalendar::setCalendarVersion(string szCalendarVersion)
{

    this->szCalendarVersion = szCalendarVersion;
}



/**
 * setCalendarTune
 * @param string szCaeldnarTune
 * @return none 
 * Function to set Caelndar tune
 */
void CCalendar::setCalendarTune(string szCalendarTune)
{
    this->szCalendarTune = szCalendarTune;

}

/**
 * getCalendarTune
 * @param NONE
 * @return string CaelndarTune 
 *
 * Function to retrieve calendar tune
 */
string CCalendar::getCalendarTune()
{
    return (this->szCalendarTune);
}


/**
 * addBirthDay
 * @param pBday - pointer to CBdayEvent object 
 * @param int * pErrorCode error code if any
 * 
 * @return boolean SUCCESS/FAILURE 
 *
 * This function is called to add a new birthday data into the database.
 **/

bool CCalendar::addBirthDay(CBdayEvent * pBday, int &pErrorCode)
{

    int istatus = -1;
    string externalId;
    /* external Id coming from contacts needs  to 
     * be stored  here */

    if (!pBday) {
	CAL_DEBUG_LOG("CCalendar pointer is 0");
	return FAILURE;
    }
    
    externalId = pBday->getId();

    CCalendarDB *pCalDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid caldb pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return FAILURE;
    }

    /* check for duplicate first */
    string localId = getExternalToLocalId(externalId, true, pErrorCode);
     if(localId!="") {
        CAL_DEBUG_LOG("Component with external id %s already exists",externalId.c_str());
        return FAILURE;
    }

    istatus = this->addComponent(pBday, E_ADD, pErrorCode);
    CAL_DEBUG_LOG("Component ID added is : %s \n", pBday->getId().c_str());

    if (istatus == FAILURE)
	return false;


    if (pBday->getRecurrence())
	if (!this->
	    addRecurrence(pBday->getRecurrence(), pBday->getId(),
		  pErrorCode))
	    return false;

    /* here we need to update MAP table which maps external ID 
     * to the internal one 
     * 
     *  */

    this->addMapTableInfo(pBday->getId(), externalId, pErrorCode);

    /*
     * Update Bday cache
     */
    CCalendarProcs procs(pCalDb);
    int add_bday_error = procs.addBDay(pBday->getId(), pBday->getBirthDate());

    if (add_bday_error != SQLITE_OK)
    {
        pCalDb->sqliteErrorMapper(add_bday_error, pErrorCode);
        return false;
    }

    return true;

}

/**
 * @param  string sLocalId - local Id of Birthday event 
 * @param string externalId Id maintained by exteranl applications
 * @param int& pErrorCode - reference to errorcode 
 * @return boolean - success/failure  indicator 
 * 
 * Function to map externalId of Bday event with the internal ones
 */

bool CCalendar::addMapTableInfo(string sLocalId, string sExternalId,
                int &pErrorCode)
{

/** 
 * INSERT_IDMAP 
 */

    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    int iCount = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    if (pCalDb == 0) {
	CAL_DEBUG_LOG("Invalid CalDb pointer \n");
	pErrorCode = CALENDAR_APP_ERROR;
	return FAILURE;
    }
/*FOR-RECURRENCE*/
    pQuery =
    sqlite3_mprintf(INSERT_IDMAP, IDMAP_FIELD_LOCALID,
            IDMAP_FIELD_EXTID);

    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);


    if (pPreparedStmt == 0) {

	CAL_DEBUG_LOG
	    ("prepared statement is 0, so returning from here  \n ");
	sqlite3_free(pQuery);

	return false;
    }
    CAL_DEBUG_LOG("query is  %s", pQuery);
    sqlite3_free(pQuery);


    sqlite3_bind_text(pPreparedStmt, ++iCount, sLocalId.c_str(),
              sLocalId.length(), SQLITE_TRANSIENT);
    sqlite3_bind_text(pPreparedStmt, ++iCount, sExternalId.c_str(),
              sExternalId.length(), SQLITE_TRANSIENT);

    pCalDb->insertRows(pPreparedStmt,iSqliteError); 
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:addMapTableInfo:addMapTableInfo failed to insert in DB");
    return true;
}

/**
 * @param sId input Id string for which mapped Id has to be found
 * @param bool fetchExternal - flag to fetch external Id 
 * @param int & reference to error code.
 * @return  string Id - local ID of the birthday event
 * 
 * Function to get local Id corresponding the the external Id 
 * fetchLocal- 1 returns local Id  corresponding to external Id 
 *           - 0 returns externalId corresponding to local Id 
 */

string CCalendar::getExternalToLocalId(string sId, bool fetchLocal,
                       int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    string sResult;

    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0) {

	pErrorCode = CALENDAR_APP_ERROR;
	CAL_DEBUG_LOG(" invalid pointer \n");
	return sResult;
    }

    if (fetchLocal == true)

	pQuery =
        sqlite3_mprintf(SELECT_DOMAIN, IDMAP_FIELD_LOCALID,
                IDMAP_TABLE, IDMAP_FIELD_EXTID, sId.c_str());
    else
	pQuery =
        sqlite3_mprintf(SELECT_DOMAIN, IDMAP_FIELD_EXTID, IDMAP_TABLE,
                IDMAP_FIELD_LOCALID, sId.c_str());

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s", pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0) {
	CAL_DEBUG_LOG("Query result is null:");
	return sResult;
    }


    sResult = pQr->pResult[1];


    if (pQr) {
	sqlite3_free_table(pQr->pResult);
    delete pQr;
	pQr = 0;
    }

    return sResult;
}



/**
 * modifyBirthDay
 * @param pBday pointer to CBdayEvent object 
 * @param int * pErrorCode error code if any
 * @return bool (SUCCESS/FAILURE)
 * 
 * This function is called to modify the birthday record
 * in the database.
 *
 */

bool CCalendar::modifyBirthDay(CBdayEvent * pBday, int &pErrorCode)
{

    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    int iSqliteError = 0;
    bool bRet = false;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pCalDb == 0) {

	CAL_DEBUG_LOG("Calendar:Invalid Caldb pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;

    }

    if (!pBday) {
	CAL_DEBUG_LOG("CCalendar pointer is 0");
	return false;
    }


    if (this->addComponent(pBday, E_MODIFY, pErrorCode) == FAILURE)
	return false;


    if (pBday->getRecurrence()) {

	bRet = checkEntryExist(RECURSIVE_TABLE, pBday->getId(), pErrorCode);
	if ((pErrorCode == CALENDAR_OPERATION_SUCCESSFUL)&& (bRet)){
	    if (!this-> modifyRecurrence(pBday->getRecurrence(),
		      pBday->getId(), pErrorCode))
		return false;
	} 
	else {

	    if (!this-> addRecurrence(pBday->getRecurrence(),
		      pBday->getId(), pErrorCode))
		return false;
	}
    }

    CAlarm *pAlarm = 0;
    pAlarm = pBday->getAlarm();
    if (pAlarm) {
	bRet = checkEntryExist(ALARM_TABLE, pBday->getId(), pErrorCode);
	if ((pErrorCode == CALENDAR_OPERATION_SUCCESSFUL) && (bRet)){
	    if (!this->IsShown()) {
	    /* Donot register alarm for an invisible calendar */

		CAL_DEBUG_LOG
		("Calendar is invisible  so not registernig the Alarm event ");
		this->addAlarm(pAlarm, E_MODIFY, pBday->getId(),
		       pErrorCode);
		if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		    CAL_DEBUG_LOG (" Errorcode is :%d,so returning  from here ",
				 pErrorCode);
		    return false;
		}
	    } 
	    else {


		if (this->manageCookie(E_MODIFY, pBday, pErrorCode)) {
            this->addAlarm(pAlarm, E_MODIFY, pBday->getId(),
                pErrorCode);
            if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
            CAL_DEBUG_LOG
                (" Errorcode is :%d,so returning  from here ",
                    pErrorCode);
            return false;
            }
		} 
		else if (pErrorCode == CALENDAR_ALARM_ERROR) {
            pBday->removeAlarm();
            this->addComponent(pBday, E_MODIFY, pErrorCode);

            CAL_DEBUG_LOG ("Alarm error so ignoring add alarm request and \
                            adding it as normal event \n");
            pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
		} 
		else
		    return false;
	    }
	} 
	else {
	    if (!this->IsShown()) {
		CAL_DEBUG_LOG
		    ("Calendar is invisible  so not registernig the Alarm event ");
		this->addAlarm(pAlarm, E_ADD, pBday->getId(), pErrorCode);
		if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
		    CAL_DEBUG_LOG
			(" Errorcode is :%d,so returning  from here ", pErrorCode);
		    return false;
		}
	    } 
	    else {

		if (this->manageCookie(E_ADD, pBday, pErrorCode)) {
            this->addAlarm(pAlarm, E_ADD, pBday->getId(),
                pErrorCode);
            if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
            CAL_DEBUG_LOG
                (" Errorcode is :%d,so returning  from here ", pErrorCode);
            return false;
            }
		} 
		else if (pErrorCode == CALENDAR_ALARM_ERROR) {
            pBday->removeAlarm();
            this->addComponent(pBday, E_MODIFY, pErrorCode);

            CAL_DEBUG_LOG ("Alarm error so ignoring add alarm request and \
                            adding it as normal event \n");
            pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

		} 
		else
		    return false;
	    }
	}
    }

    else {
    vector <long> oldcookie;
    vector <long>::iterator citer;
    /* deleting the pre-existing alarm */

    oldcookie = this->getCookie(pBday->getId(), pErrorCode);
    CAlarm alarmUtility;
	if (!oldcookie.empty()){

	    alarmUtility.deleteAlarmEvent(oldcookie[0],pErrorCode);
	    pQuery =
		    sqlite3_mprintf(DELETE_TABLE, ALARM_TABLE, ALARM_FIELD_ID);
	    ASSERTION(pQuery);
	    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			    &pPreparedStmt, &pTail);
	    sqlite3_bind_int(pPreparedStmt, 1, atoi(pBday->getId().c_str()));
	    CAL_DEBUG_LOG("%s", pQuery);
	    sqlite3_free(pQuery);
	    pQuery = 0;
	    pCalDb->updateDB(pPreparedStmt,iSqliteError);
	    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:modifyBirthday:modifyBirthday failed to update Alarm Table");
       }
    }

    CCalendarProcs procs(pCalDb);
    int add_bday_error = procs.addBDay(pBday->getId(), pBday->getBirthDate());

    if (add_bday_error != SQLITE_OK)
    {
        pCalDb->sqliteErrorMapper(add_bday_error, pErrorCode);
        return false;
    }

    return true;

}
/**
 * deleteBirthDay
 * @param sUid Ebook Uid to be deleted
 * @param int * pErrorCode error code if any
 * 
 * @return bool (SUCCESS/FAILURE)
 *
 * Function to delete Birthday contacts from database
 * This fucntion will be only called from contacts Id passed here is different 
 * from the local Id stored.
 */
bool CCalendar::deleteBirthDay(string szUId, int &pErrorCode)
{

    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    int iSqliteError = 0;
    vector <long> oldcookie;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pCalDb == 0) {
        CAL_DEBUG_LOG("invalid CCalendarDB pointer ");
        pErrorCode = CALENDAR_APP_ERROR;
        return false;
    }

    szUId = this->getExternalToLocalId(szUId, true, pErrorCode);
    if (szUId.empty()) {
        CAL_DEBUG_LOG("No mapping for contact #%s, so ignore it", szUId.c_str());
        return false;
    }
    /*      update the trash table with the 
     *      values */
    this->updateTrash(this->getCalendarId(), szUId,
              E_BDAY, time_get_time(), pErrorCode);

    oldcookie = this->getCookie(szUId, pErrorCode);
    /* Delete from components table will fire a DB trigger which deletes data 
     * from other tables. */
    pQuery =
    sqlite3_mprintf(DELETE_TABLE, COMPONENTS_TABLE, ENTRY_FIELD_ID);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);

    sqlite3_bind_text(pPreparedStmt, 1, szUId.c_str(), szUId.length(),
              SQLITE_TRANSIENT);

    CAL_DEBUG_LOG("%s", pQuery);
    sqlite3_free(pQuery);
    pQuery = 0;

    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deleteBirthday:deleteBirthDay failed to upadte COMPONENTS_TABLE in DB");

    /* its not a big deal if event is deleted and 
     * alarm still exist so not rolling back in this case 
     * as this is a corner scenario 
     */
    CAlarm alarmUtility;
    
    if(!oldcookie.empty())
        alarmUtility.deleteAlarmEvent(oldcookie[0],pErrorCode);
	
	/* delete external ID mapping */
	pQuery =
    sqlite3_mprintf(DELETE_TABLE, IDMAP_TABLE, IDMAP_FIELD_LOCALID);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);

    sqlite3_bind_text(pPreparedStmt, 1, szUId.c_str(), szUId.length(),
              SQLITE_TRANSIENT);

    CAL_DEBUG_LOG("%s", pQuery);
    sqlite3_free(pQuery);
    pQuery = 0;

    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deleteBirthday:deleteIDmap failed to upadte IDMAP_TABLE in DB");    
    return true;
}

/**
 * intToString 
 * Utility function to convert integer to string
 * 
 * @param integer
 * 
 * @return string
 */
string intToString(int i)
{
    string str;
    stringstream out;
    out << i;
    str = out.str();
    return str;
}


/**
 * updateTrash
 * This function will be called when components are deleted, to update 
 * them in trash table
 *
 * @param int Calendar id
 * @param string  component id
 * @param int component type
 * @param time_t Deleted time
 * @param int * pErrorCode error code if any
 *
 * @return SUCCESS/FAILURE
 **/

int CCalendar::updateTrash(int iCalId, string sCompId, int iCompType,
               time_t iDeletedTime, int &pErrorCode)
{

    char *pQuery = 0;
    const char *pTail = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    time_t iCreatedTime = 0;
    int iSqliteError = 0;
    int iCol = 1;
    int i = 0;
    int j = 0;
    int k = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid caldb pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return FAILURE;
    }

    QueryResult *pQr = 0;
    pQuery = sqlite3_mprintf(GET_CTIME_UID, sCompId.c_str());
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is : %s", pQuery);

    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    if (pQr == 0) {
	CAL_DEBUG_LOG("query result is null\n");
	return FAILURE;
    }

    for (i = 0; i < pQr->iRow; i++) {
	for (j = 0; j < pQr->iColumn; j++) {
	    k = pQr->iColumn + (i * pQr->iColumn);
	    switch (j) {

		case DB_COLUMN_ID1:
		    iCreatedTime = (time_t) atoi(pQr->pResult[k + j]);
		    break;

		default:
		    break;
	    }
	}
    }
    if (pQr) {
	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }

    pQuery =
    sqlite3_mprintf(INSERT_TRASH, TRASH_FIELD_ID, TRASH_FIELD_CALID,
            TRASH_FIELD_TYPE, TRASH_FIELD_TIME,
            TRASH_FIELD_CTIME);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s", pQuery);

    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);

    sqlite3_bind_text(pPreparedStmt, iCol++, sCompId.c_str(),
              sCompId.length(), SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, iCol++, iCalId);
    sqlite3_bind_int(pPreparedStmt, iCol++, iCompType);
    sqlite3_bind_int(pPreparedStmt, iCol++, iDeletedTime);
    sqlite3_bind_int(pPreparedStmt, iCol++, iCreatedTime);

    pCalDb->insertRows(pPreparedStmt,iSqliteError); 
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:updateTrash:updatTrash failed to insert in DB");
    return SUCCESS;
}


/**
 * getAllAddedEvents()
 * @param: int Date Added items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CEvent 
 *  
 * This function return all the events which are added after 
 * specified date 
 */
vector < CEvent * >CCalendar::getAllAddedEvents(int iDate, int &pErrorCode)
{
    vector < CEvent * >vEventAddList;
    vector < string > vIdList;
    unsigned int iCount = 0;
    CEvent *pEvent = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vIdList = this->getChangeIds(E_EVENT, E_ADD, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	pEvent = this->getEvent(vIdList[iCount], pErrorCode);
	if(pEvent)
        vEventAddList.push_back(pEvent);
    }
    vIdList.clear();

    return vEventAddList;
}

/**
 * getAllAddedTodos()
 * @param: int Date Added items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CTodo 
 *  
 * This function return all the Todos which are added after 
 * specified date 
 */
vector < CTodo * >CCalendar::getAllAddedTodos(int iDate, int &pErrorCode)
{
    vector < CTodo * >vTodoAddList;
    vector < string > vIdList;
    unsigned int iCount = 0;
    CTodo *pTodo = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vIdList = this->getChangeIds(E_TODO, E_ADD, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pTodo = this->getTodo(vIdList[iCount], pErrorCode);
	   if (pTodo)
            vTodoAddList.push_back(pTodo);
    }

    vIdList.clear();
    return vTodoAddList;
}

/**
 * getAllAddedJournals()
 * @param: int Date Added items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CJournal 
 *  
 * This function return all the Journals which are added after 
 * specified date 
 */
vector < CJournal * >CCalendar::getAllAddedJournals(int iDate,
                            int &pErrorCode)
{
    vector < CJournal * >vJournalAddList;
    vector < string > vIdList;
    unsigned int iCount = 0;
    CJournal *pJournal = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vIdList = this->getChangeIds(E_JOURNAL, E_ADD, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pJournal = this->getJournal(vIdList[iCount], pErrorCode);
	   if(pJournal)
            vJournalAddList.push_back(pJournal);
    }
    vIdList.clear();
    return vJournalAddList;
}

/**
 * getAllAddItems()
 * @param: int Date Added items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CComponent 
 *  
 * This function returna all the components which are added after 
 * specified date 
 */
vector < CComponent * >CCalendar::getAllAddItems(int iDate,
                         int &pErrorCode)
{

    vector < CComponent * >vAddList;
    vector < string > vIdList;
    unsigned int iCount = 0;
    CEvent *pEvent = 0;
    CTodo *pTodo = 0;
    CJournal *pJour = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vIdList = this->getChangeIds(E_EVENT, E_ADD, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pEvent = this->getEvent(vIdList[iCount], pErrorCode);
	   if(pEvent)
            vAddList.push_back((CComponent *) pEvent);
    }
    vIdList.clear();

    vIdList = this->getChangeIds(E_TODO, E_ADD, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pTodo = this->getTodo(vIdList[iCount], pErrorCode);
       if(pTodo)
            vAddList.push_back((CComponent *) pTodo);
    }
    vIdList.clear();

    vIdList = this->getChangeIds(E_JOURNAL, E_ADD, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pJour = this->getJournal(vIdList[iCount], pErrorCode);
       if(pJour)
            vAddList.push_back((CComponent *) pJour);
    }

    vIdList.clear();
    return vAddList;

}

/**
 * @param string sCompId Component Id 
 * @param int iType cand be E_EVENT,E_TODO or E_JOURNAL
 * @param int pErrorCode reference to error code 
 * 
 * Function to retrieve Entry from DB , which can be event todo or Journal 
 * It is safe to cast a derived class pointer to base class pointer.
 *
 */ 
CComponent*  CCalendar::getEntry(string sCompId , int iType, int &pErrorCode)
{
    if (iType == E_EVENT)
	return (this->getEvent(sCompId, pErrorCode));
    else if (iType == E_TODO)
	return (this->getTodo(sCompId, pErrorCode));
    else if (iType == E_JOURNAL)
	return (this->getJournal(sCompId, pErrorCode));
    else
        pErrorCode = CALENDAR_INVALID_ARG_ERROR;

    return 0;
}


/**
 * getAllModifiedEvents()
 * @param: int Date Modified items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CEvent 
 *  
 * This function returns all the events which are Modified after 
 * specified date 
 */
vector <CEvent *>CCalendar::getAllModifiedEvents(int iDate,
                           int &pErrorCode)
{

    vector < CEvent * >vModEventList;
    vector < string > vIdList;
    unsigned int iCount = 0;
    CEvent *pEvent = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vIdList = this->getChangeIds(E_EVENT, E_MODIFY, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pEvent = this->getEvent(vIdList[iCount], pErrorCode);
	   if(pEvent)
            vModEventList.push_back(pEvent);
    }
    vIdList.clear();

    return vModEventList;
}

/**
 * getAllModifiedTodos()
 * @param: int Date Modified items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CTodo 
 *  
 * This function returns all the todos which are Modified after 
 * specified date 
 */
vector < CTodo * >CCalendar::getAllModifiedTodos(int iDate,
                         int &pErrorCode)
{

    vector < CTodo * >vModTodoList;
    vector < string > vIdList;
    unsigned int iCount = 0;
    CTodo *pTodo = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vIdList = this->getChangeIds(E_TODO, E_MODIFY, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pTodo = this->getTodo(vIdList[iCount], pErrorCode);
	   if(pTodo)
            vModTodoList.push_back(pTodo);
    }
    vIdList.clear();

    return vModTodoList;
}

/**
 * getAllModifiedJournals()
 * @param: int Date Modified items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CJournal 
 *  
 * This function returns all the journals which are Modified after 
 * specified date 
 */
vector < CJournal * >CCalendar::getAllModifiedJournals(int iDate,
                               int &pErrorCode)
{

    vector < CJournal * >vModJournalList;
    vector < string > vIdList;
    unsigned int iCount = 0;
    CJournal *pJournal = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vIdList = this->getChangeIds(E_JOURNAL, E_MODIFY, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pJournal = this->getJournal(vIdList[iCount], pErrorCode);
	   if(pJournal)
            vModJournalList.push_back(pJournal);
    }
    vIdList.clear();

    return vModJournalList;
}

/**
 * getAllModifiedItems()
 * @param: int Date Modified items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CComponent 
 *  
 * This function returns all the components which are Modified after 
 * specified date 
 */
vector < CComponent * >CCalendar::getAllModifiedItems(int iDate,
                              int &pErrorCode)
{

    vector < CComponent * >vModList;
    vector < string > vIdList;
    unsigned int iCount = 0;
    CEvent *pEvent = 0;
    CTodo *pTodo = 0;
    CJournal *pJour = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vIdList = this->getChangeIds(E_EVENT, E_MODIFY, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pEvent = this->getEvent(vIdList[iCount], pErrorCode);
	   if(pEvent)
            vModList.push_back((CComponent *) pEvent);
    }
    vIdList.clear();

    vIdList = this->getChangeIds(E_TODO, E_MODIFY, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pTodo = this->getTodo(vIdList[iCount], pErrorCode);
       if(pTodo)
            vModList.push_back((CComponent *) pTodo);
    }
    vIdList.clear();

    vIdList = this->getChangeIds(E_JOURNAL, E_MODIFY, iDate, pErrorCode);

    for (iCount = 0; iCount < vIdList.size(); iCount++) {
	   pJour = this->getJournal(vIdList[iCount], pErrorCode);
       if(pJour)
            vModList.push_back((CComponent *) pJour);
    }
    vIdList.clear();

    return vModList;

}

/**
 * @param: int Date Deleted items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CComponent 
 *  
 * This function returns all the Events which are Deleted after 
 * specified date 
 */
vector < string > CCalendar::getAllDeletedEvents(int iLastSyncDate,
                         int &pErrorCode)
{
    return (this->getAllDeletedItems(iLastSyncDate, E_EVENT, pErrorCode));

}

/**
 * @param: int Date Deleted items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CComponent 
 *  
 * This function returns all the tasks which are Deleted after 
 * specified date 
 */
vector < string > CCalendar::getAllDeletedTodos(int iLastSyncDate,
                        int &pErrorCode)
{
    return (this->getAllDeletedItems(iLastSyncDate, E_TODO, pErrorCode));
}

/**
 * @param: int Date Deleted items after this date  
 * @param int * pErrorCode error code if any
 * @return: vector of CComponent 
 *  
 * This function returns all the components which are Deleted after 
 * specified date 
 */
vector < string > CCalendar::getAllDeletedJournals(int iLastSyncDate,
                           int &pErrorCode)
{


    return (this->
        getAllDeletedItems(iLastSyncDate, E_JOURNAL, pErrorCode));
}


/**
 * getAllDeletedItems()
 * @param: int Date Deleted items after this date  
 * @param : int iType signifies the component type can be Event /Todo/Journal
 * @param int * pErrorCode error code if any
 * @return: vector of CComponent 
 *  
 * This function returns all the journals which are Deleted after 
 * specified date 
 */
vector < string > CCalendar::getAllDeletedItems(int iLastSyncDate,
                        int iType, int &pErrorCode)
{
    vector < string > vDelList;
    char *pQuery = 0;
    int iSqliteError = 0;
    int i, j, k;
    /* to be discussed and finalized */

    /* Fetch the data items if lastSyncDate is less than CreatedTime */

    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
	CAL_DEBUG_LOG("invalid caldb pointer :");
	pErrorCode = CALENDAR_APP_ERROR;
	return vDelList;
    }
    string Uid;
    QueryResult *pQr = 0;

    pQuery =
    sqlite3_mprintf(GET_COMPID_FROM_TRASH, iLastSyncDate,
            this->getCalendarId(), iType);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s", pQuery);
    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    /* error code will be set in the mapper function above */
    if (pQr == 0) {
	CAL_DEBUG_LOG("Query result is null:");
	sqlite3_free(pQuery);
	return vDelList;
    }

    sqlite3_free(pQuery);

    for (i = 0; i < pQr->iRow; i++) {
	for (j = 0; j < pQr->iColumn; j++) {
	    k = pQr->iColumn + (i * pQr->iColumn);
	    switch (j) {

		case DB_COLUMN_ID1:
		    vDelList.push_back(pQr->pResult[k + j]);
		    break;

		default:
		    break;
	    }
	}
    }


    if (pQr) {

	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    return vDelList;
}


/**
 * getChangeIds()
 * @param: int iCompType specifies ComponentType (event/todo/journal)  
 * @param: int iChangeType specifies ChangeType (add/mod/delete)
 * @param: int iDate 
 * @param int * pErrorCode error code if any
 * @return: vector of string  Ids 
 *  
 * This function returns all the ids required as per the given component type,
 * change type and date 
 */
vector < string > CCalendar::getChangeIds(int iCompType, int iChangeType,
                      int iDate, int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    vector < string > vIdList;
    int iIdCount = 0;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    pDb = CCalendarDB::Instance();

    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return vIdList;
    }

    switch (iChangeType) {
        case (E_ADD):
            {
                if (iCompType == E_EVENT) {
                    CAL_DEBUG_LOG("in add-event case\n");
                    pQuery = sqlite3_mprintf(GET_ADDED_ENTRIES, ENTRY_FIELD_ID,
                            ENTRY_FIELD_CRTIME, iDate,
                            ENTRY_FIELD_TYPE, E_EVENT,
                            ENTRY_FIELD_CALID,
                            this->getCalendarId());
                }

                if (iCompType == E_TODO)
                    pQuery = sqlite3_mprintf(GET_ADDED_ENTRIES, ENTRY_FIELD_ID,
                            ENTRY_FIELD_CRTIME, iDate,
                            ENTRY_FIELD_TYPE, E_TODO,
                            ENTRY_FIELD_CALID,
                            this->getCalendarId());

                if (iCompType == E_JOURNAL)
                    pQuery = sqlite3_mprintf(GET_ADDED_ENTRIES, ENTRY_FIELD_ID,
                            ENTRY_FIELD_CRTIME, iDate,
                            ENTRY_FIELD_TYPE, E_JOURNAL,
                            ENTRY_FIELD_CALID,
                            this->getCalendarId());
                break;

            }

        case (E_MODIFY):
            {
                if (iCompType == E_EVENT)
                    pQuery =
                        sqlite3_mprintf(GET_MODIFIED_ENTRIES, ENTRY_FIELD_ID,
                                ENTRY_FIELD_CRTIME, iDate,
                                ENTRY_FIELD_MODTIME, iDate,
                                ENTRY_FIELD_TYPE, E_EVENT,
                                ENTRY_FIELD_CALID,
                                this->getCalendarId());

                if (iCompType == E_TODO)
                    pQuery =
                        sqlite3_mprintf(GET_MODIFIED_ENTRIES, ENTRY_FIELD_ID,
                                ENTRY_FIELD_CRTIME, iDate,
                                ENTRY_FIELD_MODTIME, iDate,
                                ENTRY_FIELD_TYPE, E_TODO,
                                ENTRY_FIELD_CALID,
                                this->getCalendarId());

                if (iCompType == E_JOURNAL)
                    pQuery =
                        sqlite3_mprintf(GET_MODIFIED_ENTRIES, ENTRY_FIELD_ID,
                                ENTRY_FIELD_CRTIME, iDate,
                                ENTRY_FIELD_MODTIME, iDate,
                                ENTRY_FIELD_TYPE, E_JOURNAL,
                                ENTRY_FIELD_CALID,
                                this->getCalendarId());
                break;

            }

        case (E_DELETE):
            {
                /* still to be discussed and finalized */
                break;
            }

        default:
            break;


    }                /*end of switch */


    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s", pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    /* error code will be set in the mapper function above */
    if (pQr == 0) 
	    return vIdList;

    for (iIdCount = 1; iIdCount <= pQr->iRow; iIdCount++) {
        char *val = pQr->pResult[iIdCount];
        if (val != 0) {
            vIdList.push_back(val);
        }
        else
        {
            CAL_DEBUG_LOG("Got NULL in SQL result data");
        }
    }

    if (pQr) {
        sqlite3_free_table(pQr->pResult);
	    delete pQr;
	    pQr = 0;
    }

    return vIdList;
}

/* adding event in batch */

vector<string> CCalendar::addComponents(vector<CComponent*> &compList,
		vector <string> &duplicateIds,
		int &pErrorCode)
{

	vector <string> 		IdList;		
	vector <CComponent*>::iterator	iter;
	int iCount =1 ;
	string szDummy;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL; 
	

	for (iter = compList.begin(); iter != compList.end();iter++)
	{
		if (*iter == NULL)
		{
			CAL_DEBUG_LOG("Component is invalid ");
			continue;
		}
		if ((*iter)->getType() == E_EVENT)
			this->addEvent((CEvent*) (*iter),pErrorCode);
		else if ((*iter)->getType() == E_TODO)
			this->addTodo( (CTodo*)(*iter),pErrorCode);
		else if ((*iter)->getType() == E_JOURNAL)
			this->addJournal((CJournal*)(*iter),pErrorCode);
		else{
			;
			CAL_DEBUG_LOG("Invalid type of Component sent for adding");
		}
		
		/* if error code is operation successful / 
		 * duplicate entry then maintain the ID in
		 * both the lists 
		 * if error code is CALENDAR_ENTRY_DUPLICATED then 
		 * maintain the Ids in duplicate list 
		 * If there is any other error then break the execution
		 */
		
		if (pErrorCode == CALENDAR_ENTRY_DUPLICATED)
			duplicateIds.push_back((*iter)->getId());
		
		if ((pErrorCode == CALENDAR_OPERATION_SUCCESSFUL) ||
				(pErrorCode == CALENDAR_ENTRY_DUPLICATED))
		  	IdList.push_back((*iter)->getId());
		else if (pErrorCode == CALENDAR_DISK_FULL) {
			CAL_DEBUG_LOG("Disk full so not overwriting error code :(");
		        IdList.push_back(szDummy);
		}
		else{
		      /* Any other error code please set dummy string in
		       * to the IdList
		       */
		     IdList.push_back(szDummy);
		     CAL_DEBUG_LOG("Encountered error while processing %d th"
		       " Element in Component vector with error code %d ", iCount,pErrorCode);
		     /* overwriting the error code here 
		      * so that if one error is encountered entire 
		      * tansaction is not rolled back */
		        pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
		}
		++iCount;

	}
	return IdList;
}

/* modifying events in batch */	
void  CCalendar::modifyComponents(vector<CComponent*> &compList,
		int &pErrorCode)
{

	vector <CComponent*>::iterator		iter;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	for (iter = compList.begin(); iter != compList.end();iter++)
	{
		if (*iter == NULL)
		{
			CAL_DEBUG_LOG("Component is invalid ");
			continue;
		}
		if ((*iter)->getType() == E_EVENT)
			this->modifyEvent((CEvent*) (*iter),pErrorCode);
		else if ((*iter)->getType() == E_TODO)
			this->modifyTodo( (CTodo*)(*iter),pErrorCode);
		else if ((*iter)->getType() == E_JOURNAL)
			this->modifyJournal((CJournal*)(*iter),pErrorCode);
		else{
			;
			CAL_DEBUG_LOG("Invalid type of Component sent for adding");
		}
		if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
			break;
	}
return;	
}

/*deleting events in batch */
void CCalendar::deleteComponents(vector<string> idList,
		int &pErrorCode)
{
	vector <string>::iterator		iter;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	for (iter = idList.begin(); iter != idList.end();iter++)
	{
			
		this->deleteComponent(*iter,pErrorCode);
		if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
			break;
	}
	
return;	
}


bool CCalendar::updateAttendeesForComponent(CComponentDetails *pCompDetails, 
		int &pErrorCode)
{
	bool bRetVal = SUCCESS;

	if((bRetVal = this->deleteAttendeesForComponent(pCompDetails, pErrorCode))
			== SUCCESS)
		this->addAttendeeDetails(pCompDetails->getAttendees(), 
				pCompDetails->getId(),
				pCompDetails->getType(), 
				E_ADD, pErrorCode);

	return bRetVal;
}


bool CCalendar::deleteAttendeesForComponent(CComponentDetails *pCompDetails, int &pErrorCode)
{
	char *pQuery = 0;
	sqlite3_stmt *pPreparedStmt = 0;
	int iSqliteError = 0;
	CCalendarDB *pCalDb = CCalendarDB::Instance();
	const char *pTail = 0;

	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	if ((pCalDb == 0) || (pCompDetails == NULL)) {
		CAL_DEBUG_LOG("Calendar Invalid CalDb pointer");
		pErrorCode = CALENDAR_INVALID_ARG_ERROR;
		return FAILURE;
	}

	if ((pCompDetails->getAttendees()).size() == 0)
		return SUCCESS;

	pQuery = sqlite3_mprintf(DELETE_TABLE_AND, PARAMS_TABLE, PARAM_FIELD_ID,
			PARAM_FIELD_PROPNAME);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, DB_QUERY_INDEX1, atoi(pCompDetails->getId().c_str()));
	sqlite3_bind_text(pPreparedStmt, DB_QUERY_INDEX2, ATTDEE, strlen(ATTDEE), SQLITE_TRANSIENT);
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deleteAttendeesForComponent:deleteAttendeeForComponent" 
			" Failed to upadte in DB");
	return SUCCESS;
}


bool CCalendar::updateOrganizerForComponent(CComponentDetails *pCompDetails, int &pErrorCode)
{   
	bool bRetVal = SUCCESS;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	if (!pCompDetails->getOrganizer())
		return SUCCESS;

	if(this->deleteOrganizerForComponent(pCompDetails, pErrorCode))
		bRetVal = this->addOrganizer(pCompDetails->getOrganizer(),
				pCompDetails->getId(),
				pCompDetails->getType(),
				E_ADD,
				pErrorCode);

	return bRetVal;
}   


bool CCalendar::deleteOrganizerForComponent(CComponentDetails *pCompDetails, int &pErrorCode)
{
	char *pQuery = 0;
	sqlite3_stmt *pPreparedStmt = 0;
	int iSqliteError = 0;
	CCalendarDB *pCalDb = CCalendarDB::Instance();
	const char *pTail = 0;

	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	if ((pCalDb == 0) || (pCompDetails == NULL)) {
		CAL_DEBUG_LOG("Calendar Invalid CalDb pointer");
		pErrorCode = CALENDAR_INVALID_ARG_ERROR;
		return FAILURE;
	}

	if (!pCompDetails->getOrganizer())
		return SUCCESS;

	pQuery = 0;
	pQuery = sqlite3_mprintf(DELETE_TABLE_AND, PARAMS_TABLE, PARAM_FIELD_ID,
			PARAM_FIELD_PROPNAME);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, DB_QUERY_INDEX1, atoi(pCompDetails->getId().c_str()));
	sqlite3_bind_text(pPreparedStmt, DB_QUERY_INDEX2, ORGZER,
			strlen(ORGZER), SQLITE_TRANSIENT);
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deleteOrganizerForComponent:deleteOrganizerForComponent"
		       "failed to update in DB");
	return SUCCESS;
}


bool CCalendar::updateXPropertiesForComponent(CComponentDetails *pEntry, int &pErrorCode)
{
	bool bRetVal = SUCCESS;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	if ((pEntry->getXProperties()).size() == 0)
		return SUCCESS;

	if(this->deleteXPropertiesForComponent(pEntry, pErrorCode))
		bRetVal = this->addXProperties(pEntry->getXProperties(), pEntry->getId(),
				E_ADD, pErrorCode);

	return bRetVal;     
}   

bool CCalendar::deleteXPropertiesForComponent (CComponentDetails *pEntry, int &pErrorCode)
{
	char *pQuery = 0;
	sqlite3_stmt *pPreparedStmt = 0;
	int iSqliteError = 0;
	CCalendarDB *pCalDb = CCalendarDB::Instance();

	if (pCalDb == 0) {
        	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        	pErrorCode = CALENDAR_APP_ERROR;
        	return false;
	}

	const char *pTail = 0;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


	if (pEntry->getXProperties().size()!= 0)
		return true;

	pQuery = sqlite3_mprintf(DELETE_TABLE, XPROP_TABLE, XPROP_FIELD_ID);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			&pPreparedStmt, &pTail);

	sqlite3_bind_int(pPreparedStmt, 1, atoi(pEntry->getId().c_str()));
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
		return false;
	return true;

}


bool CCalendar::updateParametersForComponent(CComponentDetails *pEntry, int &pErrorCode)
{
	bool bRetVal = SUCCESS;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	if ((pEntry->getXProperties()).size() == 0)
		return SUCCESS;

	if(this->deleteParametersForComponent(pEntry, pErrorCode))
		bRetVal = this->addParameters(pEntry->getHashMap(), pEntry->getId(), E_ADD,
				pErrorCode, false);

	return bRetVal;
}


bool CCalendar::deleteParametersForComponent (CComponentDetails *pEntry, int &pErrorCode)
{
	char *pQuery = 0;
	sqlite3_stmt *pPreparedStmt = 0;
	int iSqliteError = 0;
	CCalendarDB *pCalDb = CCalendarDB::Instance();
	const char *pTail = 0;

	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	if ((pCalDb == 0) || (pEntry == NULL)) {
		CAL_DEBUG_LOG("Calendar Invalid CalDb pointer");
		pErrorCode = CALENDAR_INVALID_ARG_ERROR;
		return FAILURE;
	}

	if ((pEntry->getHashMap()).size() == 0)
		return SUCCESS;

	pQuery = sqlite3_mprintf(DELETE_TABLE, PARAMS_TABLE, PARAM_FIELD_ID);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			&pPreparedStmt, &pTail);
	sqlite3_bind_int(pPreparedStmt, DB_QUERY_INDEX1, atoi(pEntry->getId().c_str()));
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;

	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deleteParametersForComponent:deleteParametersForComponent failed to update in DB");
	return SUCCESS;
}



bool CCalendar::deregisterAlarm (CComponentDetails *pCompDetails, int &pErrorCode)
{
	char *pQuery = 0;
	sqlite3_stmt *pPreparedStmt = 0;
	int iSqliteError = 0;
	CCalendarDB *pCalDb = CCalendarDB::Instance();
	vector <long> oldcookie;
	vector <long>::iterator citer;
	CAlarm alarmUtility;
	const char *pTail = 0;

	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	if ((pCalDb == 0) || (pCompDetails == NULL)) {
		CAL_DEBUG_LOG("Calendar Invalid CalDb pointer");
		pErrorCode = CALENDAR_INVALID_ARG_ERROR;
		return FAILURE;
	}

	oldcookie.clear();
	oldcookie = this->getCookie(pCompDetails->getId(), pErrorCode);
	if (oldcookie.empty())
		return SUCCESS;

	alarmUtility.deleteAlarmEvent(oldcookie[0], pErrorCode);
	pQuery =
		sqlite3_mprintf(DELETE_TABLE, ALARM_TABLE, ALARM_FIELD_ID);
	ASSERTION(pQuery);
	sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
			&pPreparedStmt, &pTail);

	sqlite3_bind_int(pPreparedStmt, DB_QUERY_INDEX1, atoi(pCompDetails->getId().c_str()));
	CAL_DEBUG_LOG("%s", pQuery);
	sqlite3_free(pQuery);
	pQuery = 0;
	pCalDb->updateDB(pPreparedStmt,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CALENDAR_LOG_ERROR(pErrorCode,"CCALENDAR:deregisterAlarm:deregisterAlarm failed to update ALARM_TABLE in DB");
	return SUCCESS;
}





/** batch read API */
/**
 * @param int errorcode 
 * @return vector CEvent* 
 *
 * Function returns all events in a particular calendar 
 */
vector <CEvent*> CCalendar::getEvents(int & pErrorCode )
{

    CEvent *event = 0;
    CAlarm *pAlarm = 0;
    CRecurrence *pRec = 0;
    vector <CEvent*> listEvent;
    QueryResult *pQr = 0;
    int iI_EventCount = 0;
    int iJ_EventCount = 0;
    int iK_EventCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CUtility *pUt = 0;
    pUt = CUtility::Instance();
    vector <long> vCookie;
    vector <string> vERule;
    vector <string> vEdate;
    vector <string> vRdate;
    vector <string> vRRule;



    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return listEvent;
    }

    /*this function is responsible to retrieve the values stored in 
     * Component table and uses the set function to add values in 
     * to event object*/
    pQuery = sqlite3_mprintf(SELECT_INNER_JOIN_BATCH, this->getCalendarId(),E_EVENT);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("Query is : %s",pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    /* error code will be set in the mapper function */
    if (pQr == 0) {
	return listEvent;
    }


    CAL_DEBUG_LOG("column number : %d", pQr->iColumn);
    CAL_DEBUG_LOG("Row  number : %d", pQr->iRow);
    for (iI_EventCount = 0; iI_EventCount < pQr->iRow; iI_EventCount++) {
	/* Create as many CEvent objects 
	 * as the Events are */
	event = new CEvent();
	pAlarm = new CAlarm();
	pRec = new CRecurrence();
	ASSERTION(event);
	for (iJ_EventCount = 0; iJ_EventCount < pQr->iColumn; iJ_EventCount++) {
	    iK_EventCount = pQr->iColumn + (iI_EventCount * pQr->iColumn);

	    if ((pQr->pResult[iK_EventCount + iJ_EventCount]) == 0)
	    continue;
	    switch (iJ_EventCount) {

		case DB_COLUMN_ID1:
		    event->setId(pQr->pResult[iK_EventCount + iJ_EventCount]);
		    break;
		case DB_COLUMN_ID2:
		    //event->setCalendarId(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID3:
		    event->
			setType(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID4:
		    event-> setFlags(atoi (pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID5:
		    event-> setDateStart(atoi (pQr->
				pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID6:
		    event-> setDateEnd(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID7:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setSummary(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;


		case DB_COLUMN_ID8:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setLocation(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID9:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setDescription(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID10:
		    event->setStatus(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID11:

		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setGUid(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID12:
		    event->
			setUntil(atoi
				(pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID13:
		    event->
			setAllDay(atoi
				(pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID14:
		    event-> setCreatedTime(atoi (pQr->
				pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID15:

		    event->setLastModified(atoi (pQr->
				pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID16:
		    event->setTzid(pQr->pResult[iK_EventCount + iJ_EventCount]);
		    break;
		/* getting tz ofset is not required DB_COLUMN_ID17*/    
		/* no need to fill Id and type as thereare taken care above*/
		case DB_COLUMN_ID17:
		case DB_COLUMN_ID18:
		case DB_COLUMN_ID19:
		    break;

		case DB_COLUMN_ID20:

		    if (pQr->pResult[iK_EventCount + iJ_EventCount] != 0) {
			event->setClas(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;


		case DB_COLUMN_ID21:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setGeo(pQr->pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;


		case DB_COLUMN_ID22:
		    event->setPriority(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;


		case DB_COLUMN_ID23:
		    event-> setDateStamp(atoi
			    (pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID24:
		    event-> setSequence(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID25:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setTransparency(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID26:
		    event->
			setUid(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;


		case DB_COLUMN_ID27:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setUrl((pQr->pResult[iK_EventCount + iJ_EventCount]));
		    }
		    break;

		case DB_COLUMN_ID28:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			vector <string> vAtt;
			vAtt.push_back((pQr->pResult[iK_EventCount + iJ_EventCount]));
			event->setAttachments(vAtt);
		    }
		    break;

		case DB_COLUMN_ID29:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setCategories(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID30:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setComments(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID31:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setContact(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID32:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setRelated(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID33:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setResources(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID34:
		    break;

		case DB_COLUMN_ID35:
		    break;
		case DB_COLUMN_ID36:
		    break;

		case DB_COLUMN_ID37:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setTrigger(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID38:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setRepeat(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID39:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setDuration(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID40:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setAction(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID41:

		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			vCookie.push_back(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
			pAlarm->setCookie(vCookie);
		    }
		    break;

		case DB_COLUMN_ID42:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setAttach(pQr-> pResult[iK_EventCount + iJ_EventCount]);
		    break;
		case DB_COLUMN_ID43:
		    break;
		case DB_COLUMN_ID44:
		    
		    if(pQr->pResult[iK_EventCount + iJ_EventCount]){
			vRRule = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount], EXCLAMATION);
			pRec->setRrule(vRRule);
		    }
		    break;

		case DB_COLUMN_ID45:
		    
		    if(pQr->pResult[iK_EventCount + iJ_EventCount]){
			vRdate = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],SEMI_COLON);
			pRec->setRDays(vRdate);
		    }
		    break;

		case DB_COLUMN_ID46:

		    if(pQr->pResult[iK_EventCount + iJ_EventCount]){
			vEdate = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],SEMI_COLON);
			pRec->setEDays(vEdate);
		    }
		    break;

		case DB_COLUMN_ID47:

		    if(pQr->pResult[iK_EventCount + iJ_EventCount]){
			vERule = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],EXCLAMATION);;
			pRec->setErule(vERule);
		    }
		    break;

		case DB_COLUMN_ID48:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount])
			pRec->setRecurId(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID49:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount])
			pRec->setRtype(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		default:
		    break;
	    }



	}
	CAL_DEBUG_LOG("CCalendar %d::getEvents-> %s \n",
		iI_EventCount,event->toString().c_str());
	/* Flags has the following values 
	 * HAS_RECURRENCE -- 3 
	 * HAS_ALARM ---- 4
	 * HAS_RECURRENCE_ALARM - 5 defined in common.h
	 * */
	if ((event->getFlags() == HAS_RECURRENCE) ||
		(event->getFlags() == HAS_RECURRENCE_ALARM))
	    event->setRecurrence (pRec);
	delete pRec;
	pRec = 0;

	if ((event->getFlags() == HAS_ALARM) ||
		(event->getFlags() == HAS_RECURRENCE_ALARM))
	    event->setAlarm(pAlarm);
	delete pAlarm;
	pAlarm =0 ;

	vector < CAttendee * >listAttendee;

	listAttendee = event->retrieveAttendeeDetails();
	COrganizer *pOrg = 0;

	pOrg = event->retrieveOrganizerDetails();
	if (listAttendee.size())
	    event->setAttendees(listAttendee);
	listAttendee.clear();
	pUt->releaseVector<CAttendee>(listAttendee);

	if (pOrg){
	    event->setOrganizer(pOrg);
	    delete pOrg ;
	    pOrg = 0;
	}

	/*retrieve xprop */
	vector < CProperties * >vPropList;
	vPropList = event->retrieveXPropertyDetails();
	event->setXProperties(vPropList);
	pUt->releasePropertiesVector(vPropList);

	/*retrieve params */
	map < string, vector < CParameters * > >paramMap;
	paramMap = event->retrieveParameterDetails();

	event->setHashMap(paramMap);
	paramMap.clear();
	/* push the  event in to the list */
	listEvent.push_back(event);

    }
    if (pQr) {
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }


    return listEvent;
}

/**
 * @param pErrorCode Reference to error code 
 * @return vector <CTodo*> detailed list of tasks 
 *
 * Function to retrieve list of todos per Calendar 
 */
vector <CTodo*> CCalendar::getTodos(int & pErrorCode )
{



    CTodo *todo = 0;
    CAlarm *pAlarm = 0;
    vector <CTodo*> listTodo;
    QueryResult *pQr = 0;
    int iI_TodoCount = 0;
    int iJ_TodoCount = 0;
    int iK_TodoCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CUtility *pUt = 0;
    pUt = CUtility::Instance();
    vector <long> vCookie;

    if (pDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
	pErrorCode = CALENDAR_APP_ERROR;
	return listTodo;
    }

    pQuery = sqlite3_mprintf(SELECT_INNER_JOIN_BATCH,this->getCalendarId(),E_TODO);
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0) {
	return listTodo;
    }

    for (iI_TodoCount = 0; iI_TodoCount < pQr->iRow; iI_TodoCount++) {
	todo = new CTodo();
	pAlarm = new CAlarm();
	for (iJ_TodoCount = 0; iJ_TodoCount < pQr->iColumn; iJ_TodoCount++) {
	    iK_TodoCount = pQr->iColumn + (iI_TodoCount * pQr->iColumn);
	    if ((pQr->pResult[iK_TodoCount + iJ_TodoCount]) == 0)
		continue;
	    switch (iJ_TodoCount) {

		case DB_COLUMN_ID1:
		    todo->setId(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		    break;
		case DB_COLUMN_ID2:
		    //todo->setCalendarId(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID3:
		    todo->setType(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID4:
		    todo->
			setFlags(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID5:
		    todo-> setDateStart(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID6:
		    todo-> setDateEnd(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID7:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
			todo->setSummary(pQr->
				pResult[iK_TodoCount + iJ_TodoCount]);
		    }
		    break;

		case DB_COLUMN_ID8:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
			todo->setLocation(pQr->
				pResult[iK_TodoCount + iJ_TodoCount]);
		    }
		    break;

		case DB_COLUMN_ID9:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
			todo->setDescription(pQr->
				pResult[iK_TodoCount + iJ_TodoCount]);
		    }
		    break;

		case DB_COLUMN_ID10:
		    todo->
			setStatus(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID11:

		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
			todo->setGUid(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		    }
		    break;
		case DB_COLUMN_ID12:
		    todo->
			setUntil(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID13:
		    todo->
			setAllDay(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;
		case DB_COLUMN_ID14:
		    todo-> setCreatedTime(atoi
			    (pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		    break;
		case DB_COLUMN_ID15:

		    todo-> setLastModified(atoi
			    (pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID16:
		    todo->setTzid(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		    break;
		/* DB_COLUMN 17 is tzoffset */
	    	    
		    /* no need to fill id again here */
		case DB_COLUMN_ID17:
		case DB_COLUMN_ID18:
		case DB_COLUMN_ID19:
		    /* no need to put type here as it is already filled in
		     * column3*/
		    break;

		case DB_COLUMN_ID20:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount] != 0) {
			todo->setClas(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		    }
		    break;


		case DB_COLUMN_ID21:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
			todo->setGeo(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		    }
		    break;


		case DB_COLUMN_ID22:
		    todo-> setPriority(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID23:
		    todo-> setDateStamp(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID24:
		    todo-> setSequence(atoi
			    (pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID25:
		    break;

		case DB_COLUMN_ID26:
		    todo->setUid(atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID27:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
			todo->setUrl((pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    }
		    break;

		case DB_COLUMN_ID28:
		    //  todo->setAttachments (pQr->pResult[iK+iJ]);
		    break;

		case DB_COLUMN_ID29:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			todo->setCategories(pQr->
				pResult[iK_TodoCount + iJ_TodoCount]);
		    break;

		case DB_COLUMN_ID30:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			todo->setComments(pQr->
				pResult[iK_TodoCount + iJ_TodoCount]);
		    break;

		case DB_COLUMN_ID31:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			todo->setContact(pQr-> pResult[iK_TodoCount + iJ_TodoCount]);
		    break;

		case DB_COLUMN_ID32:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			todo->setRelated(pQr->pResult[iK_TodoCount + iJ_TodoCount]);
		    break;

		case DB_COLUMN_ID33:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			todo->setResources(pQr-> pResult[iK_TodoCount + iJ_TodoCount]);
		    break;

		case DB_COLUMN_ID34:
		    todo-> setPercentComplete(atoi
			    (pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID35:
		    todo->setCompleted(
			    atoi(pQr->pResult[iK_TodoCount + iJ_TodoCount]));
		    break;
		case DB_COLUMN_ID37:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			pAlarm->setTrigger(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID38:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			pAlarm->setRepeat(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID39:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			pAlarm->setDuration(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID40:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			pAlarm->setAction(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
		    break;

		case DB_COLUMN_ID41:

		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount]) {
			vCookie.push_back(atoi(pQr-> pResult[iK_TodoCount + iJ_TodoCount]));
			pAlarm->setCookie(vCookie);
		    }
		    break;

		case DB_COLUMN_ID42:
		    if (pQr->pResult[iK_TodoCount + iJ_TodoCount])
			pAlarm->setAttach(pQr-> pResult[iK_TodoCount + iJ_TodoCount]);
		    break;
		default:
		    break;
	    }
	}
	CAL_DEBUG_LOG("CCalendar %d:getTodos-> %s \n",
		iI_TodoCount,todo->toString().c_str());
	if (todo->getFlags() == HAS_ALARM)
	    todo->setAlarm(pAlarm);
	delete pAlarm;
	pAlarm = 0;


	/*retrieve xprop */
	vector < CProperties * >vPropList;

	vPropList = todo->retrieveXPropertyDetails();
	todo->setXProperties(vPropList);
	pUt->releasePropertiesVector(vPropList);

	/*retrieve params */
	map < string, vector < CParameters * > >paramMap;
	paramMap = todo->retrieveParameterDetails();
	todo->setHashMap(paramMap);
	paramMap.clear();

	listTodo.push_back(todo);
    }

    sqlite3_free_table(pQr->pResult);
    if (pQr) {
	delete pQr;
	pQr = 0;
    }

    return listTodo;

}
/**
 * @param pErrorCode Reference to error code 
 * @return vector <CJouranl*> detailed list of events 
 *
 * Function to retrieve list of journals per Calendar 
 */
vector <CJournal*> CCalendar::getJournals(int & pErrorCode )
{

    vector <CJournal*> listJournal;
    CJournal *pJournal = 0;
    QueryResult *pQr = 0;
    int iI_JourCount = 0;
    int iJ_JourCount = 0;
    int iK_JourCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    CUtility *pUt = 0;
    pUt = CUtility::Instance();

    
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if ((pDb == 0) || (pUt == 0)) {
      CAL_DEBUG_LOG("Calendar:Invalid CalendarDB or CUtility pointer");
      pErrorCode = CALENDAR_APP_ERROR;
      return listJournal;
    }

    pQuery = sqlite3_mprintf(SELECT_INNER_JOIN_BATCH,this->getCalendarId(),E_JOURNAL);
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    CAL_DEBUG_LOG(" query in getJournal is : %s", pQuery);


    if (pQr == 0) {
	return listJournal;
    }

    for (iI_JourCount = 0; iI_JourCount < pQr->iRow; iI_JourCount++) {
    pJournal = new CJournal();
    ASSERTION(pJournal);
    for (iJ_JourCount = 0; iJ_JourCount < pQr->iColumn; iJ_JourCount++) {
	iK_JourCount = pQr->iColumn + (iI_JourCount * pQr->iColumn);
	    if ((pQr->pResult[iK_JourCount + iJ_JourCount]) == 0)
	    continue;
	switch (iJ_JourCount) {

	    case DB_COLUMN_ID1:
		pJournal->setId(pQr->pResult[iK_JourCount + iJ_JourCount]);
		break;
	    case DB_COLUMN_ID2:
		//pJournal->setCalendarId(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID3:
		pJournal->
		    setType(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID4:
		pJournal->
		    setFlags(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID5:
		pJournal-> setDateStart(atoi
			    (pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID6:
		pJournal-> setDateEnd(atoi
			    (pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID7:
		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->setSummary(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		}
		break;

	    case DB_COLUMN_ID8:
		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->setLocation(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		}
		break;

	    case DB_COLUMN_ID9:
		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->setDescription(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		}
		break;

	    case DB_COLUMN_ID10:
		pJournal->
		    setStatus(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;
		/*FOR RECURRENCE */

	    case DB_COLUMN_ID11:

		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->setGUid(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		}
		break;
	    case DB_COLUMN_ID12:
		pJournal-> setUntil(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

		/*FOR RECURRENCE */
	    /*ALLDAY*/ 
	    case DB_COLUMN_ID13:
		pJournal->
		    setAllDay(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;
	    case DB_COLUMN_ID14:
		pJournal-> setCreatedTime(atoi (pQr->
			     pResult[iK_JourCount + iJ_JourCount]));
		break;
	    case DB_COLUMN_ID15:

		pJournal-> setLastModified(atoi (pQr->
			     pResult[iK_JourCount + iJ_JourCount]));
		break;
	    
	    case DB_COLUMN_ID16:
		pJournal->setTzid(pQr->pResult[iK_JourCount + iJ_JourCount]);
		break;
		
	    /* DB_COLUMN 17 is the tzoffset and 18 is id  
	     * 19 is type   no need to fill these*/		
	    case DB_COLUMN_ID17:
	    case DB_COLUMN_ID18:
	    case DB_COLUMN_ID19:
		break;

	    case DB_COLUMN_ID20:
		if (pQr->pResult[iK_JourCount + iJ_JourCount] != 0) {
		    pJournal->setClas(pQr-> pResult[iK_JourCount + iJ_JourCount]);
		}
		break;


	    case DB_COLUMN_ID21:
	    case DB_COLUMN_ID22:
		break;

	    case DB_COLUMN_ID23:
		pJournal->
		    setDateStamp(atoi
			    (pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID24:
		pJournal-> setSequence(atoi
			    (pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID25:
		break;

	    case DB_COLUMN_ID26:
		pJournal->
		    setUid(atoi(pQr->pResult[iK_JourCount + iJ_JourCount]));
		break;

	    case DB_COLUMN_ID27:
		if (pQr->pResult[iK_JourCount + iJ_JourCount]) {
		    pJournal->
			setUrl((pQr->pResult[iK_JourCount + iJ_JourCount]));
		}
		break;

	    case DB_COLUMN_ID28:
		//       journal->setAttachments (pQr->pResult[iK+iJ]);
		break;

	    case DB_COLUMN_ID29:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setCategories(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID30:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setComments(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID31:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setContact(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID32:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setRelated(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    case DB_COLUMN_ID33:
		if (pQr->pResult[iK_JourCount + iJ_JourCount])
		    pJournal->setResources(pQr->
			    pResult[iK_JourCount + iJ_JourCount]);
		break;

	    default:
		break;
	}
	


    }
    CAL_DEBUG_LOG("CCalendar %d::getJournals-> %s \n",
			iI_JourCount,pJournal->toString().c_str());

    /*retrieve xprop */
    vector < CProperties * >vPropList;
    vPropList = pJournal->retrieveXPropertyDetails();
    pJournal->setXProperties(vPropList);
    pUt->releasePropertiesVector(vPropList);

    /*retrieve params */
    map < string, vector < CParameters * > >paramMap;
    paramMap = pJournal->retrieveParameterDetails();
    pJournal->setHashMap(paramMap);
    paramMap.clear();

    listJournal.push_back(pJournal);
    }

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    
    return listJournal;
}

string CCalendar::toString()
{
    string szRet;
    string szTemp;

    std::stringstream ss;
    ss << "Name=";
    if (getCalendarName().c_str())
	ss << getCalendarName();
    else
	ss << NULL_STRING;
    ss << ",ReadOnly=";
    ss << IsReadOnly();
    ss << ",Visible=";
    ss << IsShown() ;
    ss << ",ID=";
    ss << getCalendarId() ;
    ss << ",Tune=";
    if (szCalendarTune.c_str()){
	szTemp= szCalendarTune.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;
    ss << ",Version=";
    if (szCalendarVersion.c_str()){
	szTemp= szCalendarVersion.substr(0,100);
	ss << szTemp;
	szTemp.clear();
    }

    else
	ss << NULL_STRING;
    ss << ",Color=";
    ss << getCalendarColor();
    ss << ",Sync=";
    ss << getProfileType();
    szRet.append(ss.str());


    return szRet;
}





