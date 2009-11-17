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

#ifndef __DB_CALLS_H__
#define __DB_CALLS_H__

#define CALENDARS_TABLE  "Calendars"
#define COMPONENTS_TABLE  "Components"
#define COMPONENTDETAILS_TABLE  "ComponentDetails"
#define ALARM_TABLE  "Alarm"
#define RECURSIVE_TABLE  "Recursive"
#define SMARTCAL_TABLE "Components"
#define TRASH_TABLE "Trash"
#define PARAMS_TABLE "Parameters"
#define XPROP_TABLE "XProperties"
#define IDMAP_TABLE "IDMAP"
#define CACHE_TABLE "CACHE"
#define INSTANCES_TABLE "Instances"

#define CAL_FIELD_ID         "CalendarId"
#define CAL_FIELD_NAME         "Name"
#define CAL_FIELD_COLOR     "Colour"
#define CAL_FIELD_ISVISIBLE     "IsVisible"
#define CAL_FIELD_ISREADONLY     "IsReadonly"
#define CAL_FIELD_CALTYPE    "CalendarType"
#define CAL_FIELD_TUNE         "CalendarTune"
#define CAL_FIELD_VERSION    "CalendarVersion"
#define CAL_FIELD_PROFILE    "CalendarProfile"

#define ENTRY_FIELD_CALID     "CalendarId"
#define ENTRY_FIELD_ID         "Id"
#define ENTRY_FIELD_TYPE     "ComponentType"
#define ENTRY_FIELD_FLAGS     "Flags"
#define ENTRY_FIELD_DTSTART     "DateStart"
#define ENTRY_FIELD_DTEND     "DateEnd"
#define ENTRY_FIELD_SUMMARY     "Summary"
#define ENTRY_FIELD_LOC     "Location"
#define ENTRY_FIELD_DESC     "Description"
#define ENTRY_FIELD_STATUS     "Status"
#define ENTRY_FIELD_UID        "Uid"
#define ENTRY_FIELD_UNTIL    "Until"
#define ENTRY_FIELD_ALLDAY     "AllDay"
#define ENTRY_FIELD_CRTIME     "CreatedTime"
#define ENTRY_FIELD_MODTIME "ModifiedTime"
#define ENTRY_FIELD_TZID    "Tzid"
#define ENTRY_FIELD_TZOFFSET    "TzOffset"

#define SMARTCAL_FIELD_CALID    "CalendarId"
#define SMARTCAL_FIELD_EUID     "Uid"
#define SMARTCAL_FIELD_FNAME    "Summary"
#define SMARTCAL_FIELD_BDAY    "DateStart"
#define SMARTCAL_FIELD_TYPE     "ComponentType"

#define COMP_FIELD_ID         "Id"
#define COMP_FIELD_TYPE     "ComponentType"
#define COMP_FIELD_CLAS     "Class"
#define COMP_FIELD_GEO         "Geo"
#define COMP_FIELD_PRIO        "Priority"
#define COMP_FIELD_DTSTAMP    "DateStamp"
#define COMP_FIELD_SEQ         "Sequence"
#define COMP_FIELD_TRANSP     "Transparency"
#define COMP_FIELD_UID         "Uid"
#define COMP_FIELD_URL         "Url"
#define COMP_FIELD_ATTACH     "Attach"
#define COMP_FIELD_CATEGORY "Category"
#define COMP_FIELD_COMMENT     "Comment"
#define COMP_FIELD_CONTACT     "Contact"
#define COMP_FIELD_RELATED     "Related"
#define COMP_FIELD_RESOURCES     "Resources"
#define COMP_FIELD_PERCENT     "Percent"
#define COMP_FIELD_COMPLETED     "Completed"
#define COMP_FIELD_XPROP     "x-prop"

#define ORG_FIELD_ID        "Id"
#define ORG_FIELD_TYPE        "ComponentType"
#define ORG_FIELD_CNPARAM     "CnParam"
#define ORG_FIELD_DIRPARAM     "DirParam"
#define ORG_FIELD_SENTPARAM     "SentByParam"
#define ORG_FIELD_LANGPARAM     "LanguageParam"

#define RECUR_FIELD_ID         "Id"
#define RECUR_RRULE         "Rrule"
#define RECUR_RDATES         "Rdates"
#define RECUR_EDATES         "Exdates"
#define RECUR_EXRULE         "Exrule"
#define RECUR_RECURID         "RecurrenceId"
#define RECUR_RTYPE           "Rtype"

#define CACHE_START		"DateStart"
#define CACHE_ID		"Id"

#define ATT_FIELD_ID         "Id"
#define ATT_FIELD_TYPE         "ComponentType"
#define ATT_FIELD_CUPARAM     "CuTypeParam"
#define ATT_FIELD_MEMPARAM     "MemberParam"
#define ATT_FIELD_ROLEPARAM     "RoleParam"
#define ATT_FIELD_ROLE_PARSTAT  "ParticipationStatus"
#define ATT_FIELD_RSVP         "Rsvp"
#define ATT_FIELD_DELTO     "DelegatedTo"
#define ATT_FIELD_DELFROM     "DelegatedFrom"
#define ATT_FIELD_SENTPARAM     "SentBy"
#define ATT_FIELD_CNPARAM     "CnParam"
#define ATT_FIELD_DIRPARAM     "DirParam"
#define ATT_FIELD_LANGPARAM     "LanguageParam"

#define ALARM_FIELD_ID         "Id"
#define ALARM_FIELD_TRIGGER     "Trigger"
#define ALARM_FIELD_REPEAT     "Repeat"
#define ALARM_FIELD_DURATION     "Duration"
#define ALARM_FIELD_ACTION     "Action"
#define ALARM_FIELD_COOKIEID    "CookieId"
#define ALARM_FIELD_ATTACH     "Attach"
#define IMPORT            "imported calendar-"
#define SEMI_COLON        ";"
#define EXCLAMATION        "!"


#define TRASH_FIELD_ID        "ComponentId"
#define TRASH_FIELD_CALID    "CalendarId"
#define TRASH_FIELD_TYPE    "ComponentType"
#define TRASH_FIELD_TIME    "DeletedTime"
#define TRASH_FIELD_CTIME    "CreatedTime"

#define PARAM_FIELD_ID        "ComponentId"
#define PARAM_FIELD_PROPNAME    "PropertyType"
#define PARAM_FIELD_PROPVALUE    "PropertyValue"
#define PARAM_FIELD_PARAMNAME    "ParamName"
#define PARAM_FIELD_PARAMVALUE    "ParamValue"

#define XPROP_FIELD_ID        "ComponentId"
#define XPROP_FIELD_XPROPNAME    "XPropertyName"
#define XPROP_FIELD_XPROPVALUE    "XPropertyValue"

#define IDMAP_FIELD_LOCALID "LocalId"
#define IDMAP_FIELD_EXTID  "ExternalId"


#define TIMEZONE_TABLE 		 "TIMEZONE"
#define TIMEZONE_FIELD_TZID 	 "TzId"
#define TIMEZONE_FIELD_LOCATION  "Location"
#define TIMEZONE_FIELD_DTSTSTD 	 "DtStartStd"
#define TIMEZONE_FIELD_DTSTDST 	 "DtStartDst"
#define TIMEZONE_FIELD_TZOFFSTD	 "TzOffsetStd"
#define TIMEZONE_FIELD_TZOFFDST  "TzOffsetDst"
#define TIMEZONE_FIELD_RRULESTD	 "RRuleStd"
#define TIMEZONE_FIELD_RRULEDST	 "RRuleDst"
#define TIMEZONE_FIELD_TZNAME 	 "TzName"
#define TIMEZONE_FIELD_DSTFLAG 	 "DSTFlag"

#define CREATE_DEFAULT_CALENDAR "insert into Calendars values(1,\"cal_ti_calendar_synced\", 0, 1, 0, 4,\"\", \"1.0\", 0)"
#define CREATE_PRIVATE_CALENDAR "insert into Calendars values(2,\"cal_ti_calendar_private\", 1, 1, 0, 3,\"\", \"1.0\", 0)"
#define INSERT_CALENDAR "insert into Calendars (%s, %s, %s, %s, %s,%s, %s,%s) values (?, ?, ?, ?, ?,?,?,?)"
#define INSERT_ENTRY "insert into Components ( %s,%s,%s, %s, %s, %s, %s, %s, %s, %s,%s,%s,%s,%s, %s, %s) values (?,?,?,?,?, ?, ?, ?, ?, ?, ?, ?, ?,?,?, ?)"
#define INSERT_COMPONENT "insert into ComponentDetails (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s) values (?, ?, ?, ? ,? ,?, ?, ?, ?, ? ,? ,?, ?, ?, ?, ? ,? ,?)"
#define INSERT_RECURSIVE "insert into RECURSIVE (  %s, %s, %s, %s, %s, %s,%s) values ( ?, ?, ?, ?, ?, ?,?)"
#define INSERT_ALARM "insert into ALARM ( %s,%s,%s, %s, %s, %s, %s) values (?,?, ?, ?, ?, ?, ?)"

#define INSERT_SMARTCAL "insert into Components (%s, %s, %s, %s, %s) values (?, ?, ?, ?, ?)"
#define INSERT_TRASH "insert into Trash (%s, %s, %s, %s, %s) values (?, ?, ?, ?, ?)"
#define INSERT_PARAM "insert into Parameters (%s,%s,%s,%s,%s) values (?,?,?,?,?)"
#define INSERT_XPROP "insert into XProperties (%s,%s,%s) values (?,?,?)"
#define INSERT_IDMAP "insert into IDMAP (%s,%s) values (?,?)"
#define INSERT_CACHE "insert into CACHE (%s, %s) values (?, ?)"
#define INSERT_INSTANCES "insert into Instances (%s, %s, %s) values (?,?,?)"
#define DELETE_INSTANCES "delete from Instances where %s=%s"
#define SELECT_INSTANCES "select DateStart, DateEnd from Instances where Id = \"%s\" and DateEnd >= %d and DateStart <= %d ORDER BY DateStart"
#define SELECT_INSTANCE_NUMBER "select COUNT(*) from Instances where Id = \"%s\" and DateEnd >= %d and DateStart <= %d "

#define SELECT_IDS_FROM_INSTANCES "select DISTINCT Id from Instances where  DateEnd >= %d and DateStart <= %d "

#define UPDATE_PARAM        "update Parameters set  %s=?, %s=?, %s=? where %s=?"
#define UPDATE_TABLE         "update %s set %s=? where %s= ?"
#define UPDATE_TABLE_AND     "update %s set %s = ? where %s = ? AND %s = ?"
#define UPDATE_TABLE_OR     "update %s set %s = ? where %s = ? OR %s = ?"
#define UPDATE_COLS_TABLE_AND     "update %s set %s = ?, %s = ? where %s = ? AND %s = ?"
#define UPDATE_COLS_TABLE_OR     "update %s set %s = ?, %s = ? where %s = ? OR %s = ?"
#define UPDATE_ALARM         "update ALARM set %s=?, %s=?, %s=?, %s=?,%s=?,%s=? where %s=?"
#define CALENDAR_UPDATE     "update CALENDARS set %s=?, %s=?, %s=?, %s=?, %s=?,%s=?,%s = ?,%s=? where %s=%d"
#define UPDATE_COMPONENTS     "update Components set %s=?,%s=?,%s=?,%s=?,%s=?,%s=?, %s=?, %s=?, %s=?,%s=?,%s=?,%s=? ,%s=?,%s=?,%s=?  where %s=?"
#define UPDATE_COMPONENTDETAILS "update ComponentDetails set %s=?,%s=?,%s=?,%s=?,%s=?, %s=?, %s=?, %s=?,%s=?,%s=?, %s=?,%s=?,%s=?,%s=?,%s=?,%s=?,%s=?  where %s=?"
#define UPDATE_RECURSIVE    "update RECURSIVE set %s=?,%s=?,%s=?,%s=?,%s=?,%s=? where %s=?"
#define UPDATE_SMARTCAL        "update Components set %s=?, %s=? where %s=?"

#define DELETE_BY_ID         "delete from %s where %s = %d"
#define DELETE_TABLE         "delete from %s where %s = ?"
#define DELETE_TABLE_AND     "delete from %s where %s = ? AND  %s = ?"
#define DELETE_TABLE_OR      "delete from %s where %s = ? OR  %s = ?"
#define DEL_ROW_BY_CALID     "delete from %s where %s in (select %s from Components where %s = %d)"
#define DELETE_TABLE_PROP    "delete from %s where %s = ? AND %s != ? AND %s != ?"
#define DROP_TABLE           "delete from  %s"
#define ERASE_TABLE         "drop table %s"
#define RENAME_TABLE         "alter table %s rename to %s"
#define MIGRATE_TABLE        "insert into %s select * from %s"
#define MIGRATE_COMPONENTS   "alter table Components add column TzOffset INTEGER default 0"

#define CREATE_CAL         "CREATE TABLE IF NOT EXISTS Calendars(CalendarId INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT UNIQUE,\
                                 Colour INTEGER,IsVisible INTEGER,IsReadonly INTEGER,CalendarType INTEGER,\
                 CalendarTune TEXT,CalendarVersion TEXT,CalendarProfile INTEGER)"
#define CREATE_ENTRY         "CREATE TABLE IF NOT EXISTS Components(Id INTEGER PRIMARY KEY AUTOINCREMENT,CalendarId INTEGER,\
                 ComponentType INTEGER, Flags INTEGER, DateStart INTEGER, DateEnd INTEGER,\
                 Summary TEXT, Location TEXT, Description TEXT, Status INTEGER,Uid TEXT , \
                 Until INTEGER,AllDay INTEGER,CreatedTime INTEGER,ModifiedTime INTEGER,Tzid TEXT, TzOffset INTEGER)"
#define CREATE_COMPONENT     "CREATE TABLE IF NOT EXISTS ComponentDetails(Id INTEGER , ComponentType INTEGER, \
                 Class TEXT,Geo TEXT,Priority INTEGER, DateStamp TEXT, Sequence INTEGER,\
                                 Transparency TEXT, Uid INTEGER, Url TEXT,\
                 Attach TEXT,Category TEXT, Comment TEXT, Contact TEXT, Related TEXT,\
                 Resources TEXT, Percent INTEGER,Completed TEXT)"
#define CREATE_ORGANIZER     "CREATE TABLE IF NOT EXISTS ORGANIZER(Id TEXT, ComponentType INTEGER,\
                            CnParam TEXT, DirParam TEXT, SentByParam TEXT, LanguageParam TEXT)"

#define CREATE_RECURSIVE     "CREATE TABLE IF NOT EXISTS RECURSIVE (Id INTEGER, Rrule text,Rdates text,\
                 Exdates text,Exrule text,RecurrenceId integer,Rtype INTEGER)"
#define CREATE_ATTENDEE     "CREATE TABLE IF NOT EXISTS ATTENDEE(Id TEXT, ComponentType INTEGER,\
                            CuTypeParam INTEGET,MemberParam TEXT,RoleParam INTEGER, \
                 ParticipationStatus INTEGER, Rsvp INTEGER,DelegatedTo TEXT,DelegatedFrom TEXT,\
                 SentBy TEXT, CnParam TEXT, DirParam TEXT, LanguageParam TEXT)"
#define CREATE_ALARM         "CREATE TABLE IF NOT EXISTS ALARM (Id INTEGER,  Trigger INTEGER, Repeat INTEGER,\
                            Duration INTEGER, Action INTEGER,CookieId TEXT,Attach TEXT)"
#define CREATE_TRASH        "CREATE TABLE IF NOT EXISTS Trash (ComponentId INTEGER, CalendarId INTEGER, \
                 ComponentType INTEGER, DeletedTime INTEGER, CreatedTime INTEGER)"
#define CREATE_PARAM         "CREATE TABLE IF NOT EXISTS Parameters(ComponentId INTEGER, PropertyType TEXT,\
                        PropertyValue TEXT,ParamName TEXT,ParamValue TEXT)"

#define CREATE_XPROP         "CREATE TABLE IF NOT EXISTS XProperties(ComponentId INTEGER, XPropertyName TEXT ,\
                 XPropertyValue TEXT)"
#define CREATE_IDMAP         "CREATE TABLE IF NOT EXISTS IDMAP(LocalId INTEGER, ExternalId TEXT)"

#define CREATE_INSTANCES         "CREATE TABLE IF NOT EXISTS Instances(Id INTEGER,DateStart INTEGER,DateEnd INTEGER)"
#define SELECT_TAB_COND3      "select %s from %s where %s = %d"
#define SELECT_DOMAIN         "select %s from %s where %s =\"%s\""
#define SELECT_DOMAIN_AND     "select %s from %s where %s = %d AND %s = %d"
#define SELECT_TAB_COND2     "select * from %s where %s = \"%s\""
#define SELECT_TAB_COND_AND     "select * from %s where %s = %d AND %s=%d"
#define SELECT_TAB_COND_TODO     "select * from %s where %s = %d AND %s=%d AND %s = %d"
#define SELECT_TAB_COND_AND_VIS     "select * from %s where %s <> %d AND %s=%d AND %s in (select %s from %s where %s=1)"
#define SELECT_TAB         "select * from %s"
#define SELECT_COUNTCOMPONENT	   "select COUNT(*) from %s where %s = %d"
#define SELECT_LIMITEDCACHE	   "select * from CACHE where DateStart > %d ORDER BY DateStart DESC LIMIT %d"
#define SELECT_LIMITEDCACHE2 "select * from CACHE where DateStart <  %d ORDER BY DateStart LIMIT %d"

#define SELECT_TWO_TAB         "select * from %s %s"
#define SELECT_THREE_TAB     "select * from %s %s %s"
#define SELECT_TAB_COND     "select * from %s where %s = ?"
#define SELECT_TAB_AND         "select * from %s where %s = ? AND %s = ?"
#define SELECT_TAB_OR         "select * from %s where %s = ? OR %s = ?"
#define SELECT_RANGE         "select * from %s where %s = %d AND %s >= %d AND %s <= %d"
#define SELECT_MULTI_RANGE     "select * from %s where %s = %d AND %s = %d AND %s >= %d AND %s <= %d"
#define SELECT_INNER_JOIN     "select components.*,componentdetails.* from components INNER JOIN componentdetails where components.Id=\"%s\" AND Components.Id=ComponentDetails.Id"
#define SELECT_PRCR_OFFSET_LIMIT "select * from %s where %s = %d AND (%s = %d OR %s=%d) AND (%s = %d OR %s = %d) AND (%d < %s OR %d = %s) LIMIT %d OFFSET %d"
#define SELECT_MRCR_OFFSET_LIMIT_UNION "select * from %s where %s = %d  AND  %s = %d AND %s >= %d AND %s <= %d AND (%s != %d AND %s != %d) union select * from %s where %s = %d  AND (%s = 2  OR allday = 1) AND (%s + %s)= %d AND (%s != %d AND %s != %d) union select * from %s where %s = %d AND %s = %d  AND allday = 0 AND (%s = %d OR %s = %d) AND (%d < %s OR %d = %s) union select * from %s where %s = %d AND %s = %d  AND allday = 1 AND (%s = %d OR %s = %d) AND (%d < (%s + %s) OR %d = %s) LIMIT %d OFFSET %d"
#define SELECT_MRCR_OFFSET_LIMIT "select * from %s where %s = %d AND %s = %d AND %s >= %d AND %s <= %d AND (%s != %d AND %s != %d) LIMIT %d OFFSET %d"
#define SELECT_MULTI_RANGE_MRCR "select * from %s where %s = %d AND %s = %d AND %s >= %d AND %s <= %d AND (%s != %d AND %s != %d) union select * from %s where %s = %d AND allday = 1 AND %s = %d AND (%s + %s) >= %d AND (%s + %s) <= %d AND (%s != %d AND %s != %d)"
#define SELECT_MULTI_RANGE_PRCR_CHECK    "select * from %s where %s = %d AND (%s = %d OR %s=%d) AND (%s = %d OR %s = %d) AND (%d < %s OR  %d = %s)"
#define SELECT_MULTI_RANGE_PRCR    "select * from %s where %s = %d AND (%s = %d OR %s=%d) AND (%s = %d OR %s = %d)"
#define GET_EXPIRED_EVENTS      "select %s from components where %s= %d and %s= %d and  %s < %d"
#define GET_EXPIRED_TODOS     "select %s from components where %s= %d and %s= %d and %s = %d and  %s < %d"
#define GET_ADDED_ENTRIES    "select %s from components where %s > %d AND %s=%d AND  %s=%d"
#define GET_MODIFIED_ENTRIES    "select %s from components where %s < %d AND %s > %d and %s=%d AND %s=%d"
#define GET_TODAYS_ALLDAY_VIS    "select Id,calendarId,summary,DateStart,tzid from components where %s + %s = %d  and %s=%d AND %s=%d AND %s in (select %s from %s where %s=1)"
#define GET_ALLDAY_ENTRIES    "select %s from components where %s >= %d AND %s <= %d and %s=%d AND %s=%d"
#define GET_IMPORT_CALENDAR "select max(Name) from calendars where Name like \'%s\'"
#define GET_CTIME_UID    "select CreatedTime from Components where Id = \"%s\""
#define GET_COMPID_FROM_TRASH     "select ComponentId from TRASH where DeletedTime > %d AND CalendarId = %d AND ComponentType= %d"
#define SELECT_PROP_AND "select * from Parameters where ComponentId=\"%s\"  AND\
                           PropertyType = \"%s\" AND PropertyValue = \"%s\""
#define SELECT_DISTINCT_PROP "select distinct PropertyValue from Parameters where ComponentId = \"%s\" AND\
                                 PropertyType = \"%s\""
#define SELECT_UPCOMING_EVENT_VIS "select Id,calendarId,summary,DateStart,Tzid,AllDay from components where ID not in (select Id from components where AllDay=%d AND DateStart <= %d AND DateEnd >= %d AND componentType = %d )AND  DateStart > %d AND ComponentType=%d AND %s in (select %s from %s where %s=1) ORDER BY DateStart,Summary LIMIT 1 OFFSET %d"
#define SELECT_UPCOMING_EVENT "select Id,calendarId,summary,DateStart from components where ID not in (select Id from components where AllDay=%d AND DateStart <= %d AND DateEnd >= %d AND componentType = %d )AND  DateStart >= %d AND ComponentType=%d  ORDER BY DateStart,Summary LIMIT 1 OFFSET %d"
#define SELECT_UPCOMING_TODO_VIS "select Id,calendarId,summary,DateStart,Tzid,AllDay from components where DateStart >= %d AND ComponentType=%d AND status =%d AND %s in (select %s from %s where %s=1) ORDER BY Datestart,summary LIMIT 1 OFFSET %d"
#define SELECT_UPCOMING_TODO "select Id,calendarId,summary,DateStart from components where DateStart >= %d AND ComponentType=%d AND status =%d ORDER BY Datestart,summary LIMIT 1 OFFSET %d"

#define INDEX_CALENDAR "CREATE INDEX IF NOT EXISTS IDX_CALENDAR on Calendars(CalendarId,Name)"
#define INDEX_COMPONENT "CREATE INDEX IF NOT EXISTS IDX_COMPONENT on Components(Id,ComponentType,DateStart,DateEnd,status,Until,ModifiedTime)"
#define INDEX_COMPONENT_DETAILS "CREATE INDEX IF NOT EXISTS IDX_DETAILS on ComponentDetails(Id,ComponentType)"
#define INDEX_RECURSIVE "CREATE INDEX IF NOT EXISTS IDX_RECURSIVE on RECURSIVE(Id)"
#define INDEX_ALARM "CREATE INDEX IF NOT EXISTS IDX_ALARM on ALARM(Id,CookieId)"
#define INDEX_PARAM "CREATE INDEX IF NOT EXISTS IDX_PARAM on Parameters(ComponentId TEXT, PropertyType TEXT,PropertyValue,ParamName)"
#define INDEX_TRASH "CREATE INDEX IF NOT EXISTS IDX_TRASH on TRASH(ComponentId,CalendarId,deletedTime)"
#define INDEX_INSTANCES "CREATE INDEX IF NOT EXISTS IDX_INST on Instances(Id,DateStart,DateEnd)"


#define SELECT_DUPLICATE_ENTRY  "select Id from Components where summary = %Q AND dateStart = %d AND ComponentType = %d AND CalendarId = %d "
#define SELECT_ENTRY_RETAIN "select Id from components where ModifiedTime > %d and Id = \"%s\""
#define SELECT_PREV_NEXT_ALLDAY "select * from components where  (Allday =1 and DateStart <=  %d and DateEnd>= %d)"
#define SELECT_PREV_COMPONENT SELECT_PREV_NEXT_ALLDAY "UNION select * from components where (DateStart <= %d AND componentType = %d AND (Flags!=%d OR Flags != %d)  AND (calendarId in (select calendarid from calendars where IsVisible =1 ))) OR Id = \"%s\" ORDER BY DateStart ,Summary,CreatedTime LIMIT 20 OFFSET 0"
#define SELECT_NEXT_COMPONENT SELECT_PREV_NEXT_ALLDAY "UNION select * from components where (DateStart >= %d AND componentType = %d AND (Flags!=%d OR Flags != %d)  AND (calendarId in (select calendarid from calendars where IsVisible =1 ))) OR Id = \"%s\" ORDER BY DateStart,Summary,CreatedTime LIMIT 20 OFFSET 0"
#define SELECT_ACTIVE_ALARM "select Id from Alarm where Id in (select Id from components where calendarid = %d) AND CookieId != \"%s\" AND  Trigger >= %d "
#define SELECT_VISIBLE_ALARM "select Id from Alarm where Id in (select Id from components where calendarid = %d) AND CookieId = \"%s\" AND  Trigger >= %d "
#define SELECT_ALL_ENTRIES "select * from %s where %s = %d AND %s=%d LIMIT %d OFFSET %d"
#define SELECT_RECURSIVE_FUTURE_INSTANCE "select * from components where (Flags = %d OR Flags = %d) AND (Until >= %d OR Until = %d)  AND (calendarId in (select calendarid from calendars where IsVisible =1 ))"	
#define SELECT_RECURSIVE_PREVIOUS_INSTANCE "select * from components where (Flags = %d OR Flags = %d) AND datestart <= %d  AND (calendarId in (select calendarid from calendars where IsVisible =1 ))"
#define SELECT_EVENTS_TODOS_OFFSET_LIMIT_UNION "select * from %s where %s = %d AND (%s = %d AND %s < %d AND %s >= %d) AND (%s != %d AND %s != %d)  union select * from %s where %s = %d AND  ((%s = %d OR allday = 1) AND (%s + %s) >= %d AND (%s + %s) <= %d) AND  (%s != %d AND %s != %d) union select * from %s where %s = %d AND %s = %d AND (%s = %d OR %s = %d) AND allday = 0 AND (%d <= %s OR %d = %s) union select * from %s where %s = %d AND (%s = %d OR %s=%d)  AND (%s = %d OR %s = %d) AND allday = 1 AND (%d <= (\"%s\" + \"%s\") OR %d = %s) LIMIT %d OFFSET %d"
#define GET_EXPIRED_NONRECURSIVE_EVENTS   "select %s from components where %s= %d and %s= %d and  %s < %d and ( %s != %d AND %s!=%d) "
#define GET_EXPIRED_RECURSIVE_EVENTS   "select %s from components where %s= %d and %s= %d and  ( %s = %d OR %s=%d) LIMIT %d"
#define SELECT_NOS_VISIBLE_CAL "select * from %s where %s = %d "

#define CREATE_TIMEZONE "CREATE TABLE IF NOT EXISTS TIMEZONE (Location TEXT,TzId TEXT,DtStartStd INTEGER,DtStartDst INTEGER,TzOffsetStd INTEGER,TzOffsetDst INTEGER,RRuleStd TEXT,RRuleDst TEXT,TzName TEXT,DSTFlag INTEGER )"
#define INSERT_TIMEZONE "insert into TIMEZONE (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s) values(?, ?, ?, ?, ?, ?, ?, ?,? ,?)"
#define INDEX_TIMEZONE "CREATE INDEX IF NOT EXISTS IDX_TIMEZONE on TIMEZONE(Location)"

#define SELECT_LOCATION "select %s from %s where (%s = 1 AND %s = %d )"

# define SELECT_ALREADY_HAPPENING_EVENT "select * from components where  allday!= 1 AND ((DateStart < %d AND DateEnd > %d  AND componentType = %d AND (Flags!=%d OR Flags != %d)) OR Id = \"%s\") AND calendarId in (select calendarid from calendars where IsVisible =1 ) ORDER BY DateStart,Summary LIMIT 10 OFFSET 0"
#define SELECT_LOCATION_FROM_OFFSET "select location from timezone where %s = %d AND %s = %d AND %s=%d LIMIT 1 OFFSET 0"
#define SELECT_LOCATION_FROM_OFFSET_WITH_DST "select location from timezone where %s = %d AND %s = %d AND %s=%d AND (%s = \"%s\" OR %s =\"%s\") LIMIT 1 OFFSET 0"

#define DELETE_TRIGGER "CREATE TRIGGER IF NOT EXISTS delete_component after delete on components \
    begin \
    delete from ComponentDetails where componentdetails.Id=OLD.Id; \
    delete from Recursive where recursive.Id=OLD.Id; \
    delete from Alarm where alarm.Id=OLD.Id; \
    delete from Parameters where parameters.ComponentId=OLD.Id; \
    delete from XProperties where xproperties.ComponentId=OLD.Id; \
    delete from Instances where Instances.Id=OLD.Id; \
    end "
#define DROP_TRIGGER "DROP TRIGGER  delete_component" 

#define COUNT_NONRECURSIVECOMPONENT "select COUNT(id) from %s where id in (select id from components where \
            componenttype=1 AND dateEnd < %d AND (Flags !=3 AND Flags !=5 ) union select id from components \
        where componenttype = 2 AND status = 1 AND dateStart < %d )"

#define SELECT_ID_CALENDARID_FROM_NONRECURSIVECOMPONENT \
        "select Id,CalendarId from components where componenttype = 1 AND dateEnd < %d AND (Flags !=3 AND Flags !=5 ) \
         union \
         select Id,CalendarId from components where componenttype = 2 AND status = 1 AND dateStart < %d LIMIT %d" 

#define DELETION_NONRECURSIVECOMPONENT "delete from components where id in (select id from components where id in (select id from components where componenttype=1 AND dateEnd < ? AND (Flags !=3 AND Flags !=5 ) union select id from components where componenttype = 2 AND status = 1 AND dateStart < ?) ORDER BY dateStart DESC LIMIT ?)"

#define DELETE_GROUP_ID "delete from Components where id in ("
#define GET_EXPIRED_RECURSIVE_EVENTS_COUNT   "select COUNT(*) from components where %s= %d and %s= %d and  ( %s = %d OR %s=%d)"
 
#define SELECT_INNER_JOIN_BATCH "select * from components left join componentdetails on components.Id = componentdetails.Id Left Join alarm on components.Id = alarm.Id Left Join Recursive on components.Id = Recursive.Id where Calendarid = %d AND Components.ComponentType = %d"
#define SELECT_TAB_COND_AND2    "select * from %s where %s = \"%s\" "
#define SELECT_BIRTHDAY_COUNT "select COUNT(*) from components where componentType = 4 AND dateStart =%d"

#define  SELECT_ALL_CALCOMP__UNION "select * from %s where ((%s = %d  AND %s < %d AND %s >= %d) OR ( %s = %d AND %s >= %d AND %s <= %d )) AND  (%s != %d AND %s != %d) AND calendarId in (select calendarid from calendars where IsVisible =1 ) union select * from %s where (%s = %d OR %s=%d)  AND (%s = %d OR %s = %d) AND (%d < %s OR %d = %s) AND calendarId in (select calendarid from calendars where IsVisible =1)  ORDER BY CalendarId LIMIT %d OFFSET %d " 

#define SELECT_ALL_IN_INTERVAL_BY_INSTANCES "select * from components where (%s = %d  OR %s = %d OR %s = %d) AND Id IN (select DISTINCT Id from Instances where  DateEnd >= %d and DateStart <= %d ) AND calendarId in (select calendarid from calendars where IsVisible =1)  ORDER BY CalendarId LIMIT %d OFFSET %d "

#define SELECT_ID_LIKE "select * from cache where Id like "
#define SELECT_COUNT "select Id from %s LIMIT 1 OFFSET 0"
#define SELECT_LEFT_JOIN "select * from components left join componentdetails on components.Id = componentdetails.Id Left Join alarm on components.Id = alarm.Id Left Join Recursive on components.Id = Recursive.Id where components.Id = \"%s\" "
#define SELECT_EVENTS_IN_LIST "select * from components left join componentdetails on components.Id = componentdetails.Id Left Join alarm on components.Id = alarm.Id Left Join Recursive on components.Id = Recursive.Id where components.id in("
#define SELECT_BDAYS_BY_EXT_ID "select * from components left join componentdetails on components.Id = componentdetails.Id Left Join alarm on components.Id = alarm.Id Left Join Recursive on components.Id = Recursive.Id join IDMAP where components.id = Idmap.LocalId AND idmap.ExternalId = \"%s\""
#define SELECT_BY_ID "select * from %s where CalendarId = %d"
#define UPDATE_UNTIL "update components set until = %d where Id = %d"
#define SELECT_ALL_CALCOMP__UNION_NORMAL "select * from %s where ((%s = %d  AND %s < %d AND %s >= %d) OR ( %s = %d AND %s >= %d AND %s <= %d )) AND  (%s != %d AND %s != %d) AND calendarId in (select calendarid from calendars where IsVisible =1 ) ORDER BY CalendarId LIMIT %d OFFSET %d "
#define SELECT_ALL_CALCOMP__UNION_RECURSIVE "select * from %s where (%s = %d OR %s=%d)  AND (%s = %d OR %s = %d) AND (%d < %s OR %d = %s) AND calendarId in (select calendarid from calendars where IsVisible =1)  ORDER BY CalendarId LIMIT %d OFFSET %d "
#define CHECK_PREV_VISIBLE_EVENT "select * from components where  componenttype = 1 and   datestart <= %d and id != %q and calendarId in (select calendarid from calendars where IsVisible =1 )  ORDER BY DateStart ,Summary,CreatedTime LIMIT 1 OFFSET 0 "   
#define CHECK_NEXT_VISIBLE_EVENT "select * from components where  componenttype = 1 and  (datestart >= %d   OR until >= %d) and Id!= %q AND  calendarId in (select calendarid from calendars where IsVisible =1) ORDER BY DateStart ,Summary,CreatedTime LIMIT 1 OFFSET 0"   
#define SELECT_REPEAT_EVENT_UNTIL_MINUSONE  "select * from components where componenttype =1 and (flags = %d OR flags = %d) and until =-1 AND  calendarId in (select calendarid from calendars where IsVisible =1) ORDER BY DateStart ,Summary,CreatedTime"

#define GET_IDS_FROM_COMPONENTS_FOR_CALENDAR "select Id from Components where CalendarId=%d"

#endif
