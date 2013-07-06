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

/* Header files included in this file */

#include <sstream>
#include <fstream>
#include "Common.h"
#include "CBdayEvent.h"
#include "ICalConverter.h"
#include "CalendarErrors.h"
#include "CUtility.h"
#include "CCalendarProcs.h"
#include <clockd/libtime.h>
#include <ctime>
extern "C" {
#include <sys/vfs.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
}
extern int errno ;
//#include "CUtility.h"
/*globals used to indicate the 
 * type and count of actions performed like 
 * add/modify/delete some x number of events /todos/journals 
 */
int enable_logging = 0;
int changeCount = 0;
int changeFlag = 0;
std::list<std::string> compIdsAdded;
std::list<std::string> compIdsDeleted;
std::list<std::string> compIdsModified;

int CalId = 0;
static const char *MSG_SEPERATOR = ":";
static const int NONE = 0;
static const int EVENT_ADDED = 1;
static const int EVENT_MODIFIED = 1 << 1;
static const int EVENT_DELETED = 1 << 2;

static const int TODO_ADDED = 1 << 3;
static const int TODO_MODIFIED = 1 << 4;
static const int TODO_DELETED = 1 << 5;

static const int JOURNAL_ADDED = 1 << 6;
static const int JOURNAL_MODIFIED = 1 << 7;
static const int JOURNAL_DELETED = 1 << 8;

static const int CALENDAR_ADDED = 1 << 9;
static const int CALENDAR_MODIFIED = 1 << 10;
static const int CALENDAR_DELETED = 1 << 11;

static const int BIRTHDAY_ADDED = 1 <<12;
static const int BIRTHDAY_MODIFIED = 1 <<13;
static const int BIRTHDAY_DELETED = 1 << 14;

static const int CALENDAR_CLEARED = 1 << 15;


static const char *NONE_MSG = ":NONE:NONE:";
static const char *E_ADDED = ":EVENT:ADDED:";
static const char *E_MODIFIED = ":EVENT:MODIFIED:";
static const char *E_DELETED = ":EVENT:DELETED:";
static const char *T_ADDED = ":TODO:ADDED:";
static const char *T_MODIFIED = ":TODO:MODIFIED:";
static const char *T_DELETED = ":TODO:DELETED:";
static const char *J_ADDED = ":JOURNAL:ADDED:";
static const char *J_MODIFIED = ":JOURNAL:MODIFIED:";
static const char *J_DELETED = ":JOURNAL:DELETED:";
static const char *C_ADDED = ":CALENDAR:ADDED:";
static const char *C_MODIFIED = ":CALENDAR:MODIFIED:";
static const char *C_DELETED = ":CALENDAR:DELETED:";
static const char *C_CLEARED = ":CALENDAR:CLEARED:";

static const char *BDAY_ADDED = ":BDAY:ADDED:";
static const char *BDAY_MODIFIED = ":BDAY:MODIFIED:";
static const char *BDAY_DELETED = ":BDAY:DELETED:";
static const char *COMP_MODIFIED = ":COMPONENT:MODIFIED:";
static const int IDMAP_LENGTH = 4;
static const int YEAR_DAYS = 365;

static const int DEFAULT_START_DATE = 1;
static const int DEFAULT_END_DATE = 2145830400;
static const int DEFAULT_LIMIT = 10000000;
static const int DEFAULT_OFFSET = 1;

static const int ICS_TEXT_FIELD_LIMIT = 16*1024; // 16k

unsigned int iFileOffset = 0;
static const int FETCH_SIMPLE_EVENTS_AND_TASKS = 1;
static const int FETCH_REPEATING_EVENTS =2;
CMulticalendar* CMulticalendar::pMc = 0;
std::string CMulticalendar::szSystemTimezone;

// the name below was removed from UI spec
// however, it is still used in calendardb, until the user will change it
// now it will used as flag that default sync calendar was not renamed to avoid to use gconf to determine it
static const char * DEFAULT_NAME_OF_DEFAULT_SYNC_CALENDAR = "cal_ti_calendar_synced";

static void conpIdListToString(const std::list<std::string> &list, std::string &str)
{
    std::stringstream s;

    for (std::list<std::string>::const_iterator iter = list.begin();
         iter != list.end();
         ++iter)
    {
        s << *iter << ',';
    }

    str = s.str();
}

/**
 * @param : None
 * @return : string name of the application
 *
 * This function sets the given name as the application name.
 */
string CMulticalendar::getApplicationName()
{
    return AppName;
}

/**
 * @param: string name of the application
 * @return: void
 *
 * This function sets the given name as the application name.
 */
void CMulticalendar::setApplicationName(string ApplicationName)
{
    if (AppName.empty())
	AppName.append(ApplicationName);
}

/**
* @param pCalendar pointer to calendar to set name to
* @param szCalendarName calendar name read from database
* @return none
* 
* Function to set the name for calendar.
* If it is default synced calendar the name shall be the same with product name if was not changed.
* Otherwise it shall be name read from database
*/
void CMulticalendar::setCalendarName(CCalendar* pCalendar, string szCalendarName) const
{
    if(pCalendar)
    {
        if(szCalendarName == DEFAULT_NAME_OF_DEFAULT_SYNC_CALENDAR)
        {
            const char *productName = getenv("OSSO_PRODUCT_NAME");
            if(productName)
            {
                pCalendar->setCalendarName(productName);
            }
            else
            {
                pCalendar->setCalendarName("Device main calendar");
            }
        }
        else
        {
            pCalendar->setCalendarName(szCalendarName);
        }
    }
}

/**
 * @param: NONE 
 * @return: vector of CCalendar* 
 * 
 * Function to return list of calendars from Database.  
 * Should be freed by API user, using releaseListCalendars() function.
 * IMPORTANT: This is a thread safe function.
 * 
 */
vector < CCalendar * >CMulticalendar::getListCalFromMc()
{
    QueryResult *pQr = 0;
    CCalendar *pCal = 0;
    int iI_CalCount = 0;
    int iJ_CalCount = 0;
    int iK_CalCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    
    vector < CCalendar * >vListCal;

    pDb = CCalendarDB::Instance();

    if (pDb == 0)
	return vListCal;

    pQuery = sqlite3_mprintf(SELECT_TAB, CALENDARS_TABLE);

    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    sqlite3_free(pQuery);

    if (pQr == 0)
	return vListCal;

    for (iI_CalCount = 0; iI_CalCount < pQr->iRow; iI_CalCount++) {
	/* Memory allocated here 
	 * has to be freed by application 
	 * calling this API*/
	pCal = new CCalendar();
	ASSERTION(pCal);
	
	for (iJ_CalCount = 0; iJ_CalCount < pQr->iColumn; iJ_CalCount++) {
	    iK_CalCount = pQr->iColumn + (iI_CalCount * pQr->iColumn);
	    switch (iJ_CalCount) {
		case DB_COLUMN_ID1:
		    pCal->setCalendarId
			(atoi(pQr->pResult[iK_CalCount + iJ_CalCount]));
		    break;

		case DB_COLUMN_ID2:
		    if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
			setCalendarName(pCal, pQr->pResult[iK_CalCount + iJ_CalCount]);
		    }
		    break;

		case DB_COLUMN_ID3:
		    if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
			pCal->setCalendarColor((CalendarColour)
				atoi(pQr->
				    pResult[iK_CalCount + iJ_CalCount]));

		    }
		    break;

		case DB_COLUMN_ID4:
		    pCal-> setCalendarShown(atoi
				(pQr-> pResult[iK_CalCount + iJ_CalCount]));
		    break;

		case DB_COLUMN_ID5:
		    pCal-> setCalendarReadOnly(atoi (pQr->
				 pResult[iK_CalCount + iJ_CalCount]));
		    break;


		case DB_COLUMN_ID6:
		    pCal->setCalendarType((CalendarType) atoi(pQr->
				pResult[iK_CalCount + iJ_CalCount]));
		    break;

		case DB_COLUMN_ID7:
		    if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
			pCal->setCalendarTune(pQr->
				pResult[iK_CalCount + iJ_CalCount]);
		    }
		    break;
		case DB_COLUMN_ID8:
		    if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
			pCal->setCalendarVersion(pQr->
				pResult[iK_CalCount + iJ_CalCount]);
		    }
		    break;
		case DB_COLUMN_ID9:
		    if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
			pCal->setProfileType((SyncProfileType) atoi(pQr->
				    pResult[iK_CalCount + iJ_CalCount]));
		    }
		    break;


		default:
		    break;
	    }


	}
	    CAL_DEBUG_LOG("CMulticalendar %d:getListCalFromMc-> %s\n",
			    iI_CalCount,pCal->toString().c_str());
	vListCal.push_back((CCalendar *) pCal);
    }
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return vListCal;
}

/**
 * @param: Vector of CCalendar*
 * @return: NONE
 * 
 * Function to release a list of calendars.
 */
void CMulticalendar::releaseListCalendars(vector<CCalendar*> Calendars)
{
    vector < CCalendar * >::iterator iIter;

    for (iIter = Calendars.begin(); iIter < Calendars.end(); iIter++) {
	delete(*iIter);
	(*iIter) = 0;
    }

    Calendars.clear();
}

/**
 * @param: int calendar id 
 * @param: int type of component 
 * @param: int * pointer to ErrorCode 
 * @return: int number of instances of the particular type in DB 
 * 
 * This is common function called from getEventCount ,getTodoCount and 
 * getNoteCount to get the count for each type.
 */
int CMulticalendar::getCount(int iCalId, int iType, int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();

    if (pDb == 0)
	return 0;

    if (iType == E_TODO) {

	pQuery =
        sqlite3_mprintf(SELECT_TAB_COND_TODO, COMPONENTS_TABLE,
                ENTRY_FIELD_CALID, iCalId, ENTRY_FIELD_TYPE,
                iType, ENTRY_FIELD_STATUS, false);

    } 
    else
	pQuery =
        sqlite3_mprintf(SELECT_TAB_COND_AND, COMPONENTS_TABLE,
                ENTRY_FIELD_CALID, iCalId, ENTRY_FIELD_TYPE,
                iType);
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CAL_DEBUG_LOG("pQuery is %s", pQuery);
    sqlite3_free(pQuery);
    if (pQr == 0) {
	  CAL_DEBUG_LOG(" Error in getting Count Returnig from here :-(");
          return 0;
    }
    int temp = 0;
    temp = pQr->iRow;
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;

    return temp;
}

/**
 * @param none 
 * @return string  CTC capabilities in XML
 *
 * Function to get the parameters supported by calendar in XML format 
 */
string CMulticalendar::getCTCapXML()
{
    string CtcapXml = "<CTCap>\
       <PropName>BEGIN</PropName>\
       <ValEnum>VCALENDAR</ValEnum>\
       <PropName>TZ</PropName>\
       <PropName>DAYLIGHT</PropName>\
       <ValEnum>VEVENT</ValEnum>\
       <ValEnum>VTODO</ValEnum>\
       <ValEnum>VJOURNAL</ValEnum>\
       <PropName>END</PropName>\
       <ValEnum>VCALENDAR</ValEnum>\
       <ValEnum>VEVENT</ValEnum>\
       <ValEnum>VTODO</ValEnum>\
       <ValEnum>VJOURNAL</ValEnum>\
       <PropName>VERSION</PropName>\
       <ValEnum>1.0</ValEnum>\
       <PropName>UID</PropName>\
       <PropName>SUMMARY</PropName>\
       <PropName>DESCRIPTION</PropName>\
       <PropName>DTEND</PropName>\
       <PropName>DTSTART</PropName>\
       <PropName>ALARM</PropName>\
       <PropName>CLASS</PropName>\
       <ValEnum>PUBLIC</ValEnum>\
       <ValEnum>PRIVATE</ValEnum>\
       <ValEnum>CONFIDENTIAL</ValEnum>\
       <PropName>CATEGORIES</PropName>\
       <PropName>TRANSP</PropName>\
       <ValEnum>OPAQUE</ValEnum>\
       <ValEnum>TRANSPARENT</ValEnum>\
       <PropName>LOCATION</PropName>\
       <PropName>DCREATED</PropName>\
       <PropName>LAST-MODIFIED</PropName>\
       <PropName>PRIORITY</PropName>\
       <PropName>STATUS</PropName>\
       <ValEnum>TENTATIVE</ValEnum>\
       <ValEnum>CONFIRMED</ValEnum>\
       <ValEnum>NEEDS-ACTION</ValEnum>\
       <ValEnum>COMPLETED</ValEnum>\
       <ValEnum>IN-PROCESS</ValEnum>\
       <ValEnum>CANCELLED</ValEnum>\
       <ValEnum>DRAFT</ValEnum>\
       <ValEnum>FINAL</ValEnum>\
       <PropName>RRULE</PropName>\
       <PropName>DUE</PropName>\
       <PropName>ATTACH</PropName>\
       <PropName>EXDATE</PropName>\
       <PropName>EXRULE</PropName>\
       <PropName>ATTENDEE</PropName>\
       <PropName>CATEGORIES</PropName>\
       <PropName>COMMENT</PropName>\
       <PropName>COMPLETED</PropName>\
       <PropName>CONTACT</PropName>\
       <PropName>GEO</PropName>\
       <PropName>ORGANIZER</PropName>\
       <PropName>PERCENT-COMPLETE</PropName>\
       <PropName>PRIORITY</PropName>\
       <PropName>RECURRENCE-ID</PropName>\
       <PropName>RDATE</PropName>\
       <PropName>SEQUENCE</PropName>\
       <PropName>URL</PropName>\
       </CTCap>\
       ";
    return CtcapXml;
}

/**
 * @param: iCalid Calendar id 
 * @param: int * pointer to ErrorCode 
 * @return: integer event count
 *  
 * Function used to get the number of Bday events for particular calendar in DB. 
 */
int CMulticalendar::getBdayEventCount(int iCalId, int &pErrorCode)
{
    return (this->getCount(iCalId, E_BDAY, pErrorCode));
}

/**
 * @param: iCalid Calendar id 
 * @param: int * pointer to ErrorCode 
 * @return: integer event count
 * 
 * Function used to get the number of events for particular calendar in DB. 
 */
int CMulticalendar::getEventCount(int iCalId, int &pErrorCode)
{
    return (this->getCount(iCalId, EVENT, pErrorCode));
}


/**
 * @param: iCalid 
 * @param: int * pointer to ErrorCode 
 * @return: integer todo count
 * 
 * Function used to get the number of Todos for particular calenadar in DB.
 */
int CMulticalendar::getTodoCount(int iCalId, int &pErrorCode)
{
    return (this->getCount(iCalId, TODO, pErrorCode));

}

/**
 * @param: ICalid 
 * @param: int * pointer to ErrorCode 
 * @return: integer journal count
 * 
 * Function used to get the number of journals for particular calenadar in DB .
 */
int CMulticalendar::getNoteCount(int iCalId, int &pErrorCode)
{
    return (this->getCount(iCalId, JOURNAL, pErrorCode));

}

/**
 * getNoofCalendars ()
 * @param: NONE
 * @return: int Number of calendars in database
 *
 * This function is called to get number of calendars from DB.
 */
int CMulticalendar::getNoofCalendars()
{
    vector < CCalendar * >vListCal;
    vector < CCalendar * >::iterator iIter;
    int ret = 0;
    
    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();
    ret = vListCal.size();
            
    /* Release the list of calendars. */
    releaseListCalendars(vListCal);

    return ret;
}

/**
 * @param: rrule     recurrencerule 
 * @param: dtstart     datestart in Icaltime format.
 * @param: viewEnd    end range of current claendar view 
 * @return: true or false
 *
 * Function to parse the time of occurrances from recurrence rules.
 */
bool CMulticalendar::checkRecurrentTimes(string szRRule,
                            icaltimetype iDtStart,
                            time_t iViewEnd,
                            icaltimezone *pTz,
                            int &pErrorCode)
{
    return checkRecurrentTimes(szRRule, iDtStart, 0, 0, iViewEnd, pTz, pErrorCode);
}

/**
 * @param: rrule     recurrencerule 
 * @param: dtstart     datestart in Icaltime format.
 * @param: viewEnd    end range of current claendar view 
 * @return: true or false
 *
 * Function to parse the time of occurrances from recurrence rules.
 */
bool CMulticalendar::checkRecurrentTimes(string szRRule,
                                        icaltimetype iDtStart,
                                        int iEventDuration,
                                        time_t iViewBegin,
                                        time_t iViewEnd,
                                        icaltimezone *pTz,
                                        int &pErrorCode)
{
    vector<time_t> times;
    getRecurrentTimes(szRRule, iDtStart, iEventDuration, iViewBegin, iViewEnd, pTz, times, pErrorCode);

    return times.size() != 0;
}

/**
 * @param: rrule     recurrencerule 
 * @param: dtstart     datestart in Icaltime format.
 * @param: viewEnd    end range of current claendar view 
 * @return: vector of recurrent times 
 *
 * Function to parse the time of occurrances from recurrence rules.
 */
vector < time_t > CMulticalendar::getRecurrentTimes(string szRRule,
                            icaltimetype iDtStart,
                            time_t iViewEnd,
                            icaltimezone *pTz,
                            int &pErrorCode,
                            int limit)
{
    CAL_ERROR_LOG("DEPRICATED");

    vector < time_t > retval;
    getRecurrentTimes(szRRule, iDtStart, 0, 0, iViewEnd, pTz, retval, pErrorCode, limit);

    return retval;
}

/**
 * @param: rrule     recurrencerule 
 * @param: dtstart     datestart in Icaltime format.
 * @param: iEventDuration duration of original event
 * @param: iViewBegin  start range of current claendar view 
 * @param: viewEnd    end range of current claendar view 
 * @param: outTimes   generated event times
 * @param: pErrorCode pointer to ErrorCode 
 * @param: limit      will return if this number of instances has found
 * @return: bool result of operation 
 *
 * Function to parse the time of occurrances from recurrence rules.
 */
bool CMulticalendar::getRecurrentTimes(string szRRule,
                            icaltimetype iDtStart,
                            int iEventDuration,
                            time_t iViewBegin,
                            time_t iViewEnd,
                            icaltimezone *pTz,
                            vector <time_t> &outTimes,
                            int &pErrorCode,
                            int limit)
{
    CAL_DEBUG_LOG("\n CMulticalendar::getRecurrentTimes(%s, ..., %d) \n", szRRule.c_str(), limit);
    CAL_DEBUG_LOG("event start - %d-%d-%d %d:%d", 
                   iDtStart.day,
                   iDtStart.month,
                   iDtStart.year,
                   iDtStart.hour,
                   iDtStart.minute);

    CAL_DEBUG_LOG("view start - %s", ctime(&iViewBegin));
    CAL_DEBUG_LOG("view end   - %s", ctime(&iViewEnd));

    struct icalrecurrencetype recur;
    time_t utc_tim = 0;
    bool loopexit = false;


    /* find the recurrent rules and then for each RRULE if it has until 
     * then substitute the value after T to consider that days event also 
     * suppose the rule is RULE=FREQ=DAILY; UNTIL=20090304T000000 then replace
     * the six zeros in the end with 245959.
     * This has been done because pc suite always sends the day with until 
     * time as XXXXT 0000000 libical misses the event because the time mentioned 
     * is incorrect 
     */
    //icaltimezone* pTz = 0;
    //icaltimezone* pTz = CMulticalendar::getSystemTimeZoneAsIcalTimeZone();
    unsigned int pos = 0;
    pos = szRRule.find(UNTIL_ICAL,0);
    if(pos != string::npos)
    {
	/* Find the six zeros and then substitute them with 
	 * 245959  on the fly */
	pos = szRRule.find(T_ICAL,pos+SIX);
	if(pos != string::npos)      
	    szRRule.replace(pos+1,SIX,DAY_ICAL);
    }
    recur = icalrecurrencetype_from_string(szRRule.c_str());
    if (!strstr(szRRule.c_str(),"WKST"))
        recur.week_start = ICAL_SUNDAY_WEEKDAY;

    icalrecur_iterator *ritr = 0;
    ritr = icalrecur_iterator_new(recur, iDtStart);
    struct icaltimetype next;

    if(ritr) {
        next = icalrecur_iterator_next(ritr);

        while ((!icaltime_is_null_time(next)) && (!loopexit) && limit--) {
	       if(pTz)
	           utc_tim = icaltime_as_timet_with_zone(next,pTz);
           else
	           utc_tim = icaltime_as_timet(next);

           // ignore instances before view range
           if ((utc_tim + iEventDuration) < iViewBegin) {
               // CAL_DEBUG_LOG ("Ignoire instance at %s", ctime(&utc_tim));
           }
           else if (utc_tim > iViewEnd) {
	           loopexit = true;
	           if (utc_tim < iViewEnd + ONEDAY) {
	               CAL_DEBUG_LOG ("instance time  = %s", ctime(&utc_tim));
	               outTimes.push_back(utc_tim);
	           }
	       }
	       else{
	           CAL_DEBUG_LOG ("instance time  = %s", ctime(&utc_tim));
	           outTimes.push_back(utc_tim);
	       }
	       next = icalrecur_iterator_next(ritr);
        }
    icalrecur_iterator_free(ritr);
    }
    icalerror_clear_errno();

    return !(outTimes.empty());
}
/**
 * @param string recurence rule 
 * @parma icaltimetype date start value in icaltimetype
 * @param time_t view end parameter
 * @param pErrorCode reference to error code 
 * @return return the particular instance time
 * 
 * Function to get the next instance time given rruke and 
 * date start 
 */

time_t CMulticalendar::getInstanceBeyond(string szRRule,
                     icaltimetype iDtStart,
                     time_t iViewEnd, 
		     icaltimezone* pTz,
		     int &pErrorCode)
{

    struct icalrecurrencetype recur;
    time_t utc_tim = 0;
    vector < time_t > listtime;
    bool loopexit = false;
    bool instanceIndicator = false;
    
    unsigned int pos = 0;
    pos = szRRule.find(UNTIL_ICAL,0);
    if(pos != string::npos)
    {
	/* Find the six zeros and then substitute them with 
	 * 245959  on the fly */
	pos = szRRule.find(T_ICAL,pos+SIX);
	if(pos != string::npos)      
	    szRRule.replace(pos+1,SIX,DAY_ICAL);
    }

    recur = icalrecurrencetype_from_string(szRRule.c_str());

    CAL_DEBUG_LOG("date is given by %d:%d:%d", iDtStart.year,
          iDtStart.month, iDtStart.day);
    CAL_DEBUG_LOG("iViewEnd %s", ctime(&iViewEnd));
    
    icalrecur_iterator *ritr = 0;
    ritr = icalrecur_iterator_new(recur, iDtStart);

    if(ritr){
        struct icaltimetype next;
        next = icalrecur_iterator_next(ritr);

        while ((!icaltime_is_null_time(next)) && (!loopexit)) {
	       utc_tim = icaltime_as_timet_with_zone(next, pTz);
    	   CAL_DEBUG_LOG("time %s", ctime(&utc_tim));

	       if (utc_tim >= iViewEnd) {
	           instanceIndicator = true;
	           loopexit = true;
	       }
	       next = icalrecur_iterator_next(ritr);
        }

        icalrecur_iterator_free(ritr);
    }
    if (instanceIndicator) {
    	CAL_DEBUG_LOG("instance time %s", ctime(&utc_tim));
	return utc_tim;
    }

    return 0;


}

/**
 * @param: calid Calendar ID
 * @param: type 
 *               1 -> EVENT
 *               2 -> TODO
 *               3 -> JOURNAL
 * @param: stDate to do conditional fetching based on start date.
 *          -1 in case of no conditions
 * @param: endDate to do conditional fetching based on end date. 
 *         -1 in case of no conditions
 * @param: int * pointer to ErrorCode 
 * @return: Vector for CComponent
 * 
 * This function is used to get all the entries associated with particular 
 * component type.
 * NOTE : API user has to free memory occupied by vector<CComponent*>
 */
vector < CComponent * >CMulticalendar::getComponents(int iCalId, int iType,
                             int iStDate,
                             int iEndDate,
                             int &pErrorCode)
{
    vector < CComponent * >vListEntry;

    CCalendar *pCal = 0;
    pCal = this->getCalendarById(iCalId, pErrorCode);
    if (pCal){
    vListEntry =
        pCal->getComponents(iType, iStDate, iEndDate, pErrorCode);
        delete pCal;
        pCal = 0;
    }
    else {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	CAL_DEBUG_LOG("Invalid Calendar ID\n");
    }


    return vListEntry;
}



/**
 * @param : stDate to do conditional fetching based on start date. -1 in 
 * case of no conditions
 * @param : endDate to do conditional fetching based on end date. -1 in 
 * case of no conditions
 * @param : limit Max limit of components to be obtanined by this function call
 * @param : offset from which we need to fetch 
 * @param : vector<int> vCalid reference to place holder for calenadr id for 
 * each entry
 * @param: int * pointer to ErrorCode 
 * @return : Vector for CComponent
 * 
 * This function is overloaded API for fetching entries for all calendars at a 
 * time progressively. It gives only the minimal data required for Calendar-ui 
 * in month view. 
 * P.S : Please donot use this API if you need complete data.  
 * 
 */
vector < CComponent * >CMulticalendar::getComponentsWithLimitedData (
		int iStDate,
		int iEndDate,
		int iLimit,
		int iOffset,
		vector <int> &vCalid,
		int &pErrorCode)
{
CAL_DEBUG_LOG("\n CMulticalendar::getComponentsWithLimitedData() \n");
	QueryResult *pQr = 0;
	CComponent *pEntry = 0;
	int iI_CompCount = 0;
	int iJ_CompCount = 0;
	int iK_CompCount = 0;
	char *pQuery = 0;
	vector < CComponent * >vListEntry;
	CCalendarDB *pDb = 0;
	int iSqliteError = 0;
	int  iCalid = 0 ;
	pDb = CCalendarDB::Instance();
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	if (pDb == 0) {
		pErrorCode = CALENDAR_APP_ERROR;
		return vListEntry;
	}


	/* Implementation in getComponents function will be similar for getting 
	 * events /todo / Bdays  (or) fetching all components other than query 
	 * part. So all the functialities are written together in a single 
	 * function for better code optimization
	 **/
	CAL_DEBUG_LOG("stDate is:%d\n", iStDate);
	CAL_DEBUG_LOG("endDate is: %d\n", iEndDate);
/**
#define  SELECT_ALL_CALCOMP__UNION "select * from %s where ((%s = %d  AND %s < %d AND %s >= %d) OR ( %s = %d AND %s >= %d AND %s <= %d )) AND  (%s != %d AND %s != %d) AND calendarId in (select calendarid from calendars where IsVisible =1 ) union select * from %s where (%s = %d OR %s=%d)  AND (%s = %d OR %s = %d) AND (%d < %s OR %d = %s) AND calendarId in (select calendarid from calendars where IsVisible =1)  ORDER BY CalendarId LIMIT %d OFFSET %d " 
**/

	pQuery =
		sqlite3_mprintf(SELECT_ALL_CALCOMP__UNION,COMPONENTS_TABLE, ENTRY_FIELD_TYPE, E_EVENT,
				ENTRY_FIELD_DTSTART, iEndDate, ENTRY_FIELD_DTEND,
				iStDate, ENTRY_FIELD_TYPE, E_TODO,ENTRY_FIELD_DTSTART, iStDate, ENTRY_FIELD_DTSTART,
				iEndDate, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,
				COMPONENTS_TABLE, ENTRY_FIELD_TYPE, E_EVENT, ENTRY_FIELD_TYPE,
				E_BDAY, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM, iStDate,
				ENTRY_FIELD_UNTIL, NULLID, ENTRY_FIELD_UNTIL,iLimit, iOffset);


	ASSERTION(pQuery);


	CAL_DEBUG_LOG("Query is %s\n", pQuery );
	pQr = pDb->getRecords(pQuery,iSqliteError);
	pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	sqlite3_free(pQuery);

	if (pQr == 0) {
		return vListEntry;
}

	for (iI_CompCount = 0; iI_CompCount < pQr->iRow; iI_CompCount++) {
		pEntry = new CComponent();
		ASSERTION(pEntry);
		for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
			iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

			if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
				continue;

			switch (iJ_CompCount) {
				case DB_COLUMN_ID2:
					iCalid = atoi(pQr->pResult[iK_CompCount + iJ_CompCount]);
                    pEntry->setCalendarId(iCalid);
					break;

				case DB_COLUMN_ID1:
					pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
					break;

				case DB_COLUMN_ID3:
					pEntry-> setType(atoi
							(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID4:
					pEntry-> setFlags(atoi
							(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID5:
					pEntry-> setDateStart(atoi
							(pQr-> pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID6:
					pEntry-> setDateEnd(atoi
							(pQr-> pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID7:
					pEntry->setSummary((string) pQr->
							pResult[iK_CompCount + iJ_CompCount]);
					break;

					/* location description status guid 
					 *  */
				case DB_COLUMN_ID8:
				case DB_COLUMN_ID9:
				case DB_COLUMN_ID10:
                    			pEntry-> setStatus(atoi
                            		 	(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;
				case DB_COLUMN_ID11:
					break;
				case DB_COLUMN_ID12:
					pEntry-> setUntil(atoi
							(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID13:
					pEntry-> setAllDay(atoi
							(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;
				case DB_COLUMN_ID14:
					/* created time  last modified */
				case DB_COLUMN_ID15:
					break;
					
				case DB_COLUMN_ID16:
					pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
					break;

				default:
					break;
			}




		}

		CAL_DEBUG_LOG("CMulticalendar %d:getComponentsAllCalendars-> %s\n",
				iI_CompCount,pEntry->toString().c_str());
		CAL_DEBUG_LOG("Event id beforeis %s", pEntry->getId().c_str());
		if (((pEntry->getFlags() == HAS_RECURRENCE)
					|| (pEntry->getFlags() == HAS_RECURRENCE_ALARM))) {
			CAL_DEBUG_LOG("Event id is %s", pEntry->getId().c_str());
			pEntry->getRecurrenceProperties();
		}
		
		/* no need to fetch alarm in month view */
		/*see if any instance is in the view range
		 *  if it is not present in view range then
		 *  remove it from the list*/
		if (((pEntry->getFlags() == HAS_RECURRENCE)
					|| (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
				&& (pEntry->getType() != E_JOURNAL)) {

			if (pEntry->getInstanceNumber(iStDate, iEndDate)) {
				vCalid.push_back(iCalid);
				vListEntry.push_back(pEntry);
			} 
			else {
				delete pEntry;
				pEntry = 0;
			}

		} 
		else{
			vCalid.push_back(iCalid);
			vListEntry.push_back(pEntry);
		}


	}

	CAL_DEBUG_LOG("size of list is %d", vListEntry.size());
	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
	return vListEntry;
}

/**
 * @param : stDate to do conditional fetching based on start date. -1 in 
 * case of no conditions
 * @param : endDate to do conditional fetching based on end date. -1 in 
 * case of no conditions
 * @param : limit Max limit of components to be obtanined by this function call
 * @param : offset from which we need to fetch 
 * @param : vector<int> vCalid reference to place holder for calenadr id for 
 * each entry
 * @param: int * pointer to ErrorCode:
 *          CALENDAR_OPERATION_SUCCESSFUL- fetching is successful and can be continued;
 *          CALENDAR_FETCH_NOITEMS- fetching is finished (no more items found in DB);
 *          other error code- fetching is failed.
 * @param : iQueryType - default parameter with value 0
 *          0- Fetches all Events + tasks
 *          1- Fetches non repeating events + tasks
 *          2-  Fetches repeating events alone in the given range.
 * @return : Vector for CComponent. If pErrorCode equals to CALENDAR_OPERATION_SUCCESSFUL,
 *           but vector is empty, no component (from offset to limit) meets the conditions,
 *           continue fetching
 * 
 * This function is overloaded API for fetching entries for all calendars at a 
 * time progressively 
 */
vector <CComponent *> CMulticalendar::getComponentsAllCalendars(
		int iStDate,
		int iEndDate,
		int iLimit,
		int iOffset,
		vector <int> &vCalid,
		int &pErrorCode,
		int iQueryType)
{
    CAL_ERROR_LOG("DEPRECATED");

    QueryResult *pQr = 0;
    CComponent *pEntry = 0;
    int iI_CompCount = 0;
    int iJ_CompCount = 0;
    int iK_CompCount = 0;
    char *pQuery = 0;
    vector < CComponent * >vListEntry;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    int  iCalid = 0 ;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0) {
        pErrorCode = CALENDAR_APP_ERROR;
        return vListEntry;
    }


    /* Implementation in getComponents function will be similar for getting 
     * events /todo / Bdays  (or) fetching all components other than query 
     * part. So all the functialities are written together in a single 
     * function for better code optimization
     **/
    CAL_DEBUG_LOG("stDate is:%d\n", iStDate);
    CAL_DEBUG_LOG("endDate is: %d\n", iEndDate);
/**
#define  SELECT_ALL_CALCOMP__UNION "select * from %s where ((%s = %d  AND %s < %d AND %s >= %d) OR ( %s = %d AND %s >= %d AND %s <= %d )) AND  (%s != %d AND %s != %d) AND calendarId in (select calendarid from calendars where IsVisible =1 ) union select * from %s where (%s = %d OR %s=%d)  AND (%s = %d OR %s = %d) AND (%d < %s OR %d = %s) AND calendarId in (select calendarid from calendars where IsVisible =1)  ORDER BY CalendarId LIMIT %d OFFSET %d " 
**/
    switch(iQueryType){
	    case FETCH_SIMPLE_EVENTS_AND_TASKS:
		pQuery =
		    sqlite3_mprintf(SELECT_ALL_CALCOMP__UNION_NORMAL,COMPONENTS_TABLE, ENTRY_FIELD_TYPE, E_EVENT,
			    ENTRY_FIELD_DTSTART, iEndDate, ENTRY_FIELD_DTEND,
			    iStDate, ENTRY_FIELD_TYPE, E_TODO,ENTRY_FIELD_DTSTART, iStDate, ENTRY_FIELD_DTSTART,
			    iEndDate, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,iLimit, iOffset);
		break;
	    case FETCH_REPEATING_EVENTS:
		pQuery = sqlite3_mprintf(SELECT_ALL_CALCOMP__UNION_RECURSIVE,COMPONENTS_TABLE, ENTRY_FIELD_TYPE, E_EVENT, ENTRY_FIELD_TYPE,
			E_BDAY, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM, iStDate,
			ENTRY_FIELD_UNTIL, NULLID, ENTRY_FIELD_UNTIL,iLimit, iOffset);
		break;
	    default:
		pQuery =
		    sqlite3_mprintf(SELECT_ALL_CALCOMP__UNION,COMPONENTS_TABLE, ENTRY_FIELD_TYPE, E_EVENT,
			    ENTRY_FIELD_DTSTART, iEndDate, ENTRY_FIELD_DTEND,
			    iStDate, ENTRY_FIELD_TYPE, E_TODO,ENTRY_FIELD_DTSTART, iStDate, ENTRY_FIELD_DTSTART,
			    iEndDate, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,
			    COMPONENTS_TABLE, ENTRY_FIELD_TYPE, E_EVENT, ENTRY_FIELD_TYPE,
			    E_BDAY, ENTRY_FIELD_FLAGS, HAS_RECURRENCE,ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM, iStDate,
			    ENTRY_FIELD_UNTIL, NULLID, ENTRY_FIELD_UNTIL,iLimit, iOffset);
		break;
	}


    ASSERTION(pQuery);


    CAL_DEBUG_LOG("Query is %s\n", pQuery );
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
        for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
            iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

            if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
                continue;

            switch (iJ_CompCount) {
                case DB_COLUMN_ID2:
                    iCalid = atoi(pQr->pResult[iK_CompCount + iJ_CompCount]);
                    pEntry->setCalendarId(iCalid);
					break;                    

                case DB_COLUMN_ID1:
                    pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
                    break;

                case DB_COLUMN_ID3:
                    pEntry-> setType(atoi
                            (pQr->pResult[iK_CompCount + iJ_CompCount]));
                    break;

                case DB_COLUMN_ID4:
                    pEntry-> setFlags(atoi
                            (pQr->pResult[iK_CompCount + iJ_CompCount]));
                    break;

                case DB_COLUMN_ID5:
                    pEntry-> setDateStart(atoi
                            (pQr-> pResult[iK_CompCount + iJ_CompCount]));
                    break;

                case DB_COLUMN_ID6:
                    pEntry-> setDateEnd(atoi
                            (pQr-> pResult[iK_CompCount + iJ_CompCount]));
                    break;

                case DB_COLUMN_ID7:
                    pEntry->setSummary((string) pQr->
                            pResult[iK_CompCount + iJ_CompCount]);
                    break;

                case DB_COLUMN_ID8:
                    pEntry->setLocation((string) pQr->
                            pResult[iK_CompCount + iJ_CompCount]);
                    break;

                case DB_COLUMN_ID9:
                    pEntry->setDescription((string) pQr->
                            pResult[iK_CompCount + iJ_CompCount]);
                    break;

                case DB_COLUMN_ID10:
                    pEntry-> setStatus(atoi
                            (pQr->pResult[iK_CompCount + iJ_CompCount]));
                    break;

                case DB_COLUMN_ID11:
                    pEntry->setGUid(pQr->
                            pResult[iK_CompCount + iJ_CompCount]);
                    break;
                case DB_COLUMN_ID12:
                    pEntry-> setUntil(atoi
                            (pQr->pResult[iK_CompCount + iJ_CompCount]));
                    break;

                case DB_COLUMN_ID13:
                    pEntry-> setAllDay(atoi
                            (pQr->pResult[iK_CompCount + iJ_CompCount]));
                    break;
                case DB_COLUMN_ID14:
                    pEntry-> setCreatedTime(atoi
                            (pQr-> pResult[iK_CompCount + iJ_CompCount]));
                    break;
                case DB_COLUMN_ID15:

                    pEntry-> setLastModified(atoi
                            (pQr-> pResult[iK_CompCount + iJ_CompCount]));
                    break;
                case DB_COLUMN_ID16:
                    pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
                    break;

                default:
                    break;
            }




        }

//        CAL_DEBUG_LOG("CMulticalendar %d:getComponentsAllCalendars-> %s\n",
//                iI_CompCount,pEntry->toString().c_str());
//        CAL_DEBUG_LOG("Event id beforeis %s", pEntry->getId().c_str());
        if (((pEntry->getFlags() == HAS_RECURRENCE)
                    || (pEntry->getFlags() == HAS_RECURRENCE_ALARM))) {
//            CAL_DEBUG_LOG("Event id is %s", pEntry->getId().c_str());
            pEntry->getRecurrenceProperties();
        }

        if (((pEntry->getFlags() == HAS_ALARM)
                    || (pEntry->getFlags() == HAS_RECURRENCE_ALARM)))
            pEntry->getAlarmProperties();


        /*see if any instance is in the view range
         *  if it is not present in view range then
         *  remove it from the list*/
        if (((pEntry->getFlags() == HAS_RECURRENCE)
                    || (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
                && (pEntry->getType() != E_JOURNAL)) {

            if (pEntry->getInstanceNumber(iStDate, iEndDate)!= 0) {
                vCalid.push_back(iCalid);
                vListEntry.push_back(pEntry);
            } 
            else {
                delete pEntry;
                pEntry = 0;
            }

        } 
        else{
            vCalid.push_back(iCalid);
            vListEntry.push_back(pEntry);
        }


    }

    CAL_DEBUG_LOG("size of list is %d", vListEntry.size());
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return vListEntry;
}

/**
 * getComponents ()
 * @param : calid Calendar ID
 * @param : type 
 *               1 -> EVENT
 *               2 -> TODO
 *               3 -> JOURNAL
 *               4 -> BDAY
 * @param : stDate to do conditional fetching based on start date. -1 in 
 * case of no conditions
 * @param : endDate to do conditional fetching based on end date. -1 in 
 * case of no conditions
 * @param : limit Max limit of components to be obtanined by this function call
 * @param : offset from which we need to fetch 
 * @param: int * pointer to ErrorCode 
 * @return : Vector for CComponent
 * 
 * This function is overloaded API for Progressive updation. 
 */
vector < CComponent * >CMulticalendar::getComponents(int iCalId, int iType,
                             int iStDate,
                             int iEndDate,
                             int iLimit,
                             int iOffset,
                             int &pErrorCode)
{
    vector < CComponent * >vListEntry;
    CCalendar *pCal = this->getCalendarById(iCalId, pErrorCode);

    if (pCal){
	vListEntry =
        pCal->getComponents(iType, iStDate, iEndDate, iLimit, iOffset,
                pErrorCode);
	delete pCal;
	pCal = 0;
    }
    else
	CAL_DEBUG_LOG("Invalid Calendar ID\n");


    return vListEntry;
}

/**
 * @param: calid Calendar id
 * @param: int * pointer reference to ErrorCode 
 * @return: CCalendar pointer
 * 
 * Function used to get the calendar object based on calendar id. 
 * This api returns CCalendar* from DB , so the Calendar pointer
 * thus obtained should be freed by the end user.
 */
CCalendar *CMulticalendar::getCalendarById(int iCalId, int &pErrorCode)
{
    QueryResult *pQr = 0;
    CCalendar *pCal = 0;
    int iI_CalCount = 0;
    int iJ_CalCount = 0;
    int iK_CalCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    pDb = CCalendarDB::Instance();


    if (pDb == 0)
	return pCal;

    pQuery = sqlite3_mprintf(SELECT_BY_ID, CALENDARS_TABLE,iCalId);

    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    sqlite3_free(pQuery);

    if (pQr == 0){
	CAL_ERROR_LOG("No calendar with the given Id found in DB");
    CAL_ERROR_LOG("SQL error was %d",iSqliteError);
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return pCal;
    }
    if (pQr->iRow > TWO_ROWS) {
	CAL_ERROR_LOG("Database corruption, two rows found with same id.");
	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
	return pCal;
    }
    pCal = new CCalendar();
    ASSERTION(pCal);

    for (iJ_CalCount = 0; iJ_CalCount < pQr->iColumn; iJ_CalCount++) {
	iK_CalCount = pQr->iColumn + (iI_CalCount * pQr->iColumn);
	switch (iJ_CalCount) {
	    case DB_COLUMN_ID1:
		pCal->setCalendarId
		    (atoi(pQr->pResult[iK_CalCount + iJ_CalCount]));
		break;

	    case DB_COLUMN_ID2:
		if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
		    setCalendarName(pCal, pQr->pResult[iK_CalCount + iJ_CalCount]);
		}
		break;

	    case DB_COLUMN_ID3:
		if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
		    pCal->setCalendarColor((CalendarColour)
			    atoi(pQr-> pResult[iK_CalCount + iJ_CalCount]));

		}
		break;

	    case DB_COLUMN_ID4:
		pCal-> setCalendarShown(atoi
			(pQr-> pResult[iK_CalCount + iJ_CalCount]));
		break;

	    case DB_COLUMN_ID5:
		pCal-> setCalendarReadOnly(atoi (pQr->
			    pResult[iK_CalCount + iJ_CalCount]));
		break;


	    case DB_COLUMN_ID6:
		pCal->setCalendarType((CalendarType) atoi(pQr->
			    pResult[iK_CalCount + iJ_CalCount]));
		break;

	    case DB_COLUMN_ID7:
		if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
		    pCal->setCalendarTune(pQr->
			    pResult[iK_CalCount + iJ_CalCount]);
		}
		break;
	    case DB_COLUMN_ID8:
		if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
		    pCal->setCalendarVersion(pQr->
			    pResult[iK_CalCount + iJ_CalCount]);
		}
		break;
	    case DB_COLUMN_ID9:
		if (pQr->pResult[iK_CalCount + iJ_CalCount]) {
		    pCal->setProfileType((SyncProfileType) atoi(pQr->
				pResult[iK_CalCount + iJ_CalCount]));
		}
		break;


	    default:
		break;
	}


    }
    CAL_DEBUG_LOG("CMulticalendar %d:getCalById-> %s\n",
	    iI_CalCount,pCal->toString().c_str());
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return pCal;

}

/**
 * @param: iCalType Calendar Type
 * @param: int & irefernce  to ErrorCode 
 * @return: CCalendar object
 *
 * Function used to get the calendar object based on calendar type.
 * CCalendar object thus obtained should be freed by the API user.
 */
CCalendar *CMulticalendar::getCalendarByType(int iCalType, int &pErrorCode)
{
    vector < CCalendar * >::iterator iter;
    vector < CCalendar * >vListCal;
    CCalendar* retCal = NULL;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();

    if (vListCal.size() == 0) {
      CAL_DEBUG_LOG("No Clendar present in DB\n");
      return retCal;
    }

    for (iter = vListCal.begin(); iter != vListCal.end(); iter++) {
      if ((*iter)->getCalendarType() == iCalType) {
          retCal = *iter;
      } 
      else {
          delete(*iter);
          (*iter) = 0;
      }
    }

    /* if no match found return Null */
    if (retCal == NULL) {
      pErrorCode = CALENDAR_DOESNOT_EXISTS;
      return 0;
    }
    
    return retCal;
}

/**
 * @param: string szName calendar Name 
 * @param: int& pErrorCode reference  to integer
 * @return: CCalendar* Calendar Pointer 
 *
 * Function used to retrieve CCalendar pointer from DB.
 * user of this API should free the calendar thus obtained. 
 */
CCalendar *CMulticalendar::getCalendarByName(string szName,
                         int &pErrorCode)
{
    vector < CCalendar * >::iterator iter;
    vector < CCalendar * >vListCal;
    CCalendar* retCal = NULL;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();

    if (vListCal.size() == 0) {
      CAL_DEBUG_LOG("No Clendar present in DB\n");
      return retCal;
    }

    for (iter = vListCal.begin(); iter != vListCal.end(); iter++) {
      if ((*iter)->getCalendarName() == szName) {
          retCal = *iter;
      } 
      else {
          delete(*iter);
          (*iter) = 0;
      }
    }

    /* if no match found return Null */
    if (retCal == NULL) {
      pErrorCode = CALENDAR_DOESNOT_EXISTS;
      return 0;
    }
    
    return retCal;
}


/**
 * @param: none
 * @return: int SUCCESS/FAILURE
 * 
 * Function to set the autocommit off in sqlite database.
 */
int CMulticalendar::setAutocommitOff()
{
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
        CAL_DEBUG_LOG("invalid CalendarDB pointer ");
        return FAILURE;
    }
    return pCalDb->setAutocommitOff();
}


/**
 * @param: string DBus message to be sent when commit happens
 * @return: bool TRUE/FALSE
 * 
 * Function to commit all the changes made into database.
 */
bool CMulticalendar::commitAllChanges()
{
    bool ret;
    string szMessage;
    string szIds;

    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
        CAL_DEBUG_LOG("invalid CalendarDB pointer ");
        return false;
    }

    //CUtility *pUt = CUtility::Instance();
    //ASSERTION(pUt);
    std::stringstream ss;
    szMessage.append(AppName);
    szMessage.append(MSG_SEPERATOR);
    //szMessage.append(CalName);
    ss << CalId;
    szMessage.append(ss.str());



    /* Append the calendar name and its action to the DBUS message */
    switch (changeFlag) {
        case CALENDAR_ADDED:
            szMessage.append(C_ADDED);
            break;

        case CALENDAR_MODIFIED:
            szMessage.append(C_MODIFIED);
            break;

        case CALENDAR_DELETED:
            szMessage.append(C_DELETED);
            break;

        case CALENDAR_CLEARED:
            szMessage.append(C_CLEARED);
            break;

        default:
            break;
    }

    /* Append the Components changes in the dbus message */
    switch (changeFlag) {
        case NONE:
            szMessage.append(NONE_MSG);
            break;

        case EVENT_ADDED:
            szMessage.append(E_ADDED);
            conpIdListToString(compIdsAdded, szIds);
            break;

        case EVENT_MODIFIED:
            szMessage.append(E_MODIFIED);
            conpIdListToString(compIdsModified, szIds);
            break;

        case EVENT_DELETED:
            szMessage.append(E_DELETED);
            conpIdListToString(compIdsDeleted, szIds);
            break;

        case TODO_ADDED:
            szMessage.append(T_ADDED);
            conpIdListToString(compIdsAdded, szIds);
            break;

        case TODO_MODIFIED:
            szMessage.append(T_MODIFIED);
            conpIdListToString(compIdsModified, szIds);
            break;

        case TODO_DELETED:
            szMessage.append(T_DELETED);
            conpIdListToString(compIdsDeleted, szIds);
            break;

        case JOURNAL_ADDED:
            szMessage.append(J_ADDED);
            conpIdListToString(compIdsAdded, szIds);
            break;

        case JOURNAL_MODIFIED:
            szMessage.append(J_MODIFIED);
            conpIdListToString(compIdsModified, szIds);
            break;

        case JOURNAL_DELETED:
            szMessage.append(J_DELETED);
            conpIdListToString(compIdsDeleted, szIds);
            break;
        case BIRTHDAY_ADDED:
            szMessage.append(BDAY_ADDED);
            conpIdListToString(compIdsAdded, szIds);
            break;
        case BIRTHDAY_MODIFIED:
            szMessage.append(BDAY_MODIFIED);
            conpIdListToString(compIdsModified, szIds);
            break;
        case BIRTHDAY_DELETED:
            szMessage.append(BDAY_DELETED);
            conpIdListToString(compIdsDeleted, szIds);
            break;

        case CALENDAR_ADDED:
            break;

        case CALENDAR_MODIFIED:
            break;

        case CALENDAR_DELETED:
            break;
        
	case CALENDAR_CLEARED:
            break;

        default:
            szMessage.append(COMP_MODIFIED);
            break;
    } 

    szMessage.append(intToString(changeCount));
    szMessage.append(":");
    szMessage.append(szIds);

    ret = pCalDb->commitDB(szMessage);
    changeFlag = 0;
    changeCount = 0;
    compIdsAdded.clear();
    compIdsDeleted.clear();
    compIdsModified.clear();

    return ret;
}


/**
 * @param: none
 * @return: bool TRUE/FALSE
 * 
 * Function to rollback all the changes made into database.
 */
bool CMulticalendar::rollbackAllChanges()
{
    bool ret;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
        CAL_DEBUG_LOG("invalid CalendarDB pointer ");
        ret = false;
    }
    else {
        ret = pCalDb->rollbackDB();

        changeFlag = 0;
        changeCount = 0;
        compIdsAdded.clear();
        compIdsDeleted.clear();
        compIdsModified.clear();
    }

    return ret;
}

/**
 * @param: title title of the calendar
 * @param: CalendarColour colour of the calendar
 * @param: int readonly flag
 * @param: int visible flag 
 * @param: CalendarType type of the calendar
 * @param: string szCalendarTune Calendar tune   
 * @param: string szCalendarversion Calendar Version   
 * @param: int * pointer to ErrorCode 
 * @return: newly created calendar pointer
 *
 * This function is called to add a new calendar into the database.
 */
CCalendar* CMulticalendar::addCalendar(string szTitle, CalendarColour iColor,
                int iReadOnly, int iVisible,
                CalendarType iType, string szCalendarTune,
                string szCalendarVersion, int &pErrorCode)
{

    /* Memory allocated here will be 
     * freed when Cmulticalendar object is
     * destroyed*/

    CCalendar *pCal = new CCalendar(szTitle,
                    (CalendarColour) iColor,
                    (int) iReadOnly,
                    (int) iVisible,
                    (CalendarType) iType,
                    szCalendarTune,
                    szCalendarVersion);

    if (pCal == 0) {

	pErrorCode = CALENDAR_MEMORY_ERROR;
	CAL_DEBUG_LOG("Failure to allocate memory using new\n");
	return NULL;
    }

    this->addCalendar(pCal, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
		  pErrorCode);
	delete pCal;
	return NULL;
    }
    changeFlag = changeFlag | CALENDAR_ADDED;

    return pCal;
}

/**
 * @param szName Name of Calendar
 * @return bool 
 *
 * Function to check whether the calendar name exists or not 
 */
bool CMulticalendar::checkCalendarNameExists(string szName)
{
    vector < CCalendar * >vListCal;
    unsigned int iter = 0;
    bool bRet = false;
    
    vListCal = this->getListCalFromMc();
    for (iter = 0; iter < vListCal.size(); iter++) {
      if (vListCal[iter]->getCalendarName() == szName) {
        bRet = true;
        break;
      }
    }
    releaseListCalendars(vListCal);
    return bRet;
}

/**
 * @param: pCal pointer to CCalendar object 
 * @param: int * pointer to ErrorCode 
 * @return: success/failure  
 *
 * This function is called to add a new calendar into the database.
 */
int CMulticalendar::addCalendar(CCalendar * pCal, int &pErrorCode)
{

    int iCalId = NULLID;
    char *pQuery = 0;
    const char *pTail = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    int iCol = 1;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
	pErrorCode = CALENDAR_APP_ERROR;
	CAL_DEBUG_LOG("invalid caldb pointer");
	return FAILURE;
    }

    if (!pCal) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	CAL_DEBUG_LOG("CCalendar pointer is 0");
	return FAILURE;
    }
    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	return FAILURE;
    }

    /*Check if the calendar with same name exists 
     * in Cache of calendars if so return the 
     * error code 
     */

    if (this->checkCalendarNameExists(pCal->getCalendarName())) {
	CAL_DEBUG_LOG("Calenadr already exists");
	pErrorCode = CALENDAR_ALREADY_EXISTS;
	this->rollbackAllChanges();
	return FAILURE;
    }

     if(COLOUR_NEXT_FREE == pCal->getCalendarColor())
     {
        pCal->setCalendarColor(getUnusedCalendarColor());
     }

    pQuery =
    sqlite3_mprintf(INSERT_CALENDAR, CAL_FIELD_NAME, CAL_FIELD_COLOR,
            CAL_FIELD_ISVISIBLE, CAL_FIELD_ISREADONLY,
            CAL_FIELD_CALTYPE, CAL_FIELD_TUNE,
            CAL_FIELD_VERSION, CAL_FIELD_PROFILE);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s", pQuery);

    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);

    sqlite3_bind_text(pPreparedStmt, iCol++,
              (pCal->getCalendarName()).c_str(),
              (pCal->getCalendarName()).length(),
              SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, iCol++, pCal->getCalendarColor());
    sqlite3_bind_int(pPreparedStmt, iCol++, pCal->IsShown());
    sqlite3_bind_int(pPreparedStmt, iCol++, pCal->IsReadOnly());
    sqlite3_bind_int(pPreparedStmt, iCol++, pCal->getCalendarType());
    sqlite3_bind_text(pPreparedStmt, iCol++,
              pCal->getCalendarTune().c_str(),
              pCal->getCalendarTune().length(), SQLITE_TRANSIENT);
    sqlite3_bind_text(pPreparedStmt, iCol++,
              pCal->getCalendarVersion().c_str(),
              pCal->getCalendarVersion().length(),
              SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, iCol, pCal->getProfileType());

    iCalId = pCalDb->insertRows(pPreparedStmt,iSqliteError); 
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL){
	pCal->setCalendarId(iCalId);
	CAL_DEBUG_LOG("addCalendar: Insert Success");


	changeCount++;
	changeFlag = changeFlag | CALENDAR_ADDED;

    CalId = pCal->getCalendarId();
	this->commitAllChanges();

	return pCal->getCalendarId();

    } 
    else {
	CAL_DEBUG_LOG("addCalendar: Insert failed  error is : %d ",pErrorCode);
	this->rollbackAllChanges();
	return FAILURE;
    }
}


/**
 * Destructor for CMulticalendar class
 */

CMulticalendar::~CMulticalendar()
{
    /* closing the db file handler */
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb){
          pCalDb->closeDB();
          pCalDb-> InstanceDestroy();
    }
    icaltimezone_free_builtin_timezones();
    icalmemory_free_ring();
    pMc = NULL;
}


/**
 * Constructor for CMulticalendar class
 */
CMulticalendar::CMulticalendar()
{
}


/**
 * @param: Id of the calendar
 * @param: title title of the calendar
 * @param: CalendarColour colour of the calendar
 * @param: int readonly flag
 * @param: int visible flag 
 * @param: CalendarType type of the calendar
 * @param: string CalendarTune  of the calendar
 * @param: string CalendarVersion of the calendar
 * @param: int * pointer to ErrorCode 
 * @return: bool (SUCCESS/FAILURE)
 * 
 * This function is called to modify the calendar record
 * in the database.
 */
bool CMulticalendar::modifyCalendar(int iId, string szTitle,
                    CalendarColour iColor, int iReadOnly,
                    int iVisible, CalendarType iType,
                    string szCalendarTune,
                    string szCalendarVersion,
                    int &pErrorCode)
{
    // Get existing calendar
    CCalendar *pCal = getCalendarById(iId, pErrorCode);

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
    {
        CAL_ERROR_LOG("Failed to get calendar #%d, error %d", iId, pErrorCode);
        return true;
    }

    // Update fields
    setCalendarName(pCal, szTitle);
    pCal->setCalendarColor(iColor);
    pCal->setCalendarReadOnly(iReadOnly);
    pCal->setCalendarShown(iVisible);
    pCal->setCalendarType(iType);
    pCal->setCalendarTune(szCalendarTune);
    pCal->setCalendarVersion(szCalendarVersion);

    // Commit changes
    this->modifyCalendar(pCal, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
        CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
        pErrorCode);
        delete pCal;
        pCal = 0;
        return false;
    }

    // Cleanup
    delete pCal;
    pCal = 0;

    return true;
}

/**
 * @param: pCal Pointer to CCalendar object 
 * @param: int * pointer to ErrorCode 
 * @return: bool (SUCCESS/FAILURE)
 * 
 * This function is called to modify the calendar record
 * in the database.
 */
bool CMulticalendar::modifyCalendar(CCalendar * pCal, int &pErrorCode)
{

    char *pQuery = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    int i = 1;
    int visibilityBefore = 1;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    vector < CCalendar * >::iterator iIter;
    vector < CCalendar * >vListCal;
    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();

    if (!pCal) {
	CAL_DEBUG_LOG("CCalendar pointer is 0");
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }
    int visibilityAfter = pCal->IsShown();
    
    if (pCalDb == 0) {
	CAL_DEBUG_LOG("Calendar:Invalid Caldb pointer");
	pErrorCode = CALENDAR_FUNC_ERROR;
	return false;
    }

    /* Create buffer calendar object */
    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	return false;
    }
   /* if the profile type is not none 
    * then changed the calendar type 
    * to synchronized 		*/
    if ((pCal->getProfileType() != SYNC_METHOD_NONE) && 
	 (pCal->getCalendarType() != DEFAULT_SYNC))
	    pCal->setCalendarType(SYNC_CALENDAR);

    pQuery =
    sqlite3_mprintf(CALENDAR_UPDATE, CAL_FIELD_NAME, CAL_FIELD_COLOR,
            CAL_FIELD_ISVISIBLE, CAL_FIELD_ISREADONLY,
            CAL_FIELD_CALTYPE, CAL_FIELD_TUNE,
            CAL_FIELD_VERSION, CAL_FIELD_PROFILE, CAL_FIELD_ID,
            pCal->getCalendarId());
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s\n", pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);

    sqlite3_bind_text(pPreparedStmt, i++,
              (pCal->getCalendarName()).c_str(),
              pCal->getCalendarName().length(), SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, i++, pCal->getCalendarColor());
    sqlite3_bind_int(pPreparedStmt, i++, pCal->IsShown());
    sqlite3_bind_int(pPreparedStmt, i++, pCal->IsReadOnly());
    sqlite3_bind_int(pPreparedStmt, i++, pCal->getCalendarType());
    sqlite3_bind_text(pPreparedStmt, i++, pCal->getCalendarTune().c_str(),
              pCal->getCalendarTune().length(), SQLITE_TRANSIENT);
    sqlite3_bind_text(pPreparedStmt, i++,
              pCal->getCalendarVersion().c_str(),
              pCal->getCalendarVersion().length(),
              SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, i++, pCal->getProfileType());
    CAL_DEBUG_LOG(" ProfileType is %d", pCal->getProfileType());
    pCalDb->updateDB(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    
    if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL){

	changeCount++;
	changeFlag = changeFlag | CALENDAR_MODIFIED;

	/* Update information in calendar vector */

	for (iIter = vListCal.begin(); iIter != vListCal.end();iIter++) {
	    if (pCal->getCalendarId() == (*iIter)->getCalendarId()) {
		/* get the visibility status of the calendar
		 * prior to modify being called*/
		visibilityBefore = (*iIter)->IsShown();

		break; //break when appropriate calendar is found
	    }

	}
	releaseListCalendars(vListCal);

	CAL_DEBUG_LOG(" visibility before %d \n visibilityAfter %d\n ",
              visibilityBefore, visibilityAfter);
	if ((visibilityAfter == 1) && (visibilityBefore == 0)) {
	    /* Calendar is made visible so all entries for which 
	     * alarm is added while the calendar is invisible 
	     * should register a valid alarm */
	    pCal->registerAlarmForVisibleCalendar(pErrorCode);
	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
		CAL_DEBUG_LOG("UnSuccessful in registering alarms :-(\n");

	} 
	else if ((visibilityAfter == 0) && (visibilityBefore == 1)) {
        /* shoudl we de-register all such alarms ?
         */

	    pCal->destroyExistingValidAlarms(pErrorCode);
	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
		CAL_DEBUG_LOG("Error in deregistering alarms  :-( \n ");

	}

    CalId = pCal->getCalendarId();
	this->commitAllChanges();

	return true;
    }
    this->rollbackAllChanges();
    CAL_DEBUG_LOG("failure");
    return false;
}

/**
 * @param iCalId Calendar Id 
 * @param pErrorCode  -  Reference to errror Code 
 * 
 * API to Clear the contents of the Calendar
 * 
 */
void CMulticalendar::clearCalendarContents(int iCalId,int &pErrorCode )
{
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    this->clearCalendar(iCalId,pErrorCode);

    changeCount++;
    changeFlag = changeFlag | CALENDAR_CLEARED;
    CalId = iCalId;
    this->commitAllChanges();
}

/**
 * @param iCalId Calendar Id 
 * @param pErrorCode  -  Reference to errror Code 
 * 
 * API to Clear the contents of the Calendar
 * 
 */
void CMulticalendar::clearCalendar(int iCalId,int &pErrorCode )
{

    char *pQuery = 0;
    sqlite3_stmt *pStmt = 0;
    const char *pTail = 0;
    int iSqliteError = 0;
    CCalendar *pTemp = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Calendar:Invalid Caldb pointer");
        pErrorCode = CALENDAR_APP_ERROR;
        return;
    }

    pTemp = this->getCalendarById(iCalId, pErrorCode);
    
    /* if there is no calendar return */
    if (!pTemp)
	    return;
    
	    
    /* Destroy Valid alarms for this Calendar 
     *  */
    pTemp->destroyExistingValidAlarms(pErrorCode );
    if(pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
    {
	CAL_DEBUG_LOG("Failed to de-activate Alarm ");
	delete pTemp;
	pTemp = 0;
	return;
    }

    if (pTemp->getCalendarType() == BIRTHDAY_CALENDAR) {
        /* quick cleaning for birthdays */ 
        pQuery = sqlite3_mprintf(DELETE_BY_ID, COMPONENTS_TABLE, CAL_FIELD_ID, iCalId);
        ASSERTION(pQuery);
        pCalDb->execSQL(pQuery);
        sqlite3_free(pQuery);
        pQuery = NULL;

        /* drop the IDMAP TABLE */
        pQuery = sqlite3_mprintf(DROP_TABLE,IDMAP_TABLE);
        ASSERTION(pQuery);
        sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery), &pStmt, &pTail);
        sqlite3_free(pQuery);
        pCalDb->updateDB(pStmt,iSqliteError);
        pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
        if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
            CAL_DEBUG_LOG("Failed to drop idmap table");
            this->rollbackAllChanges();
            delete pTemp;
            pTemp = 0;
            return ;
        }
    }
    else {
        /* Delete components using mass-commit */
        deleteAllComponents(iCalId, pErrorCode);
    }
   
   delete pTemp;
   pTemp = 0; 

}

/**
 * @param: calid Calendar id
 * @param: int * pointer to ErrorCode 
 * @return: SUCCESS/FAILURE
 *
 * This function is used to delete the calendar from database.
 */
bool CMulticalendar::deleteCalendar(int iCalId, int &pErrorCode)
{

    char *pQuery = 0;
    sqlite3_stmt *pStmt = 0;
    const char *pTail = 0;
    int iSqliteError = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pCalDb == 0) {

	CAL_DEBUG_LOG("Calendar:Invalid Caldb pointer");
	return false;

    }

    if (iCalId < 0) {
	CAL_DEBUG_LOG("Calendar ID can't be negative value");
	return false;
    }



    /*
     * check whether the calendar can be deleted 
     * or not 
     * Default calenars cannot be deleted 
     * "Synchronized" & "Private"
     * 
     */
    CCalendar *pTemp = getCalendarById(iCalId, pErrorCode);
    if( pTemp == 0) { 
	    CAL_DEBUG_LOG("Calendar with Id %d not found in DB ", iCalId);
	    pErrorCode = CALENDAR_DOESNOT_EXISTS;
	    return false;
    }
     
    if ((pTemp->getCalendarType() == DEFAULT_SYNC) ||
       (pTemp->getCalendarType() == DEFAULT_PRIVATE)) {
	    pErrorCode = CALENDAR_CANNOT_BE_DELETED;
	    CAL_DEBUG_LOG("Default calendars cannot be deleted \n ");
	    delete pTemp;
	    pTemp = 0;
	    return false ;
    }

    CalId = pTemp->getCalendarId();

    /* clears the calendar contents*/
    clearCalendar(iCalId,pErrorCode);

    delete pTemp;
    pTemp = 0;

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL ) { 
        CAL_DEBUG_LOG("Clear is unsuccessful");
        return false ;
    }

    if (!this->setAutocommitOff()) {
        CAL_DEBUG_LOG("Database is locked \n");
        pErrorCode = CALENDAR_DB_LOCKED;
        return false;
    }
    
    pQuery =
    sqlite3_mprintf(DELETE_TABLE, CALENDARS_TABLE, ENTRY_FIELD_CALID);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery), &pStmt, &pTail);
    sqlite3_free(pQuery);
    sqlite3_bind_int(pStmt, 1, iCalId);
    pCalDb->updateDB(pStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
	this->rollbackAllChanges();
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | CALENDAR_DELETED;

    this->commitAllChanges();


    return true;

}

/**
 * @param: repeattype frequency in int
 * @param:  until time in utc 
 * @param: int * pointer to ErrorCode 
 * @return: string Rrule in string
 * 
 * Function used to generate RRULE in the format as per ICAL RFC 2445
 */
string CMulticalendar::generateRrule(int iRepeatType, time_t iUntil,
                     int &pErrorCode)
{

    /*Sample RRULE as per RFC "FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR;
       UNTIL=20080530T123012";*/
    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[MAX_RECUR] =
    { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY"
    };
    string result;
    struct icaltimetype uical;

    uical = icaltime_from_timet(iUntil, 1);
    string ustring = icaltime_as_ical_string(uical);
    result = s1 + test[iRepeatType - 1] + s2 + ustring;
    result = result + "Z";

    CAL_DEBUG_LOG("Result is %s\n", result.c_str());

    return result;
}


/**
 * @param: ptr CEvent object pointer
 * @param: calendarId calendar ID
 * @param: int * pointer to ErrorCode 
 * @return: bool  
 *  
 * Function used to add the Event using CMulticalendar class.
 */

bool CMulticalendar::addEvent(CEvent * pEvent, int iCalendarId,
                  int &pErrorCode)
{
    if (!pEvent) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }
    CCalendar *pCal = this->getCalendarById(iCalendarId, pErrorCode);

    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }

    /* turn off the Auto commit */

    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal = 0;
	return false;
    }
    pCal->addEvent(pEvent, pErrorCode);
    if ((pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) &&
		    (pErrorCode != CALENDAR_ENTRY_DUPLICATED))
       {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
              pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
	pCal = 0;
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | EVENT_ADDED;
    compIdsAdded.push_back(pEvent->getId());

    CalId = pCal->getCalendarId();
    this->commitAllChanges();
    delete pCal;
    pCal = 0;

    return true;

}

/**
 * @param: ptr CEvent object pointer
 * @param: calendarId calendar ID
 * @param: int * pointer to ErrorCode 
 * @return: bool 
 * 
 * Function used to modify the Event using CMulticalendar class.
 */
bool CMulticalendar::modifyEvent(CEvent * pEvent, int iCalendarId,
                 int &pErrorCode)
{

    if (!pEvent) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }


    CCalendar *pCal = this->getCalendarById(iCalendarId, pErrorCode);

    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }

    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal = 0;
	return false;
    }


    pCal->modifyEvent(pEvent, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
              pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
	pCal = 0;
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | EVENT_MODIFIED;
    compIdsModified.push_back(pEvent->getId());


    CalId = pCal->getCalendarId();
    this->commitAllChanges();
    delete pCal;
    pCal = 0;
    return true;

}

/**
 * @param: cal_id Calendar id
 * @param: compId Event ID
 * @param: int * pointer to ErrorCode 
 * @return: bool 
 * 
 * Function used to delete the Event using CMulticalendar class.
 */
bool CMulticalendar::deleteEvent(int iCalId, string sCompId,
                 int &pErrorCode)
{
    CCalendar *pCal = this->getCalendarById(iCalId, pErrorCode);


    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }

    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal = 0;
	return false;
    }

    pCal->deleteEvent(sCompId, pErrorCode);

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
              pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
	pCal = 0;
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | EVENT_DELETED;
    compIdsDeleted.push_back(sCompId);

    CalId = pCal->getCalendarId();
    this->commitAllChanges();
    delete pCal;
    pCal = 0;

    return true;
}
/**
 * @param: ptr CTodo object pointer
 * @param: calendarId calendar ID
 * @param: int * pointer to ErrorCode 
 * @return: bool 
 * 
 * Function used to add the Todo using CMulticalendar class.
 */
bool CMulticalendar::addTodo(CTodo * pTodo, int iCalendarId,
                 int &pErrorCode)
{
    if (!pTodo) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }

    CCalendar *pCal = this->getCalendarById(iCalendarId, pErrorCode);
    if (pCal == 0) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }

    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal = 0;
	return false;
    }
    pCal->addTodo(pTodo, pErrorCode);

    if ((pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) &&
		    (pErrorCode != CALENDAR_ENTRY_DUPLICATED)){
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
		  pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
	pCal = 0;
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | TODO_ADDED;
    compIdsAdded.push_back(pTodo->getId());

    CalId = pCal->getCalendarId();
    this->commitAllChanges();

    delete pCal;
    pCal = 0;
    return true;
}
/**
 * @param: ptr CTodo object pointer
 * @param: calendarId calendar ID
 * @param: int * pointer to ErrorCode 
 * @return: bool 
 * 
 * Function used to modify the Todo using CMulticalendar class.
 */
bool CMulticalendar::modifyTodo(CTodo * pTodo, int iCalendarId,
                int &pErrorCode)
{

    if (!pTodo) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }

    CCalendar *pCal = this->getCalendarById(iCalendarId, pErrorCode);

    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }

    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal = 0;
	return false;
    }
    pCal->modifyTodo(pTodo, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
              pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
	pCal = 0;
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | TODO_MODIFIED;
    compIdsModified.push_back(pTodo->getId());


    CalId = pCal->getCalendarId();

    this->commitAllChanges();
    delete pCal;
    pCal = 0;

    return true;

}


/**
 * @param: cal_id Calendar id
 * @param: compId Todo ID
 * @param: int * pointer to ErrorCode 
 * @return: bool 
 * 
 * Function used to delete the Todo using CMulticalendar class.
 */
bool CMulticalendar::deleteTodo(int iCalId, string sCompId,
                int &pErrorCode)
{

    CCalendar *pCal = this->getCalendarById(iCalId, pErrorCode);

    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }

    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal = 0;
	return false;
    }

    pCal->deleteTodo(sCompId, pErrorCode);

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
              pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
	pCal = 0;
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | TODO_DELETED;
    compIdsDeleted.push_back(sCompId);


    CalId = pCal->getCalendarId();

    this->commitAllChanges();
    delete pCal;
    pCal = 0;

    return true;
}

/**
 * @param: ptr CJournal object pointer
 * @param: calendarId calendar ID
 * @param: int * pointer to ErrorCode 
 * @return: bool 
 * 
 * Function used to add the Journal using CMulticalendar class.
 */
bool CMulticalendar::addJournal(CJournal * pJournal, int iCalendarId,
                int &pErrorCode)
{
    if (!pJournal) {
    pErrorCode = CALENDAR_INVALID_ARG_ERROR;
    return false;
    }

    CCalendar *pCal = this->getCalendarById(iCalendarId, pErrorCode);

    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }
    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
        pCal =0 ;	
	return false;
    }

    pCal->addJournal(pJournal, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
		  pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
        pCal =0 ;	
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | JOURNAL_ADDED;
    compIdsAdded.push_back(pJournal->getId());

    CalId = pCal->getCalendarId();

    this->commitAllChanges();
    delete pCal;
    pCal =0 ;	
    
    return true;
}

/**
 * @param: ptr CJournal object pointer
 * @param: calendarId Calendar id
 * @param: int * pointer to ErrorCode 
 * @return bool 
 *  
 * Function used to modify the Journal using CMulticalendar class.
 */
bool CMulticalendar::modifyJournal(CJournal * pJournal, int iCalendarId,
                   int &pErrorCode)
{

    if (!pJournal) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }

    CCalendar *pCal = this->getCalendarById(iCalendarId, pErrorCode);

    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }

    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal =0 ;	
	return false;
    }

    pCal->modifyJournal(pJournal, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
              pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
	pCal =0 ;	
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | JOURNAL_MODIFIED;
    compIdsModified.push_back(pJournal->getId());


    CalId = pCal->getCalendarId();
    this->commitAllChanges();
    delete pCal;
    pCal =0 ;	

    return true;
}

/**
* @param: iCalId - calendar ID 
* @param: int type E_EVENT = 1
* @param: int iDaybefore time for deletion of entries
* @param: int reference to ErrorCode 
* @return: vector of string Id  
*
* Function used to retrieve the ID of the entries ( only Events and Todos) 
* expired ( 2weeks over-due)
*/
vector < string > CMulticalendar::giveRecursiveExpiredEvents(int iCalId,
                                 int iType,
                                 int
                                 iConfiguredTime,
				 int iMax,
				 int &iLimit,
                                 int
                                 &pErrorCode)
{

    QueryResult *pQr = 0;
    char *pQuery = 0;
    int iI_IdCount = 0;
    int iSqliteError = 0;
    int iLeft = 0;
    /*configuredTime should be preconfigured in 
     * and stored in some place so that we can access 
     */
    vector < string > vlistId;

    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {

	pErrorCode = CALENDAR_APP_ERROR;
	CAL_DEBUG_LOG("Calendar:Invalid Caldb pointer");
	return vlistId;

    }
    pQuery =
	sqlite3_mprintf(GET_EXPIRED_RECURSIVE_EVENTS_COUNT, 
		ENTRY_FIELD_TYPE, iType, ENTRY_FIELD_CALID, iCalId,
		ENTRY_FIELD_FLAGS, HAS_RECURRENCE,
		ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM);

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("Query is %s", pQuery);

    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    if(pQr !=0){
	iLeft = atoi(pQr->pResult[ONE]);
	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
    }
    if (iLeft > 0 && iLimit == 0)
	iLimit = iMax;
    /* Count Query */
    if(iLeft > 0){

	/*
	 *#define GET_EXPIRED_RECURSIVE_EVENTS   "select %s from components where 
	 %s= %d and %s= %d and  (%s < %d  OR %s = %d) and ( %s = %d OR %s=%d)"

*/
	pQuery =
	    sqlite3_mprintf(GET_EXPIRED_RECURSIVE_EVENTS, ENTRY_FIELD_ID,
		    ENTRY_FIELD_TYPE, iType, ENTRY_FIELD_CALID, iCalId,
		    ENTRY_FIELD_FLAGS, HAS_RECURRENCE,
		    ENTRY_FIELD_FLAGS, HAS_RECURRENCE_ALARM,iLimit);

	ASSERTION(pQuery);
	CAL_DEBUG_LOG("Query is: %s", pQuery);

	pQr = pCalDb->getRecords(pQuery,iSqliteError);
	pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	sqlite3_free(pQuery);

	if (pQr == 0) 
	    return vlistId;

	for (iI_IdCount = 1; iI_IdCount <= pQr->iRow; iI_IdCount++) {
	    string temp;
	    if (pQr->pResult[iI_IdCount]) {
		temp = (pQr->pResult[iI_IdCount]);
		vlistId.push_back(temp);
	    }
	}

	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;
	iLimit = iLeft - iLimit;
	if(iLimit < 0)
	    iLimit = 0;
	
    }
    else
	iLimit = 0;
    CAL_DEBUG_LOG("size of entries is %d\n", vlistId.size());
    return vlistId;
}
/**
 * @param: iCalId - calendar ID 
 * @param: int type E_EVENT = 1,E_TODO = 2
 * @param: int Configured time for deletion of entries
 * @param: int * pointer to ErrorCode 
 * @return: vector of string Id  
 *
 * Function used to retrieve the ID of the entries ( only Events and Todos) 
 * expired ( 2weeks over-due)
 */
vector < string > CMulticalendar::giveExpiredEntries(int iCalId, int iType,
                             int iConfiguredTime,
                             int &pErrorCode)
{

    QueryResult *pQr = 0;
    char *pQuery = 0;
    int iI_IdCount = 0;
    int iSqliteError = 0;

    /*configuredTime should be preconfigured in 
     * and stored in some place so that we can access 
     */

    vector < string > vlistId;

    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {

	pErrorCode = CALENDAR_APP_ERROR;
	CAL_DEBUG_LOG("Calendar:Invalid Caldb pointer");
	return vlistId;

    }


    if (iType == E_EVENT)
    {
        /**
         * select only non recursive events ,Modify the recursive events 
         * such that only previous instances are deleted
         * 
         * #define GET_EXPIRED_EVENTS select %s from components where %s=%d 
	   and  %s= %d and %s < %d
         */
	pQuery =
        sqlite3_mprintf(GET_EXPIRED_NONRECURSIVE_EVENTS,
                ENTRY_FIELD_ID, ENTRY_FIELD_TYPE, iType,
                ENTRY_FIELD_CALID, iCalId, ENTRY_FIELD_DTEND,
                iConfiguredTime, ENTRY_FIELD_FLAGS,
                HAS_RECURRENCE, ENTRY_FIELD_FLAGS,
                HAS_RECURRENCE_ALARM);
	
    }
    else if (iType == E_JOURNAL)
        /**
         * #define GET_EXPIRED_EVENTS select %s from components where %s=%d and  
	   %s= %d and  %s < %d
         */
	pQuery =
        sqlite3_mprintf(GET_EXPIRED_EVENTS, ENTRY_FIELD_ID,
                ENTRY_FIELD_TYPE, iType, ENTRY_FIELD_CALID,
                iCalId, ENTRY_FIELD_DTSTART, iConfiguredTime);
    else if (iType == E_TODO)
        /**
         * #define GET_EXPIRED_TODOS select %s from components where %s=%d and 
	   %s= %d and %s = %d and  %s < %d
         */
	pQuery =
        sqlite3_mprintf(GET_EXPIRED_TODOS, ENTRY_FIELD_ID,
                ENTRY_FIELD_TYPE, E_TODO, ENTRY_FIELD_CALID,
                iCalId, ENTRY_FIELD_STATUS, true,
                ENTRY_FIELD_DTSTART, iConfiguredTime);


    ASSERTION(pQuery);
    CAL_DEBUG_LOG("%s is the query in CCOmponets", pQuery);

    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr == 0)
	return vlistId;
    CAL_DEBUG_LOG("ROW COUNT %d\n ", pQr->iRow);
    CAL_DEBUG_LOG("COL COUNT %d\n ", pQr->iColumn);


    for (iI_IdCount = 1; iI_IdCount <= pQr->iRow; iI_IdCount++) {
	string temp;
	if (pQr->pResult[iI_IdCount]) {
	    temp = (pQr->pResult[iI_IdCount]);
	    vlistId.push_back(temp);
	}
    }

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    CAL_DEBUG_LOG("size of entries is %d\n", vlistId.size());
    return vlistId;
}
/**
 * @param: icalid calendar ID 
 * @param: time utc beyond which all todos should be deleted
 * @param: iType Component type
 * @param: int * pointer to ErrorCode 
 * @return: bool success/failure of the function 
 * 
 * Function to delete Events/Todos beyond a particular date 
 */
bool CMulticalendar::deleteEventAndTodoBeyond(int iCalId, time_t iUtc,
                       int iType,int &pErrorCode)
{

   CMulticalendar *pMc = CMulticalendar::MCInstance();
   ASSERTION(pMc);
   pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

   if (iType == E_EVENT)
       pMc->deleteEventBeyond(iCalId,iUtc,pErrorCode);
   else if (iType == E_TODO)
       pMc->deleteTodosBeyond(iCalId,iUtc,pErrorCode);
   else 
       pErrorCode = CALENDAR_INVALID_ARG_ERROR;
   
   if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
       return false ;
  /* if control reaches it implies that 
   * the operation is successful  */
   return true;
}
/**
 * @param: icalid calendar ID 
 * @param: time utc beyond which all todos should be deleted
 * @param: int * pointer to ErrorCode 
 * @return: bool success/failure of the function 
 */
bool CMulticalendar::deleteEventBeyond(int iCalId, time_t iUtc,
                       int &pErrorCode)
{

    CMulticalendar *mc = CMulticalendar::MCInstance();
    CCalendar *pC = mc->getCalendarById(iCalId, pErrorCode);

    if (pC == 0) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }
    vector < string > vtempId;
    vector < string >::iterator iter;
    
    vtempId = giveExpiredEntries(iCalId, E_EVENT, (int) iUtc, pErrorCode);
    if (vtempId.size() == 0) {
	CAL_DEBUG_LOG("No event Found with the specified Data \n");
	delete pC;
	pC=0 ;
	return FAILURE;
    }
    for (iter = vtempId.begin(); iter != vtempId.end(); iter++)
	mc->deleteEvent(iCalId, *iter, pErrorCode);
     delete pC;
     pC=0 ;
    return true;
}

/**
 * @param: int iCalid id of the calendar
 * @param: utc time ( int  utc) before wich all todos 
 *         should be deleted
 * @param: int * pointer to ErrorCode 
 * @return: status of the operation.
 * Function used to delete todos before particulat date for a 
 * specified calendar ID.
 */
bool CMulticalendar::deleteTodosBeyond(int iCalId, time_t iUtc,
                       int &pErrorCode)
{
    CMulticalendar *mc = CMulticalendar::MCInstance();
    CCalendar *pC = mc->getCalendarById(iCalId, pErrorCode);

    if (!pC) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }
    vector < string > vtempId;
    vector < string >::iterator iter;

    vtempId = giveExpiredEntries(iCalId, E_TODO, (int) iUtc, pErrorCode);

    if (vtempId.size() == 0) {
	CAL_DEBUG_LOG("No Task Found with in the specified Date range \n");
	delete pC;
	pC = 0;
	return false;
    }

    for (iter = vtempId.begin(); iter != vtempId.end(); iter++)
	mc->deleteTodo(iCalId, *iter, pErrorCode);
    delete pC;
    pC = 0;

    return true;
}

/**
 * @param: int iCalid id of the calendar
 * @param: utc time ( int  utc) before which all journals 
 *         should be deleted
 * @param: int * pointer to ErrorCode 
 * @return: status of the operation.
 * Function used to delete journals before particular date for a 
 * specified calendar ID.
 */
bool CMulticalendar::deleteJournalsBeyond(int iCalId, time_t iUtc,
                      int &pErrorCode)
{
    CMulticalendar *mc = CMulticalendar::MCInstance();
    CCalendar *pC = mc->getCalendarById(iCalId, pErrorCode);

    if (!pC) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return FAILURE;
    }
    vector < string > vtempId;
    vector < string >::iterator iter;

    vtempId =
    giveExpiredEntries(iCalId, E_JOURNAL, (int) iUtc, pErrorCode);

    if (vtempId.size() == 0) {
	CAL_DEBUG_LOG("No Journal Found with the specified Data \n");
	delete pC;
	pC = 0;
	return FAILURE;
    }

    for (iter = vtempId.begin(); iter != vtempId.end(); iter++)
	mc->deleteJournal(iCalId, *iter, pErrorCode);

    delete pC;
    pC = 0;
    return SUCCESS;

}

/**
 * @param: cal_id Calendar id
 * @param: compId Journal id
 * @param: int * pointer to ErrorCode 
 * @return: bool 
 * 
 * Function used to delete the journal using CMulticalendar class
 */
bool CMulticalendar::deleteJournal(int iCalId, string sCompId,
                   int &pErrorCode)
{

    CCalendar *pCal = this->getCalendarById(iCalId, pErrorCode);

    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }
    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal = 0;
	return false;
    }

    pCal->deleteJournal(sCompId, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
              pErrorCode);
	this->rollbackAllChanges();
	delete pCal;
	pCal = 0;
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | JOURNAL_DELETED;
    compIdsDeleted.push_back(sCompId);


    CalId = pCal->getCalendarId();
    this->commitAllChanges();

    delete pCal;
    pCal = 0;
    return true;

}


/**
 * @param: int configuration time for  deletion of old events 
 * @param: int * pointer to ErrorCode 
 * @return: bool result of operation
 *
 * This function has to be called when ever 
 * clean up of old events and todos is required for 
 * given calendar ID. 
 */

bool CMulticalendar::enableAutomaticDeletion(int iDayBefore,int iMax,int &iLimit,
	int &pErrorCode)
{


    /* Here we have delete expired events and todos for a 
     * calendar -(automatic deletion is set)
     * Events which are expired 2 weeks before are to be deleted
     * Todos which are marked complete and expired 2 weeks before are to 
     * be deleted.
     */

    vector < string > veventId;
    vector < string >::iterator itervent;
    vector < CCalendar * >::iterator iterCal;
    CCalendarDB *pDb = CCalendarDB::Instance();
    int dstFlag_Before = 0;
    int dstFlag_Current = 0;
    int iCalId = 0;
    struct tm *pTimeToday = 0;
    time_t iCurrentTime = time_get_time();
    string recEventId;

    iLimit = iMax;

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0) {
	CAL_DEBUG_LOG("invalid Caldb pointer ");
	pErrorCode = CALENDAR_APP_ERROR;
	return false;
    }

    string sTz = this->getSystemTimeZone();

    dstFlag_Current = time_get_dst_usage(iCurrentTime,sTz.c_str());


    pTimeToday = localtime(&iCurrentTime);
    pTimeToday->tm_hour = 0;
    pTimeToday->tm_min = 0;
    pTimeToday->tm_sec = 0;
    if (iDayBefore == YEAR_DAYS)
	pTimeToday->tm_year = pTimeToday->tm_year - 1;

    iDayBefore = iDayBefore * SEC_IN_DAY;

    iCurrentTime = time_mktime(pTimeToday, 0);

    /* For deletion  before 1 Year
     * We have taken care in the above scenario  
     * so no need to subtract again 
     * Weekly - 7 days 
     * BIWeekly - 14 days 
     * Monthly - 31 days before the currentdate
     * Yearly means same date before 1 year 
     * ( for leap year feb 29 exists as a corner case)
     */
    if (iDayBefore / SEC_IN_DAY != YEAR_DAYS)
	iDayBefore = iCurrentTime - iDayBefore;

    else
	iDayBefore = iCurrentTime;

    dstFlag_Before = time_get_dst_usage(iDayBefore,sTz.c_str());

    if (dstFlag_Current == 1 &&  dstFlag_Before == 0)
	iDayBefore = iDayBefore + ONEHOUR;
    else if (dstFlag_Current == 0 &&  dstFlag_Before == 1)
	iDayBefore = iDayBefore - ONEHOUR;
    /*
     * iLimit is intialized to 100
     * for Non recursive Events
     */

    int iNonRecurciveDeleted = deleteNonRecurrentComponents(iDayBefore, iMax, pErrorCode);

    CALENDAR_LOG_ERROR(pErrorCode, "Failed to remove non-recurrent events");

    if (iNonRecurciveDeleted >= iMax) {
        // We have reached limit for one turn... try next time
        iLimit = iNonRecurciveDeleted;
        CAL_DEBUG_LOG("%d events removed and limit (%d) is reached. Thats enough for this turn", iLimit, iMax);

        return true;
    }
    else if (iNonRecurciveDeleted > 0) {
        // Some non-recurrent events was removed so reduce the limit for recurrent events for this turn.
        iMax -= iNonRecurciveDeleted;
    }

    /*
     * Recursive Events
     */
    vector <CCalendar *> vListCal;
    vListCal = getListCalFromMc();

    if (vListCal.size() == 0) {
	CAL_DEBUG_LOG("No Calendar present in the Database\n");
	/* this is not an error 
	 * its a configuration mistake*/
	pErrorCode = CALENDAR_NONE_INDB;
	return true;
    }

    vector <CComponent*>  listComp;
    vector <string> deletionId;

    for (iterCal = vListCal.begin(); iterCal != vListCal.end(); iterCal++) {
	listComp.clear();

	iCalId = (*iterCal)->getCalendarId();

	/* now we need to search recursive events 
	 * which satisfy the criteria 
	 */
	veventId = giveRecursiveExpiredEvents(iCalId, E_EVENT, iDayBefore,iMax,iLimit,
		pErrorCode);

	/* Modify the date start and date End for all these 
	 * events 
	 */
	CAL_DEBUG_LOG("Size of recursive events is %d ", veventId.size());
	int timeDiff = 0;
	CEvent *pTemp = 0;
	int dateStartModified = 0;
	for (itervent = veventId.begin(); itervent != veventId.end();
		itervent++) {

	    pTemp = (*iterCal)->getEvent(*itervent, pErrorCode);
	    if (pTemp) {
		string szTz = pTemp->getTzid();
		szTz = szTz.substr(1);
		icaltimezone* pTz = icaltimezone_get_builtin_timezone(szTz.c_str());

		timeDiff = pTemp->getDateEnd() - pTemp->getDateStart();
		dateStartModified = this->getInstanceBeyond((pTemp->
			getRecurrence ())-> getRrule()[0],
			icaltime_from_timet_with_zone (pTemp->getDateStart(),0, pTz),
			iDayBefore,pTz,pErrorCode);
		if (dateStartModified) {
			if(dateStartModified!=pTemp->getDateStart()) {
				pTemp->setDateStart(dateStartModified);
				pTemp->setDateEnd(dateStartModified + timeDiff);

				/* Modify Component list is collected per Calendar  */
				listComp.push_back(pTemp);
			}
		} 
		else{
		    /* deletion list for all calendars */
		    deletionId.push_back(pTemp->getId());
		    delete pTemp;
		    pTemp = 0;
		}
	    }
	}

	/* Call batch Modifty API here */

	this->modifyComponents(listComp, iCalId, pErrorCode);
	/* Free the listComp here */
	vector <CComponent*>::iterator iter;
	for (iter = listComp.begin();iter!=listComp.end();iter++){
	    delete (*iter) ;
	    (*iter) = 0;
	}

	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
	{
	    CAL_DEBUG_LOG("Failed to Modify the events in calId %d",iCalId);	
	    //break;
	}
	listComp.clear();
	if (iLimit > 0)
	    break;

    }

    deleteItemsByIdList(deletionId, pErrorCode);
    deletionId.clear();
    releaseListCalendars(vListCal);
    return true;

}


void CMulticalendar::deleteItemsByIdList(vector<string> &listId, int& pErrorCode)
{

    string szTemp;
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    CCalendarDB *pDb = CCalendarDB::Instance();
    int iCount = 0;
    int iSqliteError = 0;
    unsigned int i;

    if (pDb == 0) {
        CAL_DEBUG_LOG("invalid CalendarDB pointer ");
        pErrorCode = CALENDAR_APP_ERROR;
        return;
    }

    string szPrepare(DELETE_GROUP_ID);
    for ( i= 0 ;i< listId.size();i++)
    {
	if (i!= listId.size() -1){
	    szPrepare = szPrepare + "?,";
	}
	else{
	    szPrepare = szPrepare + "?)";
	}
    }
    sqlite3_prepare(pDb->getDb(), szPrepare.c_str(), szPrepare.length(),
	    &pPreparedStmt, &pTail);
    for ( i= 0 ;i< listId.size();i++){
	sqlite3_bind_text(pPreparedStmt, ++iCount, listId[i].c_str(),
		listId[i].length(), SQLITE_TRANSIENT);
    }

    pDb->updateDB(pPreparedStmt,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
}


/**
 * Function used to get the CMulticalendar instance
 */

CMulticalendar *CMulticalendar::MCInstance()
{
    if(pMc == 0) {
        pMc = new CMulticalendar();
        ASSERTION (pMc);

        pMc->enableDisableCalendarLogs(checkFileExists("/home/user/.calendar/log"));

        if (!checkFileExists("/tmp/calendar-firststart"))
        {
            std::ofstream firststart_file;
            firststart_file.open("/tmp/calendar-firststart");
            pMc->restoreAlarms();
        }
        else
        {
            CAL_DEBUG_LOG("It is not first calendar-backend start after boot. Do not restore alarms this time");
        }

        // just for debugging
        // pMc->regenerateInstances();

    }
    return pMc;
}

/**
 * overloaded function 
 * dummy
 */
CMulticalendar & CMulticalendar::operator=(CMulticalendar & right)
{
    return *this;
}

/*private Dummy copy constructor
 * because only one object needs to present  
 */
CMulticalendar::CMulticalendar(CMulticalendar & ref)
{
    // nothing to imlement here as 
    // the use of copy constructor is
    // restricted for CMulticalendar object
}



/**
 * @param: pBday CBdayEvent object pointer
 * @param: int * pointer to ErrorCode 
 * @return: bool (SUCESS/FAILURE)
 * 
 * Function used to add the Birthday data using CMulticalendar class.
 */
bool CMulticalendar::addBirthDay(CBdayEvent * pBday, int &pErrorCode)
{
    vector<CBdayEvent*> BdayList;
    BdayList.push_back(pBday);
    return addBirthdays(BdayList,pErrorCode); 
}


/**
 * @param: pBdays list of CBdayEvent object pointer
 * @param: int * pointer to ErrorCode 
 * @return: bool (SUCESS/FAILURE)
 * 
 * Function used to add the Birthday data using CMulticalendar class.
 */
bool CMulticalendar::addBirthdays(vector<CBdayEvent*>& pBdays, int &pErrorCode)
{

    if (!pBdays.size()) {
		CAL_DEBUG_LOG("Adding 0 birthdays \n");
		pErrorCode = CALENDAR_INVALID_ARG_ERROR;
		return false;
    }

    CCalendar *pCal = this->getCalendarByType(BIRTHDAY_CALENDAR, pErrorCode);

    if (!pCal) {
		pErrorCode = CALENDAR_DOESNOT_EXISTS;
		return false;
    }

    if (!this->setAutocommitOff()) {
		CAL_DEBUG_LOG("Database is locked \n");
		pErrorCode = CALENDAR_DB_LOCKED;
		delete pCal;
		return false;
    }


	vector <CBdayEvent*>::iterator		iter;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	for (iter = pBdays.begin(); iter != pBdays.end();iter++){
		pCal->addBirthDay(*iter, pErrorCode);
	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
			CAL_DEBUG_LOG("addBirthdays(%s): Error :%d, return",
				(*iter)->getId().c_str(),pErrorCode);
			this->rollbackAllChanges();
    		delete pCal;
    		pCal = 0 ;
			return false;
    	}
//      TODO compIdsDeleted must contains Component IDs, not Contacts UIDs
//         compIdsAdded.push_back((*iter)->getId());
		changeCount++;
	}

  
    changeFlag = changeFlag | BIRTHDAY_ADDED;


    CalId = pCal->getCalendarId();

    this->commitAllChanges();
    delete pCal;
    pCal = 0 ;

    return true;
}





/**
 * @param: pBday CBdayEvent object pointer
 * @param: int * pointer to ErrorCode 
 * @return: bool (SUCCESS/FAILURE)
 * 
 * Function used to modify the Birthdays using CMulticalendar class.
 */
bool CMulticalendar::modifyBirthDay(CBdayEvent * pBday, int &pErrorCode)
{

    if (!pBday) {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }

    CCalendar *pCal =
	this->getCalendarByType(BIRTHDAY_CALENDAR, pErrorCode);

    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }

    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
    	delete pCal;
        pCal = 0 ;
	return false;
    }

    pCal->modifyBirthDay(pBday, pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
	CAL_DEBUG_LOG(" Errorcode is :%d,so returning  from here ",
              pErrorCode);
	this->rollbackAllChanges();
    	delete pCal;
        pCal = 0 ;
	return false;
    }

    changeCount++;
    changeFlag = changeFlag | BIRTHDAY_MODIFIED;
    compIdsModified.push_back(pBday->getId());


    CalId = pCal->getCalendarId();
    this->commitAllChanges();
    delete pCal;
    pCal = 0 ;

    return true;

}

/**
 * @param: eUid Ebook Uid
 * @param: int & pointer to ErrorCode 
 * @return: boolean (SUCCESS/FAILURE)
 * 
 * Function used to delete the Event using CMulticalendar class.
 */
bool CMulticalendar::deleteBirthDay(string szEUid, int &pErrorCode)
{
    vector<string> szEUidList;
    szEUidList.push_back(szEUid);
    return deleteBirthdays(szEUidList,pErrorCode); 
}


/**
 * @param: vector<string>& szEUidList  list of Ids of the birthday events 
 * @param: int& pointer to ErrorCode 
 * @return: boolean (SUCCESS/FAILURE)
 * 
 * Function used to delete the Event using CMulticalendar class.
 */
bool CMulticalendar::deleteBirthdays(vector<string> &szEUidList,int& pErrorCode)
{

	if (szEUidList.size() == 0) {
		pErrorCode = CALENDAR_INVALID_ARG_ERROR;
		CAL_DEBUG_LOG("Input list of Birthdays to delete is empty");
		return false;
    }

    CCalendar *pCal =
    this->getCalendarByType(BIRTHDAY_CALENDAR, pErrorCode);

    if (!pCal) {
		pErrorCode = CALENDAR_DOESNOT_EXISTS;
		return false;
    }

    if (!this->setAutocommitOff()) {
		CAL_DEBUG_LOG("Database is locked \n");
		pErrorCode = CALENDAR_DB_LOCKED;
		delete pCal;
		pCal = 0 ;
		return false;
    }

	vector <string>::iterator		iter;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	changeCount = 0;

	for (iter = szEUidList.begin(); iter != szEUidList.end();iter++){
		if (pCal->deleteBirthDay(*iter, pErrorCode))
        {
//          TODO compIdsDeleted must contains Component IDs, not Contacts UIDs
//          compIdsDeleted.push_back(*iter);
            changeCount++;
        }

	    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
			CAL_DEBUG_LOG("deleteBirthdays(%s): Error :%d, return",
				(*iter).c_str(),pErrorCode);
			this->rollbackAllChanges();
    		delete pCal;
    		pCal = 0 ;
			return false;
    	}
	}
	
    if (changeCount > 0) {
        /* succesfully deleted  - commit everything */
        changeFlag = changeFlag | BIRTHDAY_DELETED;
        CalId = pCal->getCalendarId();

        this->commitAllChanges();
    } else {
        CAL_DEBUG_LOG("Nothing was deleted so rollback transaction");
        this->rollbackAllChanges();
    }
    delete pCal;
    pCal = 0 ;
    return true;
}



/**
 * @param: CalendarId - Id of the calendar for which export has to happen
 * @param: string - filename of the ics file 
 * @param: int * pointer to ErrorCode 
 * @return: string name of the file 
 * 
 *  Export file format will always be in ICAL format.This function
 *  exports the given calendar to a ics file.
 */
int CMulticalendar::exportCalendarEntries(int iCalendarId,
                      string szFileName,
                      int &pErrorCode)
{
    int iRet = 0;
    CCalendar *pCal = this->getCalendarById(iCalendarId, pErrorCode);
    if (pCal == 0) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	CAL_DEBUG_LOG("Invalid Calendar ID\n");
	return FAILURE;
    }
    CAL_DEBUG_LOG("before returning expoty\n");
    iRet = pCal->exportCalendarEntries(szFileName, pErrorCode);
    delete pCal;
    pCal = 0;

    return iRet;
}

/**
 * @param NONE 
 * @return CCalendar*
 * Retrieves existing default calendar.Calendar pointer thus obtained 
 * shouldn't be freed. It will be destroyed when CMulticalendar object is 
 * destroyed.
 * Returns 0 in error case
 * 
 */
CCalendar *CMulticalendar::getDefaultCalendar()
{
    vector < CCalendar * >::iterator iter;
    vector < CCalendar * >vListCal;
    CCalendar* retCal = NULL;
    
    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();

    if (vListCal.size() == 0) {
      CAL_DEBUG_LOG("No Clendar present in DB\n");
      return retCal;
    }

    for (iter = vListCal.begin(); iter != vListCal.end(); iter++) {
      if ((*iter)->getCalendarType() == DEFAULT_PRIVATE) {
          retCal = *iter;
      } 
      else {
          delete(*iter);
          (*iter) = 0;
      }
    }

    return retCal;
}


/**
 * @param int iCalid smart calendar Id to be deleted
 * @param pErrorcode int reference to error code 
 * @return bool success/failure indicator  
 * Function used to delete birthdays for a given calendar
 */
bool CMulticalendar::deleteBirthdaysForCalendar(int iCalId,
                        int &pErrorCode)
{

    char *pQuery = 0;
    sqlite3_stmt *pStmt = 0;
    const char *pTail = 0;
    int iSqliteError = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {

	CAL_DEBUG_LOG("Calendar:Invalid Caldb pointer");
	pErrorCode = CALENDAR_FUNC_ERROR;
	return false;

    }

    if (iCalId < 0) {
	CAL_DEBUG_LOG("Calendar ID can't be negative value");
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	return false;
    }
    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	return false;
    }

    /* birthdays have valid entries only in 
     * Components and recursive tables*/


    /* Delete from recursive table */
    pQuery =
    sqlite3_mprintf(DEL_ROW_BY_CALID, RECURSIVE_TABLE, RECUR_FIELD_ID,
            ENTRY_FIELD_ID, ENTRY_FIELD_CALID, iCalId);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery), &pStmt,
            &pTail);
    CAL_DEBUG_LOG("query is %s", pQuery);
    sqlite3_free(pQuery);

    pCalDb->updateDB(pStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
	this->rollbackAllChanges();
	return false;
    }
    pQuery =
    sqlite3_mprintf(DEL_ROW_BY_CALID, IDMAP_TABLE, IDMAP_FIELD_LOCALID,
            ENTRY_FIELD_ID, ENTRY_FIELD_CALID, iCalId);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery), &pStmt,
            &pTail);
    CAL_DEBUG_LOG("query is %s", pQuery);
    sqlite3_free(pQuery);

    pCalDb->updateDB(pStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
	this->rollbackAllChanges();
	return false;
    }
    /* Delete from Components table */
    pQuery =
    sqlite3_mprintf(DEL_ROW_BY_CALID, COMPONENTS_TABLE, ENTRY_FIELD_ID,
            ENTRY_FIELD_ID, ENTRY_FIELD_CALID, iCalId);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery), &pStmt,
            &pTail);
    CAL_DEBUG_LOG("query is %s", pQuery);
    sqlite3_free(pQuery);


    pCalDb->updateDB(pStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
	this->rollbackAllChanges();
	return false;
    }

    CalId = iCalId;

    this->commitAllChanges();


    return true;

}


/**
 * @param NONE 
 * @return CCalendar*
 * Retrieves existing Synchronized calendar.Calendar pointer thus obtained 
 * shouldn't be freed. It will be destroyed when CMulticalendar object is 
 * destroyed
 * Returns 0 in error case
 */
CCalendar *CMulticalendar::getSynchronizedCalendar()
{
    vector < CCalendar * >::iterator iter;
    vector < CCalendar * >vListCal;
    CCalendar* retCal = NULL;
    
    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();

    if (vListCal.size() == 0) {
      CAL_DEBUG_LOG("No Clendar present in DB\n");
      return retCal;
    }

    for (iter = vListCal.begin(); iter != vListCal.end(); iter++) {
      if ((*iter)->getCalendarType() == DEFAULT_SYNC) {
          retCal = *iter;
      } 
      else {
          delete(*iter);
          (*iter) = 0;
      }
    }

    return retCal;
}

/**
 * @param NONE 
 * @return CCalendar*
 * Retrieves existing Synchronized calendar.Calendar pointer thus obtained 
 * shouldn't be freed. It will be destroyed when CMulticalendar object is 
 * destroyed
 * Returns 0 in error case
 */
CCalendar *CMulticalendar::getBirthdayCalendar()
{
    vector < CCalendar * >::iterator iter;
    vector < CCalendar * >vListCal;
    CCalendar* retCal = NULL;
    
    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();

    if (vListCal.size() == 0) {
      CAL_DEBUG_LOG("No Clendar present in DB\n");
      return retCal;
    }

    for (iter = vListCal.begin(); iter != vListCal.end(); iter++) {
      if ((*iter)->getCalendarType() == BIRTHDAY_CALENDAR) {
          retCal = *iter;
      } 
      else {
          delete(*iter);
          (*iter) = 0;
      }
    }

    return retCal;
}





/**
 * @param NONE
 * @return string 
 * Function to retrieve the name of the Synchronized Calendar
 */
string CMulticalendar::getSynchronizedCalendarName()
{
    vector < CCalendar * >::iterator iter;
    vector < CCalendar * >vListCal;
    
    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();

    if (vListCal.size() == 0) {
      CAL_DEBUG_LOG("No Clendar present in DB\n");
      return "";
    }
    
    string szTemp;

    for (iter = vListCal.begin(); iter != vListCal.end(); iter++) {
      if ((*iter)->getCalendarType() == DEFAULT_SYNC) {
          szTemp = (*iter)->getCalendarName();
          break;
      }
    }
    
    releaseListCalendars(vListCal);

    return szTemp;
}

/**
 * @param NONE
 * @return string 
 * Function to retrieve the name of the Default Calendar
 */
string CMulticalendar::getDefaultCalendarName()
{
    vector < CCalendar * >::iterator iter;
    vector < CCalendar * >vListCal;
    
    /* Get the list of calendars. */
    vListCal = this->getListCalFromMc();

    if (vListCal.size() == 0) {
        CAL_DEBUG_LOG("No Clendar present in DB\n");
        return "";
    }
    
    string szTemp;

    for (iter = vListCal.begin(); iter != vListCal.end(); iter++) {
        if ((*iter)->getCalendarType() == DEFAULT_PRIVATE) {
            szTemp = (*iter)->getCalendarName();
            break;
        }
    }
    
    releaseListCalendars(vListCal);

    return szTemp;
}



/**
 * @param  string szFilenName  
 * @param  int& iEventCount Count of events in file 
 * @param  int& iTodoCount  Number of todos in file
 * @param  int& iJournalCount Number of journals in file
 * @param  int&  pErrorCode - error indicator 
 * @return bool status of operation  
 *
 * Function to check validy of ICS file and also report entriy count from file.
 *
 */
bool CMulticalendar::getICSFileInfo(string szFileName,
                    unsigned int &iEventCount,
                    unsigned int &iTodoCount,
                    unsigned int &iJournalCount,
                    int &pErrorCode)
{

    bool bRet = true;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    string stemp("file://");
    ifstream stream;
    unsigned int iDummy = 0;

    iDummy = (int) szFileName.find(stemp);
    if (iDummy != string::npos) {
	szFileName = szFileName.substr(iDummy + stemp.length());
    }


    CAL_DEBUG_LOG("File name is %s", szFileName.c_str());
    stream.open((char *) szFileName.c_str());

    if (stream.fail()) {
	    CAL_DEBUG_LOG("could not open the given ics file");
	    pErrorCode = CALENDAR_FILE_ERROR;
	    return false;
    }

    string szCont;
    string line;
    ICalConverter *pIcal = new ICalConverter();
    ASSERTION(pIcal);
    /* reading the contents of 
     * file in to a string*/

    while (getline(stream, line)) {
        cleanupIcsString(line);
        szCont = szCont + "\n" + line;
    }
    stream.close();

    CAL_DEBUG_LOG("contents are:%s", szCont.c_str());
    /*
     * Checking if the file is proper */

    if (!(pIcal->checkCount(szCont, BEGIN_CALENDAR, END_CALENDAR, iDummy))) {
	pErrorCode = CALENDAR_INVALID_ICSFILE;
	bRet = false;

    }

    if (!(pIcal->checkCount(szCont, BEGIN_EVENT, END_EVENT, iEventCount)) &&
		    (bRet)) {
	pErrorCode = CALENDAR_INVALID_ICSFILE;
	bRet = false;

    }
    if (!(pIcal-> checkCount(szCont, BEGIN_JOURNAL, END_JOURNAL, iJournalCount))
	    && (bRet)) {
	pErrorCode = CALENDAR_INVALID_ICSFILE;
	bRet = false;

    }

    if (!(pIcal->checkCount(szCont, BEGIN_TODO, END_TODO, iTodoCount)) &&
		    (bRet)) {

	pErrorCode = CALENDAR_INVALID_ICSFILE;
	bRet = false;

    }
    CAL_DEBUG_LOG(" Event, todo and Journal count are%d  %d %d  ",
          iEventCount, iTodoCount, iJournalCount);
    if ( (iEventCount == 0) && ( iTodoCount == 0) && (iJournalCount == 0)){
	pErrorCode = CALENDAR_INVALID_ICSFILE;
	bRet = false;
    }
	    

    delete pIcal;

    return bRet;



}

/* progressive API to  get ics 
 * file information
 */
bool CMulticalendar::getICSFileInfoProgressive(string szFileName,
                    unsigned int &iEventCount,
                    unsigned int &iTodoCount,
                    unsigned int &iJournalCount,
                    int &pErrorCode, bool *bFileReadingOver,
		    unsigned int iRequestLimit)
{

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    string stemp("file://");
    ifstream ics_file_stream;
    unsigned int iDummy = 0;
    bool bIsFirstRead = false;

    if(iFileOffset == 0) {
	    bIsFirstRead = true;
    }

    iDummy = (int) szFileName.find(stemp);
    if (iDummy != string::npos) {
	szFileName = szFileName.substr(iDummy + stemp.length());
    }


    CAL_DEBUG_LOG("File name is %s", szFileName.c_str());
    ics_file_stream.open((char *) szFileName.c_str());

    if (ics_file_stream.fail()) {
	    CAL_DEBUG_LOG("could not open the given ics file");
	    pErrorCode = CALENDAR_FILE_ERROR;
	    iFileOffset = 0;
	    return false;
    }

	/*
	 * Checking if the file is proper */

	const size_t len = sizeof(BEGIN_CALENDAR) - 1;
	char begin_file[len];
	ics_file_stream.read(begin_file, len);
	ics_file_stream.seekg(0, ios::beg);
	if (ics_file_stream.fail()) {
		CAL_DEBUG_LOG("Not able to seek given file \n");
		pErrorCode = CALENDAR_FILE_ERROR;
		iFileOffset = 0;
		return false;
	}

	if (0 != strncmp (begin_file, BEGIN_CALENDAR, len)) {
		CAL_DEBUG_LOG("BEGIN_CALENDAR mismatch: CALENDAR_INVALID_ICSFILE \n");
		pErrorCode = CALENDAR_INVALID_ICSFILE;
		iFileOffset = 0;
		return false;
	}

    string szCont;
    string szTextContent;
    string line;
    unsigned int iLimit = 0;
    bool bError = false;
    int iEventBegin = 0;
    int iJournalBegin = 0; 
    int iToDoBegin = 0;

    ics_file_stream.seekg (0, ios::end);
    if(ics_file_stream.fail()) {
            CAL_DEBUG_LOG("Not able to seek given ics file \n");
            pErrorCode = CALENDAR_FILE_ERROR;
            ics_file_stream.close();
	    iFileOffset = 0;
            return false;
    }
    unsigned int iFileLength = ics_file_stream.tellg();
    CAL_DEBUG_LOG("====Total file length is:%d\n", iFileLength);


    ics_file_stream.seekg(iFileOffset, ios_base::beg);
    if(ics_file_stream.fail()) {
	    CAL_DEBUG_LOG("Not able to seek given ics file \n");
	    pErrorCode = CALENDAR_FILE_ERROR;
	    iFileOffset = 0;
	    ics_file_stream.close();
	    return false;
    }
    iEventCount = 0;
    iTodoCount = 0;
    iJournalCount = 0;

    szCont.reserve(iFileLength);

    while (getline(ics_file_stream, line)) {
        cleanupIcsString(line);

//         CAL_DEBUG_LOG("line: '%s'", line.c_str());
        iFileOffset = ics_file_stream.tellg();
        /* no need to do it for every line we can do it only
         * for line of interest  */

        if ((line.compare(0, 6, "BEGIN:") == 0) ||
            (line.compare(0, 4, "END:") == 0))
        {
            CAL_DEBUG_LOG("Checking line %s", line.c_str());

            iLimit = iLimit + totalComponentsRead(line, bError,
                    iEventBegin, iJournalBegin, iToDoBegin, iEventCount,
                    iTodoCount, iJournalCount);
        }

        if (line[0] == ' ') { // Text content of Summary/deskription
            int len = szTextContent.length();

            if (len == 0) {
                CAL_DEBUG_LOG("Add first content line '%s'", line.c_str());
                szTextContent  = line;
            } else if (len < ICS_TEXT_FIELD_LIMIT) {
                CAL_DEBUG_LOG("Add content line '%s'", line.c_str());
                szTextContent = szTextContent + "\n" + line;
            } else {
//                 CAL_DEBUG_LOG("Text content reached limit. Ignore");
            }
        } else {
            if (!szTextContent.empty()) {
                CAL_DEBUG_LOG("Appending text content");
                szCont = szCont + "\n" + szTextContent + "\n" + line;
                szTextContent.clear();
            } else {
                szCont = szCont + "\n" + line;
            }
        }

        if((iLimit == iRequestLimit) || (bError == true)){
            break;
        }
    }


    if((iEventBegin != 0) || (iJournalBegin != 0) || (iToDoBegin != 0)) {
	    bError = true;
    }
    ics_file_stream.close();

    if(bError == true)
    {
	    pErrorCode = CALENDAR_INVALID_ICSFILE;
	    iFileOffset = 0;
	    return false;
    }

    if(iFileOffset == iFileLength) {
	    CAL_DEBUG_LOG("====End of file reached\n");
	    iFileOffset = 0;
	    *bFileReadingOver = true;
	    if(bIsFirstRead) {
		    if ( (iEventCount == 0) && ( iTodoCount == 0) && (iJournalCount == 0)){
			    pErrorCode = CALENDAR_INVALID_ICSFILE;
			    return false;
		    }
	    }
			    
	    return true;

    }

    //TODO: Remove this code
    if ( (iEventCount == 0) && ( iTodoCount == 0) && (iJournalCount == 0)){
	    pErrorCode = CALENDAR_INVALID_ICSFILE;
	    return false;
    }

    return true;
}
/** 
 * cancelImportIcsFileData
 */ 
void CMulticalendar::cancelImportIcsFileData()
{
	iFileOffset = 0;
}
/**
 * Function to fetch the count of component 
 * read from file 
 */
int CMulticalendar::totalComponentsRead(string szLine, bool &bError,
		int &iEventBegin, int &iJournalBegin, int &iToDoBegin,
		unsigned int &iEventCount, unsigned int &iTodoCount,
		unsigned int &iJournalCount)
{
	if(szLine.find(BEGIN_EVENT, 0) != string::npos) {
		if(iEventBegin != 0  || iJournalBegin != 0 || iToDoBegin != 0){
			bError = true;
			return 0;
		}
		iEventBegin = 1;
		return 0;
	}
	if(szLine.find(BEGIN_JOURNAL, 0) != string::npos) {
		if(iJournalBegin !=0 || iEventBegin != 0 || iToDoBegin != 0){

			bError = true;
			return 0;
		}
		iJournalBegin = 1;
		return 0;
	}
	if(szLine.find(BEGIN_TODO, 0) != string::npos) {
		if(iToDoBegin != 0 || iEventBegin != 0 || iJournalBegin != 0){
			bError = true;
			return 0;
		}
		iToDoBegin = 1;
		return 0;
	}

	if(szLine.find(END_EVENT, 0) != string::npos) {
		if(iEventBegin == 1) {
			iEventCount = iEventCount + 1;
			CAL_DEBUG_LOG("===The count if event is:%d\n", iEventCount);
			iEventBegin = 0;
			return 1;
		} else {
			bError = true;
			return 0;
		}
	}
	if(szLine.find(END_JOURNAL, 0) != string::npos) {
		if(iJournalBegin == 1) {
			iJournalCount = iJournalCount + 1;
			CAL_DEBUG_LOG("===The count if journal is:%d\n", iJournalCount);
			iJournalBegin = 0;
			return 1;
		} else {
			bError = true;
			return 0;
		}
	}

	if(szLine.find(END_TODO, 0) != string::npos) {
		if(iToDoBegin == 1) {
			iTodoCount = iTodoCount + 1;
			iToDoBegin = 0;
			return 1;
		} else {
			bError = true;
			return 0;
		}
	}
	return 0;
}


/**
 * importIcsFileData()
 * @param: string filename to be imported
 * @param: int iCalId Calendar in to which event have to be imported 
 * @param: string Eventid of added event 
 * @param: int * pointer to ErrorCode 
 * @return: string  cal name
 * This function parses the given ics file .All the
 * entries in the file are added to one calendar. 
 * This is a non progressive API
 */

bool CMulticalendar::importIcsFileData(string szFileName, int iCalId,
		string & eventId,int &iDuplicateCountTask, 
		int &iDuplicateCountEvent,int &iDuplicateCountJournal,int &pErrorCode)
{

    string szdefaultCalName(ICS_DEFAULT_CALENDAR_NAME);
    string calName;
    vector < CCalendar * >calList;

    iDuplicateCountTask = 0;
    iDuplicateCountEvent = 0;
    /* there is no duplicate check for journal so it will be always zero*/
    iDuplicateCountJournal = 0;

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    unsigned int iDummy;
    string stemp(ICS_FILENAME_SEPERATOR);

    iDummy = szFileName.find(stemp);
    if (iDummy != string::npos) {
	szFileName = szFileName.substr(iDummy + stemp.length());
    }

    ifstream stream;
    stream.open((char *) szFileName.c_str());

    if (stream.fail()) {
	CAL_DEBUG_LOG("Not able to open given ics file \n");
	pErrorCode = CALENDAR_FILE_ERROR;
	return false;
    }

     string szContents;
     string line;
     while (getline(stream, line)) {
        cleanupIcsString(line);
        szContents = szContents + "\n" + line;
     }
			
    vector < CComponent * >vCompList;
    ICalConverter *ical = new ICalConverter();
    ASSERTION(ical);
    vCompList = ical->icalVcalToLocal(szContents, UNKNOWN_TYPE, pErrorCode);
	
    /* Return false if icalVcalToLocal failed with some reason */
    if((vCompList.size() == 0) || (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)){ 
	delete ical;
	stream.close();
	return false;
    }
    
    /* Replacing it with the batch API */
    vector <string> IdAdded ;
    vector <string> duplicateIds;
   
   /*add components is a batch API which will add all the events /todos and journals in 
    * on error occured every thing will be roolled back */
    
    IdAdded = this->addComponents(vCompList,iCalId, duplicateIds,pErrorCode);

    if ( vCompList.size() == 1 && vCompList[0]->getType() == E_EVENT){
	eventId = vCompList[0]->getId();
	CAL_DEBUG_LOG("only one event in the list and its Id is %s ",eventId.c_str());
    }

    /*check how many entries amongst the list are 
     * duplicated  if so return the count 
     */
    if (pErrorCode == CALENDAR_ENTRY_DUPLICATED )
    {
	    unsigned int iCount = 0;
	    for (int k=0 ;k<(int)vCompList.size();k++)
	    {

		    for (iCount = 0 ;iCount < duplicateIds.size(); iCount++){
			    CAL_DEBUG_LOG("Component Id %s,Duplicate ID %s",
					    vCompList[k]->getId().c_str(),duplicateIds[iCount].c_str() );
			    if(vCompList[k]->getId() == duplicateIds[iCount]){
				    CAL_DEBUG_LOG("Match found for duplicate Id %s",duplicateIds[iCount].c_str());
				    if (vCompList[k]->getType() == E_EVENT)
					    ++iDuplicateCountEvent;			
				    else if (vCompList[k]->getType() == E_TODO)
					    ++iDuplicateCountTask;	
				    else
					    ;
			    }
		    }
	    }
    }

    
    vector<CComponent*>::iterator iter;
    /* freeing the memory allocated for the list*/
    for (iter = vCompList.begin(); iter!= vCompList.end(); iter++)
	delete (*iter);

    if ((pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) && (pErrorCode != CALENDAR_ENTRY_DUPLICATED))
    {
	    CAL_DEBUG_LOG("All entries were not imported properly ");
	    stream.close();
	    delete ical;
	    ical = 0;
	    return false;

    }
    CAL_DEBUG_LOG("%d events, %d tasks, %d journals  were duplicated  \n",
	    iDuplicateCountEvent,iDuplicateCountTask, iDuplicateCountJournal);
    stream.close();
    
    delete ical;
    ical =0;
    return true;
}

/**
 * importIcsFileDataProgressive()
 * @param: string filename to be imported
 * @param: int iCalId Calendar in to which event have to be imported 
 * @param: string Eventid of added event 
 * @param: int * pointer to ErrorCode 
 * @return: string  cal name
 * This function parses the given ics file .All the
 * entries in the file are added to one calendar. 
 */
bool CMulticalendar::importIcsFileDataProgressive(string szFileName, int iCalId,
		string & eventId,int &iDuplicateCountTask,
		int &iDuplicateCountEvent,int &iDuplicateCountJournal,
		int &pErrorCode, bool *bFileReadingOver,
		unsigned int iRequestLimit)
{

    string szdefaultCalName(ICS_DEFAULT_CALENDAR_NAME);
    string calName;
    vector < CCalendar * >calList;
    bool bRetVal = true;
    iDuplicateCountTask = 0;
    iDuplicateCountEvent = 0;
    /* there is no duplicate check for journal so it will be always zero*/
    iDuplicateCountJournal = 0;

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    unsigned int iDummy;
    string stemp(ICS_FILENAME_SEPERATOR);

    iDummy = szFileName.find(stemp);
    if (iDummy != string::npos) {
	szFileName = szFileName.substr(iDummy + stemp.length());
    }

    ifstream ics_file_stream;
    ics_file_stream.open((char *) szFileName.c_str());

    if (ics_file_stream.fail()) {
	CAL_DEBUG_LOG("Not able to open given ics file \n");
	pErrorCode = CALENDAR_FILE_ERROR;
	return false;
    }

    ics_file_stream.seekg (0, ios::end);
    unsigned int iFileLength = ics_file_stream.tellg();
    CAL_DEBUG_LOG("====Total file length is:%d\n", iFileLength);

    string szContents;
    string szTextContent;
    string line;
    unsigned int iLimit = 0;
    *bFileReadingOver = false;
    bool bError = false;
    int iEventBegin = 0;
    int iJournalBegin = 0;
    int iToDoBegin = 0;
    unsigned int iEventCount = 0;
    unsigned int iJournalCount = 0;
    unsigned int iTodoCount = 0;
    
    vector <string> IdAdded ;
    vector <string> duplicateIds;

    CAL_DEBUG_LOG("====the file offset is :%d\n", iFileOffset);
    ics_file_stream.seekg(iFileOffset, ios_base::beg);
    if(ics_file_stream.fail()) {
	CAL_DEBUG_LOG("Not able to seek given ics file \n");
	pErrorCode = CALENDAR_FILE_ERROR;
	ics_file_stream.close();
	return false;
    }

    while (getline(ics_file_stream, line)) {
        cleanupIcsString(line);
        iFileOffset = ics_file_stream.tellg();

        /* no need to do it for every line we can do it only
        * for line of interest  */
        if ((line.compare(0, 6, "BEGIN:") == 0) ||
            (line.compare(0, 4, "END:") == 0))
        {
            CAL_DEBUG_LOG("Checking line %s", line.c_str());

            iLimit = iLimit + totalComponentsRead(line, bError, 
                    iEventBegin, iJournalBegin, iToDoBegin, iEventCount, 
                    iTodoCount, iJournalCount);
        }


        if (line[0] == ' ') { // Text content of Summary/deskription
            int len = szTextContent.length();

            if (len == 0) {
                CAL_DEBUG_LOG("Add first content line '%s'", line.c_str());
                szTextContent  = line;
            } else if (len < ICS_TEXT_FIELD_LIMIT) {
                CAL_DEBUG_LOG("Add content line '%s'", line.c_str());
                szTextContent = szTextContent + "\n" + line;
            } else {
//                 CAL_DEBUG_LOG("Text content reached limit. Ignore");
            }
        } else {
            if (!szTextContent.empty()) {
                CAL_DEBUG_LOG("Appending text content");
                szContents = szContents + "\n" + szTextContent + "\n" + line;
                szTextContent.clear();
            } else {
                szContents = szContents + "\n" + line;
            }
        }

//      CAL_DEBUG_LOG("===The values of eventbegin, jou, todo begin: %d, %d, %d \n",
//                      iEventBegin, iJournalBegin, iToDoBegin);
//      CAL_DEBUG_LOG("===The value of limit is:%d\n", iLimit);

        if((iLimit == iRequestLimit) || (bError == true)) {
            break;
        }
    }
    ics_file_stream.close();
    CAL_DEBUG_LOG("=======Contents is  =\n %s", szContents.substr(0,1024).c_str());

    if(bError == true)
    {
	    CAL_DEBUG_LOG("wrong format of ics file \n");
	    pErrorCode = CALENDAR_INVALID_ICSFILE;
	    iFileOffset = 0;
	    return false;
    }

	    

    if(iFileOffset == iFileLength) {
	    CAL_DEBUG_LOG("====End of file reached\n");
	    iFileOffset = 0;
	    *bFileReadingOver = true;
    }


    vector < CComponent * >vCompList;
    ICalConverter *ical = new ICalConverter();
    ASSERTION(ical);
    CAL_DEBUG_LOG("Converting is started");
    vCompList = ical->icalVcalToLocal(szContents, UNKNOWN_TYPE, pErrorCode);
    CAL_DEBUG_LOG("Converting is finished");

    /* if the file reading is over and
     * it is the last component then we shouldn't consider it as 
     * an error 
     */
    if((vCompList.size() == 0) &&
       (*bFileReadingOver == true ) /*&& 
       (pErrorCode == CALENDAR_INVALID_ICSFILE  )*/)
    {
	/* here overwrite the error code 
	 * with CALENDAR_OPERATION_SUCCESSFUL
	 */
	    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    }
	else {
    /* Replacing it with the batch API */

   
   /*add components is a batch API which will add all the events /todos and journals in 
    * on error occured every thing will be roolled back */
    

    CAL_DEBUG_LOG("Components adding is started");
        IdAdded = this->addComponents(vCompList,iCalId, duplicateIds,pErrorCode);
    CAL_DEBUG_LOG("Components adding is finished");

    }

    if ( vCompList.size() == 1 && vCompList[0]->getType() == E_EVENT){
            eventId = vCompList[0]->getId();
    	    CAL_DEBUG_LOG("only one event in the list and its Id is %s ",eventId.c_str());
	    
    }

    /*check how many entries amongst the list are 
     * duplicated  if so return the count 
     */



    if (pErrorCode == CALENDAR_ENTRY_DUPLICATED )
    {
	    unsigned int iCount = 0;
	    for (int k=0 ;k<(int)vCompList.size();k++)
	    {

		    for (iCount = 0 ;iCount < duplicateIds.size(); iCount++){
			    CAL_DEBUG_LOG("Component Id %s,Duplicate ID %s",
					    vCompList[k]->getId().c_str(),duplicateIds[iCount].c_str() );
			    if(vCompList[k]->getId() == duplicateIds[iCount]){
				    CAL_DEBUG_LOG("Match found for duplicate Id %s",duplicateIds[iCount].c_str());
				    if (vCompList[k]->getType() == E_EVENT)
					    ++iDuplicateCountEvent;			
				    else if (vCompList[k]->getType() == E_TODO)
					    ++iDuplicateCountTask;	
				    else
					    ;
			    }
		    }
	    }
    }

    vector<CComponent*>::iterator iter;
    /* freeing the memory allocated for the list*/
    for (iter = vCompList.begin(); iter!= vCompList.end(); iter++)
	delete (*iter);
    if ((pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) && (pErrorCode != CALENDAR_ENTRY_DUPLICATED))
    {
	CAL_DEBUG_LOG("All entries were not imported properly ");
	iFileOffset = 0;
	bRetVal = false;
    }
    CAL_DEBUG_LOG("%d events, %d tasks  were duplicated  \n", iDuplicateCountEvent,iDuplicateCountTask);

    delete ical;
    return bRetVal;
}

/**
 * setNextAlarm()
 * @param : int id of the calendar
 * @param : string id of the component
 * @param : int errorcode
 * @return bool : returns SUCCESS/FAILURE of the function
 *
 * This function sets the alarm for the given component of the calendar.
 */
bool CMulticalendar::setNextAlarm(int iCalendarId, string sComponentId,int iType,
                  int &pErrorCode)
{
    bool bRet = false;
    
    
    CCalendar *pCal = this->getCalendarById(iCalendarId, pErrorCode);
    if (!pCal) {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	return false;
    }
    
    if (!pCal->IsShown())
    {
        CAL_DEBUG_LOG("Calendar is not visible. Silently ignore");
        pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
        return true;
    }
    
    if (!this->setAutocommitOff()) {
	CAL_DEBUG_LOG("Database is locked \n");
	pErrorCode = CALENDAR_DB_LOCKED;
	delete pCal;
	pCal = 0;
	return false;
    }
    
    bRet = pCal->setNextAlarm(sComponentId,iType, pErrorCode);

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
    {
	CAL_DEBUG_LOG("failure to set subsequent alarm");
	this->rollbackAllChanges();
	delete pCal;
	pCal = 0;
	return bRet;
    }
    
    changeCount++;
    if(iType == E_EVENT)
	changeFlag = changeFlag | EVENT_MODIFIED;
    else
	changeFlag = changeFlag | TODO_MODIFIED;
    
    compIdsModified.push_back(sComponentId);

    CalId = iCalendarId;

    this->commitAllChanges();
    delete pCal;
    pCal = 0;
    return bRet;
}

/**
 * getTodaysAllDay
 * @param pErrorcode reference to error code 
 * @return list of CComponent*
 *
 * Function to retrieve allday events on today 
 * 
 */

vector < CComponent * >CMulticalendar::getTodaysAllDay(int &pErrorCode)
{
    QueryResult *pQr = 0;
    CComponent *pEntry = 0;
    int iI_CompCount = 0;
    int iJ_CompCount = 0;
    int iK_CompCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    vector < CComponent * >vlistComp;
    time_t dayStart = time_get_time();
    struct tm pTimeToday ;
    int offsetUTC = 0;
    int iCalid = 0;

    memset (&pTimeToday, 0, sizeof (struct tm));
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    pDb = CCalendarDB::Instance();
    
    offsetUTC = time_get_time_diff(dayStart,this->getSystemTimeZone().c_str(),"UTC");
    time_get_remote(dayStart,this->getSystemTimeZone().c_str(),&pTimeToday);
    pTimeToday.tm_hour = 0;
    pTimeToday.tm_min = 0;
    pTimeToday.tm_sec = 0;
    dayStart = time_mktime(&pTimeToday, 0);
    dayStart = dayStart + offsetUTC;

    if (pDb == 0) {
	pErrorCode = CALENDAR_APP_ERROR;
	return vlistComp;
    }
    
    pQuery =
    sqlite3_mprintf(GET_TODAYS_ALLDAY_VIS, ENTRY_FIELD_DTSTART, ENTRY_FIELD_TZOFFSET,dayStart, 
		     ENTRY_FIELD_TYPE, E_EVENT,ENTRY_FIELD_ALLDAY, 1, CAL_FIELD_ID, CAL_FIELD_ID,
		     CALENDARS_TABLE,CAL_FIELD_ISVISIBLE);
    ASSERTION(pQuery);
    /* reset it back */
    dayStart = dayStart - offsetUTC;
    
    CAL_DEBUG_LOG("Query is : -- :%s ", pQuery);

    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if(pQr != NULL) {
    for (iI_CompCount = 0; iI_CompCount < pQr->iRow; iI_CompCount++) {
	pEntry = new CComponent();
	ASSERTION(pEntry);
	for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
	    iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

	    if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
		continue;

	    switch (iJ_CompCount) {
		case DB_COLUMN_ID2:
		    iCalid = atoi(pQr->pResult[iK_CompCount + iJ_CompCount]);
            pEntry->setCalendarId(iCalid);
		    break;
		case DB_COLUMN_ID1:
		    pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    break;

		case DB_COLUMN_ID3:

			pEntry->setSummary((string) pQr->
				pResult[iK_CompCount + iJ_CompCount]);
		    break;

		case DB_COLUMN_ID4:
		    pEntry-> setDateStart(atoi
				(pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    break;
		case DB_COLUMN_ID5:
		    pEntry->setTzid(pQr-> pResult[iK_CompCount + iJ_CompCount]);
		default:
		    break;
	    }
	    
	}
	pEntry->setAllDay(1);
	vlistComp.push_back(pEntry);
	CAL_DEBUG_LOG("CMulticalendar %d:getTodaysAllDay-> %s\n",
			    iI_CompCount,pEntry->toString().c_str());

    }

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    }
    CCalendar* pBirthdayCalendar = getBirthdayCalendar();
    if (NULL == pBirthdayCalendar) {
	    CAL_DEBUG_LOG("cant get Birthday Calendar\n");
	    return vlistComp;
    }
    if((!pBirthdayCalendar->IsShown())) {
	    return vlistComp;
    }
    int iCalId = pBirthdayCalendar->getCalendarId(); 
    CAL_DEBUG_LOG("Calendar id %d\n",iCalId);
    vector<CComponent*> vListBdayComp = pBirthdayCalendar->getComponents(E_BDAY, 
		    dayStart, dayStart + (24*60*60) - 1, pErrorCode);
    vector<CComponent*>::iterator iIter;
    for(iIter = vListBdayComp.begin(); iIter < vListBdayComp.end(); iIter++) {
	    CAL_DEBUG_LOG("Inside Birthday component");
	    (*iIter)->setCalendarId(iCalId);
	    vlistComp.push_back(*iIter);
    }
	    
    if (CALENDAR_OPERATION_SUCCESSFUL != pErrorCode) {
	    CAL_DEBUG_LOG("ErrorCode is %d\n",pErrorCode);
    }
    return vlistComp;
}

/**
 * @param vector<CComponent *>
 * @param  struct QueryResult pointer
 *
 * Function to retunrn CComponents from database based on the query result
 */
vector < CComponent * >CMulticalendar::popEventDetails(QueryResult * pQr)
{
    vector < CComponent * >listComp;
    CComponent *pEntry = 0;

    int iI_CompCount = 0;
    int iJ_CompCount = 0;
    int iK_CompCount = 0;
    int iCalid = 0;

    for (iI_CompCount = 0; iI_CompCount < pQr->iRow; iI_CompCount++) {
	pEntry = new CComponent();
	ASSERTION(pEntry);
	for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
	    iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

	    if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
	    continue;

	    switch (iJ_CompCount) {
		case DB_COLUMN_ID2:
		   	iCalid = atoi(pQr->pResult[iK_CompCount + iJ_CompCount]);
            pEntry->setCalendarId(iCalid);
		    break;
		case DB_COLUMN_ID1:
		    pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    break;

		case DB_COLUMN_ID3:
		    pEntry-> setType(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID4:
		    pEntry-> setFlags(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID5:
		    pEntry-> setDateStart(atoi
				(pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID6:
		    pEntry-> setDateEnd(atoi
				(pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID7:
			pEntry->setSummary((string) pQr->
				pResult[iK_CompCount + iJ_CompCount]);
		    break;

		case DB_COLUMN_ID8:
			pEntry->setLocation((string) pQr->
				pResult[iK_CompCount + iJ_CompCount]);
		    break;

		case DB_COLUMN_ID9:
			pEntry->setDescription((string) pQr->
				pResult[iK_CompCount + iJ_CompCount]);
		    break;

		case DB_COLUMN_ID10:
		    break;

		case DB_COLUMN_ID11:
			pEntry->setGUid(pQr->
				pResult[iK_CompCount + iJ_CompCount]);
		    break;
		case DB_COLUMN_ID12:
		    pEntry-> setUntil(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID13:
		    pEntry-> setAllDay(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;
		case DB_COLUMN_ID14:
		    pEntry-> setCreatedTime(atoi
				(pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    break;
		case DB_COLUMN_ID15:

		    pEntry-> setLastModified(atoi
				(pQr-> pResult[iK_CompCount +
				 iJ_CompCount]));
		    break;
	        
		case DB_COLUMN_ID16:
		    pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    break;

		default:
		    break;
	    }



	}
	 CAL_DEBUG_LOG("Component Count  %d: Contents %s\n",
			    iI_CompCount,pEntry->toString().c_str());



    if (((pEntry->getFlags() == HAS_RECURRENCE)
	     || (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
	    && (pEntry->getType() != E_JOURNAL))
        pEntry->getRecurrenceProperties();
    if (((pEntry->getFlags() == HAS_ALARM)
	     || (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
	    && (pEntry->getType() != E_JOURNAL))
        pEntry->getAlarmProperties();

    listComp.push_back(pEntry);

    }


    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return listComp;
}


/**
 * @param string Id of the event 
 * @param datestart of the event 
 * @param isPrev flag to check query from previous/next event
 * @param iCalid calendar Id of the component 
 * @return CComponent*
 *
 * Function used to fetch previous and next event in Database based on Date start
 * 
 */
CComponent *CMulticalendar::getPrevNextComponent(string szId,
                         time_t iDateStart,
                         bool isPrev, int &iCalid,
			 time_t &instance_time,
                         int &pErrorCode)
{
    CComponent *retval = 0;
    CComponent *current_component = 0;

    vector<CComponent*> components;
    vector <int> calendar_ids;
    int iSqliteError = 0;


    instance_time = 0;

    // Fetch current component form CalendarDB

    CCalendarDB *pDb = CCalendarDB::Instance();

    if (pDb == 0) {
        pErrorCode = CALENDAR_APP_ERROR;
        return 0;
    }

    char *pQuery = sqlite3_mprintf("select * from components  where id=%Q", szId.c_str());

    QueryResult *pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pQr) {
        vector < CComponent*> c = popEventDetails(pQr);

        // Query returns  not more that 1 record
        if (c.size() > 0)
        {
            current_component = c[0];
        }
    }

    if (current_component == 0)
    {
        CAL_ERROR_LOG("Failed to fetch current event. Aborting");
        return 0;
    }

    // Fetch all candidates

    int range_start, range_end;

    if (isPrev)
    {
        // From the beginning of the time...
        range_start = 0;

        // ... till current event starts (+1 to fetch  events started exactly)
        range_end = (int)iDateStart+1; 
    }
    else
    {
        // Fetch all components after specified event
        range_start = (int)iDateStart;
        range_end = DEFAULT_END_DATE;
    }

    components = getComponentsAllCalendars(range_start,
                                           range_end,
                                           65535, // FIXME to it propgressivelly
                                           0,
                                           calendar_ids,
                                           iSqliteError);

    CAL_DEBUG_LOG("Looking for %s for '%s' at %s",
             isPrev ? "Prev" : "Next",
             current_component->getSummary().c_str(),
             ctime(&iDateStart));

    CAL_DEBUG_LOG("Got %d candidates ", components.size());

    vector<CComponent*>::iterator candidate = components.end();
    time_t candidate_instance = -1;
    vector<CComponent*>::iterator comp;

    CAL_DEBUG_LOG("Looking for %s from %s", isPrev ? "Prev" : "Next", ctime(&iDateStart));

    // Filter candidates
    for(comp = components.begin();
        comp != components.end();
        comp++)
    {
        time_t comp_instance = -1;

        if (((*comp)->getType() != E_EVENT) && ((*comp)->getType() != E_BDAY))
        {
            // Ignore all except but tasks and bdays
            continue;
        }
        else
        if ((*comp)->getRecurrence())
        {
            vector<time_t> times;
            (*comp)->generateInstanceTimes(range_start, range_end, times);

            if (times.size() > 0)
            {
                // Find first suitabe instance
                if (isPrev) 
                {
                    // Get prev suitable instance
                    for(vector<time_t>::reverse_iterator it=times.rbegin();
                        it < times.rend();
                        ++it)
                    {
                        CAL_DEBUG_LOG("Validate instance of '%s' (#%s) at %s",
                                        (*comp)->getSummary().c_str(),
                                        (*comp)->getId().c_str(),
                                        ctime(&(*it)));

                        if (comparePrevNextComponent(current_component, *comp, iDateStart, *it) < 0)
                        {
                            comp_instance = *it;
                            break;
                        }
                    }
                }
                else
                {
                    // Get next suitable instance
                    for(vector<time_t>::iterator it=times.begin();
                        it < times.end();
                        ++it)
                    {
                        CAL_DEBUG_LOG("Validate instance of '%s' (#%s) at %s",
                                        (*comp)->getSummary().c_str(),
                                        (*comp)->getId().c_str(),
                                        ctime(&(*it)));

                        if (comparePrevNextComponent(current_component, *comp, iDateStart, *it) > 0)
                        {
                            comp_instance = *it;
                            break;
                        }
                    }
                }

                if (comp_instance < 0)
                {
                    CAL_ERROR_LOG("Coudn't find suitable candidate from recurrent event '%s'. Skip it",
                                  (*comp)->getSummary().c_str());
                    continue;
                }
            }
            else
            {
                CAL_DEBUG_LOG("Recurrent event '%s' have no instances in ranage. Skip it", (*comp)->getSummary().c_str());
                continue;
            }
        }
        else if ((*comp)->getId() == szId)
        {
            // Ignore current event
            CAL_DEBUG_LOG("Skip current event '%s'", (*comp)->getSummary().c_str());
            continue;
        }
        else
        {
            comp_instance = (*comp)->getDateStart();
        }

        // TODO Handle multiday events

        CAL_DEBUG_LOG("Check '%s' (#%s) at %s",
                        (*comp)->getSummary().c_str(),
                        (*comp)->getId().c_str(),
                        ctime(&comp_instance));

        if (isPrev)
        {
            if (comparePrevNextComponent(current_component, *comp, iDateStart, comp_instance) < 0) // comes before current
            {
                if (candidate == components.end())
                {
                    // First candidate
                    candidate = comp;
                    candidate_instance = comp_instance;

                    CAL_DEBUG_LOG("First candidate '%s' at %s",
                                (*candidate)->getSummary().c_str(),
                                ctime(&candidate_instance));
                }
                else
                if (comparePrevNextComponent(*candidate, *comp, candidate_instance, comp_instance) > 0) // comes after current candidate
                {
                    candidate = comp;
                    candidate_instance = comp_instance;

                    CAL_DEBUG_LOG("New candidate '%s' at %s",
                                (*candidate)->getSummary().c_str(),
                                ctime(&candidate_instance));
                }
            }
        }
        else
        {
            if (comparePrevNextComponent(current_component, *comp, iDateStart, comp_instance) > 0)    // comes after current time
            {
                if (candidate == components.end())
                {
                    // First candidate
                    candidate = comp;
                    candidate_instance = comp_instance;

                    CAL_DEBUG_LOG("First candidate '%s' at %s",
                                (*candidate)->getSummary().c_str(),
                                ctime(&candidate_instance));
                }
                else
                if (comparePrevNextComponent(*candidate, *comp, candidate_instance, comp_instance) < 0) // comes before current candidate
                {
                    candidate = comp;
                    candidate_instance = comp_instance;

                    CAL_DEBUG_LOG("New candidate '%s' at %s",
                                (*candidate)->getSummary().c_str(),
                                ctime(&candidate_instance));
                }
            }
        }
    }

    // prepare result if candidate is found
    if (candidate != components.end())
    {
        retval = *candidate;
        *candidate = 0;

        instance_time = candidate_instance;
        iCalid = retval->getCalendarId();
    }


    // Cleanup
    for(comp = components.begin();
        comp != components.end();
        comp++)
    {
        delete *comp;
    }

    delete current_component;

    // Some debug logging before return
    if (retval)
    {
        CAL_DEBUG_LOG("Result: '%s' (#%s) at %s",
                      retval->getSummary().c_str(),
                      retval->getId().c_str(),
                      ctime(&instance_time));
    }
    else
    {
        CAL_DEBUG_LOG("No prev/next event");
    }

    return retval;

#if 0


	QueryResult *pQr = 0;
	char *pQuery = 0;
	CCalendarDB *pDb = 0;
	int iSqliteError = 0;
	vector < CComponent * >listComp;
	CComponent *pComp = 0;
	CComponent *pEntry = 0;
	bool multipleday_present = false;
	bool selection_needed  = true;



	if (isPrev){
		CAL_DEBUG_LOG("\n***********************************************\n");
		CAL_DEBUG_LOG("                  PREV PROCEDURE CALLED           ");
		CAL_DEBUG_LOG("\n***********************************************\n");
	}
	else{
		CAL_DEBUG_LOG("\n***********************************************\n");
		CAL_DEBUG_LOG("                  NEXT PROCEDURE CALLED           ");
		CAL_DEBUG_LOG("\n***********************************************\n");
	}	
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	pDb = CCalendarDB::Instance();

	if (pDb == 0) {
		pErrorCode = CALENDAR_APP_ERROR;
		return 0;
	}

	/*
	 * This procedure finds upcoming event based on following rules
	 * 1. Multiple day events / Event that are already happening 
	 * 2. All day events on current day 
	 * 3. Normal events 
	 * 4. Recursive events (if 1,2 not found and 3 is found  then there could 
	 * be possibility that some recursive events instance might be already there)
	 */
	CAL_DEBUG_LOG("checking for Multiple day event ");

	pQuery = sqlite3_mprintf(SELECT_ALREADY_HAPPENING_EVENT,iDateStart,
			iDateStart,E_EVENT,HAS_RECURRENCE,HAS_RECURRENCE_ALARM ,szId.c_str());


	ASSERTION(pQuery);
	CAL_DEBUG_LOG("Query is : -- :%s ", pQuery);
	pQr = pDb->getRecords(pQuery,iSqliteError);
	pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	sqlite3_free(pQuery);

	if (pQr){
		listComp = this->popEventDetails(pQr);
		multipleday_present = true;
		if (listComp.size() == 1 )
		{
			multipleday_present = false;
			delete listComp[0];
			listComp.clear();
		}
		// pQR is released inside popEventDetails function
	}
# if 0

	if ( listComp.empty()){

		/*
		 *   Allday events if exist should be reported iafter mulitple day  
		 *      #define SELECT_PREV_NEXT_ALLDAY "select * from components where 
		 Id != \"%s\" and Allday =1 and DateStart =<  %d and DateEnd>= %d"
		 *      gives the allday event for the day denoted by iDateStart
		 */

		CAL_DEBUG_LOG("Checking for Allday Event ::");

		pQuery =
			sqlite3_mprintf(SELECT_PREV_NEXT_ALLDAY, iDateStart, iDateStart,szId.c_str());


		ASSERTION(pQuery);
		CAL_DEBUG_LOG("Query is : -- :%s ", pQuery);

		pQr = pDb->getRecords(pQuery,iSqliteError);
		pDb->sqliteErrorMapper(iSqliteError,pErrorCode);

		sqlite3_free(pQuery);
		if (pQr) 
			listComp = this->popEventDetails(pQr);
	
		if(listComp.size() == 1)
		{
			CAL_DEBUG_LOG("size of allday event is 1:)");
			delete listComp[0];
			listComp.clear();
		}
		else{
			
			int i =	listComp.size();
			if (!isPrev)
			{	
				/* if the component is last in the list
				 * and we are searching for the next one
				 * then we are searching at the wrong place 
				 * search for next event to avoid looping 
				 *   */
				if ((listComp.size()!= 0) &&
				(listComp[listComp.size() -1 ]->getId() == szId))
				{
					for ( i =0 ; i< listComp.size();i++)
						delete listComp[i];
					CAL_DEBUG_LOG("Deleted the Next Allday list ");
					listComp.clear();
				}


			}
			else
			{
				/* if the component is first  in the list
				 * and we are searching for the previous one 
				 * then we are searching at the wrong place 
				 * search for previous event to avoid looping 
				 *   */

				if ((listComp.size()!= 0) && (listComp[0]->getId() == szId)){
					for ( i =0 ; i < listComp.size();i++)
						delete listComp[i];
					listComp.clear();
					CAL_DEBUG_LOG("Deleted the Prev Allday list ");
				}	

			}
				
			
		}
	}

#endif 
	/* Allday event check is clubbed with normal event */
	if (listComp.empty()) {

		/* Find first the set of non recursive events in the list
		 * of these find the latest component  */
		CAL_DEBUG_LOG("Checking for Normal Event ::");

		if (isPrev)
			pQuery =
				sqlite3_mprintf(SELECT_PREV_COMPONENT,time_get_time(), time_get_time(),
					       	iDateStart,E_EVENT,HAS_RECURRENCE,HAS_RECURRENCE_ALARM,szId.c_str());

		else
			pQuery =
				sqlite3_mprintf(SELECT_NEXT_COMPONENT, time_get_time(), time_get_time(),
						 iDateStart, E_EVENT,HAS_RECURRENCE,HAS_RECURRENCE_ALARM,szId.c_str());

		ASSERTION(pQuery);
		CAL_DEBUG_LOG("Query is : -- :%s ", pQuery);

		pQr = pDb->getRecords(pQuery,iSqliteError);
		pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
		sqlite3_free(pQuery);
	    
		if (pQr == 0)
			selection_needed = false;
		else {
		        listComp = this->popEventDetails(pQr);
			// pQR is released inside popEventDetails function
		}
		
		if (listComp.size() == 1 ){
			delete listComp[0];
			listComp.clear();
		}
	}

	/* now we have the list of allday events *
	 * we need to figure out whether the allday event input 
	 * present in the list or not 
	 */
	if ( selection_needed ) {
	unsigned int i = 0;
	int found = NULLID;
	for (i = 0; i < listComp.size(); i++) {
		
		if (listComp[i]->getId() == szId.c_str()) {
			found = i;
			CAL_DEBUG_LOG("Input  event found at %d\n",found);
		}

	}
	if ((!isPrev) && (found != -1)) {
		if (found + 1 < (int) listComp.size()) {

			pComp = new CComponent();
			ASSERTION(pComp);
			CAL_DEBUG_LOG
				("Invoking the overloaded assignment operator for %d\n",
				 (found + 1));
			*pComp = *listComp[found + 1];
			iCalid = pComp->getCalendarId();

		}
	} 
	else if ((isPrev) && (found != 0) && (found != -1)) {
		// going to the previous event 

		pComp = new CComponent();
		ASSERTION(pComp);
		CAL_DEBUG_LOG
			("Invoking the overloaded assignment operator for %d\n",
			 (found + 1));
		*pComp = *listComp[found - 1];
		iCalid = pComp->getCalendarId();

	}
	for (i = 0; i < listComp.size(); i++) {
		CAL_DEBUG_LOG("Freeing the memory allocated for the list \n");
		delete listComp[i];
	}
	if (pComp)
	instance_time = pComp -> getDateStart();
	/* returing if it is multiple day*/
	if (multipleday_present )
		return pComp;
	/* if the event is normal one then 
	 * go to next step and find if there 
	 * is any recursive event before /after this 
	 * for prev and next case */
		pEntry = pComp;
		
	}

	
    string eventId;
    time_t  timebasedDateStart = 0;
    bool no_time_based_event = false;
    if (pEntry)	
    {
        timebasedDateStart = pEntry->getDateStart();
    }
    else
    {
	 no_time_based_event = true;
	 timebasedDateStart = 0;

    }
    CComponent * pEnRec =0 ;
    findRecursiveImpendingEventId(iDateStart,timebasedDateStart,
		    isPrev,eventId,instance_time,pErrorCode); 
    if (!eventId.empty())
    {
	iCalid =  this ->getCalendarIdForComponent(eventId, pErrorCode);
	CCalendar* pTemp = this->getCalendarById(iCalid,pErrorCode);
	if (!pTemp)
		return 0;
	if (pTemp->getCalendarType() == BIRTHDAY_CALENDAR )
		pEnRec = (CComponent*) pTemp->getBirthDayEvent(eventId, pErrorCode);

	else 
		pEnRec = (CComponent*) pTemp->getEvent(eventId,pErrorCode);
	delete pTemp;
	pTemp = 0;
   }
   
  /* decision to recturn which component has to be 
   * made here */
    
    if (no_time_based_event)
    {
	    CAL_DEBUG_LOG("No time Based event so returing ");
	    return pEnRec;
    }
    if (!pEnRec)
    {
	    CAL_DEBUG_LOG("No Reccursive event so returing ");
	    return pEntry;
    }
    
    if (isPrev){
    CAL_DEBUG_LOG(" PREV instance time is %ld event date start is %ld" , instance_time,pEntry ->getDateStart());
        if (instance_time > pEntry->getDateStart()) 
	{
		delete pEntry;
		pEntry = 0;
		return pEnRec;
	}
	else{
		delete pEnRec;
		pEnRec = 0;
	}
		
    }
    else
    {
            CAL_DEBUG_LOG(" NEXT instance time is %ld event date start is %ld" , instance_time,pEntry ->getDateStart());
	    if (instance_time < pEntry ->getDateStart()) 
	    {
		    delete pEntry;
		    pEntry = 0;
		    return pEnRec;
	    }
	    else{
		    delete pEnRec;
		    pEnRec = 0;
	    }
    }
    return pEntry;
#endif
}
/*
 * Function to find recursive impending events
 */

void CMulticalendar::findRecursiveImpendingEventId(time_t OriginalDateStart,
		time_t timebasedDateStart,
		bool isPrev,
		string &szEventId,
		time_t &instance_time,
		int &pErrorCode)
{
    CAL_DEBUG_LOG("\n CMulticalendar::findRecursiveImpendingEventId \n");
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    vector < CComponent * >listComp;
    CComponent *pEntry = 0;
    vector < time_t > instanceTimes ;
    map <time_t,string > timeEventIdMap;

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    pDb = CCalendarDB::Instance();

    if (pDb == 0) {
	pErrorCode = CALENDAR_APP_ERROR;
	return ;
    }

/*
#define SELECT_RECURSIVE_FUTURE_INSTANCE "select * from components where (Flags = %d OR Flags = %d) 
AND (Until >= %d OR Until = %d)"	
#define SELECT_RECURSIVE_PREVIOUS_INSTANCE "select * from components where (Flags = %d OR Flags = %d) 
AND datestart =< %d" 	
*/
	if (isPrev)

		/* find all Recursive events which start in date range from 0 ie ( 1/ 1/ 1970 ) to
		 * Original datestart */
    pQuery = sqlite3_mprintf(SELECT_RECURSIVE_PREVIOUS_INSTANCE,HAS_RECURRENCE,
		    		HAS_RECURRENCE_ALARM,OriginalDateStart);
		
	else
	        /* Find all recursive events for which  until > originalDateStart  || until = -1 
		 */
    pQuery = sqlite3_mprintf(SELECT_RECURSIVE_FUTURE_INSTANCE,HAS_RECURRENCE,
		    		HAS_RECURRENCE_ALARM,OriginalDateStart,NULLID);



	ASSERTION(pQuery);
	CAL_DEBUG_LOG("Query is : -- :%s ", pQuery);

	pQr = pDb->getRecords(pQuery,iSqliteError);
	pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	sqlite3_free(pQuery);

	if (pQr == 0)
	    return ;

	int iI_CompCount = 0;
	int iJ_CompCount = 0;
	int iK_CompCount = 0;
    int iCalid = 0;

	for (iI_CompCount = 0; iI_CompCount < pQr->iRow; iI_CompCount++) {
	pEntry = new CComponent();
	ASSERTION(pEntry);
	for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
	    iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

	    if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
		continue;

	    switch (iJ_CompCount) {
		case DB_COLUMN_ID2:
			iCalid = atoi(pQr->pResult[iK_CompCount + iJ_CompCount]);
            pEntry->setCalendarId(iCalid);
		    break;

		case DB_COLUMN_ID1:
		    pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    break;

		case DB_COLUMN_ID3:
		    pEntry-> setType(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID4:
		    pEntry-> setFlags(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID5:
		    pEntry-> setDateStart(atoi
				(pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID6:
		    pEntry-> setDateEnd(atoi
				(pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID7:
		    if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			pEntry->setSummary((string) pQr->
				pResult[iK_CompCount + iJ_CompCount]);
		    }
		    break;

		case DB_COLUMN_ID8:
		    if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
			pEntry->setLocation((string) pQr->
				pResult[iK_CompCount + iJ_CompCount]);
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
			pEntry->setGUid(pQr->
				pResult[iK_CompCount + iJ_CompCount]);
		    }
		    break;
		case DB_COLUMN_ID12:
		    pEntry->
			setUntil(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;

		case DB_COLUMN_ID13:
		    pEntry-> setAllDay(atoi
				(pQr->pResult[iK_CompCount + iJ_CompCount]));
		    break;
		case DB_COLUMN_ID14:
		    pEntry-> setCreatedTime(atoi
				(pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    break;
		case DB_COLUMN_ID15:

		    pEntry-> setLastModified(atoi
				(pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    break;
	        case DB_COLUMN_ID16:
		    pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    break;

		default:
		    break;
	    }


	}

	if (((pEntry->getFlags() == HAS_RECURRENCE)
		 || (pEntry->getFlags() == HAS_RECURRENCE_ALARM))
		&& (pEntry->getType() != E_JOURNAL))
	    pEntry->getRecurrenceProperties();
	/* For this entry find the list of times on which the event occurs
	 * Collect the information and put it inside a hash map 
	 *  */


	
	 if (isPrev) { 
		CAL_DEBUG_LOG("Searching Previous Instances ");
		pEntry->getInstanceTimes(0 , OriginalDateStart, instanceTimes);
	 }	
	 else{
		 
		CAL_DEBUG_LOG("Searching Future  Instances ");
		pEntry -> getInstanceTimes(OriginalDateStart,0, instanceTimes); 
	 }	 
	 
	 /* Collect these instance times in to a Map which is of type 
	  * <time_t ,Id > time_t is chosen as key because Map sorts the 
	  * key field automatically while inserting in to it 
	  */
	unsigned int iter = 0;
	while (iter < instanceTimes.size()) {
		time_t insttime = instanceTimes[iter];
		if (pEntry->getAllDay()) {
			string sTzid = pEntry->getTzid();
			if (sTzid.empty() == 0)
			sTzid = sTzid.substr(1);
			icaltimetype timeIcal = icaltime_from_timet_with_zone(insttime, pEntry->getAllDay(),
				icaltimezone_get_builtin_timezone(sTzid.c_str()));
			insttime = icaltime_as_timet_with_zone(timeIcal,this->getSystemTimeZoneAsIcalTimeZone());
			CAL_DEBUG_LOG(" Instance with current time zone = %s ", ctime(&insttime));
		}
		timeEventIdMap.insert(pair < time_t, string >
		       	(insttime, pEntry->getId()));
		++iter; 
	}

	 /* Clear the instanceTimes vector after the information is stored */

	instanceTimes.clear();

	 /* To reduce the memory consumption delete the entry immediately after we 
	  * get necessary information from the component 
	  */
	delete pEntry;
	pEntry = 0;
	

	}


	if (pQr) {
        sqlite3_free_table(pQr->pResult);
	    delete pQr;
	    pQr = 0;
	}


/* Now it is time to see of there is any time_t value in the 
 * required date range */
szEventId = this->getEventIdFromMap(timeEventIdMap,timebasedDateStart,OriginalDateStart,isPrev,instance_time);
}


/* get Id from Map */

string CMulticalendar::getEventIdFromMap(map <time_t,string > &timeEventIdMap,
		time_t &timebasedDateStart,time_t &OriginalDateStart ,bool isPrev,
		time_t &instance_time)
{
  string szEventId;
  map<time_t,string>::iterator iter;
  

  /* if it is prevous event at t1 then pick the value between
   *  timebasedDateStart < t1 < OriginalDateStart
   *  else if it is next event then pick the value between
   *
   *  if (timebasedDatestart !=0) OriginalDateStart < t1 < timebasedDatestart
   *  or
   *  if (timebasedDatestart == 0) OriginalDateStart < t1
   *  
   */
  if (isPrev){
	CAL_DEBUG_LOG("Fetching the previous instance :");
    for (iter = timeEventIdMap.begin() ; iter!= timeEventIdMap.end(); iter++){
		/* in this case the last instance 
		 * which satisfies the condition 
		 * should be taken, so not breaking after hte */
	   if (( (*iter).first > timebasedDateStart) &&
			   ((*iter).first < OriginalDateStart)){
		  szEventId =  (*iter).second;
		  instance_time =  (*iter).first;
	   }
	}
	  CAL_DEBUG_LOG("Previous instance is at %s",ctime(&instance_time));
  }
         
  else{	
	  CAL_DEBUG_LOG("Fetching the next instance ");
        for (iter = timeEventIdMap.begin() ; iter!= timeEventIdMap.end(); iter++)
	{
	   if (( (*iter).first < timebasedDateStart || timebasedDateStart == 0) &&
			   ((*iter).first > OriginalDateStart)){
		  szEventId =  (*iter).second;
		  instance_time =  (*iter).first;
		  CAL_DEBUG_LOG("Next instance is at %s",ctime(&instance_time));
		  break;
	   }
	}
  }
  return szEventId;	  

}

/**
 * function used by applet to retrieve the latest components
 * @param type Can be Event or todo
 * @param ioffset offset from which the event can be fetched
 * @param pErrorCode , error code to define the possible errros
 *
 *  Function to retrieve upcoming events
 * The usage of API - First
 *
 */

CComponent *CMulticalendar::getLatestComponent(int iType, time_t &startdate,
                           int &ioffset,
                           int &pErrorCode)
{

    QueryResult *pQr = 0;
    CComponent *pEntry = 0;
    int iI_CompCount = 0;
    int iJ_CompCount = 0;
    int iK_CompCount = 0;
    int iCalid = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    pDb = CCalendarDB::Instance();

    if (pDb == 0) {
	pErrorCode = CALENDAR_APP_ERROR;
	return 0;
    }

    if (iType == E_EVENT)
	pQuery =
        sqlite3_mprintf(SELECT_UPCOMING_EVENT_VIS, 1,
			time_get_time(), time_get_time(), E_EVENT, startdate, E_EVENT,
			CAL_FIELD_ID,CAL_FIELD_ID,CALENDARS_TABLE,CAL_FIELD_ISVISIBLE,ioffset);
    else if (iType == E_TODO)
	pQuery =
        sqlite3_mprintf(SELECT_UPCOMING_TODO_VIS, startdate, E_TODO, false,CAL_FIELD_ID,CAL_FIELD_ID,CALENDARS_TABLE,CAL_FIELD_ISVISIBLE,
                ioffset);
    else {

    pErrorCode = CALENDAR_INVALID_ARG_ERROR;
    return 0;
    }


    ASSERTION(pQuery);
    CAL_DEBUG_LOG("Query is : -- :%s ", pQuery);

    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    /* error Code will be set in the Mapper*/
    if (pQr != 0) {
	iI_CompCount = 0; 
	pEntry = new CComponent();
	ASSERTION(pEntry);
	for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
	    iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

	    if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
		continue;

	    switch (iJ_CompCount) {
		case DB_COLUMN_ID2:
		    /*WA*/
			iCalid = atoi(pQr->pResult[iK_CompCount + iJ_CompCount]);
            pEntry->setCalendarId(iCalid);
		    CAL_DEBUG_LOG("Calid --: %d", iCalid);
		    break;
		case DB_COLUMN_ID1:
		    pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    CAL_DEBUG_LOG("event id-- : %s", pEntry->getId().c_str());
		    break;

		case DB_COLUMN_ID3:

		    pEntry->setSummary((string) pQr->
			    pResult[iK_CompCount + iJ_CompCount]);
		    CAL_DEBUG_LOG("title : %s", pEntry->getSummary().c_str());
		    break;

		case DB_COLUMN_ID4:
		    pEntry-> setDateStart(atoi
			    (pQr-> pResult[iK_CompCount + iJ_CompCount]));
		    CAL_DEBUG_LOG("date start : %d",
			    (int) pEntry->getDateStart());
		    break;
		case DB_COLUMN_ID5:
		    pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
		    CAL_DEBUG_LOG("event Tzid-- : %s", pEntry->getTzid().c_str());
		    break;
		case DB_COLUMN_ID6:
		    pEntry->setAllDay(atoi
				    (pQr->pResult[iK_CompCount + iJ_CompCount]));
		    CAL_DEBUG_LOG("event isAllDay-- : %d", pEntry->getAllDay());
		    break;
		default:
		    break;
	    }
	}


	sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;

	if (pEntry->getDateStart() == -1) {
	    CAL_DEBUG_LOG(" Implies that query result is null\n");
	    delete pEntry;
	    pEntry = 0;
	}
    }
    if(iType == E_EVENT) {
        CComponent * pEnRec = 0;
        int iDateStart 		= 0;
        time_t instance_time 	= 0;
        time_t timebasedDateStart 	= 0;
        string eventId;
        iDateStart = startdate;
        if (pEntry)
            timebasedDateStart = pEntry->getDateStart();
        else
            timebasedDateStart = MAX_SUPPORTED_YEAR_IN_SEC;
        /* if there is no timebasedevevent then
         * timebasedDateStart will be 0 and the event will be expanded
         * one instance beyond the iDateStart
         */

        findRecursiveImpendingEventId(iDateStart,timebasedDateStart,
            false,eventId,instance_time,pErrorCode);
        if (!eventId.empty()) {
            int iCalid = 0;
            iCalid =  this ->getCalendarIdForComponent(eventId, pErrorCode);
            CCalendar* pTemp = this->getCalendarById(iCalid,  pErrorCode);
            if (!pTemp)
            {
                delete pEntry;
                return 0;
            }
            if (pTemp->IsShown()) {
                if (pTemp->getCalendarType() == BIRTHDAY_CALENDAR )
                    pEnRec = (CComponent*) pTemp->getBirthDayEvent(eventId, pErrorCode);
                else
                    pEnRec = (CComponent*) pTemp->getEvent(eventId,pErrorCode);
                if (pEnRec)
                    pEnRec->setCalendarId(iCalid);

                CAL_DEBUG_LOG(" NEXT instance time is %ld %s" , instance_time, ctime(&instance_time));
                int entry_time = 0;
                if (pEntry) {
                    entry_time = pEntry->getDateStart();
                }
                if((instance_time != 0) && ((entry_time == 0)  || (instance_time < entry_time))) {
                    CAL_DEBUG_LOG("Returning recursive event");
                    if (pEnRec){
                        CAL_DEBUG_LOG("Rec Entry Summary : %s", pEnRec->getSummary().c_str());
                        CAL_DEBUG_LOG("Rec Entry Time : %ld", pEnRec->getDateStart());
                        startdate = instance_time;
                        pEnRec->setDateStart(startdate);
                    }
                    if (pEntry)
                        delete pEntry;
                    pEntry = 0;
                    if (pTemp)
                        delete pTemp;
                    return pEnRec;
                }
                else {
                    ioffset += 1;
                    if(pEnRec)
                        delete pEnRec;
                    pEnRec = 0;
                }
            }
            if(pTemp)
                delete pTemp;
            pTemp = 0;
        }
    }
    CAL_DEBUG_LOG("Returning non-recursive event");
    if(pEntry) {
    CAL_DEBUG_LOG("Entry Summary : %s", pEntry->getSummary().c_str());
    CAL_DEBUG_LOG("Entry Time : %ld", pEntry->getDateStart());
    startdate = pEntry->getDateStart();
    }
    return pEntry;
}

/**
 * @param iCalId int
 * @return int
 *
 * This function is used to get the number of undone tasks.
 * This function returns the calendar id in outparam iCalId
 * if there is only one undone task.
 */
int CMulticalendar::getNumOfUndoneTasks(int *iCalId, int &pErrorCode)
{
    QueryResult *pQr = 0;
    int iI_CalCount = 0;
    int iJ_CalCount = 0;
    int iK_CalCount = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    *iCalId = -1;
    if (pDb == 0)
	return 0;
/**select * from Components where Status <>1 AND ComponentType =2 AND 
 * CalendarId in (select CalendarId from calendars where IsVisible =1);
 * NOTE: Status<>1 means not completed status. This shall be changed if it will be possible to have more than 1 completed status.
 */

    pQuery =
    sqlite3_mprintf(SELECT_TAB_COND_AND_VIS, COMPONENTS_TABLE,
            ENTRY_FIELD_STATUS, DB_COLUMN_ID2, ENTRY_FIELD_TYPE, DB_COLUMN_ID3,CAL_FIELD_ID,CAL_FIELD_ID,CALENDARS_TABLE,CAL_FIELD_ISVISIBLE);

    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CAL_DEBUG_LOG("pQuery is %s", pQuery);
    sqlite3_free(pQuery);

    /* Error Code will be set in the Mapper */

    if (pQr == 0)  
	return 0;
    
    int temp = 0;
    
    temp = pQr->iRow;
    if (temp == 1) {
        for (iI_CalCount = 0; iI_CalCount < pQr->iRow; iI_CalCount++) {

	    for (iJ_CalCount = 0; iJ_CalCount < pQr->iColumn; iJ_CalCount++) {
		iK_CalCount = pQr->iColumn + (iI_CalCount * pQr->iColumn);
		switch (iJ_CalCount) {
		    case DB_COLUMN_ID2:
			*iCalId =
			    atoi(pQr->pResult[iK_CalCount + iJ_CalCount]);
			break;
		    default:
			break;
		}
	    }
        }
    }
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;

    return temp;

}

/**
 * @param pErrorCode int
 * @return int
 *
 * This function is used to get the number of allday events for today.
 */
int CMulticalendar::getNumOfAllDayEventsForToday(int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0; 
    pDb = CCalendarDB::Instance();

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0)
	return 0;

    pQuery = sqlite3_mprintf(GET_ALLDAY_ENTRIES, ENTRY_FIELD_ID,
                 ENTRY_FIELD_DTEND, time(0),
                 ENTRY_FIELD_DTSTART, time(0),
                 ENTRY_FIELD_TYPE, E_EVENT, ENTRY_FIELD_ALLDAY,
                 1);

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("pQuery is %s", pQuery);

    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);

    sqlite3_free(pQuery);
    /* Error code will be set in the Mapper function above */
    if(pQr == 0)
	return 0;
    
    int temp = 0;
    sqlite3_free_table(pQr->pResult);
    temp = pQr->iRow;
    delete pQr;
    pQr = 0;
    return temp;
}

/**
 * @param NONE 
 * @return NONE
 * Function used to destroy CMulticalendar object
 */
void CMulticalendar::MCdestroy()
{
    if(pMc) {
	delete pMc;
	pMc = 0;
    }
}

/**
 * @param : iCalid Id of the Calendar for which event belongs
 * @param : stDate to do conditional fetching based on start date. -1 in 
 * case of no conditions
 * @param : endDate to do conditional fetching based on end date. -1 in 
 * case of no conditions
 * @param : limit limit for number of events fetched by the calenadr
 * @param int * pErrorCode error code if any
 * @return : Vector for CComponent
 * This function is used to get all the Events /Todos /Bdays  associated with 
 * particular Calendar progressively 
 * It is an overloaded API for get componets used to fetch Components in bursts
 * of specified size
 */

vector <CComponent *> CMulticalendar::getAllEventsTodos(int iCalid,
                                   int iStDate,
                                   int iEndDate,
                                   int iLimit,
                                   int iOffset,
                                   int &pErrorCode)
{

    vector < CComponent * >vCompList;
    CMulticalendar *pMc = CMulticalendar::MCInstance();
    CCalendar *pCal = pMc->getCalendarById(iCalid, pErrorCode);

    if (pCal) {

	vCompList = pCal->getAllEventsTodos(iStDate, iEndDate,
                               iLimit, iOffset, pErrorCode);
	delete pCal;
    }
    


    return vCompList;

}


/**
 * @param string iI 
 * @param error_code 
 * @return Calendar Id for component  0 in case of error 
 */
int CMulticalendar::getCalendarIdForComponent(string iI, int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();

    if (pDb == 0)
	return 0;


    pQuery =
    sqlite3_mprintf(SELECT_DOMAIN, ENTRY_FIELD_CALID, COMPONENTS_TABLE,
            ENTRY_FIELD_ID, iI.c_str());


    ASSERTION(pQuery);
    CAL_DEBUG_LOG("pQuery is %s", pQuery);

    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);
    /* error code will be set in the mapper function above */
    if (pQr == 0) {
	CAL_DEBUG_LOG("returning before processing\n");
	return 0;
    }
    int iCalid = 0;
    iCalid = atoi(pQr->pResult[1]);

    CAL_DEBUG_LOG("Calendar Id obtained is%d", iCalid);

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return iCalid;


}
/**
 *  utility API can be used by applications 
 * using backend to get the current timezone
 * */
string CMulticalendar::getSystemTimeZone()
{
    if(szSystemTimezone.empty()) {

        char *zone = new char[DEFAULT_SIZE];
        int   nchars = 0;
        ASSERTION(zone);
        memset(zone,0,DEFAULT_SIZE);
        nchars = time_get_timezone(zone, DEFAULT_SIZE);
        if (nchars > 0 && nchars < DEFAULT_SIZE) {
           szSystemTimezone = zone;
           CAL_DEBUG_LOG("Current timezone is %s\n", zone);
        }
    
        /*hack to ensure libical works properly */
    
        if (szSystemTimezone.find("Calcutta") != string ::npos)
            szSystemTimezone.replace(szSystemTimezone.find("Calcutta"),8,"Kolkata");
    
        delete [] zone;
    }
    return szSystemTimezone;
}

time_t CMulticalendar::getSystemTimeShift()
{
    static time_t shift = -1;

    if( shift == -1)
        shift = time_get_time_diff(time_get_time(), getSystemTimeZone().c_str(), "UTC"); 

    return shift;
}


/**
 * getSystemTimeZoneAsIcalTimeZone
 *
 */
icaltimezone* CMulticalendar::getSystemTimeZoneAsIcalTimeZone()
{
  string szZone(getSystemTimeZone());
  unsigned int i = 0;
  i = szZone.find(":",0); 
  
  if(i != string::npos) 
  szZone = szZone.substr(i+1);
  CAL_DEBUG_LOG(" Timezone after parsing is %s",szZone.c_str());

  if(szZone.empty()){
	  CAL_DEBUG_LOG("Something is terribly wrong with the environment");
	  return 0;
  }
  
   icaltimezone* pTz = icaltimezone_get_builtin_timezone(szZone.c_str());
   if(!pTz)
   {
	;
	CAL_DEBUG_LOG("Requested timezone not found in the system %s",szZone.c_str());
   }
   return pTz;
}
/**
 * This function is used esp for debugging 
 * logs time in microsec
 */
void CMulticalendar:: printCurrentTime()
{
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm=localtime(&tv.tv_sec);
   CAL_DEBUG_LOG (" %d:%02d:%02d %ld \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
}

/* batch APIs for adding entries *
*/
vector <string> CMulticalendar::addComponents(
	vector<CComponent*> &compList,
	int iCalId, 
	vector <string> &duplicateIds,
	int &pErrorCode)
{

    vector <string>  		idList;

    if (compList.size() == 0)
    {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	CAL_DEBUG_LOG("Input list of Components is empty");
	return idList;
    }

    /** fetch the Calendar for which the Component has to be added */
    CCalendar *pCal = 0;
    pCal = this->getCalendarById(iCalId, pErrorCode);
    if (!pCal)
    {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	CAL_DEBUG_LOG("Calendar with Id %d doesn't exist in DB \n", iCalId);
	return idList;
    }
    /* turn-off the autocommit feature */	
    if (!this->setAutocommitOff()) {
	    CAL_DEBUG_LOG("Database is locked \n");
	    pErrorCode = CALENDAR_DB_LOCKED;
	    delete pCal;
	    pCal = 0;
	    return idList;
    }


    /** Call the CCalendar batch API here **/
    idList = pCal->addComponents(compList,duplicateIds,pErrorCode);
    if ((pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) &&
		    (pErrorCode != CALENDAR_ENTRY_DUPLICATED))
    {
	    CAL_DEBUG_LOG("Error obtained while adding in batch,erroCode is  %d",pErrorCode); 
	    this->rollbackAllChanges();
	    delete pCal;
	    pCal = 0;
	    return idList;
    }

    /* set the error flag for maesync to 
     * realize that there are duplicate IDs 
     */
    if (duplicateIds.size()){
	CAL_DEBUG_LOG("There are duplicate entries in the list you have passed ");
	pErrorCode = CALENDAR_ENTRY_DUPLICATED;
    }
    CalId = iCalId;

    changeCount = changeCount + idList.size();
    changeFlag = changeFlag | EVENT_ADDED;

    for (vector<string>::iterator iter=idList.begin();
         iter != idList.end();
         ++iter)
    {
        compIdsAdded.push_back(*iter);
    }


    /* commit in successful scenario */
    this->commitAllChanges();
    /* free memory allocated for CCalendar*/
    delete pCal;
    pCal = 0;

    return idList;
}

/* batch API to Modify Components */

void CMulticalendar::modifyComponents(
	vector<CComponent*> &compList,
	int iCalId, 
	int &pErrorCode)

{
    if (compList.size() == 0)
    {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	CAL_DEBUG_LOG("Input list of Components is empty");
	return;
    }

    /** fetch the Calendar for which the Component has to be added */
    CCalendar *pCal = 0;
    pCal = this->getCalendarById(iCalId, pErrorCode);
    if (!pCal)
    {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	CAL_DEBUG_LOG("Calendar with Id %d doesn't exist in DB \n", iCalId);
	return ;
    }
    /* turn-off the autocommit feature */
    if (!this->setAutocommitOff()) {
	    CAL_DEBUG_LOG("Database is locked \n");
	    pErrorCode = CALENDAR_DB_LOCKED;
	    delete pCal;
	    pCal = 0;
	    return ;
    }

    /** Call the CCalendar batch API here **/
    pCal->modifyComponents(compList,pErrorCode);
    /* Roll Back in Error scenario */
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
    {
	    CAL_DEBUG_LOG("Error obtained while adding in batch,erroCode is  %d",pErrorCode); 
	    this->rollbackAllChanges();
	    delete pCal;
	    pCal = 0;
	    return ;
    }

    CalId = iCalId;
    changeCount = changeCount + compList.size();
    changeFlag = changeFlag | EVENT_MODIFIED;

    for (vector<CComponent*>::iterator iter = compList.begin();
         iter != compList.end();
         ++iter)
    {
        compIdsModified.push_back((*iter)->getId());
    }


    /* commit in successful scenario */
    this->commitAllChanges();
    delete pCal;
    pCal = 0;

    return;
}

/* batch API to delete Components */
void CMulticalendar::deleteComponents(
	vector<string> &idList,
	int iCalId, 
	int &pErrorCode)
{

    if (idList.size() == 0)
    {
	pErrorCode = CALENDAR_INVALID_ARG_ERROR;
	CAL_DEBUG_LOG("Input list of Components is empty");
	return;
    }

    /** fetch the Calendar for which the Component has to be added */
    CCalendar *pCal = 0;
    pCal = this->getCalendarById(iCalId, pErrorCode);
    if (!pCal)
    {
	pErrorCode = CALENDAR_DOESNOT_EXISTS;
	CAL_DEBUG_LOG("Calendar with Id %d doesn't exist in DB \n", iCalId);
	return ;
    }

    /* turn-off the autocommit feature */	
    if (!this->setAutocommitOff()) {
	    CAL_DEBUG_LOG("Database is locked \n");
	    pErrorCode = CALENDAR_DB_LOCKED;
	    delete pCal;
	    pCal =0;
	    return ;
    }

    /** Call the CCalendar batch API here **/
    pCal->deleteComponents(idList,pErrorCode);
    /* Roll Back in Error scenario */
    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
    {
	    CAL_DEBUG_LOG("Error obtained while adding in batch,erroCode is  %d",pErrorCode); 
	    this->rollbackAllChanges();
	    delete pCal;
	    pCal =0;
	    return;
    }

    CalId = iCalId;
    changeCount++;
    changeFlag = changeFlag | CALENDAR_DELETED;

    for (vector<string>::iterator iter=idList.begin();
         iter != idList.end();
         ++iter)
    {
        compIdsDeleted.push_back(*iter);
    };


    /* commit in successful scenario */
    if(this->commitAllChanges())
    {
        CCalendarDB *pCalDb = CCalendarDB::Instance();
        if (pCalDb == 0) 
        {
            CAL_DEBUG_LOG("invalid CalendarDB pointer ");
        }
        else
        {
            string szMessage;
            std::stringstream ss;
            string szIds;

            CalId = iCalId;
            changeCount = changeCount + idList.size();
            szMessage.append(AppName);
            szMessage.append(MSG_SEPERATOR);
            ss << CalId;
            szMessage.append(ss.str());
            szMessage.append(E_DELETED);
            conpIdListToString(compIdsDeleted, szIds);
            szMessage.append(intToString(changeCount));
            szMessage.append(":");
            szMessage.append(szIds);
            pCalDb->sendDBusMessage(szMessage);
            changeFlag = 0;
            changeCount = 0;
            compIdsDeleted.clear();
        }
    }
    delete pCal;
    pCal =0;

    return ;
}



int CMulticalendar::getVisibleCalendarCount(int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();

    if (pDb == 0)
	return 0;
    pQuery =
	sqlite3_mprintf(SELECT_NOS_VISIBLE_CAL, CALENDARS_TABLE,
		CAL_FIELD_ISVISIBLE,CAL_VISIBLE);


    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CAL_DEBUG_LOG("pQuery is %s", pQuery);
    sqlite3_free(pQuery);
    if (pQr == 0) {
	CAL_DEBUG_LOG(" Error in getting Count of Visible calendars Returnig from here ");
	return 0;
    }
    int temp = 0;
    if (pQr) {
	temp = pQr->iRow;
    sqlite3_free_table(pQr->pResult);
	delete pQr;
	pQr = 0;

    }
    return temp;
}


/**
 *  @param  int Value 
 *  @return NONE 
 *  
 * function used to enable and disable calendar logs 
 * pass 0 to disable calendar logs 
 * pass 1 ro enable calendar logs 
 */
void CMulticalendar::enableDisableCalendarLogs(int value )
{
 enable_logging = value;
}

/**
 *  moving the calendardb file from 
 * /etc/skel/.claendar/calendardb
 **/
bool CMulticalendar::moveFileFromSkel(string oldpath, string newpath)
{
	char* pTemp = 0;

	asprintf(&pTemp,"cp %s %s",oldpath.c_str(),newpath.c_str());
	CAL_DEBUG_LOG("System Command is %s",pTemp);
	system(pTemp);
	if(pTemp)
		free(pTemp);
	pTemp = 0;
	return true;
}

/**
 *  API to see if DB file already exists 
 **/
bool CMulticalendar::checkFileExists(string szPath )
{

	struct statfs file_statistics ;
	memset (&file_statistics, 0, sizeof (struct statfs));
	
	if(statfs(szPath.c_str(), &file_statistics) != 0){
		/* check if errno is according to 
		 * file not present */
		if (errno == ENOENT){
			CAL_DEBUG_LOG("%s doesn't exist ",szPath.c_str());
			return false;
		}

		else
			CAL_DEBUG_LOG("%s, exist some other error with errno: %d" ,
					szPath.c_str(),errno);
	}
	else
		CAL_DEBUG_LOG("%s exists",szPath.c_str());

	return true; 

}

/* uI needs to call this API progressively 
 * so that events are shown in chunks in UI */
vector<CComponent*>  CMulticalendar::getEventInList(vector<string> listId ,vector<int>& vCalId, int & pErrorCode)
{
    CEvent *event = 0;
    CAlarm *pAlarm = 0;
    CRecurrence *pRec = 0;
    vector <CComponent*> listEvent;
    QueryResult *pQr = 0;
    int iI_EventCount = 0;
    int iJ_EventCount = 0;
    int iK_EventCount = 0;
    CCalendarDB *pDb = 0;
    int iCalId = 0;
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
	pErrorCode = CALENDAR_APP_ERROR;
	return listEvent;
    }
    string szQuery(SELECT_EVENTS_IN_LIST);

    for (unsigned int i = 0 ; i<listId.size(); i++ )
    {
	szQuery = szQuery+  "\""+ listId[i] + "\""+ "," ;
    }
    

    szQuery = szQuery.substr(0,szQuery.length() -1 );
    szQuery = szQuery + ")" + "ORDER BY " + "CalendarId";
    CAL_DEBUG_LOG("query string is %s", szQuery.c_str());
    
    /**
     * this function is responsible to retrieve the values stored in 
     * Component table and uses the set function to add values in 
     * to event object
     **/
    pQr = pDb->getRecords((char*)szQuery.c_str(),iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);

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
	if (!event || !pAlarm || !pRec)  {
        delete event;
        delete pAlarm;
        delete pRec;
        pErrorCode = CALENDAR_APP_ERROR;
        return listEvent;
    }
	for (iJ_EventCount = 0; iJ_EventCount < pQr->iColumn; iJ_EventCount++) {
	    iK_EventCount = pQr->iColumn + (iI_EventCount * pQr->iColumn);

	    switch (iJ_EventCount) {

		case DB_COLUMN_ID1:
		    event->setId(pQr->pResult[iK_EventCount + iJ_EventCount]);
		    break;
		case DB_COLUMN_ID2:
		    iCalId = atoi(pQr->pResult[iK_EventCount + iJ_EventCount]);
		    vCalId.push_back(iCalId);
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

		case DB_COLUMN_ID18:
		    event->
			setType(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID19:

		    if (pQr->pResult[iK_EventCount + iJ_EventCount] != 0) {
			event->setClas(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;


		case DB_COLUMN_ID20:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setGeo(pQr->pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;


		case DB_COLUMN_ID21:
		    event->setPriority(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;


		case DB_COLUMN_ID22:
		    event-> setDateStamp(atoi
			    (pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID23:
		    event-> setSequence(atoi
			    (pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID24:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setTransparency(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID25:
		    event->
			setUid(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
		    break;


		case DB_COLUMN_ID26:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setUrl((pQr->pResult[iK_EventCount + iJ_EventCount]));
		    }
		    break;

		case DB_COLUMN_ID27:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			vector <string> vAtt;
			vAtt.push_back((pQr->pResult[iK_EventCount + iJ_EventCount]));
			event->setAttachments(vAtt);
		    }
		    break;

		case DB_COLUMN_ID28:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setCategories(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID29:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setComments(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID30:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setContact(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID31:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setRelated(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID32:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			event->setResources(pQr->
				pResult[iK_EventCount + iJ_EventCount]);
		    }
		    break;

		case DB_COLUMN_ID33:
		    break;


		case DB_COLUMN_ID34:
		    break;

		case DB_COLUMN_ID36:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setTrigger(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID37:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setRepeat(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID38:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setDuration(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID39:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setAction(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID40:

		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
			vCookie.push_back(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
			pAlarm->setCookie(vCookie);
		    }
		    break;

		case DB_COLUMN_ID41:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount]) 
			pAlarm->setAttach(pQr-> pResult[iK_EventCount + iJ_EventCount]);
		    break;
		case DB_COLUMN_ID43:
		    
		    if(pQr->pResult[iK_EventCount + iJ_EventCount]){
			vRRule = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount], EXCLAMATION);
			pRec->setRrule(vRRule);
		    }
		    break;

		case DB_COLUMN_ID44:
		    
		    if(pQr->pResult[iK_EventCount + iJ_EventCount]){
			vRdate = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],SEMI_COLON);
			pRec->setRDays(vRdate);
		    }

		    break;

		case DB_COLUMN_ID45:

		    if(pQr->pResult[iK_EventCount + iJ_EventCount]){
			vEdate = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],SEMI_COLON);
			pRec->setEDays(vEdate);
		    }
		    break;

		case DB_COLUMN_ID46:

		    if(pQr->pResult[iK_EventCount + iJ_EventCount]){
			vERule = event->extractSubStrings(pQr-> pResult[iK_EventCount + iJ_EventCount],EXCLAMATION);;
			pRec->setErule(vERule);
		    }
		    break;

		case DB_COLUMN_ID47:
		    if (pQr->pResult[iK_EventCount + iJ_EventCount])
			pRec->setRecurId(atoi(pQr-> pResult[iK_EventCount + iJ_EventCount]));
		    break;

		case DB_COLUMN_ID48:
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

	delete pRec ;
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


int CMulticalendar::getComponentCount(int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    CCalendarDB *pDb = 0;
    int iSqliteError = 0;
    pDb = CCalendarDB::Instance();
    int count = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0)
	return NULLID;
    pQuery =
	sqlite3_mprintf(SELECT_COUNT, COMPONENTS_TABLE);

    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery,iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    CAL_DEBUG_LOG("pQuery is %s", pQuery);
    sqlite3_free(pQuery);
    if (pQr == 0) {
	    if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL){
		    CAL_DEBUG_LOG("DB is empty ");
		    return 0;
	    }
	    CAL_DEBUG_LOG(" Error in getting Count of Components  Returnig from here ");
	    return NULLID;
    }

    count = atoi(pQr->pResult[ONE]);
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return count;
}


// 
//TODO: Timezone adjustments to be made
//TODO: It is possible that there was an error when
//reading the data base or it is possible that
//no entries are there for the specified range. In that case
//iStartDate and/or iEndDate will be same as currDate (not currTime)
//Ui needs to check the error code when startdate and end date
//are same as current date to decipher the actual reason


bool CMulticalendar::getRequiredDates(time_t iCurrTime, int iFutureDateNum,
		int iBackDateNum, int &iStartDate, int &iEndDate,vector<string> &vIdList,
		int &pErrorCode)
{
	CUtility objUtility;
	time_t iCurrDate = 0;
	string szIdString;

	if(iCurrTime == 0) {
		pErrorCode = CALENDAR_APP_ERROR;
		return false;
	}
		
	iCurrDate = objUtility.getDateFromTime(iCurrTime);

	iEndDate = this->getDateRange(iCurrDate, iFutureDateNum, true,
				szIdString,pErrorCode);
	iStartDate =this-> getDateRange(iCurrDate, iBackDateNum, false,
			 	szIdString,pErrorCode);

	CAL_DEBUG_LOG("List Id string is %s",szIdString.c_str());
	/*TODO: remove duplicates from Id string */
	
	
	/* create a vector from comma seperated string */
	vIdList = objUtility.parseIds(szIdString);
	

	//TODO: Better if this logic is at UI level
	//If are no entries in future then set iEndDate
	//to today's date
	if(iEndDate < iCurrDate) {
	       iEndDate = iCurrDate;
	}

	//Similarly if no entries in the past
	//set iStartDate as today's date
	if((iStartDate == 0) || (iStartDate > iCurrDate)) {
		iStartDate = iCurrDate;
	}

	//End Date in the above scenario is set as
	//Beginning of a day example 1 Jan 2000 00:00:00
	//But the whole day needs to be considered when
	//UI queries hence adding 86400 - 1; which will be
	//1 Jan 2000 23:59:59
	//TODO: To check with UI team if is ok for iEndDate
	//is more than iCurrTime when no entries are there
	//after iCurrTime. iCurrTime may be 5:00pm 12th May 2008
	//But when no events are found after 12th May, the end
	//date will be returned as 23:59:59 12th May 2008
	
	iEndDate = iEndDate + (ONEDAY - 1);

	return true;
		
}

// 
int CMulticalendar::getDateRange(time_t currTime, int iNum, bool iOperation,
				string &szTemp,	int &pErrorCode)
{
	char *pQuery = 0;
	QueryResult *pQr = 0;
	int iSqliteError = 0;
	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	int iReqDate = 0;
	string szTempLocal ;

	if((currTime == 0) || (iNum == 0)) {
		pErrorCode = CALENDAR_APP_ERROR;
		return iReqDate;
	}

	CCalendarDB *pDb = CCalendarDB::Instance();
	if(pDb == 0) {
		CAL_DEBUG_LOG("invalid caldb pointer when getting date range");
		pErrorCode = CALENDAR_APP_ERROR;
		return iReqDate;
	}

	//To find dates greater than current date, iOperation is set as true
	//To find dates lesser than current date iOperation is sent as false
	if(iOperation) {
		pQuery = sqlite3_mprintf(SELECT_LIMITEDCACHE, currTime, iNum);
	} else {
		pQuery = sqlite3_mprintf(SELECT_LIMITEDCACHE2, currTime, iNum);
	}

	ASSERTION(pQuery); 

	pQr = pDb->getRecords(pQuery,iSqliteError);
	pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	CAL_DEBUG_LOG("query is given by  %s", pQuery);

	sqlite3_free(pQuery);
	if (pQr == 0) {
		CAL_DEBUG_LOG("Query result is null when getting date range");
		return iReqDate;
	}

	iReqDate = atoi(pQr->pResult[2]);

	/* Here iterate through the  query result to 
	 * know the component Ids */
	int iI_ColCount =0;
	int iJ_ColCount =0;
	int iK_ColCount =0;

	for (iI_ColCount = 0; iI_ColCount < pQr->iRow; iI_ColCount++) {
		for (iJ_ColCount = 0; iJ_ColCount < pQr->iColumn; iJ_ColCount++) {
			iK_ColCount = pQr->iColumn + (iI_ColCount * pQr->iColumn);

			if ((pQr->pResult[iK_ColCount + iJ_ColCount]) == 0)
				continue;

			switch (iJ_ColCount) {

				case DB_COLUMN_ID2:
					szTempLocal = pQr->pResult[iK_ColCount + iJ_ColCount];
					if (szTemp.empty() == 0)
						szTempLocal = szTempLocal.substr(1);
					szTemp += szTempLocal; 
					break ;
				default :
					break;
			}
		}
	}
	/* now szTemp has all IDs we are interested in */

	CAL_DEBUG_LOG("The date range is:%d\n",iReqDate);

	sqlite3_free_table(pQr->pResult);
	delete pQr;
	return iReqDate;
}

CEvent *CMulticalendar::localBdayEventFromRemoteId(string szExId,int &pErrorCode)
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
    vector <long> vCookie;
    vector <string> vERule;
    vector <string> vEdate;
    vector <string> vRdate;
    vector <string> vRRule;
    pDb = CCalendarDB::Instance();
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0) {
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
    pQuery = sqlite3_mprintf(SELECT_BDAYS_BY_EXT_ID,szExId.c_str());
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
        CAL_DEBUG_LOG("Database corruption, two rows found with same entry id.\n");
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

   iI_EventCount = 0;
   for (iJ_EventCount = 0; iJ_EventCount < pQr->iColumn; iJ_EventCount++) {
       iK_EventCount = pQr->iColumn + (iI_EventCount * pQr->iColumn);
       if (pQr->pResult[iK_EventCount + iJ_EventCount] == NULL)
	       continue;
       CAL_DEBUG_LOG ("In switch case");
       switch (iJ_EventCount) {

	   case DB_COLUMN_ID1:
	       event->setId(pQr->pResult[iK_EventCount + iJ_EventCount]);
	       break;
	   case DB_COLUMN_ID2:
	       break;

	   case DB_COLUMN_ID3:
	       event-> setType(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID4:
	       event-> setFlags(atoi (pQr->pResult[iK_EventCount + iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID5:
	       event-> setDateStart(atoi (pQr->pResult[iK_EventCount + iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID6:
	       event-> setDateEnd(atoi (pQr->pResult[iK_EventCount + iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID7:
	       if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		   event->setSummary(pQr->pResult[iK_EventCount + iJ_EventCount]);
	       }
	       break;

	   case DB_COLUMN_ID8:
	       if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		   event->setLocation(pQr-> pResult[iK_EventCount + iJ_EventCount]);
	       }
	       break;

	   case DB_COLUMN_ID9:
	       if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		   event->setDescription(pQr->pResult[iK_EventCount +
			   iJ_EventCount]);
	       }
	       break;

	   case DB_COLUMN_ID10:
	       event-> setStatus(atoi (pQr->pResult[iK_EventCount + iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID11:

	       if (pQr->pResult[iK_EventCount + iJ_EventCount]) {
		   event->setGUid(pQr-> pResult[iK_EventCount + iJ_EventCount]);
	       }
	       break;

	   case DB_COLUMN_ID12:
	       event-> setUntil(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID13:
	       event->setAllDay(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID14:
	       event-> setCreatedTime(atoi (pQr-> pResult[iK_EventCount + 
			   iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID15:

	       event-> setLastModified(atoi (pQr-> pResult[iK_EventCount + 
			   iJ_EventCount]));
	       break;

	   case DB_COLUMN_ID16:
	       event->setTzid(pQr->pResult[iK_EventCount + iJ_EventCount]);
	       break;
	   /* 17 - tZid */
	   case DB_COLUMN_ID17:
	       break;
	   
	   case DB_COLUMN_ID19:
	       event->
		   setType(atoi(pQr->pResult[iK_EventCount + iJ_EventCount]));
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
	//Rrule|Rdates|Exdates|Exrule|RecurrenceId|Rtype|LocalId|ExternalId
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

   if (pQr) {
       sqlite3_free_table(pQr->pResult);
       delete pQr;
       pQr = 0;
   }
   if ((event->getFlags() == HAS_RECURRENCE) ||
	   (event->getFlags() == HAS_RECURRENCE_ALARM))
       event->setRecurrence (pRec);

   delete pRec ;
   pRec = 0;

   if ((event->getFlags() == HAS_ALARM) ||
	   (event->getFlags() == HAS_RECURRENCE_ALARM))
    {
       event->setAlarm(pAlarm);
    }

   delete pAlarm;

   return event;
}   



int CMulticalendar::regenerateInstances()
{
    vector <CComponent *>   events;
    vector <int >       calids;
    int number = 0;
    CCalendarDB *pDb = 0;
    char* pQuery = 0;

    CAL_DEBUG_LOG("\n\n\nCMulticalendar::regenerateInstances() - starting\n\n\n");
    pDb = CCalendarDB::Instance();
    int pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    if (pDb == 0) {
        pErrorCode = CALENDAR_APP_ERROR;
        return 0;
    }

    CAL_DEBUG_LOG("\n\n\nCMulticalendar::regenerateInstances() - calling getComponentsAllCalendars\n\n\n");
    // fetch events
    events = getComponentsAllCalendars(DEFAULT_START_DATE,DEFAULT_END_DATE,DEFAULT_LIMIT,DEFAULT_OFFSET,calids,pErrorCode);
    number =  events.size();
    CAL_DEBUG_LOG("%d components found in DB for All Calendars\n",number);
    if ( number == 0) {
        return number;
    }

    // drop instances table
    pQuery = sqlite3_mprintf(ERASE_TABLE,INSTANCES_TABLE);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("Erasing query is %s\n", pQuery);
    pDb->execSQL(pQuery); 
    sqlite3_free(pQuery);

    // create new instances table
    pDb->execSQL(CREATE_INSTANCES);


    for (int i = 0; i < number; i++) {
        CComponent* event = events[i];
        //calendar =  getCalendarById(calids[i],pErrorCode);
        CAL_DEBUG_LOG("adding event %s", event->getId().c_str());

        // calculate recurrent instances
        vector<time_t> instances;
        event->generateInstanceTimes(1,2145830400, instances);

        CAL_DEBUG_LOG("found  %d instances", instances.size());        
        
        time_t duration = event->getDateEnd() - event->getDateStart();

        if(instances.size()){
            // add instances
            addInstances(event->getId(),instances,duration);
        }
        else {
            // add this event start date
            addInstance(event->getId(),event->getDateStart(),duration);
        }

        delete events[i];
    }

    events.clear();
    return number;

}

 /**
     * @param string sId Id of the component
     * @param time_t timeFrom the start time of the interval
     * @param time_t timeTo the end time of the interval
     * @return int number of occurences
     *
     * This function returns number of instances of the event by given id
     */
int CMulticalendar::getInstanceNumber(string sId, time_t timeFrom, time_t timeTo, time_t tzshift)
{
QueryResult *pQr = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    int pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    int number = 0;

    // get db
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Invalid CCalendarDB::Instance()");
        return number;
    }

    //prepare query
    pQuery = sqlite3_mprintf(SELECT_INSTANCE_NUMBER,sId.c_str(),timeFrom - tzshift,timeTo-tzshift);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("CCalendar::getInstanceNumber: pQuery is : %s", pQuery);

    //execute
    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if(pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
        CAL_DEBUG_LOG("SQL error");
        if ( pQr ){
            sqlite3_free_table(pQr->pResult);
            delete pQr;
        }
        return number;
    }

    if ( pQr ){
        number = atoi(pQr->pResult[1]);
        sqlite3_free_table(pQr->pResult);
        delete pQr;
    }

    CAL_DEBUG_LOG("CCalendar::getInstanceNumber: the result is  %d", number);
    return number;
}


    /**
     * @param string sId Id of the component
     * @param time_t timeFrom the start time of the interval
     * @param time_t timeTo the end time of the interval
     * @param vector<time_t> result  placeholder for the results
     * @return boolean SUCCESS/FAILURE 
     *
     * This function returns all instances of the event by given id
     */
bool CMulticalendar::getInstances(string sId, time_t timeFrom, time_t timeTo, vector <time_t> &result, time_t tzshift)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    int pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    CAL_DEBUG_LOG("CCalendar::getInstances(%s, %d, %d)\n",sId.c_str(),
        (int)(timeFrom - tzshift),(int)(timeTo-tzshift));

    // get db
    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Invalid CCalendarDB::Instance()");
        return FAILURE;
    }

    //prepare query
    pQuery = sqlite3_mprintf(SELECT_INSTANCES,sId.c_str(),timeFrom-tzshift,timeTo-tzshift);
    ASSERTION(pQuery);
    CAL_DEBUG_LOG("CCalendar::getInstances: pQuery is : %s", pQuery);

    //execute
    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if(pErrorCode != CALENDAR_OPERATION_SUCCESSFUL){
        CAL_DEBUG_LOG("SQL error");
        if (pQr){
            sqlite3_free_table(pQr->pResult);
            delete pQr;
        }
        return FAILURE;
    }

    //ret results
    result.clear();
    CAL_DEBUG_LOG("CCalendar::getInstances: now parsing the result\n");
    if ( pQr ) {
        // we know that the result is 2-column table, ant the first row contains name
        for (int i = 0; i < pQr->iRow; i+=2) { 
CAL_DEBUG_LOG("\n for: %s %s \n", pQr->pResult[i+2],pQr->pResult[i+3]);
            int timeStart = atoi(pQr->pResult[i+2]);

            if (timeStart == 0)
                continue;

//            int timeEnd = atoi(pQr->pResult[i+3]);
	    /* Commented By ADITYA : why normal event time shift is required 
	     * this offset will get added automatically ehrn time_t 
	     * is converted to local zone in  glue code */

            //if(timeStart==timeEnd)    
                result.push_back((time_t)timeStart);
            //else
              //  result.push_back((time_t)timeStart + tzshift);
        }
        sqlite3_free_table(pQr->pResult);
        delete pQr;
    }

    return SUCCESS;
}

    /**
     * @param string sId Id of the component     
     *
     * This function drops all instances of the event by given id
     */
bool CMulticalendar::dropInstances(string sId)
{
    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;

    int iSqliteError = 0;
    int pErrorCode;

    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Invalid CCalendarDB::Instance()");
        return FAILURE;
    }

    char *pQuery = 0;
    //prepare query
    pQuery = sqlite3_mprintf(DELETE_TABLE, INSTANCES_TABLE, ENTRY_FIELD_ID); // "delete from Instances where Id=?"

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("CCalendar::dropInstances: pQuery is : %s", pQuery);

    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery), &pPreparedStmt, &pTail);

    sqlite3_bind_text(pPreparedStmt, DB_QUERY_INDEX1, sId.c_str(), sId.length(), SQLITE_TRANSIENT); // TODO Text for integer field: Perfomance issue?

    sqlite3_free(pQuery);
    pQuery = 0;

    //execute
    pCalDb->updateDB(pPreparedStmt, iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError, pErrorCode);
    CALENDAR_LOG_ERROR(pErrorCode, "CCALENDAR:dropInstances: Failed to upadte in DB");

    return SUCCESS;
}

    /**
     * @param string sId Id of the component
     * @param time_t instTime one instance time
     * @return boolean SUCCESS/FAILURE 
     *
     * This function adds to the database one instance of the event by given id
     */
bool CMulticalendar::addInstance(string sId, time_t instTime, time_t duration, time_t tzshift)
{
  CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
        CAL_DEBUG_LOG("Invalid CCalendarDB::Instance()");
        return FAILURE;
    }

    sqlite3_stmt *pPreparedStmt = 0;
    const char *pTail = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    int pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;


    // prepare query
    pQuery =
    sqlite3_mprintf(INSERT_INSTANCES,ENTRY_FIELD_ID,ENTRY_FIELD_DTSTART,ENTRY_FIELD_DTEND);
    ASSERTION(pQuery);

 //   CAL_DEBUG_LOG("CCalendar::addInstance : query is  %s (%s,%d,%d)", pQuery, sId.c_str(), (int)instTime, (int)instTime+duration);

    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);

    // bind parameters
    sqlite3_bind_text(pPreparedStmt, 1, sId.c_str(), sId.length(), SQLITE_TRANSIENT);
    sqlite3_bind_int(pPreparedStmt, 2, instTime - tzshift);
    sqlite3_bind_int(pPreparedStmt, 3, duration? instTime + duration - tzshift : instTime);


    pCalDb->insertRows(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode); 
    CALENDAR_LOG_ERROR(pErrorCode,"CCalendar::addInstance failed to insert in DB");

    return SUCCESS;
}

    /**
     * @param string sId Id of the component
     * @param vector<time_t> instances array of instances
     * @return boolean SUCCESS/FAILURE 
     *
     * This function adds to the database the array of instances of the event by given id
     */
bool CMulticalendar::addInstances(string sId, vector <time_t> instances, time_t duration, time_t tzshift)
{
    CAL_DEBUG_LOG("CCalendar::addInstances\n");
 
    bool rezult = SUCCESS;
    for (unsigned int i = 0; rezult==SUCCESS && i < instances.size(); i++) {
        rezult = addInstance(sId, instances[i], duration, tzshift);
    }

    return rezult;
}

/**
 * Next prev Presence can be found by this method. 
 * API used to see if there is next prev instance so that 
 * NEXT PREV Menus are updated in UI in event view.   
 * This has to be used intelligently by UI 
 * How to use this API: 
 * 
 * 1. Api quickly indicates the presence of PREV /NEXT event  
 * 2. If the event opened  in event view  is nth repeat and the 
 *    event has p repeats 
 *    event 1 has total p repeats and Ui is showing the nth repeat in 
 *    event view 
 *    0, 1,2,........ n ........p
 *    In this case no need to call the checkNextPrev as the instance being shown in UI 
 *    middle instance there will be next and previous instances.
 *    So enable the Next and Previous menus. ( exception case is when repeat type is complex 
 *    In this case call getNextPrevious to get the actual instance in this case it will be a bit slower ) 
 *
 * 3. PRev logic  checks if previous event exist or not is --- event has to start before the input date 
 *    be it timebased repeating 
 * 5. Next logic is optimised to see if there is any event which starts after this time / any repeating event 
 *   if this fails then we check for any repeating event with until=-1 ( if one such event is found then 
 *   we return at that point in time.)
 */
bool CMulticalendar::checkForNextPrev(time_t instanceTime,string Id ,bool isPrev, int &pErrorCode)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    bool prevNextPresent = false;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    CCalendarDB *pCalDb = CCalendarDB::Instance();
    if (pCalDb == 0) {
	pErrorCode =CALENDAR_APP_ERROR;
        CAL_DEBUG_LOG("Invalid CCalendarDB::Instance()");
        return false;
    }

	    // see if the next event is there 
    if (isPrev)
    {
	    // see if the previous events is there 
	    // the logic is event has to start before it is being shown 
	    // if any event starts before it means prev event is present
    	    CAL_DEBUG_LOG("Fetching Previous "); 
	    pQuery = sqlite3_mprintf(CHECK_PREV_VISIBLE_EVENT, instanceTime,Id.c_str());
	    ASSERTION(pQuery);

    }
    else
    {
            CAL_DEBUG_LOG("Fetching Next"); 
	    pQuery = sqlite3_mprintf(CHECK_NEXT_VISIBLE_EVENT,instanceTime,instanceTime,Id.c_str());
	    ASSERTION(pQuery);

    }
    CAL_DEBUG_LOG("Query is : %s", pQuery);
    pQr = pCalDb->getRecords(pQuery,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    sqlite3_free(pQuery);

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
    {
	    CAL_DEBUG_LOG("Problem fetching events from DB !! Errorcode is %d",pErrorCode);
	    return false;
    }
    if(!pQr)
	    prevNextPresent=false;
    else  { 
	    prevNextPresent = true;

	    sqlite3_free_table(pQr->pResult);
	    delete pQr;
	    pQr = 0;
    }

    /** If there is next previous present then return */
    if (prevNextPresent == true)
    {
	    CAL_DEBUG_LOG("prev next component is present");
	    return prevNextPresent;
    }
    
    // if there is no previous event then return 
    if (isPrev){
            CAL_DEBUG_LOG("Returning prev component");	    
	    return prevNextPresent;
    }
    else{
	    CAL_DEBUG_LOG("<--------- No local event in future--------------> "
		   "<-------- Need to spend some  time in this function to crack this one ------->");
    }
    /* pick all repeating events for which until=-1 and then return if atleast
     * one among them is next event  */
    
   return ( checkExistenceOfFutureRepeatingEvent(instanceTime,Id,pErrorCode ));

}
/**
 *  checkExistenceOfFutureRepeatingEvent 
 */
bool CMulticalendar::checkExistenceOfFutureRepeatingEvent(time_t OriginalDateStart,
		string sId,
		int& pErrorCode)
{
/* find repeating events fo which until = -1 which can have a repeat beyond this 
 * Originaldate start */
	QueryResult *pQr = 0;
	char *pQuery = 0;
	CCalendarDB *pDb = 0;
	int iSqliteError = 0;
	vector < CComponent * >listComp;
	CComponent *pEntry = 0;
	vector < time_t > instanceTimes ;
	bool nextPresence = false;

	pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
	pDb = CCalendarDB::Instance();

	if (pDb == 0) {
		pErrorCode = CALENDAR_APP_ERROR;
		CAL_DEBUG_LOG("FATAL Error not able to get CCalendarDB handler error Code %d",pErrorCode);
		return false;
	}

	/* Find all recursive events for which   until = -1 and see if any one of it is 
	 * beyond the current event in event view
	 * (It need not be the  next event but its presence definitely indicates the 
	 * next event so that menu can be updated )
	 */
	pQuery = sqlite3_mprintf(SELECT_REPEAT_EVENT_UNTIL_MINUSONE,HAS_RECURRENCE,HAS_RECURRENCE_ALARM );

	ASSERTION(pQuery);
	CAL_DEBUG_LOG("Query is : -- :%s ", pQuery);
	pQr = pDb->getRecords(pQuery,iSqliteError);
	pDb->sqliteErrorMapper(iSqliteError,pErrorCode);
	sqlite3_free(pQuery);

	if (pQr == 0)
		return false ;

	int iI_CompCount = 0;
	int iJ_CompCount = 0;
	int iK_CompCount = 0;
    int iCalid=0;

	for (iI_CompCount = 0; iI_CompCount < pQr->iRow; iI_CompCount++) {
		pEntry = new CComponent();
		ASSERTION(pEntry);
		for (iJ_CompCount = 0; iJ_CompCount < pQr->iColumn; iJ_CompCount++) {
			iK_CompCount = pQr->iColumn + (iI_CompCount * pQr->iColumn);

			if ((pQr->pResult[iK_CompCount + iJ_CompCount]) == 0)
				continue;

			switch (iJ_CompCount) {
				case DB_COLUMN_ID2:
					iCalid = atoi(pQr->pResult[iK_CompCount + iJ_CompCount]);
                    pEntry->setCalendarId(iCalid);
					break;

				case DB_COLUMN_ID1:
					pEntry->setId(pQr->pResult[iK_CompCount + iJ_CompCount]);
					break;

				case DB_COLUMN_ID3:
					pEntry-> setType(atoi
							(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID4:
					pEntry-> setFlags(atoi
							(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID5:
					pEntry-> setDateStart(atoi
							(pQr-> pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID6:
					pEntry-> setDateEnd(atoi
							(pQr-> pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID7:
					if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
						pEntry->setSummary((string) pQr->
								pResult[iK_CompCount + iJ_CompCount]);
					}
					break;

				case DB_COLUMN_ID8:
					if (pQr->pResult[iK_CompCount + iJ_CompCount]) {
						pEntry->setLocation((string) pQr->
								pResult[iK_CompCount + iJ_CompCount]);
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
						pEntry->setGUid(pQr->
								pResult[iK_CompCount + iJ_CompCount]);
					}
					break;
				case DB_COLUMN_ID12:
					pEntry->
						setUntil(atoi
								(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;

				case DB_COLUMN_ID13:
					pEntry-> setAllDay(atoi
							(pQr->pResult[iK_CompCount + iJ_CompCount]));
					break;
				case DB_COLUMN_ID14:
					pEntry-> setCreatedTime(atoi
							(pQr-> pResult[iK_CompCount + iJ_CompCount]));
					break;
				case DB_COLUMN_ID15:

					pEntry-> setLastModified(atoi
							(pQr-> pResult[iK_CompCount + iJ_CompCount]));
					break;
				case DB_COLUMN_ID16:
					pEntry->setTzid(pQr->pResult[iK_CompCount + iJ_CompCount]);
					break;

				default:
					break;
			}


		}

		pEntry->getRecurrenceProperties();
		CAL_DEBUG_LOG("Searching Future  Instances ");
		pEntry->getInstanceTimes(OriginalDateStart,0, instanceTimes); 
		CAL_DEBUG_LOG("instance times size is %d",instanceTimes.size());
		if (instanceTimes.size()){
			CAL_DEBUG_LOG("instance time %ld ",(long)OriginalDateStart);
			if (instanceTimes[instanceTimes.size() -1] >= OriginalDateStart){
				CAL_DEBUG_LOG("Next is present");
				nextPresence = true;
				goto finalize;
			}
		}

		instanceTimes.clear();

		/* To reduce the memory consumption delete the entry immediately after we 
		 * get necessary information from the component 
		 */
		delete pEntry;
		pEntry = 0;


	}
finalize:
	
	instanceTimes.clear();	
	/* cleanup the memory here
	*/
	if (pEntry)
	{
		delete pEntry;
		pEntry = 0;
	}


	if (pQr) {
		sqlite3_free_table(pQr->pResult);
		delete pQr;
	}
	CAL_DEBUG_LOG("Next presence is %d", nextPresence);
	return nextPresence;

}

void CMulticalendar::reloadSystemTimezone()
{
    CAL_DEBUG_LOG("Resetting timezone");

    // Reset current system timezone value so next call of getSystemTimeZone() reloads it.
    szSystemTimezone.erase();
}

int CMulticalendar::deleteNonRecurrentComponents(int iDayBefore, int iMax, int &pErrorCode)
{
    char *pQuery = 0;
    QueryResult *pQr = 0;
    int iSqliteError = 0;

    int retval = 0;

    CCalendarDB *pDb = CCalendarDB::Instance();

    if (pDb == 0) {
        CAL_DEBUG_LOG("invalid CalendarDB pointer ");
        pErrorCode = CALENDAR_APP_ERROR;
        return retval;
    }

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    pQuery = sqlite3_mprintf(SELECT_ID_CALENDARID_FROM_NONRECURSIVECOMPONENT, iDayBefore, iDayBefore, iMax);
    ASSERTION(pQuery);

    CAL_DEBUG_LOG("Query is %s", pQuery);

    pQr = pDb->getRecords(pQuery, iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);

    sqlite3_free(pQuery);
    pQuery = NULL;

    if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL) {
        return 0;
    }

    if(pQr != NULL) {
        if (pQr->iColumn == 2) {
            int iLeft = pQr->iRow;// First row is column names

            // Limit number of deletion per call
            if(iLeft >  iMax) {
                iLeft = iMax;
            }

            CAL_DEBUG_LOG("Got %d candidates to delete", iLeft);

            // MAP: CalendarId->ComponentsToDelete
            std::map<int, std::vector<string> > componenets_to_delete;

            char **data = pQr->pResult + pQr->iColumn; // set pointer to first data (e.i. skip column names);

            for (int row = 0; row < iLeft; row++, data += pQr->iColumn) {
                if (!data[0]) {
                    CAL_ERROR_LOG("Got NULL string in SQL result at (%d,0)", row);
                    continue;
                };

                if (!data[1]) {
                    CAL_ERROR_LOG("Got NULL string in SQL result at (%d,1)", row);
                    continue;
                };

                // convert data from SQL request "SELECT Id,CalendarId FROM ..."
                std::string component_id = data[0]; // Id
                int calendar_id  = atoi(data[1]);   // CalendarId

                CAL_DEBUG_LOG("Prepare to remove Component #%s from Calendar #%d", component_id.c_str(), calendar_id);

                // Put data into map
                componenets_to_delete[calendar_id].push_back(component_id);
            }

            if (!componenets_to_delete.empty()) {
                // We have some events to remove... Let go through calendars and remove components

                for (std::map<int, std::vector<string> >::iterator iter = componenets_to_delete.begin();
                    iter != componenets_to_delete.end();
                    iter++) {
                    int error = CALENDAR_OPERATION_SUCCESSFUL;

                    // Remove items
                    deleteComponents(iter->second, // list of component ids to delete
                                     iter->first,  // calendar id to remove from
                                     error);

                    if (error == CALENDAR_OPERATION_SUCCESSFUL) {
                        // update number of actually remove components
                        retval += iter->second.size();
                    }
                    else {
                        CAL_ERROR_LOG("Failed to remove old components from calendar #%d (error=%d)", iter->first, error);
                        pErrorCode = error;

                        break;
                    }
                }

            }
            else {
                CAL_DEBUG_LOG("No non-recurce events to delete");
            }
        }
        else {
            CAL_ERROR_LOG("ERROR: Query result have have unexpected column number (%d). Expecting 2 columns", pQr->iColumn);
        }

        sqlite3_free_table(pQr->pResult);
        delete pQr;
        pQr = 0;
    }
    else {
        retval = 0;
        CAL_DEBUG_LOG("Got NULL SQL responce");
    }

    CAL_DEBUG_LOG("Removed %d events, error code = %d", retval, pErrorCode);
    return retval;
};

bool CMulticalendar::deleteAllComponents(int iCalId, int &pErrorCode)
{
    char *pQuery = 0;
    QueryResult *pQr = 0;
    int iSqliteError = 0;

    int retval = 0;

    CCalendarDB *pDb = CCalendarDB::Instance();

    if (pDb == 0) {
        CAL_DEBUG_LOG("invalid CalendarDB pointer ");
        pErrorCode = CALENDAR_APP_ERROR;
        return retval;
    }

    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    pQuery = sqlite3_mprintf(GET_IDS_FROM_COMPONENTS_FOR_CALENDAR, iCalId);
    ASSERTION(pQuery);

    CAL_DEBUG_LOG("Query is %s", pQuery);

    pQr = pDb->getRecords(pQuery, iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError,pErrorCode);

    sqlite3_free(pQuery);
    pQuery = NULL;

    CALENDAR_LOG_ERROR(pErrorCode, "Failed to execute SQL query");

    if(pQr != NULL) {
        if (pQr->iColumn == 1) {
            CAL_DEBUG_LOG("Got %d candidates to delete", pQr->iRow);

            // MAP: CalendarId->ComponentsToDelete
            std::vector<string> components_to_delete;

            char **data = pQr->pResult + pQr->iColumn; // set pointer to first data (e.i. skip column names);

            for (int row = 0; row < pQr->iRow; row++, data += pQr->iColumn) {
                if (!data[0]) {
                    CAL_ERROR_LOG("Got NULL string in SQL result at row %d", row);
                    continue;
                };

                // convert data from SQL request "SELECT Id FROM ..."
                std::string component_id = data[0]; // Id

                CAL_DEBUG_LOG("Prepare to remove Component #%s", component_id.c_str());

                // Put data into array
                components_to_delete.push_back(component_id);
            }

            if (!components_to_delete.empty()) {
                // We have some events to remove...
                deleteComponents(components_to_delete,
                                 iCalId,
                                 pErrorCode);

            }
            else {
                CAL_DEBUG_LOG("No events to delete");
            }
        }
        else {
            CAL_ERROR_LOG("ERROR: Query result have have unexpected column number (%d). Expecting 1 column (Id)", pQr->iColumn);
        }

        sqlite3_free_table(pQr->pResult);
        delete pQr;
        pQr = 0;
    }
    else {
        retval = SUCCESS;
        CAL_DEBUG_LOG("Got NULL SQL response");
    }

    retval = (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL);

    CAL_DEBUG_LOG("Finished with result %d;  error code = %d", (int)retval, pErrorCode);

    return retval;
};

/**
 * Get the unused color in the calendar
 * applcation. Returns COLOUR_DARKBLUE if all the colors have been used.
 * @param void No arguments
 * @return CalendarColour Calendar color
 */
CalendarColour
CMulticalendar::getUnusedCalendarColor(void)
{
    vector <bool> colors_in_use(COLOUR_NEXT_FREE, false);

    vector <CCalendar *> listCal = getListCalFromMc();

    vector <CCalendar *>::iterator it;
    for (it = listCal.begin(); it < listCal.end(); it++) 
    {
        CCalendar* calendar = *it;
        colors_in_use[calendar->getCalendarColor()] = true;
    }

    CalendarColour first_free_color = COLOUR_DARKBLUE;
    for (int test_color = COLOUR_DARKBLUE; test_color < COLOUR_NEXT_FREE ; test_color++)
    {
        if(!colors_in_use[test_color])
        {
            first_free_color = static_cast<CalendarColour>(test_color);
            break;
        }
    }

    releaseListCalendars(listCal);

    return first_free_color; 
}

void CMulticalendar::getImpendingAlarm (string szRrule,time_t & triggerTime,
        time_t dateStart, int alarmFlag, icaltimezone* pTz)
{
  /* For a recursive event if the alarm time is
   * expired then we need to find the impending alarm time 
   * for next event
   */ 
   vector<time_t>   listtime ;
   vector<time_t>   instance_time;
   vector<time_t>::iterator time_iter;
   int          errVal;
   if ( triggerTime > time_get_time())
       return ;

   if(alarmFlag != E_AM_DAYBEFORE) {
        getRecurrentTimes(szRrule,
                  icaltime_from_timet_with_zone(triggerTime, ZERO,pTz),
                  0,
                  0,
                  MAX_SUPPORTED_YEAR_IN_SEC,
                  pTz,
                  listtime,
                  errVal);
   }
   else {
        getRecurrentTimes(szRrule,
           icaltime_from_timet_with_zone(dateStart, ZERO,pTz),
           0,
           0,
                   MAX_SUPPORTED_YEAR_IN_SEC,
           pTz,
           instance_time,
           errVal);
    unsigned int titer = 0;
    for (titer = 0 ; titer < instance_time.size(); titer++) {
           int inst_time = instance_time[titer] - 86400;
           struct tm *tm_trigger = localtime (&triggerTime);
           inst_time += (3600 * tm_trigger->tm_hour ) +
               (60 * tm_trigger->tm_min)  +
               tm_trigger->tm_sec;
           listtime.push_back(inst_time);
    }
   }

    for (time_iter = listtime.begin(); time_iter != listtime.end(); time_iter++) {
        if (time_get_time() < (*time_iter)) {
        triggerTime = (time_t) (*time_iter);
        break;
        }
    }
}

void CMulticalendar::cleanupIcsString(std::string & ics_line)
{
    // check for embedded NULLs
    if (strlen(ics_line.c_str()) != ics_line.length())
    {
        CAL_DEBUG_LOG("ICS: Line have \\0 inside");

        for(size_t pos = ics_line.find((char)0);
            pos != string::npos;
            pos = ics_line.find((char)0))
        {
            CAL_DEBUG_LOG("ICS: Removing one zero");
            ics_line.erase(pos);
        }
    }
}

void CMulticalendar::restoreAlarms()
{
    // Reset all alarms
    CAlarm::purgeAlarms();

    int iSqlError;

    CCalendarDB *pDb = CCalendarDB::Instance();

    if (pDb == 0)
    {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB pointer");
        return;
    }

    QueryResult *pQr = pDb->getRecords("select Components.Id,CalendarId,ComponentType,CookieId from Components LEFT JOIN  ALARM on ALARM.Id = Components.Id where Components.Id in (select id from alarm)", iSqlError);

    if (pQr && pQr->pResult)
    {
        CAL_DEBUG_LOG("Have %d alarms to restore", pQr->iRow);

        CAlarm helper;

        int cols = pQr->iColumn;

#if 0
        // Dump column names for debugging purposes
        std::stringstream s;
        for (int col = 0; col < cols; col++)
        {
            s << pQr->pResult[col] << ",";
        }
        CAL_DEBUG_LOG("Got columns: %s", s.str().c_str());
#endif

        for (int row = 1; row <= pQr->iRow; row++)
        {
            const char *comp_id = pQr->pResult[cols * row];
            int calendar_id = atoi(pQr->pResult[cols * row + 1]);
            int comp_type = atoi(pQr->pResult[cols * row + 2]);
            long cookie = atoi(pQr->pResult[cols * row + 3]);

            if (comp_type > 0 && calendar_id > 0)
            {
                int error;

                if (cookie > 0)
                {
                    CAL_DEBUG_LOG("Cleanup alarm with cookie %ld", cookie);
                    helper.deleteAlarmEvent(cookie, error);
                }

                if (setNextAlarm(calendar_id, comp_id, comp_type, error))
                {
                    CAL_DEBUG_LOG("Updated alarm for component  %d:%s, type=%d", calendar_id, comp_id, comp_type);
                }
                else
                {
                    CAL_ERROR_LOG("Failed to set alarm for compoenent %d:%s (type=%d). Error core %d", 
                                   calendar_id,
                                   comp_id,
                                   comp_type,
                                   error);
                }
            }
        }

        sqlite3_free_table(pQr->pResult);
    }
    else
    {
        CAL_DEBUG_LOG("No alarms to restore (SQL Error  %d)", iSqlError);
    }

    delete pQr;
}

int CMulticalendar::comparePrevNextComponent(CComponent * c1, CComponent * c2, time_t t1/* = -1*/, time_t t2 /*= -1*/)
{
    int retval = 0;
    if (t1 < 0)
    {
        t1 = c1->getDateStart();
    }

    if (t2 < 0)
    {
        t2 = c2->getDateStart();
    }


    time_t d1 = c1->getDateEnd() - c1->getDateStart();
    time_t d2 = c2->getDateEnd() - c2->getDateStart();

    int summary_compare;

    // Earlest first
    if (t1 < t2)
    {
        retval = 1;
    }
    else if (t1 > t2)
    {
        retval = -1;
    }
    //All-day first
    else if (c1->getAllDay() && !(c2->getAllDay()))
    {
        retval = 1;
    }
    else if (!(c1->getAllDay()) && c2->getAllDay())
    {
        retval = -1;
    }
    // longest first
    else if (d1 != d2)
    {
        retval = d1 > d2 ? 1 : -1;
    }
    // compare titles
    else if ((summary_compare = strcmp(c2->getSummary().c_str(), c1->getSummary().c_str())) != 0)
    {
        retval = summary_compare;
    }
    // Fallback: less ID first
    else if (c1->getId() < c2->getId())
    {
         retval = 1;
    }
    else if (c1->getId() > c2->getId())
    {
        retval = -1;
    }
    else
    {
        retval = 0;
    }

//     CAL_DEBUG_LOG("'%s' VS '%s' = %d",
//                   c1->getSummary().c_str(),
//                   c2->getSummary().c_str(),
//                   retval);

    return retval;
}

int CMulticalendar::getComponentsAllCalendars(int iStDate, int iEndDate, int iLimit, int iOffset, vector< CComponent * > & vComponents, int iQueryType)
{
    CCalendarProcs procs(CCalendarDB::Instance());

    return procs.getComponentsAllCalendars(iStDate,iEndDate,iLimit,iOffset,vComponents,iQueryType);
}

int CMulticalendar::getComponentsAllCalendarsBySummary(std::string sSummary,int iLimit, int iOffset, vector< CComponent * > & vComponents)
{
    CCalendarProcs procs(CCalendarDB::Instance());

    return procs.getComponentsAllCalendarsBySummary(sSummary,iLimit,iOffset,vComponents);
}
