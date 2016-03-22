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


#include "CComponent.h"
#include "Common.h"
#include "CalendarErrors.h"


/**
 * generateInstanceTimes()
 * @param viewBegin lower range of current view
 * @param viewEnd higher range of current view
 * @return vector <time_t> values on which event has to happen
 *
 * Function used to retrieve the recurrence dates
 */
vector < time_t > CComponent::generateInstanceTimes(time_t viewBegin,
        time_t viewEnd)
{
    CAL_ERROR_LOG("CComponent::generateInstanceTimes(time_t,time_t): DEPRECTAED\n");

    vector < time_t > times;
    generateInstanceTimes(viewBegin, viewEnd, times);

    return times;
}
void CComponent::generateInstanceTimes(time_t viewBegin, time_t viewEnd, vector<time_t> &instance_times)
{
    CAL_DEBUG_LOG("Getting instances for %s \n", getId().c_str());


    instance_times.clear();
    iStDate =  viewBegin;
    iEndDate = viewEnd;

    /*return a empty list if there is no
     * recurrence defined for the event yet this
     * API is called
     */
    if(pRecurrence == 0) {
        CAL_DEBUG_LOG("No recurrence present so returning empty list\n");
        return;
    }

    if((getType() == E_BDAY) && (viewEnd - viewBegin < YEARDAYS * ONEDAY)) {
        generateYearlyInstanceTime(viewBegin, viewEnd, instance_times);
    }
    else {
        pRecurrence->generateInstanceTimes(viewBegin, viewEnd, iDateStart,
                                           (iDateEnd - iDateStart), this->getAllDay(), this->getTzid(), instance_times);
    }
}

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
bool CComponent::generateYearlyInstanceTime(time_t viewBegin, time_t viewEnd, vector<time_t> &times)
{
    CAL_DEBUG_LOG("Generate yearly instance for short interval\n");
    bool res = false;

    if(getDateStart() <= viewEnd) {
        // Get nearest event date only
        struct tm instance_date = {0};
        struct tm vbegin_date = {0};
        instance_date.tm_isdst = -1;
        CMulticalendar *mc = CMulticalendar::MCInstance();

        // Convert time_t to struct tm
        if(time_get_remote(getDateStart(), getTzid().c_str(), &instance_date) == 0 &&
                time_get_remote(viewBegin, mc->getSystemTimeZone().c_str(), &vbegin_date) == 0) {
            instance_date.tm_year = vbegin_date.tm_year;

            // Adjust year if upcoming instance will be on next year.
            if(instance_date.tm_mon < vbegin_date.tm_mon ||
                    (instance_date.tm_mon == vbegin_date.tm_mon && instance_date.tm_mday < vbegin_date.tm_mday)) {
                instance_date.tm_year++;
            }

            instance_date.tm_isdst = -1;
            // Get time_t for next birthday occurrence
            time_t instancedate_time = time_mktime(&instance_date, getTzid().c_str());
            CAL_DEBUG_LOG("Instance '%s'(%d.%d) TZ='%s' date %s, time=%ld\n",
                          getSummary().c_str(),
                          instance_date.tm_mday,
                          instance_date.tm_mon,
                          getTzid().c_str(),
                          ctime(&instancedate_time),
                          (long)instancedate_time);

            if((instancedate_time >= viewBegin) && (instancedate_time <= viewEnd) &&
                    (instance_date.tm_year + NINETEEN_HUNDRED <= MAX_SUPPORTED_YEAR_IN_YEAR)) {
                time_t instance_time = time_mktime(&instance_date, getTzid().c_str());
                CAL_DEBUG_LOG("Instance is in interval, date %s, date=%ld \n", ctime(&instance_time), instance_time);

                times.push_back(instance_time);
                res = true;
            }
            else {
                CAL_DEBUG_LOG("Instance is out of interval\n");
            }
        }
        else {
            CAL_DEBUG_LOG("Failed to convert time_t to struct tm\n");
        }
    }

    return res;
}

/**
  * @param viewBegin lower range of current view
  * @param viewEnd higher range of current view
  * @return vector <time_t> values on which event has to happen
  *
  * Function used to retrieve the recurrence dates from the DB
  */
vector < time_t > CComponent::getInstanceTimes(time_t viewBegin, time_t viewEnd)
{
    CAL_ERROR_LOG("CComponent::getInstanceTimes(time_t,time_t): DEPRECTAED\n");

    vector < time_t > times;

    generateInstanceTimes(viewBegin, viewEnd, times);
    return times;
}

/**
  * @param viewBegin lower range of current view
  * @param viewEnd higher range of current view
  * @param instance_times values on which event has to happen
  *
  * Function used to retrieve the recurrence dates from the DB
  */
void CComponent::getInstanceTimes(time_t viewBegin, time_t viewEnd, vector<time_t> &instance_times)
{
    generateInstanceTimes(viewBegin, viewEnd, instance_times);
};

/**
     * @param viewBegin lower range of current view
     * @oaram viewEnd higher range of current view
     * @return int number of events
     *
     * Function used to retrieve the number of recurrence events from the DB
     */
int CComponent::getInstanceNumber(time_t viewBegin,
                                  time_t viewEnd)
{
    CAL_DEBUG_LOG("getInstanceNumber  for %s \n", getId().c_str());


    /*return a empty list if there is no
     * recurrence defined for the event yet this
     * API is called
     */
    if(pRecurrence == 0) {
        CAL_DEBUG_LOG("No recurrence present so returning empty list\n");
        return 0;
    }

    if((getType() == E_BDAY) && (viewEnd - viewBegin < YEARDAYS * ONEDAY)) {
        vector<time_t> times;
        generateYearlyInstanceTime(viewBegin, viewEnd, times);
        return times.size();
    }

    return     pRecurrence->checkInstances(viewBegin, viewEnd, iDateStart,
                                           (iDateEnd - iDateStart), this->getAllDay(), this->getTzid());
}




/**
 * retrieveRecurrenceRuleObject
 * @param NONE
 * @return vector<CRecurrenceRule*> pointer to CRecurrenceRule class
 *          from which UI can access other fields
 *         function used to retrieve CRecurrenceRule Object
 *         present in CRecurrence Class
 */
vector < CRecurrenceRule * >CComponent::retrieveRecurreceRuleObject()
{
    vector < CRecurrenceRule * >tmp;

    if(pRecurrence != 0) {
        tmp = pRecurrence->getRecurrenceRule();
    }
    else {
        CAL_DEBUG_LOG("No Recurrence Exists for current component");
    }

    return (tmp);
}

/**
 * getDurationFromAlarm()
 * @param NONE
 * @return duration parameter from alarm class
 *
 *
 * this function is used to return duration value stored in Alarm class
 */
int CComponent::getDuration()
{
    if(!pAlarm) {
        return 0;
    }

    return (pAlarm->getDuration());
}




/**
 *getAllDay()
 *@param  NONE
 *@return value of flag allday stored in DB
 * all day event is one in which time is not required eg Birthday, function (all day event )
 */
int CComponent::getAllDay()
{
    return (this->iAllDay);
}

/**
 *setAllDay()
 *@param int (0 not all day ,1 allday event )
 *@return bool result of the operation
 * all day event is one in which time is not required eg Birthday, function (all day event )
 */
bool CComponent::setAllDay(int iAllDay)
{
    this->iAllDay = iAllDay;
    return true;
}


/**
 * setAlarm(Calarm *)
 * @param CAlarm* pointer to CAlarm object
 * @return bool indicating result of operation
 *
 * This function is used to set Alarm properties
 */
bool CComponent::setAlarm(CAlarm *uAlarm)
{

    if(uAlarm == 0) {
        return false;
    }

    removeAlarm(); //delete old pAlarm

    // TODO Remove this hack
    if(iType == E_TODO) {
        CAL_DEBUG_LOG("Adjust alarm type for Task");

        uAlarm->setDuration(E_AM_EXACTDATETIME);
        uAlarm->setTimeBefore(0);
    }

    pAlarm = new CAlarm();
    ASSERTION(pAlarm);
    *pAlarm = *uAlarm;
    return true;
}

/**
 * @param time_before - Alarm time in seconds before component occurance start time
 * @return bool indicating result of operation
 *
 * This function is used to set Alarm properties
 */
bool CComponent::setAlarmBefore(int time_before)
{
    int alarmFlag = E_AM_ETIME;

    if(time_before == 0) {
        CAL_DEBUG_LOG("time_before = %d -> E_AM_ETIME", time_before);
        alarmFlag = E_AM_ETIME;
    }
    else if(time_before == 5 * 60) {
        CAL_DEBUG_LOG("time_before = %d -> E_AM_5MIN", time_before);
        alarmFlag = E_AM_5MIN;
    }
    else if(time_before == 15 * 60) {
        CAL_DEBUG_LOG("time_before = %d -> E_AM_15MIN", time_before);
        alarmFlag = E_AM_15MIN;
    }
    else if(time_before == 30 * 60) {
        CAL_DEBUG_LOG("time_before = %d -> E_AM_30MIN", time_before);
        alarmFlag = E_AM_30MIN;
    }
    else if(time_before == 60 * 60) {
        CAL_DEBUG_LOG("time_before = %d -> E_AM_1HR", time_before);
        alarmFlag = E_AM_1HR;
    }
    else if(time_before == 3 * 60 * 60) {
        CAL_DEBUG_LOG("time_before = %d -> E_AM_3HR", time_before);
        alarmFlag = E_AM_3HR;
    }
    else if(time_before == 24 * 60 * 60) {
        CAL_DEBUG_LOG("time_before = %d -> E_AM_DAYBEFORE", time_before);
        alarmFlag = E_AM_DAYBEFORE;
    }
    else {
        CAL_DEBUG_LOG("time_before = %d -> E_AM_EXACTDATETIME", time_before);
        alarmFlag = E_AM_EXACTDATETIME;
        time_before = this->iDateStart - time_before;
    }

    CAL_DEBUG_LOG("time_before = %d, alarm flag is guessed as %d", time_before, alarmFlag);

    return setAlarm(time_before, alarmFlag);
};

/**
 * @param time_before - Alarm time in seconds before component occurance start time
 * @param alarm_duration - Alarm type (E_AM_NONE, E_AM_xxxMIN, E_AM_DAYBEFORE, etc)
 * @return bool indicating result of operation
 *
 * This function is used to set Alarm properties
 */

bool CComponent::setAlarm(int time_before, int alarm_duration)
{
    bool  retval = false;

    removeAlarm(); //delete old pAlarm

    pAlarm = new CAlarm();

    if(pAlarm) {
        pAlarm->setDuration(alarm_duration);

        if(alarm_duration == E_AM_EXACTDATETIME) {
            pAlarm->setTrigger(time_before);
        }
        else {
            pAlarm->setTimeBefore(time_before);
        }

        retval = true;
    }
    else {
        CAL_ERROR_LOG("Failed to allocate CAlarm");
    }

    return retval;
}

/**
 * Update Alarm trigger time for upcoming event instance
 * param pOutInstanceTime -  pointer to time_t to recieve instance time the alarm is set for
 * @return true if alarm is set for instnce false if no more instances
 */

bool CComponent::updateAlarmTriggerTime(time_t *pOutInstanceTime)
{
    bool retval = false;
    time_t instanceTime = -1;
    time_t triggerTime = -1;

    if(!pAlarm) {
        CAL_ERROR_LOG("No alarm set. Ignoring");
        return retval;
    }

    if(pAlarm->getDuration() == E_AM_EXACTDATETIME) {
        instanceTime = iDateStart;
        triggerTime = pAlarm->getTrigger();

        CAL_DEBUG_LOG("Event has Exact Datetime alarm at %ld (%s). Ignore recurrence.", triggerTime, ctime(&triggerTime));
    }
    else {
        int time_before = pAlarm->getTimeBefore();
        time_t now = time_get_time();

        // Get time of impendiong occurance
        if(pRecurrence) {
            time_t until = getUntil();

            if(until <= 0) {
                // Event have no Until date
                until = MAX_SUPPORTED_YEAR_IN_SEC;
            }

            vector <time_t> times = generateInstanceTimes(now + time_before, until);

            for(vector <time_t>::const_iterator iter = times.begin();
                    iter != times.end();
                    iter++) {
                if(now <= (*iter - time_before)) {
                    instanceTime = *iter;
                    triggerTime = *iter - time_before;
                    break;
                }
            }

            CAL_DEBUG_LOG("Event is recurrent, Impending occurance at %ld (%s)", instanceTime, ctime(&instanceTime));
        }
        else {
            if(now <= iDateStart - time_before) {
                instanceTime = iDateStart;
                triggerTime  = iDateStart - time_before;
            }

            CAL_DEBUG_LOG("Event is non-recurrent at %ld (%s)", instanceTime, ctime(&instanceTime));
        }

        CAL_DEBUG_LOG("Alarm trigger time %ld (%s)", triggerTime, ctime(&triggerTime));
    }

    if(triggerTime >= 0) {
        CAL_DEBUG_LOG("Set alarm at %ld (%s), flag=%d",
                      triggerTime,
                      ctime(&triggerTime),
                      pAlarm->getDuration());

        pAlarm->setTrigger(triggerTime);

        if(pOutInstanceTime) {
            *pOutInstanceTime = instanceTime;
        }

        retval = true;
    }
    else {
        CAL_DEBUG_LOG("All instances are in past. Ignoring");
    }

    return retval;
};


/**
 * getAlarm()
 * @param NONE
 * @return CAlarm* pointer to CAlarm object
 *
 * This function is used to get Alarm properties
 */
CAlarm *CComponent::getAlarm()
{
    return (this->pAlarm);
}

/**
 * @return seconds before event ocuurence, -1 if alarm is not set
 *
 * This function is used to get Alarm properties
 */
int CComponent::getAlarmBefore()
{
    int retval = -1;

    if(pAlarm) {
        retval = pAlarm->getTimeBefore();

        CAL_DEBUG_LOG("%d sec", retval);
    }
    else {
        CAL_DEBUG_LOG("No alarm");
    }

    return retval;
};


/*FOR-RECURRENCE*/
/**
 *  setUntil
 *  @param : int time in UTC (integer) format.
 *  @return : bool to indicate the result of operation.
 *
 *  This function is used to set the Until value in Recurrence object.
 *
 */
bool CComponent::setUntil(time_t iuntil)
{
    this->iuntil = iuntil;
    return true;
}

/**
 * getUntil
 * @param : NONE.
 * @return : int time in UTC (integer) format.
 *
 * This function is used to get the Until(time) value in Recurrence object.
 *
 */
time_t CComponent::getUntil()
{
    return this->iuntil;
}

/**
 *  setGUid
 *  @param : string  globally unique ID for component.
 *  @return : bool to indicate the result of operation.
 *
 *  This function is used to set the UID  value in Recurrence object.
 *
 */
bool CComponent::setGUid(string szuid)
{
    if(szuid.empty()) {
        return false;
    }

    this->szuid = szuid;
    return true;
}

/**
 * getGUid
 * @param : NONE
 * @return : string globally unique ID for each component.
 *
 * This function is used to set the UID  value in Recurrence object.
 */
string CComponent::getGUid()
{
    return this->szuid;
}

/**
 * setRtype
 * @param : int Type of recurrence  .
 * @return : bool to indicate the result of operation.
 *
 * This function is used to set the Recurrence type  value in Recurrence object.
 * It is used for recurrence events and todos. It can take values like
 * 'BY_DAILY' =1
 * 'BY_WEEKLY' =2
 * 'BY_MONTHLY'=3 and so on.
 * For external events/todos the value should be set to 0.
 */
bool CComponent::setRtype(int irtype)
{
    if(pRecurrence == 0) {
        return false;
    }

    return (pRecurrence->setRtype(irtype));
}

/**
 *  getRtype
 *  @param : NONE.
 *  @return : Int Type of internal recurrence.
 *
 *  This function is used to get the Recurrence type  value in Recurrence object.
 *  It is used for recurrence events and todos. It can take values like
 *  'BY_DAILY' =1
 *  'BY_WEEKLY' =2
 *  'BY_MONTHLY'=3 and so on.
 *  For external events/todos the value should be set to 0.
 */
int CComponent::getRtype()
{
    if(pRecurrence == 0) {
        return 0;
    }

    /* in case recurrence is not null */
    return (pRecurrence->getRtype());
}


/**
 * setRecurrence()
 * @param :CRecurrence *
 * @return : bool
 *
 * This function is used to set the pointer to CRecurrence
 */
bool CComponent::setRecurrence(CRecurrence *uRecurrence)
{
    if(uRecurrence == 0) {
        return false;
    }

    removeRecurrence();
    pRecurrence = new CRecurrence();
    ASSERTION(pRecurrence);
    *pRecurrence = *uRecurrence;
    return true;
}

/**
 * getRecurrence()
 * @param :None
 * @return : Used to get pointer to Recurrence
 *
 * This function is used to get the pointer to CRecurrence
 */
CRecurrence *CComponent::getRecurrence()
{
    return (pRecurrence);
}

/**
 * overloaded assignment operator
 */

const CComponent &CComponent::operator=(const CComponent &right)
{
    if(&right != this) {       //prevent self assignment

        sId = right.sId;
        iType = right.iType;
        iFlags = right.iFlags;
        szSummary = right.szSummary;
        sTzid = right.sTzid;
        tzOffset = right.tzOffset;
        szDescription = right.szDescription;
        szLocation = right.szLocation;
        iStatus = right.iStatus;
        iDateEnd = right.iDateEnd;
        iDateStart = right.iDateStart;
        iAllDay = right.iAllDay;
        iCalId = right.iCalId;
        removeRecurrence();

        if(right.pRecurrence) {
            this->setRecurrence(right.pRecurrence);
        }

        removeAlarm();

        if(right.pAlarm) {
            this->setAlarm(right.pAlarm);
        }

    }

    return *this;

}

/**
 * Component()
 * CComponent copy constructor
 */
CComponent::CComponent(CComponent &ref)
{
    sId = ref.sId;
    iType = ref.iType;
    iFlags = ref.iFlags;
    sTzid = ref.sTzid;
    tzOffset = ref.tzOffset;
    szSummary = ref.szSummary;
    szDescription = ref.szDescription;
    szLocation = ref.szLocation;
    iStatus = ref.iStatus;
    iDateStart = ref.iDateStart;
    iDateEnd = ref.iDateEnd;
    iAllDay = ref.iAllDay;
    iCreatedTime = ref.iCreatedTime;
    iLastModified = ref.iLastModified;
    iuntil = ref.iuntil;
    pRecurrence = 0;
    pAlarm = 0;

    if(ref.pRecurrence) {
        this->setRecurrence(ref.pRecurrence);
    }

    if(ref.pAlarm) {
        this->setAlarm(ref.pAlarm);
    }

    iEndDate = ref.iEndDate;
    iStDate = ref.iStDate;
    iCalId = ref.iCalId;
}

/**
 * CComponent ()
 * Default CComponent constructor
 */
CComponent::CComponent(): iCalId(0), iType(-1), iFlags(-1), iStatus(-1), iDateStart(-1), iDateEnd(-1),
    iLastModified(-1), iCreatedTime(-1), iuntil(-1), iAllDay(0),
    pRecurrence(0), pAlarm(0), tzOffset(0), iStDate(-1), iEndDate(-1)
{
}


/**
 * CComponent
 * Overloaded Parametrized constructor
 */
CComponent::CComponent(string szSummary, time_t stTododue,
                       int iStatus): iCalId(0), iType(-1), iFlags(-1), iStatus(iStatus),
    iDateStart(stTododue), iDateEnd(-1), iLastModified(-1), iCreatedTime(-1), iuntil(-1), iAllDay(0),
    szSummary(szSummary), pRecurrence(0), pAlarm(0), tzOffset(0), iStDate(-1), iEndDate(-1)
{
}



/**
 * CComponent
 * Overloaded Parametrized constructor for CBdayEvent
 */
CComponent::CComponent(string szuid, string szSummary, time_t iDateStart):
    sId(szuid), iCalId(0), iType(E_BDAY), iFlags(-1), iStatus(-1), iDateStart(iDateStart),
    iDateEnd(-1), iLastModified(-1), iCreatedTime(-1), iuntil(-1), iAllDay(0), szSummary(szSummary),
    pRecurrence(0), pAlarm(0), tzOffset(0), iStDate(-1), iEndDate(-1)
{
}


/**
 * CComponent
 * Overloaded Parametrized  with minimal parameters for UI
 */

CComponent::CComponent(string szSummary, string szDescription, string szLocation, time_t iDateStart, time_t iDateEnd):
    iCalId(0), iType(-1), iFlags(-1), iStatus(-1), iDateStart(iDateStart),
    iDateEnd(iDateEnd), iLastModified(-1), iCreatedTime(-1), iuntil(-1), iAllDay(0), szSummary(szSummary),
    szDescription(szDescription), szLocation(szLocation), pRecurrence(NULL),
    pAlarm(NULL), tzOffset(0), iStDate(-1), iEndDate(-1)
{
}


/**
 * CComponent
 * Overloaded Parametrized constructor
 */
CComponent::CComponent(string sId, int iType, int iFlags, string szSummary,
                       string szDescription, string szLocation,
                       int iStatus, time_t iDateStart, time_t iDateEnd,
                       time_t iLastModified, time_t iCreatedTime): sId(sId),
    iCalId(0), iType(iType), iFlags(iFlags), iStatus(-1), iDateStart(iDateStart),
    iDateEnd(iDateEnd), iLastModified(iLastModified),
    iCreatedTime(iCreatedTime), iuntil(-1), iAllDay(0), szSummary(szSummary),
    szDescription(szDescription), szLocation(szLocation), pRecurrence(NULL),
    pAlarm(NULL), tzOffset(0), iStDate(-1), iEndDate(-1)
{
}

/**
 * Overloaded Constructor
 */
CComponent::CComponent(string szDescription):
    iCalId(0), iType(-1), iFlags(-1), iStatus(-1), iDateStart(time_get_time()),
    iDateEnd(-1), iLastModified(-1), iCreatedTime(-1), iuntil(-1), iAllDay(0),
    szSummary(szDescription), pRecurrence(NULL),
    pAlarm(NULL), tzOffset(0), iStDate(-1), iEndDate(-1)
{
}


/**
 * setId
 * @param : string sId
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the id of the entry created.
 */
bool CComponent::setId(string sId)
{
    this->sId = sId;
    return true;
}


/**
 * getId()
 * @param :None
 * @return : string  > > used to get id of entry
 *
 * This function is used to get the  id of the entry created.
 */
string CComponent::getId()
{
    return this->sId;
}

/**
 * setType()
 * @param : num(int) > > used to set type of entry
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the type of the entry created.
 * It could be an event,todo or journal
 */
bool CComponent::setType(int type)
{
    this->iType = type;
    return true;
}

/**
 * getType()
 * @param :None
 * @return : type(int) > > used to get type of entry
 *
 * This function is used to get the type of the entry created.
 */
int CComponent::getType()
{
    return this->iType;
}


/**
 * setFlags()
 * @param : calendarId(int) > > Used to set the flags.
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the flag value to identify the associated tables
 */
bool CComponent::setFlags(int flags)
{

    this->iFlags = flags;
    return true;
}


/**
 * getFlags()
 * @param :None
 * @return : id(int) > > used to get flag value.
 *
 * This function is used to get the  flag value to identify associated tables.
 */
int CComponent::getFlags()
{
    return this->iFlags;
}

/**
 * setSummary()
 * @param : string > > it is the summary of the entry
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the summary of the entry created.
 * The summary is in string format
 */
bool CComponent::setSummary(string summary)
{
    if(summary.empty()) {
        return false;
    }

    this->szSummary = summary;
    return true;
}


/**
 * getSummary()
 * @param : None
 * @return :string > > it is the summary of the entry
 *getAttach()
 * This function is used to get the summary of the entry created.
 * The title is in string format
 */
string CComponent::getSummary()
{
    return this->szSummary;
}

/**
 * setDescription()
 * @param : string > > it is the description of the entry
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the description of the entry created.
 * The description is in string format
 */
bool CComponent::setDescription(string description)
{
    this->szDescription = description;
    return true;
}

/**
 * getDescription ()
 * @param : None
 * @return :string > > it is the description of the entry
 *getAttach()
 * This function is used to get the description of the entry created.
 * The description is in string format
 */
string CComponent::getDescription()
{
    return this->szDescription;
}

/**
 * setLocation()
 * @param : string > > it is the location of the entry
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the location of the entry created.
 * The location is in string format
 */
bool CComponent::setLocation(string location)
{
    this->szLocation = location;
    return true;
}



void CComponent::removeAlarm()
{
    if(pAlarm) {
        delete pAlarm ;
    }

    pAlarm = 0;

    if(iFlags == HAS_ALARM) {
        iFlags = -1;
    }
    else if(iFlags == HAS_RECURRENCE_ALARM) {
        iFlags = HAS_RECURRENCE;
    }
}
void CComponent::removeRecurrence()
{
    if(pRecurrence) {
        delete pRecurrence;
    }

    pRecurrence = 0;

}
/**
 * getLocation ()
 * @param : None
 *
 * @return :string > > it is the location of the entry
 *getAttach()
 * This function is used to get the location of the entry created.
 * The location is in string format
 */



/**
 * setCreatedTime
 * @param int time in utc
 * @return bool indicating result of operation
 *
 * This function is used to set created time
 */
void CComponent::setCreatedTime(time_t iCreatedTime)
{
    this->iCreatedTime = iCreatedTime;
}

/**
 * getCreatedTime
 * @param none
 * @return created time in utc
 *
 * This function is used to get created time
 *
 */

time_t CComponent::getCreatedTime()
{
    return this->iCreatedTime;
}


string CComponent::getLocation()
{
    return this->szLocation;
}

/**
 * setStatus()
 * @param : Used to set the status for entry.
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the status.
 */
bool CComponent::setStatus(int status)
{
    this->iStatus = status;
    return true;
}


/**
 * getStatus()
 * @param :None
 * @return : id(int) > > used to get status value
 *
 * This function is used to get the  status value.
 */
int CComponent::getStatus()
{
    return this->iStatus;
}


/**
 * setDateStart()
 * @param : Used to set the start date for event.
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the start date along with timestamp.
 */
bool CComponent::setDateStart(time_t dateStart)
{
    if(dateStart < 0) {
        return false;
    }

    this->iDateStart = dateStart;
    return true;
}


/**
 * getDateStart()
 * @param :None
 * @return : used to get start date.
 *
 * This function is used to get the  start date value.
 */
time_t CComponent::getDateStart()
{
    return this->iDateStart;
}



/**
 * setDateEnd()
 * @param : Used to set the date end for entry.
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the date end
 */
bool CComponent::setDateEnd(time_t dateEnd)
{
    this->iDateEnd = dateEnd;
    return true;
}


/**
 * getDateEnd()
 * @param :None
 * @return : used to get date end value
 *
 * This function is used to get the  date end.
 */
time_t CComponent::getDateEnd()
{
    return this->iDateEnd;
}


/**
 * setLastModified()
 * @param : Used to set the last modified value
 * @return :bool (SUCCESS/FAILURE)
 *
 * This function is used to set the last modified
 */
bool CComponent::setLastModified(time_t lastModified)
{
    this->iLastModified = lastModified;
    return true;
}


/**
 * getLastModified()
 * @param :None
 * @return : Used to get last modified value
 *
 * This function is used to get the last modified value
 */

time_t CComponent::getLastModified()
{
    return this->iLastModified;
}


/**
 * extractSubStrings
 * @param string which needs to be parsed
 * @return vector <strind> sub strings parsed and returned in to
 * a vector
 * Function to parse a substring in to string
 */
vector < string > CComponent::extractSubStrings(string sub, string by)
{

    vector < string > vRrules;
    vRrules.clear();
    char *cstr = 0;
    char *ptr = 0;

    cstr = new char[sub.length() + 1];
    ASSERTION(cstr);
    memset(cstr, 0, sub.length() + 1);
    strcpy(cstr, sub.c_str());
    ptr = strtok(cstr, by.c_str());

    while(ptr != NULL) {
        string test;
        test = ptr;
        vRrules.push_back(test);
        ptr = strtok(NULL, by.c_str());
    }

    unsigned int iCount;

    for(iCount = 0; iCount < vRrules.size(); iCount++) {
        CAL_DEBUG_LOG("%s", vRrules[iCount].c_str());
    }

    delete [] cstr;
    cstr = 0;
    return vRrules;
}





/**
 * getRecurrenceProperties()
 * @param ptrEntry Component Entry template , can be event*,todo*,journal* or CComponent*
 *
 * @return Template class (Event/todo/journal)
 * Function used to get the recurrence properties. This will be used in getEvent function.
 */
bool CComponent::getRecurrenceProperties()
{
    QueryResult *pQr = NULL;
    int iI_RecurCount = 0;
    int iJ_RecurCount = 0;
    int iK_RecurCount = 0;
    char *pQuery = NULL;
    CCalendarDB *pDb = NULL;
    int iSqliteError = 0;
    vector < string > vListexdates;
    vector < string > vListrdates;
    vector < string > vRrules;
    vector < string > vExrules;
    string cache;
    vExrules.clear();
    vRrules.clear();
    vListexdates.clear();
    vListrdates.clear();

    pDb = CCalendarDB::Instance();

    if((pDb == NULL) || (this->getId().empty())) {
        return FAILURE;
    }

    pQuery = sqlite3_mprintf(SELECT_TAB_COND2, RECURSIVE_TABLE,
                             ENTRY_FIELD_ID, this->getId().c_str());

    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery, iSqliteError);
    sqlite3_free(pQuery);

    /* delete pqr is being done inside getRecords Procedure */
    if(pQr == 0) {
        return FAILURE;
    }

    if(pQr->iRow > TWO_ROWS) {
        CAL_DEBUG_LOG
        ("Database corruption, two rows found with same entry id.\n");
        sqlite3_free_table(pQr->pResult);
        delete pQr;
        pQr = 0;
        return FAILURE;
    }

    CRecurrence *pRecur = new CRecurrence();
    ASSERTION(pRecur);
    iI_RecurCount = 0;

    for(iJ_RecurCount = 0; iJ_RecurCount < pQr->iColumn; iJ_RecurCount++) {
        iK_RecurCount = pQr->iColumn + (iI_RecurCount * pQr->iColumn);

        if(!pQr->pResult[iK_RecurCount + iJ_RecurCount]) {
            continue;
        }

        switch(iJ_RecurCount) {

        case DB_COLUMN_ID1:
            this->setId((pQr->pResult[iK_RecurCount + iJ_RecurCount]));
            break;

        case DB_COLUMN_ID2:
            vRrules =
                this->extractSubStrings(pQr->
                                        pResult[iK_RecurCount +
                                                iJ_RecurCount],
                                        EXCLAMATION);
            pRecur->setRrule(vRrules);
            break;

        case DB_COLUMN_ID3:
            vListrdates =
                this->extractSubStrings(pQr->
                                        pResult[iK_RecurCount +
                                                iJ_RecurCount],
                                        SEMI_COLON);
            pRecur->setRDays(vListrdates);
            break;

        case DB_COLUMN_ID4:
            vListexdates =
                this->extractSubStrings(pQr->
                                        pResult[iK_RecurCount +
                                                iJ_RecurCount],
                                        SEMI_COLON);
            pRecur->setEDays(vListexdates);
            break;

        case DB_COLUMN_ID5:
            vExrules =
                this->extractSubStrings(pQr->
                                        pResult[iK_RecurCount +
                                                iJ_RecurCount],
                                        EXCLAMATION);
            pRecur->setErule(vExrules);
            break;

        case DB_COLUMN_ID6:
            pRecur->setRecurId(atoi(pQr->pResult[iK_RecurCount + iJ_RecurCount]));
            break;

        case DB_COLUMN_ID7:
            pRecur->setRtype(atoi(pQr->pResult[iK_RecurCount + iJ_RecurCount]));
            break;

        default:
            break;

        }
    }

    this->setRecurrence(pRecur);
    delete pRecur;
    pRecur = 0;

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;

    return SUCCESS;
}


/**
 * getAlarmProperties()
 * @param template can be event todo or Journal pointer
 * @return boolean result of the operation
 *
 * this function is used to retreive Alarm properties from DB
 */
bool CComponent::getAlarmProperties()
{

    QueryResult *pQr = NULL;
    int iI_AlarmCount = 0;
    int iJ_AlarmCount = 0;
    int iK_AlarmCount = 0;
    int iSqliteError = 0;
    char *pQuery = NULL;
    CCalendarDB *pDb = NULL;
    vector < long >icookie;

    pDb = CCalendarDB::Instance();

    if((pDb == NULL) || (this->getId().empty())) {
        return FAILURE;
    }


    pQuery = sqlite3_mprintf(SELECT_TAB_COND2, ALARM_TABLE, ENTRY_FIELD_ID,
                             this->getId().c_str());

    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery, iSqliteError);
    sqlite3_free(pQuery);

    /* delete pqr is being done inside getRecords Procedure */
    if(pQr == 0) {
        return FAILURE;
    }

    if(pQr->iRow > TWO_ROWS) {
        CAL_DEBUG_LOG
        ("Database corruption, two rows found with same entry id.\n");
        sqlite3_free_table(pQr->pResult);
        delete pQr;
        pQr = 0;
        return FAILURE;
    }

    CAlarm *pAlarm = new CAlarm();
    ASSERTION(pAlarm);
    iI_AlarmCount = 0;

    for(iJ_AlarmCount = 0; iJ_AlarmCount < pQr->iColumn; iJ_AlarmCount++) {
        iK_AlarmCount = pQr->iColumn + (iI_AlarmCount * pQr->iColumn);

        switch(iJ_AlarmCount) {

        case DB_COLUMN_ID1:
            //not required as it will be already set
            //always before reaching here
            //this->setId(atoi(pQr->pResult[iK + iJ]));
            break;

        case DB_COLUMN_ID2:
            pAlarm->
            setTrigger(atoi
                       (pQr->pResult[iK_AlarmCount + iJ_AlarmCount]));
            break;

        case DB_COLUMN_ID3:
            pAlarm->
            setRepeat(atoi(pQr->pResult[iK_AlarmCount + iJ_AlarmCount]));
            break;

        case DB_COLUMN_ID4: {
            int raw_duration = atoi(pQr->pResult[iK_AlarmCount + iJ_AlarmCount]);
            pAlarm->setDuration(raw_duration % 10);
            pAlarm->setTimeBefore(raw_duration / 10);
        }
        break;

        case DB_COLUMN_ID5:
            pAlarm->
            setAction(atoi
                      (pQr->pResult[iK_AlarmCount + iJ_AlarmCount]));
            break;

        case DB_COLUMN_ID6:
            icookie.clear();

            if(pQr->pResult[iK_AlarmCount + iJ_AlarmCount] != 0)
                icookie.push_back(atoi(pQr->pResult[iK_AlarmCount +
                                                    iJ_AlarmCount]));

            pAlarm->setCookie(icookie);
            break;

        case DB_COLUMN_ID7:
            if(pQr->pResult[iK_AlarmCount + iJ_AlarmCount]) {
                pAlarm->setAttach(pQr->
                                  pResult[iK_AlarmCount + iJ_AlarmCount]);
            }

            break;

        default:
            break;
        }
    }

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    this->setAlarm(pAlarm);
    delete pAlarm;
    pAlarm = 0;
    return SUCCESS;
}



/**
 * Destructor for CComponent class.
 */
CComponent::~CComponent()
{
    if(pAlarm != NULL) {
        delete pAlarm;
    }

    if(pRecurrence != NULL) {
        delete pRecurrence;
    }
}

/*
 * getTzid
 */
string  CComponent ::getTzid()
{
    return this->sTzid ;
}
/*
 * setTzid
 */
void CComponent::setTzid(string sZoneId)
{
    sTzid = sZoneId;
    setTzOffset(time_get_time_diff(getDateStart(), sZoneId.c_str(), "UTC"));
}


void CComponent::setTzOffset(time_t offset)
{
    tzOffset = offset;
}

time_t CComponent::getTzOffset()
{
    return tzOffset;
}

string CComponent::toString()
{
    CAlarm *pAlarm;
    CRecurrence *cRec;
    string szRet;
    string szTemp;
    std::stringstream ss;
    ss << "\n**********************\n";
    ss << "ID=";

    if(sId.c_str()) {
        szTemp = sId.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",CalendarId=";
    ss << getCalendarId();

    ss << ",summary=";

    if(szSummary.c_str()) {
        szTemp = szSummary.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",description=";

    if(szDescription.c_str()) {
        szTemp = szDescription.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",location=";

    if(szLocation.c_str()) {
        szTemp = szLocation.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",TimeZone=";

    if(sTzid.c_str()) {
        szTemp = sTzid.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",UId=";

    if(szuid.c_str()) {
        szTemp = szuid.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Type=";
    ss << iType ;
    ss << ",Flags=";

    switch(iFlags) {
    case HAS_ATTENDEE:
        ss << "Has Attendee";
        break;

    case HAS_ORGANIZER:
        ss << "Has Organizer";
        break;

    case HAS_RECURRENCE:
        ss << "Has Recurrence";
        break;

    case HAS_ALARM:
        ss << "Has Alarm";
        break;

    case HAS_RECURRENCE_ALARM :
        ss << "Has Recurrent Alarm";
        break;

    case HAS_PARTICIPANT :
        ss << "Has Participant";
        break;

    case HAS_CATEGORIES :
        ss << "Has Categories";
        break;

    case HAS_COMMENT:
        ss << "Has Comment ";
        break;

    case HAS_EXTRA:
        ss << "Has Extra ";
        break;

    default:
        break;
    }

    ss << ",Status=";
    ss << iStatus;
    ss << ",Start date=";
    ss << ctime(&iDateStart);
    ss << ",End date=";
    ss << ctime(&iDateEnd);
    ss << ",Last modified date=";
    ss << ctime(&iLastModified);
    ss << ",created time=";
    ss << ctime(&iCreatedTime);
    ss << ",until=";
    ss << ctime(&iuntil);
    ss << ",All day=";
    ss << iAllDay;
    ss << ",Timezone= ";
    ss << this->getTzid();

    pAlarm = getAlarm();

    if(pAlarm) {
        ss << pAlarm->toString();
    }
    else {
        ss << ",Alarm=NULL" ;
    }

    cRec = getRecurrence();

    if(cRec) {
        ss << cRec->toString();
    }
    else {
        ss << ",Recurrence=NULL";
    }

    ss << "\n**********************\n";
    szRet.append(ss.str());
    return szRet;
}




/**
 @param string sId  Id of the component
 @param time_t until value
 @param int& reference to error code
 @return NONE

 Function to update until value for repeating event which doesn't have unitl
 in its rrule. Until value will be updated when event view is opened for the
 first time.
*/
void  CComponent::updateUntilInDB(time_t until, int &pErrorCode)
{
    int iSqlError = 0;
    char *pQuery = 0;
    CCalendarDB *pCalDb = CCalendarDB::Instance();

    if(pCalDb == NULL) {
        CAL_DEBUG_LOG("Calendar:Invalid CalendarDB");
        pErrorCode = CALENDAR_APP_ERROR;
        return;
    }

    pQuery = sqlite3_mprintf(UPDATE_UNTIL, until, atoi(sId.c_str()));
    ASSERTION(pQuery);
    iSqlError = pCalDb->execSQL(pQuery);
    sqlite3_free(pQuery);
    pCalDb->sqliteErrorMapper(iSqlError, pErrorCode);
    CAL_DEBUG_LOG("Error code is %d", pErrorCode);
    return;
}



/**
 * @param int  iId: Calendar id to be set
 *
 * This function is called to set the id of calendar.
 * it sets the id as the calendarid.
 */
void CComponent::setCalendarId(int iId)
{
    iCalId = iId;
}

/**
 * @param: None
 * @return int: returns the calendar id
 *
 * This function is called to get the calendar id.
 */
int CComponent::getCalendarId()
{
    return iCalId;
}
