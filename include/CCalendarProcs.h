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

#ifndef __CALENDARD_PROCS_H__
#define __CALENDARD_PROCS_H__

#include <vector>
#include <time.h>
#include <CCalendarDB.h>

class CComponent;
class CBdayEvent;
class CEvent;

class  CCalendarProcs
{
public:
    CCalendarProcs(CCalendarDB *pDb);
    /**
     * Drop instances for specified component
     * @param CompId
     */
    void dropInstances(int CompId, int &Error);

    /**
     * 
     * @param CompId 
     * @param InstanceTimes 
     * @param Error 
     */
    void updateInstances(int CompId, const std::vector<time_t> &InstanceTimes, int &Error);

    /**
     * 
     * @param CompId 
     * @param InstanceTimes 
     * @param Error 
     */
    void getInstances(int CompId, std::vector<time_t> &InstanceTimes, int &Error);


    /**
     * Init Birthdays cache
     */
    int initBDay();

    /**
     * Flush Birthdays cache
     */
    int dropBDays();

    /**
     * Add Birthday event into cache
     * @param CompId compoenent id to be added
     * @param bday_date Birthday date to be cached
     * @return error code
     */
    int addBDay(std::string CompId, time_t bday_date);

    /**
     * Remove birthday event from cache
     * @param CompId - Component ID to delete
     * @return error code
     */
    int delBDay(std::string CompId);

    /**
     * Get IDs of birthday events in date range
     * @param iStart range start date
     * @param iEnd   range end date
     * @param CompIds vector of component IDs
     * @return error code
     */
    int getBDays(time_t iStart, time_t iEnd, std::vector<std::string> &CompIds);

    /**
     * Get CBdayEvent objects of birthday events in date range
     * @param iStart range start date
     * @param iEnd   range end date
     * @param CompIds vector of pointers to CBdayEvent
     * @return error code
     */
    int getBDays(time_t iStart, time_t iEnd, std::vector<CBdayEvent*> &Comps);

    /**
     * Creates CEvent from single row of Components table
     * @param pSqlRowData Raw SQL data of row from Components table
     * @param columns numner ov columns in data
     * @return pointer to newly created CCompomenent object
     */
    static CEvent *createComponentFromTableRecord(const char **pSqlRowData, int columns);

    /**
    * @param iStDate start date to do conditional fetching based on start date. -1 in case of no conditions
    * @param iEndDate end date to do conditional fetching based on end date. -1 in case of no conditions
    * @param iLimit Max limit of components to be obtanined by this function call
    * @param iOffset offset from which we need to fetch 
    * @param vComponents Vector for CComponent. If pErrorCode equals to CALENDAR_OPERATION_SUCCESSFUL,
    *           but vector is empty, no component (from offset to limit) meets the conditions,
    *           continue fetching
    * @param iQueryType query type
    *           value 1 - Indicates simple events and tasks 
    *           value 2 - indicates Repeating events 
    *           Any other value fetches all ( repeating events , non repeating events and tasks)
    *           Default value is 0.
    * @return error code
    *          CALENDAR_OPERATION_SUCCESSFUL- fetching is successful and can be continued;
    *          CALENDAR_FETCH_NOITEMS- fetching is finished (no more items found in DB);
    *          other error code- fetching is failed.
    * 
    * This function is overloaded API for fetching entries for all calendars at a time progressively 
    */
    int getComponentsAllCalendars( int iStDate, int iEndDate,
            int iLimit,int iOffset, vector<CComponent*> &vComponents, int iQueryType=0);

private:
    CCalendarDB *m_pDb;

    bool time2monthday(time_t t, int &daymonth, int &year);

};

#endif
