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

#ifndef __CALARM_IFACE_H_
#define __CALARM__IFACE_H_

/* Headers */
#include <string>
#include "time.h"


using namespace std;
using std::string;

/**
 * CAlarmIface class
 * @brief This is a  pure Abstract class
 */

class CAlarmIface{

  public:

    /**
     * addAlarmEvent
     * Function to add the alarm event
     *
     * @param event_time Event time in time_t format
     * @param message Event summary displays in alarm message
     * @param location Event location displays in alarm message
     * @param dateStart Event start date
     * @param dateEnd Event end date
     * @param compId component Id
     * @param iCalendarId Calendar Id
     * @param szDescription component Id
     * @param iType  component Type
     * @param pErrorCode reference to error code 
     * 
     * @return long Alarm cookie value
     */
    virtual long addAlarmEvent(time_t event_time, string message,
                   string location, time_t dateStart,
                   time_t dateEnd, string compId, int iCalendarId,
                   string szDescription,int iType,bool allday,string szZone,int& pErrorCode) = 0;


    /**
     * deleteAlarmEvent
     * Function to delete the alarm event
     *
     * @param long Alarm cookie value
     * @param pErrorCode reference to Error code 
     * 
     * @return int Return value for delete alarm event
     */
    virtual int deleteAlarmEvent(long cookie,int& pErrorCode)= 0;

    /**
     * modifyAlarmEvent
     *
     * @param oldcookie Old cookie value
     * @param modify_time Event time in time_t format
     * @param message Event summary displays in alarm message
     * @param location Event location displays in alarm message
     * @param dateStart Event start date
     * @param dateEnd Event end date
     * @param iCalendarId Calendar Id string
     * @param szDescription  description
     * @param iType  component Type
     * @param pErrorCode reference to error code 
     *
     * @return long New Alarm cookie value
     *
     * Function to modify the alarm event
     */
    virtual long modifyAlarmEvent(long oldcookie, time_t modify_time,
                  string message, string location, time_t dateStart,
                  time_t dateEnd, string sCompId, int iCalendarId,
                  string szDescription,int iType,bool allday,string szZone,
		  int& pErrorCode)= 0;
    /* default constructor */
    CAlarmIface();
    /* virtual destructor */
    virtual ~CAlarmIface();
};

#endif
