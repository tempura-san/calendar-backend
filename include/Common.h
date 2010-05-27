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
 * common include file for 
 * all the classes*/

#ifndef _COMMON_H__
#define _COMMON_H__

#include <vector>
#include <algorithm>
#include <list>
#include <string>
#include <iostream>
#include <sqlite3.h>
#include <assert.h>
#include "CCache.h"
#include "CMulticalendar.h"
#include "DbCalls.h"
#include "CCalendar.h"
#include "CCalendarDB.h"
#include "CComponent.h"
#include "CComponentDetails.h"
#include "CEvent.h"
#include "CTodo.h"
#include "CJournal.h"
#include "CRecurrence.h"
#include "CalendarLog.h"
#include "CParameters.h"
#include "CAttendee.h"
#include "CProperties.h"
#include "COrganizer.h"
#include "CAlarm.h"
#include <clockd/libtime.h>
using namespace std;
using std::vector;
using std::string;


/** global variables used in Calendar
 */
//const char SEMI_COLON[] =  ";"; 
const char DEFAULT_CALENDAR[] = "Private";
const char SYNCHRONIZED_CALENDAR[] = "Synchronized";
const char OPAQUE[] = "OPAQUE";
const char TRANSPERANCY[] = "TRANSPARENT";
const char BEGIN[] = "begin";
const char END[] = "end";
const char MAXID[] = "MaxId";
const char CALAPP[] = "CALENDAR-UI";
const char NULL_STRING[] = "NULL";
/**
 * for ical parsing*/
const char BEGIN_EVENT[] = "BEGIN:VEVENT";
const char END_EVENT[] = "END:VEVENT";
const char BEGIN_TODO[] = "BEGIN:VTODO";
const char END_TODO[] = "END:VTODO";
const char BEGIN_JOURNAL[] = "BEGIN:VJOURNAL";
const char END_JOURNAL[] = "END:VJOURNAL";
const char BEGIN_CALENDAR[] = "BEGIN:VCALENDAR";
const char END_CALENDAR[] = "END:VCALENDAR";
const char BEGIN_ALARM[] = "BEGIN:VALARM";
const char END_ALARM[] = "END:VALARM";
const char BEGIN_TIMEZONE[] = "BEGIN:VTIMEZONE";
const char END_TIMEZONE[] = "END:VTIMEZONE";
const char ICS_DEFAULT_CALENDAR_NAME[] = "imported calendar-%";
const char ICS_CALNAME_SEPERATOR[] = "-";
const char ICS_FILENAME_SEPERATOR[] = "file://";
const char V_EVENT[] = "VEVENT";
const char V_TODO[] = "VTODO";
const char VJOURNAL[] = "VJOURNAL";
const char RDATE[] = "RDATE";
const char EXDATE[] = "EXDATE";
const char RDATE_COL[] = "RDATE:";
const char EXDATE_COL[] = "EXDATE:";
const char RRULE[] = "RRULE";
const char EXRULE[] = "EXRULE";
const char RRULE_COL[] = "RRULE:";
const char EXRULE_COL[] = "EXRULE:";
const char A_ALARM[] = "AALARM";
const char AALARM_COL[] = "AALARM:";
const char DAILY[] = "DAILY";
const char WEEKLY[] = "WEEKLY";
const char MONTHLY[] = "MONTHLY";
const char YEARLY[] = "YEARLY";
const char WEEKDAY[] = "BYDAY=MO,TU,WE,TH,FR";
const char UNTIL_ICAL[] = "UNTIL=";
const char PUBLIC[] = "PUBLIC";
const char PRIVATE[] = "PRIVATE";
const char CONFIDENTIAL[] = "CONFIDENTIAL";
const char ALTREP[] = "ALTREP=";
const char DOUBLE_QUOTE[] = "\"";
const char T_ICAL[] ="T";
const char DAY_ICAL[] = "235959";
const char SIX_O[] ="T000000";
const char SIX_T_SIX[] = "000000T000000";
const char WHITE_SPACE[] = " ";
const char FW_SLASH[] = "/";
/**
 * for vcal parsing*/
const char DCREATED[] = "DCREATED:";
const char RSVP_YES[] = "RSVP=YES";
const char RSVP_NO[] = "RSVP=NO";
const char PARTSTAT_SENT[] = "STATUS=SENT";
const char PARTSTAT_CONFIRMED[] = "STATUS=CONFIRMED";
const char STATUS_SENT[] = "STATUS:SENT";
const char STATUS_DECLINED[] = "STATUS:DECLINED";
const char STATUS_DELEGATED[] = "STATUS:DELEGATED";
const char STATUS_ACCEPTED[] = "STATUS:ACCEPTED";
const char STATUS_NEEDS_ACTION[] = "STATUS:NEEDS-ACTION";
const char STATUS_CONFIRMED[] = "STATUS:CONFIRMED";
const char STATUS_TENTATIVE[] = "STATUS:TENTATIVE";
const char STATUS_COMPLETED[] = "STATUS:COMPLETED";
const char ROLE_ATTENDEE[] = "ROLE=ATTENDEE";
const char ROLE_ORGANIZER[] = "ROLE=ORGANIZER";
const char ROLE_OWNER[] = "ROLE=OWNER";
const char ROLE_DELEGATE[] = "ROLE=DELEGATE";
const char EXPECT[] = "EXPECT=";
const char TRANSP_ZERO[] = "TRANSP:0";
const char TRANSP_ONE[] = "TRANSP:1";
const char TRANSP_GT_ONE[] = "TRANSP:>1";
const char MON_DAY[] = "MO";
const char TUES_DAY[] = "TU";
const char WEDNES_DAY[] = "WE";
const char THURS_DAY[] = "TH";
const char FRI_DAY[] = "FR";
const char SATUR_DAY[] = "SA";
const char COMMA[] = ",";
const char HYPHEN[] = "-";
const char PLUS[] = "+";
const char HASH[] = "#";
const char SUN_DAY[] = "SU";
const char COUNT_VCAL[] = ";COUNT=";
const char UNTIL_VCAL[] = ";UNTIL=";
const char FREQ_DAILY[] = "FREQ=DAILY";
const char FREQ_WEEKLY[] = "FREQ=WEEKLY";
const char FREQ_MONTHLY[] = "FREQ=MONTHLY";
const char FREQ_YEARLY[] = "FREQ=YEARLY";
const char INTERVAL_VCAL[] = ";INTERVAL="; 
const char BY_MONTH_VCAL[] = ";BYMONTH="; 
const char BY_MONTHDAY_VCAL[] = ";BYMONTHDAY=";
const char BY_DAY_VCAL[] = ";BYDAY="; 
const char WKSTRT_SU_VCAL[] = ";WKST=SU";
const char UNTIL[] = "UNTIL";
const char COUNT[] = "COUNT";
const char FREQUENCY[] = "FREQ";
const char INTERVAL[] = "INTERVAL";
const char BY_DAY[] = "BYDAY";
const char BY_MONTHDAY[] = "BYMONTHDAY";
const char BY_MONTH[] = "BYMONTH";
const char TZID[] = "TZID";
const char PERIOD[] = "PERIOD";
const char DATE[] = "DATE";
const char DURATION[] = "DURATION";
const char REPEAT[] = "REPEAT";
const char ACTION_AUDIO[] = "ACTION:AUDIO";
const char DURATION_COL[] = "DURATION:";
const char REPEAT_COL[] = "REPEAT:";
const char DALARM[] = "DALARM";
const char PALARM[] = "PALARM";
const char M[] = "M";
const char P[] = "P";
const char Y[]= "Y";
const char D[] = "D";
const char W[] = "W";
const char NEWLINE[] = "\n";
const char HASH_ZERO[] = "#0";
const char TIMEZONE[] = "T000000Z";
const char TRIGGER_VALUE_DATE_TIME[] = "TRIGGER;VALUE=DATE-TIME:";
const char ALARM_TYPE_WAVE_VALUE[] = "AALARM:"; 
const char AUDIO_ALARM_DELIMITER[] = ":=;,"; 
const char DATE_DELIMITER[] = ":=;,/"; 
const char VCALRULE_DELIMITER[] = ":=;,";
const char ZEROSEMI[] = "0;";
const int ZEROSEC =  0;
const int THREEOOSEC = 300;
const int NINEOOSEC = 900;
const int HALFHOUR = 1800;
const int ONEHOUR = 3600;
const int THREEHOUR = 10800;
const int ONEDAY = 86400;
const int YEARDAYS = 365;
const int MAX_DAYINSEC = 2145830400;
const int HALFDAY = 43200;
const int TENSEVEN = 17;
const int THREE = 3;
const int EIGHT = 8;
const int SIX = 6;
const int MAX_ALARM_TIME = 9;
const int MAX_SUPPORTED_YEAR_IN_SEC = 2145830400;
const int MAX_SUPPORTED_YEAR_IN_YEAR = 2037;
const char ENCODING[] = "ENCODING=";
const int altime[MAX_ALARM_TIME] = { ZEROSEC,
    ZEROSEC,
    THREEOOSEC,
    NINEOOSEC,
    HALFHOUR ,
    ONEHOUR,
    THREEHOUR ,
    ONEDAY ,
    HALFDAY
};

/**
 *  Constant definitions */
const int ZERO = 0;
const int ONE = 1;
const int HUNDRED = 100;
const int NINETEEN_HUNDRED = 1900;
const int NINE = 9;
const int MAX_PRIORITY = 255;
const int EXTERNAL_ENTRY = 8;
const int SUCCESS = 1;
const int FAILURE = 0;
const int NULLID = -1;
const int MAX_ENTRIES = 100;
const int MAX_ENTRY_COL = 10;
const int MAX_JOUR_COL = 7;
const int MAX_TODO_COL = 13;
const int MAX_APRIM_COL = 7;
const int MAX_ORZER_COL = 6;
const int MAXSIZE = 32;
const int MACLENGTH = 18;
const int MAX_RECUR = 6;
const int ADDRESSLENGTH = 6;
const int HEX_252 = 0xfc; 
const int HEX_3 = 0x03; 
const int HEX_240 = 0xf0; 
const int HEX_15 = 0x0f; 
const int HEX_192 = 0xc0; 
const int HEX_63 = 0x3f; 
const int CAL_VISIBLE = 1;
/**
 * for ical parsing*/
const int DEFAULT_ICS_CALNAME_LENGTH = 15;
const int ICS_FILENAME_SEPERATOR_LENGTH = 6;
const int ALARM_DUR_DIFF1 = 600;
const int ALARM_DUR_DIFF2 = 1350;
const int ALARM_DUR_DIFF3 = 2700;
const int ALARM_DUR_DIFF4 = 7200;
const int SEC_IN_HR = 3600;

/**
 * for properties */
const char DESCRIPTION[] = "DESCRIPTION";
const char SUMMARY[] = "SUMMARY";
const char RESOURCES[] = "RESOURCES";
const char CATEGORIES[] = "CATEGORIES";
const char ATTDEE[]    = "ATTEND";
const char ORGZER[]    = "ORGNI";
const char ATTACH[]    = "ATTACH";
const char ATTACH_ALARM[] = "ATTACH;FMTTYPE=audio/basic:";
/**
 * for parameters */
const char RELATEDTO[] = "RELATED-TO";
const char COMMENT[] = "COMMENT";
const char LOCATION[] = "LOCATION";
const char CONTACT[] = "CONTACT";

const char LANGUAGE[] = "LANGUAGE";
const char ALTER[] = "ALTER";
const char RELTYPE[] = "RELTYPE";
const char FMTYPE[] = "FMTYPE";

const char SENT[] = "SENTBY";
const char CNAME[] = "CNAME";
const char DIRPAR[] = "DIRPARAM";
const char GNMAILTO[] = "MAILTO";
const char ATCUTYPE[] = "CUTYPE";
const char ATMEMBER[] = "MEMBER";
const char ATROLE[] = "ROLE";
const char ATPSTAT[] = "PSTAT";
const char ATRSVP[] = "RSVP";
const char ATDELEGATEES[] = "DELEGATEES";
const char ATDELEGATOR[] = "DELEGATOR";
const int ATTDEE_LEN = 6;
const int ORGZER_LEN = 5;
const int TWO_ROWS = 2;
/** 
 * DB related strings 
 */
const char COMMIT[] = "commit";
const char ROLLBACK[] = "rollback";
const int DEFAULT_SIZE = 50;
/**
 * Macro for null assertion 
 */

#define ASSERTION(X) assert(X);

/**
 *  fields present in Attendee 
  and Organizer
 */
typedef enum {
    CNPARAM = 0,
    DIRPARAM,
    SENTBYPARAM,
    ORGLANGUAGE
}OrganizerParams;

typedef enum {
    CALENDAR_USER_TYPE = 0,
    MEMBER ,
    ROLE,
    PART_STATUS,
    RSVP,
    DELEGATEES,
    DELEGATOR,
    SENTBY,
    COMMON_NAME,
    DIR_PARAM,
    ATTLANGUAGE,
    MAILTO
} AttendeeParams;

typedef enum DATATYPE{
    STRING = 0,
    INTEGER,
    FLOAT,
    BOOL,
    NONE_TYPE
};

enum EntryType{
    ALL_ENTRIES = 0,
    EVENT,
    TODO,
    JOURNAL,
    BIRTHDAY
};

enum OPTION_TYPE {

    HAS_ATTENDEE = 1,
    HAS_ORGANIZER,
    HAS_RECURRENCE,
    HAS_ALARM,
    HAS_RECURRENCE_ALARM,
    HAS_PARTICIPANT,
    HAS_CATEGORIES,
    HAS_COMMENT,
    HAS_EXTRA
};

enum RecurrencType{
    E_NONE =0 ,
    E_DAILY,
    E_WEEKDAY,
    E_WEEKLY,
    E_MONTHLY,
    E_YEARLY,
    E_COMPLEX
};

enum entryType {
    E_CALENDAR = 0,
    E_EVENT,
    E_TODO,
    E_JOURNAL,
    E_BDAY,
    E_SPARE,
    E_VTIMEZONE
};

enum columnId {
    DB_COLUMN_ID1 = 0,
    DB_COLUMN_ID2,
    DB_COLUMN_ID3,
    DB_COLUMN_ID4,
    DB_COLUMN_ID5,
    DB_COLUMN_ID6,
    DB_COLUMN_ID7,
    DB_COLUMN_ID8,
    DB_COLUMN_ID9,
    DB_COLUMN_ID10,
    DB_COLUMN_ID11,
    DB_COLUMN_ID12,
    DB_COLUMN_ID13,
    DB_COLUMN_ID14,
    DB_COLUMN_ID15,
    DB_COLUMN_ID16,
    DB_COLUMN_ID17,
    DB_COLUMN_ID18,
    DB_COLUMN_ID19,
    DB_COLUMN_ID20,
    DB_COLUMN_ID21,
    DB_COLUMN_ID22,
    DB_COLUMN_ID23,
    DB_COLUMN_ID24,
    DB_COLUMN_ID25,
    DB_COLUMN_ID26,
    DB_COLUMN_ID27,
    DB_COLUMN_ID28,
    DB_COLUMN_ID29,
    DB_COLUMN_ID30,
    DB_COLUMN_ID31,
    DB_COLUMN_ID32,
    DB_COLUMN_ID33,
    DB_COLUMN_ID34,
    DB_COLUMN_ID35,
    DB_COLUMN_ID36,
    DB_COLUMN_ID37,
    DB_COLUMN_ID38,
    DB_COLUMN_ID39,
    DB_COLUMN_ID40,
    DB_COLUMN_ID41,
    DB_COLUMN_ID42,
    DB_COLUMN_ID43,
    DB_COLUMN_ID44,
    DB_COLUMN_ID45,
    DB_COLUMN_ID46,
    DB_COLUMN_ID47,
    DB_COLUMN_ID48,
    DB_COLUMN_ID49
};

enum DaysOfWeek
{
    Sunday    = 1,
    Monday    = 2,
    Tuesday   = 4,
    Wednesday = 8,
    Thursday  = 16,
    Friday    = 32,
    Saturday  = 64
};

enum RecurrenceType
{
    RecursSecondly = 0,
    RecursMinutely ,
    RecursHourly   ,
    RecursDaily    ,
    RecursWeekly   ,
    RecursMonthly  ,
    RecursMonthNth ,
    RecursYearly   ,
    RecursYearNth  
};

typedef struct {
    RecurrenceType freq;
    char* interval;
    char* modifier;    
    char* duration;
}RecurrenceItem;

enum queryId {
    DB_QUERY_INDEX1 = 1,
    DB_QUERY_INDEX2,
    DB_QUERY_INDEX3,
    DB_QUERY_INDEX4,
    DB_QUERY_INDEX5,
    DB_QUERY_INDEX6,
    DB_QUERY_INDEX7,
    DB_QUERY_INDEX8
};

#endif
