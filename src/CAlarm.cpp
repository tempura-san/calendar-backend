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
 * Header files included here 
 */
#include "CAlarm.h"
#include "CalendarLog.h"
#include <dbus/dbus.h>
#include "CalendarErrors.h"
#include "CalInterface.h"
#include <sstream>
#include "Common.h"
extern "C" {
#include <alarmd/libalarm.h>
}

/**
 * CAlarm
 * Default constructor
 */
CAlarm::CAlarm():CAlarmIface()
{
    iTrigger = 0;
    iDuration = 0;
    iRepeat = ONE;
    iAction = 0;

}

/**
 * ~CAlarm
 * Default destructor
 */
CAlarm::~CAlarm()
{

}

/**
 * Overloaded assignment operator for CAlarm class
 */
const CAlarm & CAlarm::operator=(const CAlarm & right)
{
    /* Check for self assignment */
    if (&right != this) {
    iTrigger = right.iTrigger;
    iRepeat = right.iRepeat;
    iDuration = right.iDuration;
    iAction = right.iAction;
    vcookie = right.vcookie;
    szAttach = right.szAttach;
    }
    return *this;
}

/**
 * CAlarm:: CAlarm(CAlarm & ref)
 * Copy constructor for CAlarm class
 */
CAlarm::CAlarm(CAlarm & ref)
{
    iTrigger = ref.iTrigger;
    iRepeat = ref.iRepeat;
    iDuration = ref.iDuration;
    iAction = ref.iAction;
    vcookie = ref.vcookie;
    szAttach = ref.szAttach;
}

/**
 * CAlarm
 * Parameterised constructor
 * 
 * @param trigger an integer time in time_t format used to know the time of occurance of alarm
 * @param Repeat an integer number of times alarm gonna repeat
 * @param duration an integer, duration of alarm 
 * @param action an integer, Action required to be taken Eg AUDIO ,EMAIL etc
 * @param attachments a string
 * 
 * @return none
 */
CAlarm::CAlarm(int iTrigger, int iRepeat, int iDuration, int iAction, string szAttach):iTrigger(iTrigger),
iRepeat(iRepeat), iDuration(iDuration),
iAction(iAction), szAttach(szAttach)
{
}


/**
 * CAlarm
 * Parameterized constructor
 *
 * @param iTrigger Integer time in time_t format used to know the time of occurance of alarm
 * @oparm duration Duration of alarm
 * @param cookie   Alarm cookie value
 *
 * @return none
 */
CAlarm::CAlarm(int iTrigger, int iDuration):iTrigger(iTrigger),
iDuration(iDuration)
{
    iRepeat = ONE;
    iAction = 0;
}

/**
 * setTrigger
 * Function to set the trigger value in class
 * 
 * @param int
 * 
 * @return TRUE/FALSE
 */
bool CAlarm::setTrigger(int iTrigger)
{
    if (iTrigger < 0)
    {
        CAL_ERROR_LOG("Negative value %d is passed. Ignore", iTrigger);
        return false;
    }

    this->iTrigger = iTrigger;
    return true;
}


/**
 * getTrigger
 * Function to get the trigger
 * 
 * @param none
 * 
 * @return int
 */
int CAlarm::getTrigger()
{
    return this->iTrigger;
}


/**
 * setRepeat
 * Function to set the repeat value
 * 
 * @param int
 * 
 * @return TRUE/FALSE
 */
bool CAlarm::setRepeat(int iRepeat)
{
    if (iRepeat < 0)
    {
        CAL_ERROR_LOG("Negative value %d is passed. Ignore", iRepeat);
        return false;
    }

    this->iRepeat = iRepeat;
    return true;
}

/**
 * getRepeat
 * Function to get the repeat value
 * 
 * @param none
 * 
 * @return int
 */
int CAlarm::getRepeat()
{
    return this->iRepeat;
}

/**
 * getCookie
 * Function to get the cookie value
 * 
 * @param None
 * 
 * @return integer cookie for an Alarm
 */
vector < long >CAlarm::getCookie()
{
    return (this->vcookie);
}

/**
 * setCookie
 * Function to set the cookie value
 * 
 * @param vector<integer> cookie value of an alarm
 * 
 * @return boolean value to indicate success or failure
 */
bool CAlarm::setCookie(vector < long >vCookie)
{
    this->vcookie = vCookie;
    return true;
}


/**
 * setDuration
 * Function to set the duration 
 * 
 * @param int
 * 
 * @return TRUE/FALSE
 */
bool CAlarm::setDuration(int iDuration)
{
    if (iDuration < 0)
    {
        CAL_ERROR_LOG("Negative value %d is passed. Ignore", iDuration);
        return false;
    }

    this->iDuration /= 10;
    this->iDuration *= 10;
    this->iDuration += iDuration % 10;
    return true;
}

/**
 * getDuration
 * Function to get the duration
 * 
 * @param none
 * 
 * @return int
 */
int CAlarm::getDuration()
{
    return this->iDuration % 10;
}

/**
 * setAction
 * Function to set the value for Action
 * 
 * @param int
 * 
 * @return TRUE/FALSE
 */
bool CAlarm::setAction(int iAction)
{
    this->iAction = iAction;
    return true;
}

/**
 * getAction
 * Function to get the value for Action
 * 
 * @param none
 * 
 * @return int
 */
int CAlarm::getAction()
{
    return this->iAction;
}

/**
 * setAttach
 * Function to set the value for Attach
 * 
 * @param int
 * 
 * @return TRUE/FALSE
 */
bool CAlarm::setAttach(string szAttach)
{
    this->szAttach = szAttach;
    return true;
}

/**
 * getAttach
 * Function to get the value for Attach
 * 
 * @param none
 * 
 * @@return string
 */
string CAlarm::getAttach()
{
    return this->szAttach;
}


/**
 * addAlarmEvent
 *
 * @param time_t     Event time in time_t format
 * @param string     Event summary displays in alarm message
 * @param string    Event location displays in alarm message
 * @param time_t     Date start
 * @param time_t    Date End
 * @param string    Component Id
 * @param int        Calendar Id
 * @param string    sDescription 
 * @param iType     Component Type   
 * @param pErrorCode refernece to error code 
 * @return long Alarm cookie value
 *
 * Function to add the alarm event
 */
long CAlarm::addAlarmEvent(time_t event_time, string title,
    			string location, time_t dateStart,
    			time_t dateEnd, string sCompId, int iCalendarId,
    			string szDescription,int iType,bool  allday,
    			string szZone,int& pErrorCode)
{
    alarm_event_t *event = 0;
    alarm_action_t *actionSnooze = 0;
    alarm_action_t *actionView = 0;
    alarm_action_t *actionStop = 0;
    /*alarm_action_t *actionMissedAlarm = 0;*/
    long cookie = ALARM_INVALID_COOKIE;
    string id(sCompId);
    const char *sid = sCompId.c_str();
    dbus_uint32_t view = OPEN_EVENT_VIEW;
    dbus_int32_t currentTime = 0; 
    eventType type_of_event = E_NORMAL_EVENT;
    time_t offset_to_target = 0;    

    /* this is the offset that needs to be added 
     * to alarm time  so that trigger time is obtained */
    offset_to_target = dateStart - event_time;
    
    if ( iType  == E_EVENT) {
    		currentTime = NULLID;
    		type_of_event = E_NORMAL_EVENT;
    } 
    else if (iType == E_TODO){
    		view = OPEN_TASK_VIEW;
    		currentTime = NULLID -1 ;
    		type_of_event = E_TASK;
    }
    else if (iType == E_BDAY){
    		currentTime = NULLID -2;
    		type_of_event = E_BIRTHDAY;
    }
    CAL_DEBUG_LOG("Inside add alarm procedure (event_time=%s)", ctime(&event_time));
    CMulticalendar *mc = CMulticalendar::MCInstance();
    CCalendar *pCal = mc->getCalendarById(iCalendarId, pErrorCode);
    ASSERTION(pCal);

    event = alarm_event_create();
    if (!event){
    		CAL_DEBUG_LOG(" event is null so returning from here \n");
    		pErrorCode = CALENDAR_LIBALARM_ERROR;
		delete pCal;
		pCal = 0;
    		return FAILURE;
    }
    /* determine the actions based on the 
     * type of event */
    alarm_event_add_actions(event, ALARM_ACTION_COUNT-1);

    alarm_event_set_icon(event, ALARM_ICON_PATH);
    CAL_DEBUG_LOG("%p  Action tab in event is ", event->action_tab);

    if (event->action_tab == 0) {
    		CAL_DEBUG_LOG("Failed to create a valid alarm\n");
    		alarm_event_delete(event);
    		pErrorCode =  CALENDAR_LIBALARM_ERROR;
		delete pCal;
		pCal = 0;
    		return FAILURE;
    }

    /*  First action button snooze button */
    actionSnooze = alarm_event_get_action(event, ALARM_ACTION_SNOOZE);
    alarm_action_set_label(actionSnooze, SNOOZE);
    actionSnooze->flags =
    		ALARM_ACTION_TYPE_SNOOZE | ALARM_ACTION_WHEN_RESPONDED;

    CAL_DEBUG_LOG("label for actionSnooze is %s\n",
    				alarm_action_get_label(actionSnooze));
    CAL_DEBUG_LOG("label for action tab is %s\n",
    				alarm_action_get_label(event->action_tab));

    /* Second Action button View */
    /* Setting DBus action when alarm view is pressed 
     * has to open the task in event view  */
    actionView = alarm_event_get_action(event, ALARM_ACTION_VIEW);
    alarm_action_set_label(actionView, VIEW);
    actionView->flags =
    		ALARM_ACTION_TYPE_DBUS | ALARM_ACTION_WHEN_RESPONDED;
    alarm_action_set_dbus_interface(actionView, CALENDAR_INTERFACE);
    alarm_action_set_dbus_service(actionView, CALENDAR_SERVICE);
    alarm_action_set_dbus_path(actionView, CALENDAR_PATH);
    alarm_action_set_dbus_name(actionView, CALENDAR_LAUNCH_METHOD);
    CAL_DEBUG_LOG("label for actionView is %s\n",
    				alarm_action_get_label(actionView));
    /* Third Action button stop */

    /* Setting DBus action when alarm stop */
    actionStop = alarm_event_get_action(event, ALARM_ACTION_STOP);
    alarm_action_set_label(actionStop, CLOSE);
    alarm_event_set_attr_string (event, "textdomain",CLOCK_TEXT_DOMAIN); 
    actionStop->flags = 
    		ALARM_ACTION_TYPE_EXEC | ALARM_ACTION_WHEN_RESPONDED | ALARM_ACTION_WHEN_DELAYED;
    int   cid = atoi(sCompId.c_str());
    char *cmd = 0;
    asprintf(&cmd, "%s %d %d %d ",SET_NEXT_ALARM,cid,iCalendarId,iType);
    alarm_action_set_exec_command(actionStop,cmd);


    /* handling the missed alarms is done by this 
     * action parameter , only for recursive events 
     * we need to set alarm for next impending instance 
     *  */

    string szAlarmTune;
    /* if alarm tune is other than the default one whichis chime 
     * then set appropriate value here */
    if (!pCal->getCalendarTune().empty()) {
    		szAlarmTune = pCal->getCalendarTune();
    		alarm_event_set_sound(event, szAlarmTune.c_str());
    		CAL_DEBUG_LOG("Alarm tune is set for calendar : %s ",
    						szAlarmTune.c_str());
    }

    	/* tasks also can have alarm location is empty for task */
    	if (!location.empty())
    			alarm_event_set_attr_string(event, EVENT_LOCATION,
    							location.c_str());

    	/* setting the description parameter, event time, application id */
    	if (!szDescription.empty())
    			alarm_event_set_attr_string(event, EVENT_DESCRIPTION,
    							szDescription.c_str());

    	/* Need to set broken time if component is allday/birth event or todo component */
    	if(szZone.empty())
    			szZone = mc->getSystemTimeZone();
    	struct tm alarm_tm_t;

    	CAL_DEBUG_LOG("All day event %d and type = %d ", allday,iType); 
    	if((allday == true)|| (iType == E_TODO)) {
    			/* getting the alarm time in the events 
    			 * original time zone */ 
    			time_get_remote(event_time ,szZone.c_str(), &alarm_tm_t );
    			event->alarm_tm.tm_year = alarm_tm_t.tm_year;
    			event->alarm_tm.tm_mon  = alarm_tm_t.tm_mon;
    			event->alarm_tm.tm_mday = alarm_tm_t.tm_mday;
    			event->alarm_tm.tm_hour = alarm_tm_t.tm_hour;
    			event->alarm_tm.tm_min  = alarm_tm_t.tm_min;
    			CAL_DEBUG_LOG("Alarm date time  set to Year %d: Month %d: Day %d ::Hr %d: Min %d", 
    							alarm_tm_t.tm_year+ 1900 ,alarm_tm_t.tm_mon,
    							alarm_tm_t.tm_mday,alarm_tm_t.tm_hour,alarm_tm_t.tm_min);
    			/*in case of allday events and tasks 
    			 * date start shouldnt be moved wrt to 
    			 * timezone changes
    			 * if the instance should be at 1200 AM then because
    			 * of zone changes it gets shifted by time diff wrt  new zone 
    			 */
    			dateStart = dateStart + time_get_time_diff(dateStart,szZone.c_str(),mc->getSystemTimeZone().c_str());	     
    			if (iType == E_EVENT)
    					type_of_event = E_ALLDAY;
    	}
    	else {
    			/* in other cases it is ok if we
    			 *  set the alarm time in seconds since epoch 
    			 *  since 1970. Alarmd will ajust the time 
    			 *  based on the zone information.
    			 *   */
    			CAL_DEBUG_LOG("Setting alarm as normal event ");
    			event->alarm_time = event_time;
    			CAL_DEBUG_LOG("Alarm date time set to %s Event cookie: %ld\n",
    							ctime(&event_time), cookie);

    	}
    	/* Calling set methods to set attributes 
    	 * to alarm_event_t structure 
    	 */
    	/* FIXME : TARGET_TIME attribute is no longer required as per new 
    	 * design, we need to remove it once alarm ui implementation 
    	 * is complete 
    	 */
    	alarm_event_set_attr_time(event, TARGET_TIME, dateStart);
    	alarm_event_set_title(event, title.c_str());
    	alarm_event_set_alarm_appid(event, ALARM_APPID);


    	string event_indicator;
    	/* eventindicator has all the values 
    	*/
    	if (type_of_event == E_ALLDAY)
    			event_indicator = AALLDAY ;
    	else if (type_of_event == E_TASK )
    			event_indicator = ATASK;
    	else if ( type_of_event == E_BIRTHDAY)
    			event_indicator = ABIRTHDAY;
    	else
    			event_indicator = ANORMALEVENT;

    	/* add attributes for event_type and target_offset  */
    	alarm_event_set_attr_time(event,TARGET_TIME_OFFSET,offset_to_target);
    	/* add attribute for event type */
    	alarm_event_set_attr_string(event,CAL_EVENT_TYPE,event_indicator.c_str());


    	CAL_DEBUG_LOG(" Target Time offset %ld and event type %s",
    					alarm_event_get_attr_time(event,TARGET_TIME_OFFSET,0),
    					alarm_event_get_attr_string(event,CAL_EVENT_TYPE,0 ));

    	alarm_event_set_action_dbus_args(event, DBUS_ARGUMENT_1,
    					DBUS_TYPE_UINT32, &view,
    					DBUS_TYPE_INT32, &currentTime,
    					DBUS_TYPE_STRING, &sid,
    					DBUS_TYPE_INT32, &iCalendarId,
    					DBUS_TYPE_INVALID);
    	CAL_DEBUG_LOG("Dbus Arguments are : %s\n",
    					alarm_event_get_action_dbus_args(event, DBUS_ARGUMENT_1));

    	if ((cookie = alarmd_event_add(event)) == ALARM_INVALID_COOKIE)
    			pErrorCode = CALENDAR_ALARM_ERROR;


    	/* Deleting the action parameters */
    	alarm_event_del_action_dbus_args(event, DBUS_ARGUMENT_1);
    	alarm_event_del_actions(event);
    	alarm_event_delete(event);

    	/* freeing the cmd */
    	free(cmd);
    	cmd = 0;
    	delete pCal;
    	pCal = 0;

    	return cookie;
}


/**
 * registerAlarmForAutoDeletion
 * Function to register an alarm for automatic deletion of events.
 *
 * @param alarm_time Event time in time_t format
 *
 * @return long Alarm cookie value
 */
long CAlarm::registerAlarmForAutoDeletion(time_t alarm_time,int& pErrorCode )
{
    alarm_event_t *event = 0;
    long cookie = ALARM_INVALID_COOKIE;
    alarm_action_t *actionAutoCleanUp = 0;


    event = alarm_event_create();
    if (!event){
        CAL_DEBUG_LOG(" event is null so returning from here \n");
            pErrorCode = CALENDAR_LIBALARM_ERROR;
        return FAILURE;
    }
    alarm_event_add_actions(event,ALARM_ACTION_ONE);
    actionAutoCleanUp = alarm_event_get_action(event, 0);

    /* Setting DBus actionAutoCleanUp when alarm triggers */
    actionAutoCleanUp->flags =
    ALARM_ACTION_TYPE_DBUS | ALARM_ACTION_WHEN_TRIGGERED;
    alarm_action_set_dbus_interface(actionAutoCleanUp, CALENDAR_INTERFACE);
    alarm_action_set_dbus_service(actionAutoCleanUp, CALENDAR_SERVICE);
    alarm_action_set_dbus_path(actionAutoCleanUp, CALENDAR_PATH);
    alarm_action_set_dbus_name(actionAutoCleanUp,
                   CALENDAR_AUTOCLEAN_METHOD);

    /* Setting alarm event structure */
    event->alarm_time = alarm_time;
    alarm_event_set_alarm_appid(event, ALARM_APPID);

    /* Register the alarm */
    cookie = alarmd_event_add(event);

    if (cookie == ALARM_INVALID_COOKIE) {
    CAL_DEBUG_LOG("Failure in registering the alarm");
    pErrorCode =  CALENDAR_ALARM_ERROR;
    }

    CAL_DEBUG_LOG("Alarm event successfully set to %s. Event cookie: %ld\n",
        ctime(&(event->alarm_time)), cookie);
    alarm_event_del_action_dbus_args(event, 0);
    alarm_event_delete(event);

    return cookie;
}


/**
 * deleteAlarmEvent
 * Function to delete the alarm event
 *
 * @param long Alarm cookie value
 * 
 * @return int Return value for delete alarm event
 */
int CAlarm::deleteAlarmEvent(long cookie, int& pErrorCode)
{

    int ret = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    /* Check if there is a valid alarm for this cookie *
     */
   /*alarm_event_t * pAlarmdEvent = 0;*/

   /* This is a bit wierd, even for valid alarms 
    * alarmd_event_get is returning NULL 
    * so the alarms are not getting registered
    */
   /*
   pAlarmdEvent = alarmd_event_get(cookie);

   if (pAlarmdEvent)
   {
   alarm_event_delete(pAlarmdEvent);
   */
   CAL_DEBUG_LOG("Inside delete alarm event \n");
   ret = alarmd_event_del(cookie);
   if (ret < 0) {
        CAL_DEBUG_LOG("Failed to de-register alarm with Cookie %ld",cookie);    	   
    // this will fail only in case the Alarm is alread deregistered 
    //
       //   pErrorCode = CALENDAR_ALARM_ERROR;   
   }
   else
       
    CAL_DEBUG_LOG("Alarm Event found and deleted successfully"
    	   "corresponding to cookie %ld ", cookie);
       
   /*
    }
    CAL_DEBUG_LOG("Alarm Event not found corresponding to cookie %d "
       	"it is a benign warning -probably cookie has expired already\n ",
    				cookie);
*/    				
  return ret;
}


/**
 * modifyAlarmEvent
 * Function to modify the alarm event
 *
 * @param long Old cookie value
 * @param time_t Event time in time_t format
 * @param string Event summary displays in alarm message
 * @param string Event location displays in alarm message
 * @param time_t Date start
 * @param time_t Date End
 * @param string Component Id
 * @param int Calendar Id
 * @param string sDescription    
 * @param int& reference to error code 
 *
 * @return long New Alarm cookie value
 */
long CAlarm::modifyAlarmEvent(long oldcookie, time_t modify_time,
                  string title, string location,
                  time_t dateStart, time_t dateEnd,
                  string sCompId, int iCalendarId,
                  string szDescription,int iType,bool allday,string szZone,int& pErrorCode)
{
    long cookie = ALARM_INVALID_COOKIE;

    // Delete the old event
    if ((deleteAlarmEvent(oldcookie,pErrorCode)) < 0)
    {
        CAL_ERROR_LOG("Failed to delete alarm");

        return FAILURE;
    }
    
    // Add and return the new cookie */
    cookie = addAlarmEvent(modify_time, title, location, dateStart, dateEnd,
        sCompId, iCalendarId, szDescription,iType,allday,szZone,pErrorCode);

    return cookie;
}

string  CAlarm::toString()
{
    string szRet;
    string szTemp;
    std::stringstream ss;
    ss << "Attach=";
    if ((szAttach).c_str()){
    szTemp = szAttach.substr(0,100);
    ss << szAttach;
    szTemp.clear();
    }
    else
    ss << NULL_STRING;
    ss << ",Trigger=";
    ss << iTrigger;
    ss << ",Repeat=";
    ss << iRepeat;
    ss << ",Duration=";
    ss << iDuration % 10;
    ss << ",MinutesBefore=";
    ss << iDuration / 10;
    if((vcookie).size()>0)
    {
    ss << ",Cookie=";
    vector<long>::iterator it;
    it=vcookie.begin();
    ss << (*it);
    }
    szRet.append(ss.str());
    return szRet;
}

bool CAlarm::setTimeBefore(int iSecondsBeforeEvent)
{
    if (iSecondsBeforeEvent < 0)
    {
        CAL_ERROR_LOG("Negative value %d is passed. Ignore", iSecondsBeforeEvent);
        return false;
    }

    this->iDuration %= 10;
    this->iDuration += iSecondsBeforeEvent * 10;

    return true;
}

int CAlarm::getTimeBefore()
{
    int time_before = this->iDuration / 10;

    if (time_before == 0)
    {
        int flag = this->iDuration % 10;

        if (flag != E_AM_ETIME && flag != E_AM_NONE && flag != E_AM_EXACTDATETIME)
        {
            CAL_ERROR_LOG("Adjust time_before value for alarm type %d", flag);
            time_before = CAlarm::getDefaultTimeBefore((dataAlarm)flag);
        }
    }
    return time_before;
}

int CAlarm::getDefaultTimeBefore(dataAlarm alarmType)
{
    int retval = -1;

    switch (alarmType)
    {
    case E_AM_NONE:
        retval = -1;
        break;

    case E_AM_ETIME:
        retval = 0;
        break;

    case E_AM_5MIN:
        retval = 60 * 5;
        break;

    case E_AM_15MIN:
        retval = 60 * 15;
        break;

    case E_AM_30MIN:
        retval = 60 * 30;
        break;

    case E_AM_1HR:
        retval = 60 * 60;
        break;

    case E_AM_3HR:
        retval = 60 * 60 * 3;
        break;

    case E_AM_DAYBEFORE:
        retval = 60 * 60 * 24;
        break;

    case E_AM_EXACTDATETIME:
        retval = 0;
        break;

    /* no default in order to get compilation error if some E_AM_xxx is not processed */
    }

    return retval;
}

int CAlarm::purgeAlarms()
{
    int retval = -1;

    cookie_t *cookies = alarmd_event_query(0, 0, 0, 0, ALARM_APPID);

    if (cookies != 0)
    {
        int cookie_count = 0;

        for(cookie_t *cookie = cookies; *cookie != 0; ++cookie, ++cookie_count)
        {
            int del_status = alarmd_event_del(*cookie);
            CAL_DEBUG_LOG("Alarm #%x is removed with status %d", (int)*cookie, del_status);
        };

        retval = cookie_count;
        free(cookies);
    }

    return retval;
}


