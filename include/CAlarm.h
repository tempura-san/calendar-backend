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

#ifndef __CALARM__H_
#define __CALARM__H_

/* Headers */
#include <vector>
#include <string>
#include "CAlarmIface.h"
#include "time.h"


/* Time definition for alarm notification */
enum dataAlarm {
    E_AM_NONE = 0,
    E_AM_ETIME,
    E_AM_5MIN,
    E_AM_15MIN,
    E_AM_30MIN,
    E_AM_1HR,
    E_AM_3HR,
    E_AM_DAYBEFORE,
    E_AM_EXACTDATETIME,
};


using namespace std;
using std::vector;

/**
 * CAlarm class
 * @brief This class contains the API's to interact with alarm daemon as well store the
 * alarm data's in the local calendar database
 */
class CAlarm : public CAlarmIface
{

public:

    /**
     * default constructor
     */

    CAlarm();

    /**
     * copy constructor
     */
    CAlarm(CAlarm &ref);

    /**
     * default destructor
     */
    ~CAlarm();

    /**
     * overloaded assignment operator
     */
    const CAlarm &operator=(const CAlarm &right);

    /**
     * parameterised constructor
     * @param trigger an integer
     * @param Repeat an integer
     * @param duration an integer
     * @param action an integer
     * @param attachments a string
     * @return none
     */

    CAlarm(int iTrigger, int iRepeat, int iDuration, int iAction, string szAttach);

    CAlarm(int iTrigger, int iDuration);

    /**
     * setTrigger
     * @param int
     * @return TRUE/FALSE
     */
    bool setTrigger(int);

    /**
     * getTrigger
     * @param none
     * @return int
     */
    int getTrigger();

    /**
     * setRepeat
     * @param int
     * @return TRUE/FALSE
     **/

    bool setRepeat(int iRepeat);
    /**
     * getRepeat
     * @param none
     * @return int
     */

    int getRepeat();
    /**
     * setDuration
     * @param int
     * @return TRUE/FALSE
     */

    bool setDuration(int iDuration);
    /**
     * getDuration
     * @param none
     * @return int
     */

    int getDuration();
    /**
     * setTimeBefore
     * @param int
     * @return TRUE/FALSE
     */

    bool setTimeBefore(int iSecondsBeforeEvent);
    /**
     * getTimeBefore
     * @param none
     * @return int
     */

    int getTimeBefore();
    /**
     * setAction
     * @param int
     * @return TRUE/FALSE
     */

    bool setAction(int iAction);
    /**
     * getAction
     * @param none
     * @return int sdfsd
     */

    int getAction();
    /**
     * setAttach
     * @param int
     * @return TRUE/FALSE
     */

    bool setAttach(string szAttach);

    /**
     * getAttach
     * @param none
     * @@return string
     */
    string getAttach();

    /**
     * getCookie()
     * @param None
     * @return vector<integer> cookie for an Alarm
     *
     * Returns the cookie value for an Alarm
     */
    vector < long > getCookie();
    /**
     * setCookie(int)
     * @param vector<integer> cookie value of an alarm
     * @return boolean value to indicate success or failure
     *
     * Sets the cookie value of an Alarm
     */
    bool setCookie(vector < long > icookie);


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
     * @param szdescription Description of the entry
     * @param iType Component Type
     * @param pErrorCode reference to integer
     *
     * @return long Alarm cookie value
     */
    long addAlarmEvent(time_t event_time, string message,
                       string location, time_t dateStart,
                       time_t dateEnd, string compId, int iCalendarId,
                       string szDescription, int iType, bool allday,
                       string szZone, int &pErrorCode);

    /**
     * registerAlarmForAutoDeletion
     * Function to register an alarm for automatic deletion of events.
     *
     * @param alarm_time Event time in time_t format
     * @param pErrorCode refece to intger error code
     *
     * @return long Alarm cookie value
     */
    long registerAlarmForAutoDeletion(time_t alarm_time, int &pErrorCode);

    /**
     * deleteAlarmEvent
     * Function to delete the alarm event
     *
     * @param long Alarm cookie value
     * @param pErrorCode refece to intger error code
     *
     * @return int Return value for delete alarm event
     */
    int deleteAlarmEvent(long cookie, int &pErrorCode);

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
     * @param iType component type
     * @param pErrorCode reference to integer
     *
     * @return long New Alarm cookie value
     *
     * Function to modify the alarm event
     */
    long modifyAlarmEvent(long oldcookie, time_t modify_time,
                          string message, string location, time_t dateStart,
                          time_t dateEnd, string sCompId, int iCalendarId,
                          string szDescription, int iType, bool allday, string szZone, int &pErrorCode);
    /**
     * toString
     */
    string toString();

    /**
     *
     * @param alarmType
     * @return
     */
    static int getDefaultTimeBefore(dataAlarm alarmType);

    /**
     * Remove all Calendar alarms from alarm daemon
     * @return >=0 - number of removed alarms, <0 error ossured
     */
    static int purgeAlarms();
private:

    /* Private variables are declared as per RFC */

    int iTrigger; /*!< Trigger parameter as per RFC */

    int iRepeat; /*!< Alarm repeat parameter */

    int iDuration; /*!< Alarm Duration */
    /*! Combined value of "Minutes Before" alarm type (E_AM_XXX) as (MinsBefore * 10 + E_AM_XXX)*/

    int iAction; /*!< Alarm action */

    vector < long > vcookie; /*!< Cookies vector to store them in database */

    string szAttach; /*!< Attachements */
};

#endif
