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

#include <iostream>
#include <calendar-backend/Common.h>
#include <calendar-backend/CalendarErrors.h>
extern "C"{
#include <libical/icaltimezone.h>
#include <libical/icalarray.h>
#include <libical/icaltime.h>
#include <time.h>
#include <ctime>
}
using namespace::std;
/* README 
 *   Executable CTimezoneTest is intalled in /usr/bin can be used to regenerate the 
 *   TIMEZONE table using the system zone information.
 *
 *
 *
 * 
 *   a) Compilation instruction:
 *         g++ ConfigureTimeZone.cpp -o CTimezoneTest `pkg-config calendar-backend dbus-1 --cflags --libs`
 *   b) Example considered: Helsinki timezone (following is the content coming from phone/PC Suite)
 *         TZ:+02
 *         DAYLIGHT:TRUE;+03;20090329T010000Z;20091025T010000Z;;
 **/

bool insert_data(string tzId,string location,time_t dtstartStd,time_t dtstartDst,int offsetStd,int offsetDst,
		                        int dstflag,string tzname,string rruleStd,string rruleDst);
const char * getLocation(int offsetstd,bool dstflag, time_t searchDate );
void printCurrentTime();

struct zoneInfor{
    string location;
    string RruleDst;
    time_t dstStart;
} *info;

int main(int arg,char **argv)
{

    icalarray *zones;
    icaltimezone *zone;
    icalcomponent *component;
    icalcomponent *comp =0 ;
    icalcomponent *comp2 = 0;
    icalproperty *prop;
    struct icaltimetype dtstartStd;
    struct icaltimetype dtstartDst;
    struct icalrecurrencetype rruleStd,rruleDst;
    struct icaldatetimeperiodtype rdateStd,rdateDst;
    int  prev_offsetStd = 0, offsetStd = 0;
    int  prev_offsetDst = 0, offsetDst = 0;

    int i=0;
    int dstflag=0;
    time_t rawtime;
    time_t daylightStart = 0;
    time_t stdStart = 0; 
    struct tm *time_stdate;
    struct tm *time_rdate;
    char* szQuery = 0;
    char* szErr_msg = 0;
    int iRet = 0;
    string Loc;
    bool retDb = false;



    /* First check if TIMEZONE table exist or not if not 
     * create one here */
    CCalendarDB* pCalendarDb = CCalendarDB::Instance();
    if (pCalendarDb == 0) {
	printf("Failed to create Calendar database ");
	exit(1);
    }
    pCalendarDb->execSQL("delete from timezone"); 
    
    szQuery = CREATE_TIMEZONE;
    iRet = sqlite3_exec(pCalendarDb->getDb(), szQuery, 0, 0, &szErr_msg);
    if (iRet != 0 ) {
	printf ("Failed to create Calendar timezone table ");	
	exit(1);
    }
    zones = icaltimezone_get_builtin_timezones();
    /* get all the timezones from libical and iterate
    */
    
    for (i; i < zones->num_elements; i++) {
	string ruleStd;
	string ruleDst;
	string location;
	string tzid ;
	string tznameStd;
	string tznameDst;
	ruleStd.clear();
	ruleDst.clear();
	location.clear();
	tzid.clear();
	tznameStd.clear();
	tznameStd.clear();
	
	bool flag = false;
	zone = (icaltimezone *)icalarray_element_at(zones, i);
	location = (char*)icaltimezone_get_location(zone);
	tzid = (char*)icaltimezone_get_tzid(zone);
	component = icaltimezone_get_component(zone);
	comp = icalcomponent_get_first_component (component, ICAL_XSTANDARD_COMPONENT);
        
	stdStart = 0 ;
	prev_offsetStd =0 ;
	daylightStart =0 ;
        prev_offsetDst = 0;
	icaltimezone* pTz = icaltimezone_get_builtin_timezone("UTC");

	if (!pTz)
		printf("time zone is incorrect %s",location.c_str());
	if (comp){
	    prop = icalcomponent_get_first_property (comp, ICAL_ANY_PROPERTY);
	    while (prop) {
		switch (icalproperty_isa (prop)) {
		    case ICAL_TZNAME_PROPERTY:
			tznameStd = (char*)icalproperty_get_tzname (prop);
			break;

		    case ICAL_DTSTART_PROPERTY:
			dtstartStd = icalproperty_get_dtstart (prop);
			stdStart = icaltime_as_timet_with_zone(dtstartStd,pTz);
			break;

		    case ICAL_TZOFFSETTO_PROPERTY:
			prev_offsetStd = icalproperty_get_tzoffsetto (prop);
			break;

		    case ICAL_RRULE_PROPERTY:
			rruleStd = icalproperty_get_rrule (prop);
			ruleStd=icalrecurrencetype_as_string(&rruleStd);
			break;
		    default:
			break;
		};

		prop = icalcomponent_get_next_property (comp, ICAL_ANY_PROPERTY);
	    }
	}



	comp2 = icalcomponent_get_next_component (component, ICAL_XDAYLIGHT_COMPONENT);

	if (comp2){
	    flag = true;
	    prop = icalcomponent_get_first_property (comp2, ICAL_ANY_PROPERTY);
	    while (prop) {
		switch (icalproperty_isa (prop)) {
		    case ICAL_TZNAME_PROPERTY:
			tznameDst = (char*)icalproperty_get_tzname (prop);
			break;


		    case ICAL_DTSTART_PROPERTY:
			dtstartDst = icalproperty_get_dtstart (prop);
			daylightStart = icaltime_as_timet_with_zone(dtstartDst,pTz);
			break;

		    case ICAL_TZOFFSETTO_PROPERTY:
			prev_offsetDst = icalproperty_get_tzoffsetto (prop);
			break;

		    case ICAL_RRULE_PROPERTY:
			rruleDst = icalproperty_get_rrule (prop);
			ruleDst = icalrecurrencetype_as_string(&rruleDst);	
			break;


		    default:
			break;
		};

		prop = icalcomponent_get_next_property (comp2, ICAL_ANY_PROPERTY);
	    }

	}

	if(flag)
	    dstflag = 1;
	
	else 
	    dstflag = 0;	
	retDb = insert_data(tzid,location,stdStart,daylightStart,prev_offsetStd,prev_offsetDst, dstflag,tznameStd,ruleStd,ruleDst);
	ruleStd.clear();
	ruleDst.clear();
	location.clear();
	tzid.clear() ;
	tznameStd.clear();
	tznameDst.clear();
    }
    printCurrentTime();
    return 0;

}

bool insert_data(string tzId,string location,time_t dtstartStd,time_t dtstartDst,int offsetStd,int offsetDst,
  int dstflag,string tzname,string rruleStd,string rruleDst){
    int ret = NULLID;
    
    char* pQuery = 0;
    const char *pTail = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    int iCol = 1;
    int iSqliteError = 0;
    int pErrorCode;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if (pCalDb == 0) {
	pErrorCode = CALENDAR_APP_ERROR;
	CAL_DEBUG_LOG("invalid caldb pointer");
	return FAILURE;
    }

    pQuery = sqlite3_mprintf(INSERT_TIMEZONE,TIMEZONE_FIELD_LOCATION,TIMEZONE_FIELD_TZID,
	    TIMEZONE_FIELD_DTSTSTD,TIMEZONE_FIELD_DTSTDST,TIMEZONE_FIELD_TZOFFSTD,TIMEZONE_FIELD_TZOFFDST,
	    TIMEZONE_FIELD_RRULESTD,TIMEZONE_FIELD_RRULEDST,TIMEZONE_FIELD_TZNAME,
	    TIMEZONE_FIELD_DSTFLAG);
    ASSERTION(pQuery);
    sqlite3_prepare(pCalDb->getDb(), pQuery, strlen(pQuery),
	    &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);
    if (location.length())
	sqlite3_bind_text(pPreparedStmt, iCol++, location.c_str(), location.length(),
	    SQLITE_TRANSIENT);
    else
	sqlite3_bind_text(pPreparedStmt, iCol++, NULL,0,SQLITE_TRANSIENT);

    if (tzId.length())
	sqlite3_bind_text(pPreparedStmt, iCol++, tzId.c_str(), tzId.length(),
	    SQLITE_TRANSIENT);
    else
	sqlite3_bind_text(pPreparedStmt, iCol++, NULL,0,SQLITE_TRANSIENT);
    
    sqlite3_bind_int(pPreparedStmt, iCol++, (int)dtstartStd);
    sqlite3_bind_int(pPreparedStmt, iCol++, (int)dtstartDst);
    sqlite3_bind_int(pPreparedStmt, iCol++, offsetStd);
    sqlite3_bind_int(pPreparedStmt, iCol++, offsetDst);
    if (rruleStd.length())
	sqlite3_bind_text(pPreparedStmt,iCol++,rruleStd.c_str(),rruleStd.length(),
	    SQLITE_TRANSIENT);
    else
	sqlite3_bind_text(pPreparedStmt, iCol++, NULL,0,SQLITE_TRANSIENT);
    if (rruleDst.length())
	sqlite3_bind_text(pPreparedStmt, iCol++, rruleDst.c_str(),rruleDst.length(),
	    SQLITE_TRANSIENT);
    else
	    sqlite3_bind_text(pPreparedStmt, iCol++, NULL,0,SQLITE_TRANSIENT);

    if(tzname.length() )
	sqlite3_bind_text(pPreparedStmt, iCol++, tzname.c_str(),tzname.length(), 
		    SQLITE_TRANSIENT);
    else
	sqlite3_bind_text(pPreparedStmt, iCol++, NULL,0,SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, iCol++, dstflag);
    
    pCalDb->insertRows(pPreparedStmt,iSqliteError);
    pCalDb->sqliteErrorMapper(iSqliteError,pErrorCode);
    if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL){
	CAL_DEBUG_LOG("addTimeZone: Insert Success");
	return true;
    }
    return false;
}


void printCurrentTime()
{               
    struct timeval tv; 
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm=localtime(&tv.tv_sec);
    printf("\n%d:%02d:%02d %ld \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
} 
