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

#ifndef __CCOMPONENT_H__
#define __CCOMPONENT_H__

/* Headers */
#include <string>
#include<sstream>
#include <vector>
using namespace::std;
using std::string;
using std::vector;
/* Forward declaration of classes*/
class CAlarm;
class CRecurrence;
class CRecurrenceRule;
class CAttendee;
class COrganizer;
#include <sys/time.h>
/* supported values for status*/

typedef enum {
    NEEDSACTION_STATUS = 0,
    COMPLETED_STATUS,
    INPROCESS_STATUS,
    CANCELLED_STATUS,
    CONFIRMED_STATUS,
    TENTATIVE_STATUS,
    DRAFT_STATUS,
    FINAL_STATUS,
    SENT_STATUS,
    DECLINED_STATUS,
    DELEGATED_STATUS,
    ACCEPTED_STATUS,
}statusParam;
    
/**
 * CComponent class
 * @brief This class contains the API's to get/set the basic parameters for any events/todo/notes
 * This holds the basic information for components.
 */
class CComponent {

  public:

    /**
     * CComponent()
     * This is the defult constructor for CComponent class  */
    CComponent();

    /**
     * Copy Constructor 
     */
    CComponent(CComponent &);

    /**
     * Overloaded Constructor
     */
    CComponent(string description);

    /**
     * Overloaded constructor 
     */
    CComponent(string summary, time_t tododue, int status);

    /**
     * Overloaded Constructor 
     */
    CComponent(string summary, string description, string location, time_t dateStart, time_t dateEnd);

    /**
     * overloaded Constructor
     */
    CComponent(string szuid, string szDescription, time_t stDateStart);

    /**
     * @param NONE
     * @return CRecurrenceRule* pointer to CRecurrenceRule class 
     *          from which UI can access other fields
     * 
     * funtion used to retrieve CRecurrenceRule Object 
     * present in CRecurrence Class
     */
    vector < CRecurrenceRule * >retrieveRecurreceRuleObject();

    /**
     * @param Constant reference to CComponent
     * @return Reference to CComponent
     *
     * Overloaded assignment operator for the class 
     *
     */
    const CComponent & operator=(const CComponent & right);

    /**
     * @param id entryid
     * @param type entry type
     * @param calendarId Calendar ID
     * @param flags    Flags to identify secondary table
     * @param summary Summary for event / todo / journal
     * @param description Entry description
     * @param status Entry status
     * @param dateStart Start date for event
     * @param dateEnd End date for event
     * @param lastModified Last modified date for event
     * @return: None
     * 
     * This is the parameter constructor of class calEntry where all variables are being initialised
     */

    CComponent(string id, int type,int flags, string summary, string description,
                       string location, int status, time_t dateStart, time_t dateEnd,
                       time_t lastModified,time_t iCreatedTime);


    /**
     * @param : string sId Id of the string  
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the id of the entry. 
     */
    bool setId(string sId);

    /**
     * @param :None
     * @return :string  id used to get id of entry 
     *
     * This function is used to get the  id of the entry created.
     */
    string  getId();

    /**
     * @param : int num  used to set type of entry
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the type of the entry created.
     * It could be an event,todo or journal
     * E_EVENT= 1
     * E_TODO =2
     * E_JOURNAL=3
     */
    bool setType(int num);

    /**
     * @param :None
     * @return : int  used to get type of entry 
     *
     * This function is used to get the type of the entry created.
     */
    int getType();
    /**
     * @param int iflag to indicate recurrence or alarm
     * @return bool status of the operation
     */
    bool setFlags(int iflag);


    /**
     * @param :None
     * @return : int used to get flag value.
     *
     * This function is used to get the  flag value to identify associated tables.
     */
    int getFlags();

    /**
     * @param : string szSummary it is the summary of the entry
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the summary of the entry created.
     * The summary is in string format
     */
    bool setSummary(string szSummary);

    /**
     * @param : None
     * @return :string it is the summary of the entry
     * 
     * This function is used to get the summary of the entry created.
     * The title is in string format
     */
    string getSummary();


    /**
     * @param : string szDescripton  it is the description of the entry
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the description of the entry created.
     * The description is in string format
     */
    bool setDescription(string strDescription);

    /**
     * @param : None
     * @return :string it is the description of the entry
     *
     * This function is used to get the description of the entry created.
     * The description is in string format
     */
    string getDescription();


    /**
     * @param : string szLocation it is the location of the entry
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the location of the entry created.
     * The location is in string format
     */
    bool setLocation(string szLocation);

    /**
     * @param : None
     * @return :string  it is the location of the entry
     * 
     * This function is used to get the location of the entry created.
     * The location is in string format
     */
    string getLocation();


    /**
     * @param : int iStatus Used to set the status for entry.
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the status.
     */
    bool setStatus(int iStatus);


    /**
     * @param :None
     * @return : int used to get status value
     *
     * This function is used to get the  status value.
     */
    int getStatus();


    /**
     * @param : time_t Used to set the start date for event.
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the start date along with timestamp.
     */
    bool setDateStart(time_t dateStart);


    /**
     * @param :None
     * @return :time_t used to get start date.
     *
     * This function is used to get the  start date value.
     */
    time_t getDateStart();



    /**
     * @param : time_t Used to set the date end for entry.
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the date end
     */
    bool setDateEnd(time_t dateEnd);


    /**
     * @param :None
     * @return : time_t used to get date end value
     *
     * This function is used to get the  date end.
     */
    time_t getDateEnd();


    /**
     * @param : time_t Used to set the last modified value
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the last modified
     */
    bool setLastModified(time_t lastModified);


    /**
     * @param :CRecurrence * pTrRecur  pointer to recurrence object 
     * @return : bool -status of operation
     *
     * This function is used to set the pointer to CRecurrence 
     */
    bool setRecurrence(CRecurrence * ptrRecur);
    /**
     * @param :None
     * @return : Used to get pointer to Recurrence
     *
     * This function is used to get the pointer to CRecurrence 
     */
    CRecurrence *getRecurrence();


    /**
     * @param :None
     * @return :time_t  Used to get last modified value
     *
     * This function is used to get the last modified value
     */
    time_t getLastModified();

    /**
     * @param : time_t time in UTC (integer) format.
     * @return : bool to indicate the result of operation.
     *
     * This function is used to set the Until value in Recurrence object.
     */
    bool setUntil(time_t iuntil);
    /**
     * @param : NONE.
     * @return : int time in UTC (integer) format.
     *
     * This function is used to get the Until(time) value in Recurrence object.
     */
    time_t getUntil();

    /**
     * @param : string  globally unique ID for component.
     * @return : bool to indicate the result of operation.
     *
     * This function is used to set the UID  value in Recurrence object.
     * TODO: NEED TO REMOVE THIS PARAMETER AS GUID IS REDUNDANT WE USE COMPONENT ID 
     *       AS GUID
     */
    bool setGUid(string szuid);
    /**
     * @param : NONE
     * @return : string globally unique ID for each component.
     *
     * This function is used to get the UID  value in Recurrence object.
     * TODO: NEED TO REMOVE THIS PARAMETER AS GUID IS REDUNDANT WE USE COMPONENT ID 
     *       AS GUID
     */
    string getGUid();
    /**
     * @param : NONE.
     * @return : Int Type of internal recurrence.
     *
     * This function is used to get the Recurrence type  value in Recurrence object.
     * It is used for recurrence events and todos. It can take values like 
     * 'BY_DAILY' =1
     * 'BY_WEEKLY' =2
     * 'BY_MONTHLY'=3 and so on.
     * For external events/todos the value should be set to 0.
     */
    bool setRtype(int irtype);

    /**
     * @param : NONE.
     * @return : Int Type of internal recurrence.
     *
     * This function is used to get the Recurrence type  value in Recurrence object.
     * It is used for recurrence events and todos. It can take values like 
     * 'BY_DAILY' =1
     * 'BY_WEEKLY' =2
     * 'BY_MONTHLY'=3 and so on.
     *  For external events/todos the value should be set to 0.
     * 
     */
    int getRtype();

    /**
     * @param CAlarm* uAlarm  pointer to CAlarm object
     * @return bool indicating result of operation
     *
     * This function is used to set Alarm properties
     */
    bool setAlarm(CAlarm * uAlarm);

    /**
     * @param time_before - Alarm time in seconds before component occurance start time
     * @param alarm_duration - Alarm duration (E_AM_NONE, E_AM_xxxMIN, E_AM_DAYBEFORE, etc)
     * @return bool indicating result of operation
     *
     * This function is used to set Alarm properties
     */
    bool setAlarm(int time_before, int alarm_duration);

    /**
     * @param time_before - Alarm time in seconds before component occurance start time
     * @return bool indicating result of operation
     *
     * This function is used to set Alarm properties
     */
    bool setAlarmBefore(int time_before);

    /**
     * @param NONE
     * @return CAlarm* pointer to CAlarm object
     *
     * This function is used to get Alarm properties
     */
    CAlarm *getAlarm();

    /**
     * @return seconds before event ocuurence
     *
     * This function is used to get Alarm properties
     */
    int getAlarmBefore();

    /**
     * @param  NONE
     * @return value of flag allday stored in DB
     * 
     * all day event is one in which time is not required eg Birthday, function (all day event )
     */
    int getAllDay();

    /**
     * @param int (0 not all day ,1 allday event ) 
     * @return bool result of the operation 

     * all day event is one in which time is not required eg Birthday, function (all day event )
     */
    bool setAllDay(int iAllDay);

    /**
     * @param viewBegin lower range of current view 
     * @param viewEnd higher range of current view 
     * @return vector <time_t> values on which event has to happen
     *
     * Function used to calculate the recurrence dates 
     * DO NOT USE due to performance issues.
     * Use void generateInstanceTimes(time_t,time_t,vector<time_t>&) instead
     */
    vector < time_t > generateInstanceTimes(time_t viewBegin, time_t viewEnd);

    /**
     * @param viewBegin lower range of current view 
     * @param viewEnd higher range of current view 
     * @return vector <time_t> values on which event has to happen
     *
     * Function used to calculate the recurrence dates 
     */
    void generateInstanceTimes(time_t viewBegin, time_t viewEnd, vector<time_t> &instance_times);


     /**
     * @param viewBegin lower range of current view 
     * @param viewEnd higher range of current view 
     * @return vector <time_t> values on which event has to happen
     *
     * Function used to retrieve the recurrence dates from the DB
     */
    vector < time_t > getInstanceTimes(time_t viewBegin, time_t viewEnd);

     /**
     * @param viewBegin lower range of current view 
     * @param viewEnd higher range of current view 
     * @param instance_times values on which event has to happen
     *
     * Function used to retrieve the recurrence dates from the DB
     */
    void getInstanceTimes(time_t viewBegin, time_t viewEnd, vector<time_t> &instance_times);

     /**
     * @param viewBegin lower range of current view 
     * @oaram viewEnd higher range of current view 
     * @return int number of events 
     *
     * Function used to retrieve the number of recurrence events from the DB
     */

    int getInstanceNumber(time_t viewBegin, time_t viewEnd);


    /**
     * @param NONE 
     * @return duration parameter from alarm class
     *
     * this function is used to return duration value stored in Alarm class
     */
    int getDuration();
    /**
     * @param ptrEntry Component Entry template , can be event*,todo*,journal* or CComponent*
     * @return Template class (Event/todo/journal)
     *
     * Function used to get the recurrence properties. This will be used in getEvent function.
     */
     bool getRecurrenceProperties();
     /**
      * @param template can be event todo or Journal pointer
      * @return boolean result of the operation
      *
      * this function is used to retreive Alarm properties from DB
      */
     bool getAlarmProperties();

    /**
     * Destructor for CComponent class.
     */
    virtual ~ CComponent();

    /**
     * @param: time_t iCreatedTime time in utc
     * @return: NONE 
     *
     * This function is used to set created time
     */
    void setCreatedTime(time_t iCreatedTime);

    /**
     * @param none
     * @return: time_t created time in utc
     *
     * This function is used to get created time
     */

    time_t getCreatedTime();
    /**
     * removeAlarm compoent
     */
    void removeAlarm();
    /**
     * remove recurrence compoennt
     */
    void removeRecurrence();
    /* 
     * getTzid 
     */
    string  getTzid();

    /*
     * setTzid
     */
    void setTzid(string sZone);
    /**
     * get Method to retrieve Tzoffset
     */
    time_t getTzOffset(void);

    /** 
     * function to set the tzoffset
     */
    void setTzOffset(time_t);
    
    /**
     * to string is used to print the class members
     */
    string toString();
    /**
      Function to parse substring based on a delimiter
     */
    vector < string > extractSubStrings(string SourceString, string byField);
    /**
      @param string sId  Id of the component 
      @param time_t until value 
      @param int& reference to error code   
      @return NONE

      Function to update until value for repeating event which doesn't have unitl 
      in its rrule. Until value will be updated when event view is opened for the 
      first time.
      */
    void updateUntilInDB(time_t until,int& pErrorCode);

    /**
     * Update Alarm trigger time for upcoming event instance
     * param pOutInstanceTime -  pointer to time_t to recieve instance time the alarm is set for
     * @return true if alarm is set for instnce false if no more instances
     */
    bool updateAlarmTriggerTime(time_t *pOutInstanceTime);

    /**
     * @param int  iId: Calendar id to be set
     *
     * This function is called to set the id of calendar.
     * it sets the id as the calendarid.
     */
    void setCalendarId(int iId);

    /**
     * @param: None
     * @return int: returns the calendar id
     * 
     * This function is called to get the calendar id.
     */
    int getCalendarId();

  private:

    /**
     * generateYearlyInstanceTime()
     * @param viewBegin lower range of current view 
     * @param viewEnd higher range of current view 
     * @param times vector to add generated event time
     * @return bool result of the operation,
     *          true if time was found in interval and added to times vector,
     *          false otherwise
     *
     * Function used to calculate the yearly recurrence date for short interval
     */
    bool generateYearlyInstanceTime(time_t viewBegin, time_t viewEnd, vector<time_t> &time);

  private:

    /**
     * extractSubStrings
     * @param string which needs to be parsed 
     * @return vector <strind> sub strings parsed and returned in to 
     * a vector 
     * 
     * Function to parse a substring in to string
     */
    //vector < string > extractSubStrings(string SourceString, string byField);

    string sId; /*!< Component ID */

    int iCalId; /*!< Calendar ID */

    int iType; /*!< Component Type */

    int iFlags; /*!< Flags to identify the presence of recurrence/attendees etc., */

    int iStatus; /*!< Status */

    time_t iDateStart; /*!< Start date for event */

    time_t iDateEnd; /*!< End date for event */

    time_t iLastModified; /*!< Last modified date */

    time_t iCreatedTime; /*!< Component created time */

    time_t iuntil; /*!< Recurrence until date */

    int iAllDay; /*!< All day event */

    string szSummary; /*!< Event summary */

    string szDescription; /*!< Event description */

    string szLocation; /*!< Event location */

    string szuid; /*!< Event Uid */

    CRecurrence *pRecurrence; /*!< CRecurrence class containment */

    CAlarm *pAlarm; /*!< CAlarm class containment */

    string sTzid ;

    time_t tzOffset;

    // for recurrence cache
    time_t iStDate; /*!< Start date of the interval of calculated recurrences */

    time_t iEndDate; /*!< End date of the interval of calculated recurrences */

    vector < time_t > rTimes;  /*!< already calculated recurrences */
    

};

#endif
