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

#ifndef __MULTI_CALENDAR_H__
#define __MULTI_CALENDAR_H__

/* Header files */
extern "C" {
#include <libical/ical.h>
}
#include <string>
#include <vector>
#include "CCalendar.h"
/* Forward delclaration of classes*/
class CComponent;
class CCalendarDB;
class CBdayEvent;
#ifndef _QRESULT__
#define _QRESULT__
typedef struct _QueryResult QueryResult;

struct _QueryResult {
  public:
    char **pResult;
    int iRow;
    int iColumn;
};
#endif

/**
 * CMulticalendar class
 * @brief CMulticalendar class is a single point of entry to access the calendar backend.
 * The design pattern used is Facade, hence this class is face of the calendar-backend.
 * There can be only one instance  of CMulticalendar per process. It has API's for calendar
 * management and fetching the components from database
 */


class CMulticalendar {

  public:
	  /**
	   * Function to return the dates ranges for events 
	   * distributed for given number of days 
	   */
	  bool getRequiredDates(time_t iCurrTime, int iFutureDateNum,
		int iBackDateNum, int &iStartDate, int &iEndDate,
		vector<string> &vIdList,int &pErrorCode);
	  /**
	   * fundtion to find the 
	   * actual numner of events 
	   */
	  int getDateRange(time_t currTime, int iNum, bool iOperation,
				string &szTemp,	int &pErrorCode);
	  /* function to fetch events with ids in list 
	   */
	  vector<CComponent*>  getEventInList(vector<string> listId , 
			  vector<int> &vCalId,int & pErrorCode);

	  /**
           * @param NONE 
           * @return CCalendar*
           * Retrieves existing Synchronized calendar.Calendar pointer thus obtained 
           * shouldn't be freed. It will be destroyed when CMulticalendar object is 
           * destroyed
           * Returns NULL in error case
           */
          CCalendar* getSynchronizedCalendar();
          /**
           * @param NONE 
           * @return CCalendar*
           * Retrieves existing default calendar.Calendar pointer thus obtained 
           * shouldn't be freed. It will be destroyed when CMulticalendar object is 
           * destroyed.
           * Returns NULL in error case
           * 
           */
          CCalendar* getDefaultCalendar();
          /**
           * @param  string szFilenName  
           * @param  int& iEventCount Count of events in file 
           * @param  int& iTodoCount  Number of todos in file
           * @param  int& iJournalCount Number of journals in file
           * @param  int&  pErrorCode - error indicator 
	   * @return bool status of operation  
	   *
	   * Function to check validy of ICS file and also report entriy count from file.
	   *
	   */
	  bool getICSFileInfo(string szFileName,unsigned int& iEventCount,
			  	unsigned  int& iTodoCount,unsigned int& iJournalCount ,
				int& pErrorCode );
	  /* progressive API to  get ics 
	   * file information
	   */
	  bool getICSFileInfoProgressive(string szFileName,
			  unsigned int &iEventCount,
			  unsigned int &iTodoCount,
			  unsigned int &iJournalCount,
			  int &pErrorCode, bool *bFileReadingOver,
			  unsigned int iRequestLimit);
	  /** 
	   * cancelImportIcsFileData
	   */ 
	  void cancelImportIcsFileData();
	  /**
	   * @param NONE
           * @return string 
           * Function to retrieve the name of the Synchronized Calendar
           */
          string getSynchronizedCalendarName();

          /**
           * getCTCapXML
           * @param none 
           * @return string  CTC capabilities in XML
           *
           * Function to get the parameters supported by calendar in XML format 
           */
          string getCTCapXML();

          /*TODO: As import is done per calendar basis we have to move this 
           * function in CCalendar and provide a wrapper here */
          /**
	   * Progressive API for importing events 
           */
	  bool importIcsFileDataProgressive(string szFileName, int iCalId,
			  string & eventId,int &iDuplicateCountTask,
			  int &iDuplicateCountEvent,int &iDuplicateCountJournal,
			  int &pErrorCode, bool *bFileReadingOver,
			  unsigned int iRequestLimit);
	  /**
	   * importIcsFileData()
	   * @param: string filename to be imported
	   * @param: int iCalId Calendar in to which event have to be imported 
	   * @param: string Eventid of added event 
	   * @param: int * pointer to ErrorCode 
	   * @return: string  cal name
	   * This function parses the given ics file .All the
	   * entries in the file are added to one calendar. 
	   * This is a non progressive API
	   */

	  bool importIcsFileData(string szFileName, int iCalId,
			  string & eventId,int &iDuplicateCountTask, 
			  int &iDuplicateCountEvent,int &iDuplicateCountJournal,int &pErrorCode);
	  /**
	   * Function to fetch the count of component 
	   * read from file 
	   */
	  int totalComponentsRead(string szLine, bool &bError,
			  int &iEventBegin, int &iJournalBegin, int &iToDoBegin,
			  unsigned int &iEventCount, unsigned int &iTodoCount,
			  unsigned int &iJournalCount);
	  /**
	   * @param: CalendarId - Id of the calendar for which export has to happen
           * @param: string - filename of the ics file 
           * @param: int & reference to ErrorCode 
           * @return: boolean status of operation true/false 
           * 
           *  Export file format will always be in ICAL format.This function
           *  exports the given calendar to a ics file.
           */

          int exportCalendarEntries(int iCalendarId,string szFileName,int& pErrorCode);

          /**
           * @param: int iCalid id of the calendar
           * @param: utc time ( int  utc) before wich all events 
           *         should be deleted
           * @param: int& pErrorcode reference to errorcode 
           * @return: status of the operation.
           * 
           * Function used to delete events before particular date for a 
           * specified calendar ID .
           * This API is specifically designed for applications like synchronization.
           */
          bool deleteEventBeyond(int iCalId, time_t iUtc,int& pErrorCode);


          /**
           * @param: int iCalid id of the calendar
           * @param: time  iUtc( int  utc) before wich all todos 
           *         should be deleted
           * @param: int& pErrorcode reference to errorcode 
           * @return:bool  status of the operation.
           * 
           * Function used to delete todos before particular date for a 
           * specified calendar ID.
           * This API is specifically designed for applications like synchronization.
           */
          bool deleteTodosBeyond(int iCalId, time_t iUtc,int& pErrorCode);

          /**
           * @param: string szName calendar Name 
           * @param: int& pErrorCode reference  to integer
           * @return: CCalendar* Calendar Pointer 
           *
           * Function used to retrieve CCalendar pointer from DB.
           * user of this API should free the calendar thus obtained. 
           */
          CCalendar *getCalendarByName(string szName,int& pErrorCode);


          /**
           * @param: int iCalid id of the calendar
           * @param: time iUtc( int  utc) before which all journals 
           *         should be deleted
           * @param: int & pErrorCode reference to integer 
           * @return: status of the operation.
           * 
           * Function used to delete journals before particular date for a 
           * specified calendar ID.
           * This API is specifically designed for applications like synchronization.
           *
           */
          bool deleteJournalsBeyond(int iCalId, time_t iUtc,int& pErrorCode);

          /**
           * @param: int iDayBefore configuration time for deletion of old events 
           * @param: int& pErrorcode reference to integer
           * @return: bool result of operation
           *
           * This function has to be called when ever  clean up of old 
           * events and todos is required for given calendar ID. 
           * All events before the days specified are deleted 
           * All Todos which are marked done and are before specified days are deleted
           * Journals are never deleted.
           */
          bool enableAutomaticDeletion(int iDayBefore,int iMax,int& iLimit,int& pErrorCode);

          /**
           * @param: string rrule     recurrencerule 
           * @param: icaltimetype iDtdtstart datestart in Icaltime format.
           * @param: time_t iViewEnd    end range of current claendar view 
           * @param: int& pErrorCode  pointer to ErrorCode 
           * @param: int limit  will return if this number of instances has found
           * @return: vector of recurrent times 
           *
           * Function to parse the time of occurrances from recurrence rules.
           * For a recursive entry if rule is provided along with Datestart 
           * and view range then it will return the set of times in view range.
           * This API is used by caledar-UI.
           * @note: This function will  be removed soon. Please switch to 'bool getRecurrentTimes(...)'. 
           */
          vector < time_t > getRecurrentTimes(string szRRule, icaltimetype iDtStart,
                          time_t iViewEnd,icaltimezone *pTz,int& pErrorCode, int limit=-1);

          /**
           * @param: string rrule     recurrencerule 
           * @param: icaltimetype iDtdtstart datestart in Icaltime format.
           * @param: int iEventDuration duration of original event
           * @param: time_t iViewBegin  start range of current claendar view 
           * @param: time_t iViewEnd    end range of current claendar view 
           * @param: vector<time_t> outTimes - generated event times
           * @param: int& pErrorCode  pointer to ErrorCode 
           * @param: int limit  will return if this number of instances has found
           * @return: bool result of operation
           *
           * Function to parse the time of occurrances from recurrence rules.
           * For a recursive entry if rule is provided along with Datestart 
           * and view range then it will return the set of times in view range.
           * This API is used by caledar-UI.
           */
          bool getRecurrentTimes(string szRRule, 
                                icaltimetype iDtStart,
                                int iEventDuration,
                                time_t iViewBegin,
                                time_t iViewEnd,
                                icaltimezone *pTz, 
                                vector <time_t> &outTimes, 
                                int& pErrorCode, 
                                int limit=-1);
          /**
           * @param: time_t iViewEnd    end range of current claendar view 
           * @param: int& pErrorCode  pointer to ErrorCode 
           * @return: true if found 
           *
           * Function to parse the time of occurrances from recurrence rules.
           * For a recursive entry if rule is provided along with Datestart 
           * and view range then it will return the set of times in view range.
           * This API is used by caledar-UI.
           * @note: This function will  be removed soon. Please switch to extended function below. 
           */
          bool checkRecurrentTimes(string szRRule, icaltimetype iDtStart,
                          time_t iViewEnd,icaltimezone *pTz,int& pErrorCode);

          /**
           * @param: string rrule     recurrencerule 
           * @param: icaltimetype iDtdtstart datestart in Icaltime format.
           * @param: int iEventDuration duration of original event
           * @param: time_t iViewBegin  start range of current claendar view 
           * @param: time_t iViewEnd    end range of current claendar view 
           * @param: int& pErrorCode  pointer to ErrorCode 
           * @return: true if found 
           *
           * Function to parse the time of occurrances from recurrence rules.
           * For a recursive entry if rule is provided along with Datestart 
           * and view range then it will return the set of times in view range.
           * This API is used by caledar-UI.
           */
          bool checkRecurrentTimes(string szRRule,
                                   icaltimetype iDtStart,
                                   int iEventDuration,
                                   time_t iViewBegin,
                                   time_t iViewEnd,
                                   icaltimezone *pTz,
                                   int& pErrorCode);

          /**
           * @param: int iCalId Calendar ID
           * @param: int iType 
           *               1 -> EVENT
           *               2 -> TODO
           *               3 -> JOURNAL
           * @param: int iStDate to do conditional fetching based on start date.
           *          -1 in case of no conditions
           * @param: int iEndDate to do conditional fetching based on end date. 
           *         -1 in case of no conditions
           * @param: int & refernce to ErrorCode 
           * @return: Vector for CComponent
           * 
           * This function is used to get all the entries associated with particular component type.
           * NOTE : API user has to free memory occupied by vector<CComponent*>
           */
          vector < CComponent * >getComponents(int iCalId, int iType, int iStDate, int iEndDatei,int& pErrorCode);


          /**
           * @param: NONE
           * @return: int Number of calendars in database
           *
           * This function is called to get number of existing calendars from DB.
           */
          int getNoofCalendars();
          /**
           * @param: string szTitle title of the calendar
           * @param: CalendarColour colour of the calendar. If it is COLOUR_NEXT_FREE, next available free color will be choosen automatically.
           * @param: int readonly flag
           * @param: int visible flag 
           * @param: CalendarType type of the calendar
           * @param: string szCalendarTune Calendar tune   
           * @param: string szCalendarVersion Calendar version   
           * @param: int& refernce to ErrorCode 
           * @return: Newly created calendar pointer, NULL in case of any error
           *
           * This function is called to add a new calendar into the database.
           * It is an overloaded API used by UI.
           */
          CCalendar* addCalendar(string szTitle, CalendarColour iColor, int iReadOnly,
                     int iVisible,CalendarType iType,string szCalendarTune,
                 string szCalendarVersion,int& pErrorCode);
          /**
           * @param: int iId of the calendar
           * @param: string szTitle title of the calendar
           * @param: CalendarColour colour of the calendar
           * @param: int iReadonly flag
           * @param: int iVisible flag 
           * @param: string szCalendarTune Calendar tune   
           * @param: string szCalendarVersion Calendar version   
           * @param: CalendarType type of the calendar
           * @param: int reference  to ErrorCode 
           * @return: bool (True/False)
           * 
           * This function is called to modify the calendar record
           * in the database.
           */
          bool modifyCalendar(int iId ,string szTitle, CalendarColour iColor,
                      int iReadOnly,int iVisible,CalendarType iType,string szCalendarTune,
                      string szCalendarVersion,int& pErrorCode);

          /**
           * @param: int iCalid Calendar id
           * @param: int & reference  to ErrorCode 
           * @return: SUCCESS/FAILURE
           *
           * This function is used to delete the calendar from database.
           */

          bool deleteCalendar(int iCalId,int& pErrorCode);

          /**
           * @param: calid Calendar id
           * @param: int * pointer reference to ErrorCode 
           * @return: CCalendar pointer
           * 
           * Function used to get the calendar object based on calendar id. 
           * This api returns CCalendar* from DB , so the Calendar pointer
	   * thus obtained should be freed by the end user.
           */
          CCalendar *getCalendarById(int iCalId,int& pErrorCode);

          /**
           * @param : stDate to do conditional fetching based on start date. -1 in case of no conditions
           * @param : endDate to do conditional fetching based on end date. -1 in case of no conditions
           * @param : limit Max limit of components to be obtanined by this function call
           * @param : offset from which we need to fetch 
           * @param : vector<int> vCalid reference to place holder for calenadr id for each entry
           * @param : int * pointer to ErrorCode:
           *          CALENDAR_OPERATION_SUCCESSFUL- fetching is successful and can be continued;
           *          CALENDAR_FETCH_NOITEMS- fetching is finished (no more items found in DB);
           *          other error code- fetching is failed.
           * @param : int iQueryType 
           *           value 1 - Indicates simple events and tasks 
           *           value 2 - indicates Repeating events 
           *           Any other value fetches all ( repeating events , non repeating events and tasks)
           *           Default value is 0.
           * @return : Vector for CComponent. If pErrorCode equals to CALENDAR_OPERATION_SUCCESSFUL,
           *           but vector is empty, no component (from offset to limit) meets the conditions,
           *           continue fetching
           * 
           * This function is overloaded API for fetching entries for all calendars at a time progressively 
           */
          vector < CComponent * > getComponentsAllCalendars( int iStDate, int iEndDate,
                  int iLimit,int iOffset,vector<int > &vCalid , int &pErrorCode,int iQueryType=0);
          /**
           * @param : stDate to do conditional fetching based on start date. -1 in case of no conditions
           * @param : endDate to do conditional fetching based on end date. -1 in case of no conditions
           * @param : limit Max limit of components to be obtanined by this function call
           * @param : offset from which we need to fetch 
           * @param : Vector for CComponent. If pErrorCode equals to CALENDAR_OPERATION_SUCCESSFUL,
           *           but vector is empty, no component (from offset to limit) meets the conditions,
           *           continue fetching
           * @param : int iQueryType 
           *           value 1 - Indicates simple events and tasks 
           *           value 2 - indicates Repeating events 
           *           Any other value fetches all ( repeating events , non repeating events and tasks)
           *           Default value is 0.
           * @return Error code:
           *          CALENDAR_OPERATION_SUCCESSFUL- fetching is successful and can be continued;
           *          CALENDAR_FETCH_NOITEMS- fetching is finished (no more items found in DB);
           *          other error code- fetching is failed.
           * 
           * This function is overloaded API for fetching entries for all calendars at a time progressively 
           */
          int getComponentsAllCalendars( int iStDate, int iEndDate,
                  int iLimit,int iOffset, vector<CComponent*> &vComponents, int iQueryType=0);
          /**
           * @param : iCalid Id of the Calendar for which event belongs
           * @param : stDate to do conditional fetching based on start date. -1 in case of no conditions
           * @param : endDate to do conditional fetching based on end date. -1 in case of no conditions
           * @param : limit limit for number of events fetched by the calenadr
           * @param int * pErrorCode error code if any
           * @return : Vector for CComponent
           * This function is used to get all the Events /Todos /Bdays  associated with particular Calendar progressively 
           * It is an overloaded API for get componets used to fetch Components in bursts of specified size
	   */

	  vector < CComponent * > getAllEventsTodos(int iCalid, int iStDate,
			  int iEndDate, int iLimit , int iOffset,int& pErrorCode);

	  /**
	   * @param: icalid calendar ID 
	   * @param: time utc beyond which all todos should be deleted
	   * @param: iType Component type
	   * @param: int * pointer to ErrorCode 
	   * @return: bool success/failure of the function 
	   * 
	   * Function to delete Events/Todos beyond a particular date 
	   */
	  bool deleteEventAndTodoBeyond(int iCalId, time_t iUtc,
			  int iType,int &pErrorCode);
          /**
           * @param: CEvent *CEvent object pointer
           * @param: int iCalendarId calendar ID
           * @param: int & reference  to ErrorCode 
           * @return: bool 
           *  
           * Function used to add the Event using CMulticalendar class.In successful scenario 
           * Id of the newly added event will be set in the passed pEvent object itself.
           *  Can be retrieved using  pEvent->getId();
           * 
           */
          bool addEvent(CEvent * pEvent, int iCalendarId,int& pErrorCode);

          /**
           * @param: ptr CEvent object pointer
           * @param: calendarId calendar ID
           * @param: int * pointer reference to ErrorCode 
           * @return: bool 
           * 
           * Function used to modify the Event using CMulticalendar class.
           */
          bool modifyEvent(CEvent * pEvent, int iCalendarId,int& pErrorCode);

          /**
           * @param: cal_id Calendar id
           * @param: compId Event ID
           * @param: int * pointer reference to ErrorCode 
           * @return: bool 
           * 
           * Function used to delete the Event using CMulticalendar class.
           */
          bool deleteEvent(int iCalId, string sCompId,int& pErrorCode);


          /**
           * @param: ptr CTodo object pointer
           * @param: calendarId calendar ID
           * @param: int * pointer reference to ErrorCode 
           * @return: NONE
           * 
           * Function used to add the Todo using CMulticalendar class.If addition is 
           * successful then the id of newly created task will be set in passed CTodo object itself.
           * Can be retrieved using pTodo->getId()
           */
          bool  addTodo(CTodo * pTodo, int iCalendarId,int& pErrorCode);

          /**
           * @param: ptr CTodo object pointer
           * @param: calendarId calendar ID
           * @param: int * pointer reference to ErrorCode 
           * @return: bool 
           * 
           * Function used to modify the Todo using CMulticalendar class.
           */
          bool  modifyTodo(CTodo * pTodo, int iCalendarId,int& pErrorCode);

          /**
           * @param: cal_id Calendar id
           * @param: compId Todo ID
           * @param: int * pointer reference to ErrorCode 
           * @return: Bool 
           * 
           * Function used to delete the Todo using CMulticalendar class.
           */
          bool  deleteTodo(int iCalId, string  sCompId,int& pErrorCode);


          /**
           * @param: ptr CJournal object pointer
           * @param: calendarId calendar ID
           * @param: int * pointer reference to ErrorCode 
           * @return: bool 
           *  
           * Function used to add the Journal using CMulticalendar class.
           */
          bool  addJournal(CJournal * pJournal, int iCalendarId,int& pErrorCode);

          /**
           * @param: CJournal* pJournal pointer to CJournal.
           * @param: int iCalendarId Calendar id
           * @param: int reference to error code.
           * @return bool 
           *  
           * Function used to modify the Journal using CMulticalendar class.
           */
          bool  modifyJournal(CJournal * pJournal, int iCalendarId,int& pErrorCode);

          /**
           * @param: int iCalId Calendar id
           * @param: string sCompId Journal id
           * @param: int reference to error code 
           * @return: bool 
           * 
           * Function used to delete the journal using CMulticalendar class
           */
          bool  deleteJournal(int iCalId, string sCompId,int& pErrorCode);

          /**
           * This fuction is used to create single multicalendar instance 
           * Function used to get the CMulticalendar instance
           * 
           */
          static CMulticalendar *MCInstance();

          /**
           * @param: int iCalid Calendar id 
           * @param: int & reference to error code. 
           * @return: integer event count
           * 
           * Function used to get the number of events for particular calendar in DB. 
           */
          int getEventCount(int iCalId,int& pErrorCode);

          /**
           * @param: int iCalid  calendar ID 
           * @param: int & reference to error code  
           * @return: integer todo count
           * 
           * Function used to get the number of Todos for particular calenadar in DB.
           */
          int getTodoCount(int iCalId,int& pErrorCode);

          /**
           * @param: int iCalid  calenadar Id 
           * @param: int * pointer reference to ErrorCode 
           * @return: integer journal count
           * 
           * Function used to get the number of journals for particular calenadar in DB .
           */
          int getNoteCount(int iCalId,int& pErrorCode);

          /**
           * @param: NONE 
           * @return: vector of CCalendar* 
           * 
           * Function to return list of calendars from Database.  
           * Should be freed by API user, using releaseListCalendars() function.
           * IMPORTANT: This is a thread safe function.
           * 
           */
          vector < CCalendar * > getListCalFromMc();

          /**
           * @param: Vector of CCalendar*
           * @return: NONE
           * 
           * Function to release a list of calendars.
           * 
           */
          void releaseListCalendars(vector<CCalendar*> Calendars);

          /**
           * @param: CCalendar* pCal Pointer to CCalendar object 
           * @param: int refernce to ErrorCode 
           * @return: bool (SUCCESS/FAILURE)
           * 
           * This function is called to modify the calendar record
           * in the database.
           */
          bool modifyCalendar(CCalendar* pCal,int& pErrorCode);

          /**
           * @param: pCal pointer to CCalendar object 
           * @param: int& reference to ErrorCode 
           * @return: newly created calendar Id  
           *
           * This function is called to add a new calendar into the database.
           * It is an overloaded API.
           * Calendar pointer object passed should be freed by the API user.
           */
          int addCalendar(CCalendar* pCal,int& pErrorCode);
          /**
           * @param iCalId int
           * @return int
           *
           * This function is used to get the number of undone tasks.
           * This function returns the calendar id in outparam iCalId
           * if there is only one undone task.
           */
          int getNumOfUndoneTasks(int* iCalId,int& pErrorCode);
          /**
           * @param pErrorCode int
           * @return int
           *
           * This function is used to get the number of allday events for today.
           */
          int getNumOfAllDayEventsForToday(int& pErrorCode);
          /**
           * getTodaysAllDay
           * @param pErrorcode reference to error code 
           * @return list of CComponent*
           *
           * Function to retrieve allday events on today 
           * 
           */

          vector <CComponent*>  getTodaysAllDay(int &pErrorCode);

          /**
           * Destructor for Multicalendar class
           */
          ~CMulticalendar();

          /**
           * @param: pBday CBdayEvent object pointer
           * @param: int &refernce to ErrorCode 
           * @return: bool (SUCESS/FAILURE)
           * 
           * Function used to add the Birthday data using CMulticalendar class.
           * Id of the newly added birthdayevent will be set inside the CBdayEvent object
           * and can be retrieved using pBday->getId()
           */
          bool addBirthDay(CBdayEvent * pBday,int& pErrorCode);

          /**
           * @param: pBdays list of CBdayEvent object pointer
           * @param: int * pointer to ErrorCode 
           * @return: bool (SUCESS/FAILURE)
           * 
           * Function used to add the Birthday data using CMulticalendar class.
           */
           bool addBirthdays(vector<CBdayEvent*>& pBdays, int &pErrorCode);

          /**
           * @param: pBday CBdayEvent object pointer
           * @param: int  reference to ErrorCode 
           * @return: bool (SUCCESS/FAILURE)
           * 
           * Function used to modify the Birthdays using CMulticalendar class.
           */
          bool modifyBirthDay(CBdayEvent * pBday,int& pErrorCode);

          /**
           * @param: string szEUid  Id of the birthday event 
           * @param: int& pointer to ErrorCode 
           * @return: boolean (SUCCESS/FAILURE)
           * 
           * Function used to delete the Event using CMulticalendar class.
           */
          bool deleteBirthDay(string szEUid,int& pErrorCode);

          /**
           * @param: vector<string>& szEUidList  list of Ids of the birthday events 
           * @param: int& pointer to ErrorCode 
           * @return: boolean (SUCCESS/FAILURE)
           * 
           * Function used to delete the Event using CMulticalendar class.
           */
          bool deleteBirthdays(vector<string> &szEUidList,int& pErrorCode);

          /**
           * @param: iCalType Calendar Type
           * @param: int & irefernce  to ErrorCode 
           * @return: CCalendar object
           *
           * Function used to get the calendar object based on calendar type.
           * CCalendar object thus obtained should be freed by the API user.
           */
          CCalendar *getCalendarByType(int iCalTypei,int& pErrorCode);

          /**
           * @param : int iCalid Calendar ID
           * @param : type 1 -> EVENT
           *               2 -> TODO
           *               3 -> JOURNAL
           *               4 -> BDAY
           * @param : int stDate to do conditional fetching based on start date. -1 in case of no conditions
           * @param : int endDate to do conditional fetching based on end date. -1 in case of no conditions
           * @param : int limit Max limit of components to be obtanined by this function call
           * @param : int offset from which we need to fetch 
           * @param: int& reference to ErrorCode 
           * @return : Vector of  CComponent*
           * 
           * 
           * For vector<CComponent*>  memory is allocated inside this procedure , it has to be freed 
           * by the API user.
           * This function is overloaded API for Progressive updation , used by calendar UI. 
           */

          vector < CComponent * > getComponents(int iCalId, int iType, int iStDate,
                          int iEndDate,int iLimit,int iOffset,int& pErrorCode);


          /**
           * @param: none
           * @return: int SUCCESS/FAILURE
           * 
           * Function to set the autocommit off in sqlite database.
           * Autocommit by default is in "ON" state. While doing Mass logical updations 
           * this can be turned to "OFF" state by calling this API.
           */
          int setAutocommitOff ();


          /**
           *  @param NONE
           *  @return: bool TRUE/FALSE
           * 
           * Function to commit all the changes made into database.
           * If Auto commit is turned "OFF" using setAutocommitOff  then all changes acan be commited
           *  using this API. Only in case of Mass Logical updations this will happen.
           */
          bool commitAllChanges ();

          /**
           * @param: none
           * @return: bool TRUE/FALSE
           * 
           * Function to rollback all the changes made into database.
           * Rollback function to cancel all the changes done to DB.
           */
          bool rollbackAllChanges ();

          /**
           * @param : int iCalendar of the calendar
           * @param : string sComponentId of the component
           * @param : int iType Component type 
           * @param : int&  reference to errorcode
           * @return bool : returns SUCCESS/FAILURE of the function
           *
           * This function sets the alarm for the given component of the calendar.
           * For any recursive event with ALARM , only one alarm is stored in DB 
           * Up on the invocation of alarm next impending alarm is calculated and set
           * TODO: Can be made private
           * As outside applications need not use it.
           */
          bool setNextAlarm (int iCalendarId, string sComponentId,int iType,int& pErrorCode);

          /**
           * @param: string name of the application
           * @return: void
           *
           * This function sets the given name as the application name.
           */
          void setApplicationName (string ApplicationName);
          /**
           * @param : None
           * @return : string name of the application
           *
           * This function sets the given name as the application name.
           */
          string  getApplicationName ();
          /**
           * @param string szName Name of Calendar
           * @return bool true/fals 
           *
           * Function to check whether the calendar name exists or not 
           */
          bool checkCalendarNameExists(string szName);
          /**
           * @param int iType Can be Event or todo( E_EVENT,E_TODO)
           * @param time_t time beyond which event can be fetched 
           * @param ioffset - offset period on which the event can be fetched 
           * @param int reference pErrorCode , error code to define the possible errros
           * Function used by calendar applet to retrieve the latest components
           * Function to retrieve upcoming events,todos  
           */

          CComponent* getLatestComponent(int iType,time_t &startdate,int &ioffset,int& pErrorCode );
          /**
           * @param NONE
           * @return string 
           *
           * Function to retrieve the name of the Default Calendar
           */
          string getDefaultCalendarName();
          /**
           * @param string Id of the event 
           * @param datestart of the event 
           * @param isPrev flag to check query from previous/next event
           * @param iCalid calendar Id of the component 
           * @param perrorcode reference to int to store error code if any
           * @return CComponent*
           *
           * Function used to fetch previous and next event in Database based on Date start
           * 
           */
          CComponent* getPrevNextComponent(string szId , time_t iDateStart,
                  bool isPrev,int& iCalid,time_t &instance_time,int& perrorcode);
          /**
           * @param NONE 
           * @return NONE
           * Function used to destroy CMulticalendar object
           */
          void MCdestroy();
          /**
           * @param vector<CComponent *>
           * @param  struct QueryResult pointer
           *
           * Function to retunrn CComponents from database based on the query result
           */
          vector<CComponent*> popEventDetails (QueryResult* pQr);

          /**
           * @param string iI 
           * @param error_code 
           * @return Calendar Id for component  0 in case of error 
           */
          int getCalendarIdForComponent(string iI,int &pErrorCode);

          /**
           * @param NONE 
           * @return CCalendar*
           * Retrieves existing Synchronized calendar.Calendar pointer thus obtained 
           * shouldn't be freed. It will be destroyed when CMulticalendar object is 
           * destroyed
           * Returns NULL in error case
           */
          CCalendar* getBirthdayCalendar();
          /**
           * @param int iCalid smart calendar Id to be deleted
           * @param pErrorcode int reference to error code 
           * @return bool success/failure indicator  
           * Function used to delete birthdays for a given calendar
           */
          bool deleteBirthdaysForCalendar(int iCalid,int & pErrorCode);
          /**
           * @param: iCalid Calendar id 
           * @param: int * pointer to ErrorCode 
           * @return: integer event count
           *  
           * Function used to get the number of Bday events for particular calendar in DB. 
           */
          int getBdayEventCount(int iCalId,int& pErrorCode);

	  /**
	   *  utility API can be used by applications 
	   * using backend to get the current timezone
	   * */
	  static time_t getSystemTimeShift();
	  /**
	   *  utility API can be used by applications 
	   * using backend to get the current timezone
	   * */
	  static string getSystemTimeZone();
	  /**
	   * @param iCalId Calendar Id 
	   * @param pErrorCode  -  Reference to errror Code 
	   * 
	   * API to Clear the contents of the Calendar
	   * 
	   */
	  void clearCalendar(int iCalId,int &pErrorCode );
	  /**
	   * @param iCalId Calendar Id 
	   * @param pErrorCode  -  Reference to errror Code 
	   * 
	   * API to Clear the contents of the Calendar
           * @note This function MUST NOT be called between setAutoCommitOff() and commitAllChanges()
	   * 
	   */
	  void clearCalendarContents(int iCalId,int &pErrorCode );
		  /**
	   * getSystemTimeZoneAsIcalTimeZone
	   *
	   */
	  icaltimezone* getSystemTimeZoneAsIcalTimeZone();
	  /**
	  */
	  void printCurrentTime();
	  /**
	   * Function to disable debugging at runtime 
	   */
	  //void setDebugOutput(bool logstate);
	  /**
	   * @param vector<CComponent*> vector of CComponent * to be added 
	   * @param Calendar Id for which the entry belongs  
	   * @param vector <string> duplicate Ids if at all they exist
	   * @param Error code 
	   * @return vector<string> added Component Ids in Calendar database.
	   *
	   * Function to add Events/tasks /Journals in batch
	   */
	  vector <string> addComponents(vector<CComponent*> &compList,
			  int iCalId,
			  vector<string> & duplicateId, 
			  int &ErroCode);
	 
	  /**
	   * @param   vector<string> id of Component to be deleted 
	   * @param   Calendar Id for which the entry belongs  
	   * @param   ErrorCode   
	   * @return  void 
	   * 
	   * Function to delete Events/tasks /Journals in batch
	   */
	  void   deleteComponents(vector<string> &idList,
			int  iCalId,
			int &ErrorCode);

	  /**
	   * @param   vector<CComponent*> vector of CComponent* 
	   * @param   Calendar Id for which the entry belongs  
	   * @param   ErrorCode   
	   * @return  void 
	   *
	   * Function to modify Events/tasks /Journals in batch
	   */
	  void  modifyComponents(vector<CComponent*> &compList,
		int iCalId, 
		int &ErrorCode);
	  /**
	   * getVisibleCalendarCount
	   * @param  ErrorCode
	   * @return count of VisibleCalendars
	   */

	  int getVisibleCalendarCount(int &pErrorCode);
	  /*
	   * Function to find recursive impending events
	   */

	  void findRecursiveImpendingEventId(time_t OriginalDateStart,
			  time_t timebasedDateStart,
			  bool isPrev,
			  string &szEventId,
			  time_t &instance_time,
			  int &pErrorCode);
	  /* get Id from Map */
	  string getEventIdFromMap(map<time_t,string > &timeEventIdMap,
			  time_t &timebasedDateStart,
			  time_t &OriginalDateStart ,
			  bool isPrev,
			  time_t &instance_time);
	  /**
	   *  @param  int Value 
	   *  @return NONE 
	   *  
	   * function used to enable and disable calendar logs 
	   * pass 0 to disable calendar logs 
	   * pass 1 ro enable calendar logs 
	   */
	  void enableDisableCalendarLogs(int value );

	  void deleteItemsByIdList(vector<string> &listId, int& pErrorCode);

	  /**
	   *  moving the calendardb file from 
	   * /etc/skel/.claendar/calendardb
	   **/
	  bool moveFileFromSkel(string oldpath, string newpath);
	  /**
	   *  API to see if DB file already exists 
	   **/
	  static bool checkFileExists(string szPath);
	  /**
	   * API to find the number of Components
	   * in the DB
	   */
	  int getComponentCount(int &pErrorCode);
	  /**
	   * API to find the Birthday using Contact
	   * Application Id 
	   */
	  CEvent *localBdayEventFromRemoteId(string szExId,int &pErrorCode);

	  /**
	   * @param string sId Id of the component
	   * @param time_t timeFrom the start time of the interval
	   * @param time_t timeTo the end time of the interval
	   * @param vector<time_t> result  placeholder for the results
	   * @param time_t tzshift time zone shitf to be added from each value
	   * @return boolean SUCCESS/FAILURE 
	   *
	   * This function returns all instances of the event by given id
	   */
	  bool getInstances(string sId, time_t timeFrom, time_t timeTo, vector <time_t> &result, time_t tzshift=0);


	  /**
	   * @param string sId Id of the component
	   * @param time_t timeFrom the start time of the interval
	   * @param time_t timeTo the end time of the interval
	   * @param time_t tzshift time zone shitf to be added to each value
	   * @return int number of occurences
	   *
	   * This function returns number of instances of the event by given id
	   */
	  int getInstanceNumber(string sId, time_t timeFrom, time_t timeTo, time_t tzshift=0);

	  /**
	   * @param string sId Id of the component     
	   * @return boolean SUCCESS/FAILURE 
	   *
	   * This function drops all instances of the event by given id
	   */
	  bool dropInstances(string sId);

	  /**
	   * @param string sId Id of the component
	   * @param time_t instTime one instance time
	   * @param time_t duration duration of the event
	   * @param time_t tzshift time zone shitf to be substracted from each value
	   * @return boolean SUCCESS/FAILURE 
	   *
	   * This function adds to the database one instance of the event by given id
	   */
	  bool addInstance(string sId, time_t instTime, time_t duration, time_t tzshift=0);

	  /**
	   * @param string sId Id of the component
	   * @param vector<time_t> instances array of instances
	   * @param time_t duration duration of the event
	   * @param time_t tzshift time zone shitf to be substracted from each value
	   * @return boolean SUCCESS/FAILURE 
	   *
	   * This function adds to the database the array of instances of the event by given id
	   */
	  bool addInstances(string sId, vector <time_t> instances, time_t duration, time_t tzshift=0);

	  /**
	   * @param : stDate to do conditional fetching based on start date. -1 in 
	   * case of no conditions
	   * @param : endDate to do conditional fetching based on end date. -1 in 
	   * case of no conditions
	   * @param : limit Max limit of components to be obtanined by this function call
	   * @param : offset from which we need to fetch 
	   * @param : vector<int> vCalid reference to place holder for calenadr id for 
	   * each entry
	   * @param: int * pointer to ErrorCode 
	   * @return : Vector for CComponent
	   * 
	   * This function is overloaded API for fetching entries for all calendars at a 
	   * time progressively. It gives only the minimal data required for Calendar-ui 
	   * in month view. 
	   * P.S : Please donot use this API if you need complete data.  
	   * 
	   */
	  vector <CComponent*> getComponentsWithLimitedData (
			  int iStDate,int iEndDate,int iLimit,
			  int iOffset, vector <int> &vCalid,int &pErrorCode);
	  /**
	   *  checkExistenceOfFutureRepeatingEvent 
	   */
	  bool checkExistenceOfFutureRepeatingEvent(time_t OriginalDateStart,
			  string sId,
			  int& pErrorCode);

	  /**
	   * @param : time_t instanceTime time beyond which Next/Prev entry has to be fetched
	   * @param : string Id Id of event currently visible in Event view.
	   * @param : bool isPrev whether we want previous event.
	   * @param :  int &pErrorCode place holder for error code.
	   * @return : boolean to indicate the Next/Prev presence.
	   * 
	   * Next prev Presence can be found by this method. 
	   * API used to see if there is next prev instance so that 
	   * NEXT PREV Menus are updated in UI in event view.   
	   * This has to be used intelligently by UI 
	   * API Usage: 
	   * 
	   * 1. Api quickly indicates the presence of PREV /NEXT event  
	   * 2. If the event opened  in event view  is nth repeat and the 
	   *    event has p repeats 
	   *    event 1 has total p repeats and Ui is showing the nth repeat in 
	   *    event view 
	   *    0, 1,2,........ n ........p
	   *    In this case no need to call the checkNextPrev as the instance being shown in UI 
	   *    middle instance there will be next and previous instances.
	   *    So enable the Next and Previous menus. ( exception case is when repeat type is complex 
	   *    In this case call getNextPrevious to get the actual instance in this case it will be a bit slower ) 
	   *
	   * 3. PRev logic  checks if previous event exist or not , idea is  event has to start before the input date 
	   *     for it to be previous event ( same logic holds goog for timebased and repeating events) 
	   * 5. Next logic is optimised to see if there is any event which starts after this time / any repeating event 
	   *   if this fails then we check for any repeating event with until=-1 ( if one such event is found then 
	   *   we return at that point itself with out expanding any further.)
	   */
	  bool checkForNextPrev(time_t instanceTime,string Id ,bool isPrev, int &pErrorCode);


      /**
       * Get the unused color in the calendar
       * applcation. Returns COLOUR_DARKBLUE if all the colors have been used.
       * @param void No arguments
       * @return CalendarColour Calendar color
       */
       CalendarColour getUnusedCalendarColor(void);

       /**
         * Reload  timezone information from system
         */
       static void reloadSystemTimezone();

       /**
         * Function to get the impending alarm time
         */
       void getImpendingAlarm (string szRrule,time_t & triggerTime,
                time_t dateStart, int alarmFlag, icaltimezone* pTz);


  private:


          /**
           * Default constructor for CMulticalendar class
           */
          CMulticalendar();


          /**
           * @param: iCalId - calendar ID 
           * @param: int type E_EVENT = 1
           * @param: int iDaybefore time for deletion of entries
           * @param: int reference to ErrorCode 
           * @return: vector of string Id  
           *
           * Function used to retrieve the ID of the entries ( only Events and Todos) 
           * expired ( 2weeks over-due)
	   */
	  vector<string> giveRecursiveExpiredEvents(int iCalId,int iType,int iDayBefore,int iMax,int &iLimit,int &pErrorCode);
	  /**
	   * @param string recurence rule 
	   * @parma icaltimetype date start value in icaltimetype
	   * @param time_t view end parameter
	   * @param icaltimezone pointer
	   * @param pErrorCode reference to error code 
	   * @return return the particular instance time
	   * 
	   * Function to get the next instance time given rruke and 
	   * date start 
	   */

	  time_t getInstanceBeyond(string szRRule, icaltimetype iDtStart, time_t iViewEnd, icaltimezone* pTz, int& pErrorCode);
	  /**
           * @param: int iCalId - calendar ID 
           * @param: int type E_EVENT = 1,E_TODO = 2
           * @param: int Configured time for deletion of entries
           * @param: int & pointer to ErrorCode 
           * @return: vector of string Id
           *
           * Function used to retrieve the ID of the entries ( only Events and Todos) 
           * expired ( 2weeks over-due)
           */
          vector < string > giveExpiredEntries(int iCalId, int iType, int iConfiguredTime,int& pErrorCode);

          /**
           * @param: int calendar id 
           * @param: int type of component 
           * @param: int&  pointer to ErrorCode 
           * @return: int number of instances of the particular type in DB 
           * 
           * This is common function called from getEventCount ,getTodoCount and getNoteCount
           * to get the count for each type.
           */
          int getCount(int iCalId, int iType,int& pErrorCode);

          /**
           * @param: repeattype frequency in int
           * @param:  until time in utc 
           * @param: int & reference  to ErrorCode 
           * @return: string Rrule in string
           * 
           * Function used to generate RRULE in the format as per ICAL RFC 2445
           * The generated RULE is of type FREQ=****UNTIL=****;
           * This is useful only for events added from calendar-UI
           */
          string generateRrule(int iRepeatType, time_t iUntil,int& pErrorCode);


          /**
           * private Dummy copy constructor
           * because only one object needs to present  
           * To make sure copies are not created at runtime
           */
          CMulticalendar(CMulticalendar & ref);

          /**
           * private dummy overloaded assignment operator 
           * to avoid creation of mutiple instances
           */
          CMulticalendar & operator=(CMulticalendar & calendardb);

          /**
          * @param pCalendar pointer to calendar to set name to
          * @param szCalendarName calendar name read from database
          * @return none
          * 
          * Function to set the name for calendar.
          * If it is default synced calendar the name shall be the same with product name if was not changed.
          * Otherwise it shall be name read from database
          */
          void setCalendarName(CCalendar* pCalendar, string szCalendarName) const;

          /* Max numeric id of component in DB 
           * populated at the time of construction*/    
          int iMaxId;
          /*application which us using the calendar backend can use to set the ID of it 
           */
          string AppName;
	  static CMulticalendar *pMc;
          static std::string szSystemTimezone; //! Current timezone

           /*regenerete instances */
           int regenerateInstances();

          /**
           * Remove non-recurrent events older than specified date
           * @param iDayBefore - target date. All events older than that date will be deleted.
           * @param iMax - max nubmer of events to be deleted at once
           * @param pErrorCode - error code
           * @return number of deleted components
           */
          int deleteNonRecurrentComponents(int iDayBefore, int iMax, int &pErrorCode);


          /**
           * Remove all events from specified calendar (prior calendar deletion)
           * @param iCalId - calendar to be flushed
           * @param pErrorCode - error code
           * @return operation complete status (SUCCESS/FAILURE)
           */
          bool deleteAllComponents(int iCalId, int &pErrorCode);

          /**
           * Remove unsupported symbols from ICS file string.
           * @param ics_line string to clean up
           */
          void cleanupIcsString(std::string &ics_line);

          /**
           * Restore daemon-side alarms based on calendar alarms
           */
          void restoreAlarms();

          /**
           * Compare compoenents in the order of "Prev/Next event"
           * @param c1 - pointer to first compoenent
           * @param c2 - pointer to second compoenent
           * @return 1 - c1 сome after c2 (direct order)
                    -1 - c1 comes before c1 (revers order)
                     0 - components are equal (i.e. same components are compared) 

           */
          int comparePrevNextComponent(CComponent *c1, CComponent *c2, time_t t1 = -1, time_t t2 = -1);
};

#endif
