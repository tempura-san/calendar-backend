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
 * Description of possible Calendar errors
 */
#ifndef __CALENDAR_ERROR_H__
#define __CALENDAR_ERROR_H_

/* System errors */
const int CALENDAR_SYSTEM_ERROR        = 100;
/* Any DB operation which is unsuccessful with result other than
 * SQLITE_OK is treaed as DB error*/
const int CALENDAR_DATABASE_ERROR    = CALENDAR_SYSTEM_ERROR + 1;
/*Failure to allocate memory with new */
const int CALENDAR_MEMORY_ERROR        = CALENDAR_SYSTEM_ERROR + 2;
/*File not present / not able to read file from disk*/
const int CALENDAR_FILE_ERROR        = CALENDAR_SYSTEM_ERROR + 3;
/*Failed to get the seesion Dbus */
const int CALENDAR_DBUS_ERROR        = CALENDAR_SYSTEM_ERROR + 3;
/*Failed to insert data in to DB because disk if full*/
const int CALENDAR_DISK_FULL        = CALENDAR_SYSTEM_ERROR + 4;
/*Clock Daemon is not active*/
const int CALENDAR_LTIME_ERROR        = CALENDAR_SYSTEM_ERROR + 6;
/** Libalarm ERROR defined as -ve value **/
const int CALENDAR_LIBALARM_ERROR     =  0 - CALENDAR_SYSTEM_ERROR;
/*Alarm daemon not active*/
const int CALENDAR_ALARM_ERROR         = -1 - CALENDAR_SYSTEM_ERROR;

/* Application error */
const int CALENDAR_APP_ERROR        = 200;
/*DB returns 2 rows with same ID*/
const int CALENDAR_FUNC_ERROR        = CALENDAR_APP_ERROR + 1;
/*Input parameters are not valid*/
const int CALENDAR_INVALID_ARG_ERROR    = CALENDAR_APP_ERROR + 2;
/*All errors using ICAL parser for ICS file*/
const int CALENDAR_ICAL_PARSE_ERROR    = CALENDAR_APP_ERROR + 3;
/*Unable to open file,read write because file not present*/
const int CALENDAR_INVALID_FILE        = CALENDAR_APP_ERROR + 4;
/*calendar with specified ID doesn't exist*/
const int CALENDAR_DOESNOT_EXISTS   = CALENDAR_APP_ERROR + 5;
/*trying to add a calendar which already exist*/
const int CALENDAR_ALREADY_EXISTS   = CALENDAR_APP_ERROR + 6;
/*No calendar in DB*/
const int CALENDAR_NONE_INDB        = CALENDAR_APP_ERROR + 7 ;
/*Invalid ics file */
const int CALENDAR_INVALID_ICSFILE = CALENDAR_APP_ERROR + 8;
/* Cannot delete calendars- default calendars cannot be deleted
 */
const int CALENDAR_CANNOT_BE_DELETED = CALENDAR_APP_ERROR + 9;

/* Duplicate Entry exist in  Database so no new entry will be created 
 * old entry will be updated
 */
const int CALENDAR_ENTRY_DUPLICATED = CALENDAR_APP_ERROR + 10 ;
/* no duplicate item in database */
const int NO_DUPLICATE_ITEM = CALENDAR_APP_ERROR + 11;
/* external item is latest one so it is retained */
const int EXT_ITEM_RETAINED = CALENDAR_APP_ERROR + 12;
/* local item is the recent one so it is retained */
const int LOCAL_ITEM_RETAINED = CALENDAR_APP_ERROR + 13;
/* Calendar database Full */
const int CALENDAR_DB_FULL = CALENDAR_APP_ERROR + 14;
/* database file locked */
const int CALENDAR_DB_LOCKED = CALENDAR_APP_ERROR + 15;
/* change in Database schema */
const int CALENDAR_SCHEMA_CHANGED = CALENDAR_APP_ERROR + 16;
/* Some of entries/ all didnt get imported */
const int CALENDAR_IMPORT_INCOMPLETE = CALENDAR_APP_ERROR + 17 ;
/* Check for Ical component */
const int CALENDAR_ICS_COMPONENT_INVALID = CALENDAR_APP_ERROR + 18;
/*Batch Adding of Components encountered invalid/corrupted component */
const int CALENDAR_BATCH_ADD_INVALID = CALENDAR_APP_ERROR + 19;
/*No more commponets to fetch */
const int CALENDAR_FETCH_NOITEMS = CALENDAR_APP_ERROR + 20;
/* Calendar Success Indicator*/
const int CALENDAR_OPERATION_SUCCESSFUL    = 500;

#endif

