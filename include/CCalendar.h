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

#ifndef __CCALENDAR_H__
#define __CCALENDAR_H__
#include <map>
#include <sqlite3.h>
#include <string>
#include <vector>
using namespace std;
using std::vector;
using std::string;
/* Forward declaration of 
 * class members */
class CEvent;
class CTodo;
class CJournal;
class CBdayEvent;
class CProperties;
class CCache;
class CProperties;
class CComponentDetails;
class CRecurrence;
class CAlarm;
class CComponent;
class CParameters;
class CAttendee;
class COrganizer;
class CBdayEvent;


const int SEC_IN_DAY = 86400;
const int ATTACHMENTS = 2;
const int RECURSIVE = 4;
const int ATTENDEES = 6;
const int ORGANIZER = 8;

enum ACTION {
    E_ADD = 0,
    E_MODIFY,
    E_DELETE
};

enum  CalendarColour {
    COLOUR_DARKBLUE = 0,
    COLOUR_DARKGREEN,
    COLOUR_DARKRED,
    COLOUR_ORANGE,
    COLOUR_VIOLET,
    COLOUR_YELLOW,
    COLOUR_WHITE,
    COLOUR_BLUE,
    COLOUR_RED,
    COLOUR_GREEN,

    COLOUR_NEXT_FREE //keep it the last one
    };

enum SyncProfileType{
    SYNC_METHOD_NONE=0,
    SYNC_METHOD_TWOWAY ,
    SYNC_METHOD_SEND,
    SYNC_METHOD_RECEIVE,
    SYNC_METHOD_WEB
};

typedef enum {
    LOCAL_CALENDAR = 0,
    BIRTHDAY_CALENDAR,
    SYNC_CALENDAR,
    DEFAULT_PRIVATE,
    DEFAULT_SYNC
} CalendarType;


/**
 * @param: integer i 
 * @return: string
 * 
 * Utility function to convert integer to string.
 * TODO: This function should be moved to a utility class.
 */
string intToString(int i);


/**
 * CCalendar class
 *
 * @brief This class has the API's to add/modify/delete the calendars
 * and to add/modify/delete the components such as
 * Event/Todo/Journal/Birthday's.
 */

class CCalendar {

  public:
    /*TODO: Make these private after unit testing
    * Gets cache entry pertaining to the input start date
    */
    
    string getCacheEntry(time_t startDate, int &pErrorCode);

    /**
     * Adds a single Id into cache table at specified start date
     */
    bool addCacheEntry(string sId, time_t startDate, int &pErrorCode);


    /**
     * deletes a single cache entry
     */
    bool deleteCacheEntry(time_t startDate, int &pErrorCode);

    /**
     *  Checks if entries exist for a given date
     */
    int getCacheTableCount(time_t inputDate, int &pErrorCode);
    /**
     * 	
   	 Adds mutiple entries for a recurrent event
    	 and single entry for events and to do
    	 TODO: No rollback mechanism and error scenarios handled
         TODO: To check impact of sync when adding, deleting, modifying
     **/
    bool addCacheEntries(CComponent* pEntry, int &pErrorCode);

    /*
     * If an entry is available for a particular date updates
     * the entry with new Ids
     */
    bool updateCacheEntry(string szId, time_t startDate, int &pErrorCode);

    /***
     * Add a new Id to the comma separated Id string
     */
    bool appendToIds(string Id, string& szIds);

    /**
     * Remove Id from comma separated Id string
     */
    string removeFromIds(string Id, string szIdStrings);

    /**
     * Remove entries from cache table
     * TODO: Use enum input param
     */
    bool deleteCacheEntries(string szId, int eEntry, int &pErrorCode);

    /**
     * Modify the Cache entry 
     */
    bool modifyCacheEntry(CComponent* oldEntry, int &pErrorCode);
    /**
     * check if the event is multiday 
     */
    void checkIfMultiDayEvent(CComponent* pEntry, vector <time_t> &vStartdates);

    /**
     * copy Constructor for CCalendarClass
     */
    CCalendar(CCalendar & ref);


    /**
     * overloaded assignment operator for CCalendar class 
     */
    const CCalendar & operator=(const CCalendar & right);

    /**
     * @param: int iCalId represents calendar id
     * @param: string szName represents calendar name
     * @param: CalendarColour iColor represents calendar color
     * @param: bool fIsReadOnly represents whether calendar read only or not 
     * @param: bool fIsVisbile represents whether calendar visible/invisible
     * @param: CalendarType iCalType represents type of the calendar
     * @param: string szCalendarTune represents ring tone of the calendar
     *
     * This is parameterized constructor for Calendar class.
     * it initializes the name,color,id,readonly,visible parameters and type
     * of the calendar.
     */
     CCalendar(int iCalId, string szName, CalendarColour iColour, bool fIsReadOnly, 
             bool fIsVisible, CalendarType iCalType ,string szCalendarTune);

     /**
      * @param: string szName represents calendar name
      * @param: CalendarColour iColor represents calendar color
      * @param: bool fIsReadOnly represents whether calendar read only or not 
      * @param: bool fIsVisbile represents whether calendar visible/invisible
      * @param: CalendarType iCalType represents type of the calendar
      * @param: string szCalendarTune represents ring tone of the calendar
      * @param: string szCalendarVersion represents Version of the calendar
      *
      * This is parameterized constructor for Calendar class.
      * it initializes the name,color,id,readonly,visible parameters and type
      * of the calendar.
      */
     CCalendar(string szName, CalendarColour iColour, bool fIsReadOnly,
             bool fIsVisible, CalendarType iCalType,string szCalendarTune,string szCalendarVersion);

     /**
      * @param : stDate to do conditional fetching based on start date. -1 in case of no conditions
      * @param : endDate to do conditional fetching based on end date. -1 in case of no conditions
      * @param : limit limit for number of events fetched by the calenadr
      * @param int * pErrorCode error code:
      *          CALENDAR_OPERATION_SUCCESSFUL- fetching is successful and can be continued;
      *          CALENDAR_FETCH_NOITEMS- fetching is finished (no more items found in DB);
      *          other error code- fetching is failed.
      * @return : Vector for CComponent. If pErrorCode equals to CALENDAR_OPERATION_SUCCESSFUL,
      *           but vector is empty, no component (from offset to limit) meets the conditions,
      *           continue fetching
      * This function is used to get all the Events /Todos /Bdays  associated with particular Calendar progressively 
      * It is an overloaded API for get componets used to fetch Components in bursts of specified size
      */

     vector < CComponent * > getAllEventsTodos( int iStDate, int iEndDate,
             int iLimit , int& iOffset,int& pErrorCode);

     /**
      * @param: None
      * @return: None
      * 
      * This is default constructor of Calendar class.
      * it calls initCalendar() to set default values to member variables.
      */
     CCalendar();

     /**
      * @param iCalendarColour: Calendar Color to be set from any one 
      * of the following
      * COLOUR_RED,
      * COLOUR_GREEN,
      * COLOUR_BLUE,
      * COLOUR_PURPLE,
      * COLOUR_YELLOW
      * @return: bool (SUCCESS/FAILURE)
      * 
      * This function is called to set the color of calendar.
      * it sets the strColor as the calendar color.
      */
     bool setCalendarColor(CalendarColour iCalendarColor);

    /**
     * @param: None
     * @return: returns calendar color as a Enumeration
     * COLOUR_RED,
     * COLOUR_GREEN,
     * COLOUR_BLUE,
     * COLOUR_PURPLE,
     * COLOUR_YELLOW
     * 
     * This function is called to get the calendar color.
     */
    CalendarColour getCalendarColor();

    /**
     * @param string szName: Calendar Name to be set
     * @return: bool (SUCCESS/FAILURE)
     *
     * This function is called to set the name of calendar.
     * it sets the strName as the calendar name.
     */
    bool setCalendarName(string szName);

    /**
     * @param: None
     * @return: returns calendar name as a char pointer
     *
     * This function is called to get the calendar name.
     */
    string getCalendarName();

    /**
     * @param int  iId: Calendar id to be set
     * @return: bool (SUCCESS/FAILURE)
     *
     * This function is called to set the id of calendar.
     * it sets the id as the calendarid.
     */
    bool setCalendarId(int iId);

    /**
     * @param: None
     * @return int: returns the calendar id
     * 
     * This function is called to get the calendar id.
     */
    int getCalendarId();


    /**
     * @param: CalendarType iCaltype: Calendar Type to be set
     * @return: bool (SUCCESS/FAILURE)
     *
     * This function is called to set the type of calendar
     * 0 - Local calendar
     * 1 - Sync calendar
     * 2 - Smart calendar
     */
    bool setCalendarType(CalendarType iCalType);
    /**
     * @param SyncProfileType - Profile type of calendar
     * @return NONE
     *
     *  Function to set profile Type reuired from maesync point of view 
     */
    void setProfileType(SyncProfileType iProfile);
    /**
     * @param NONE
     * @return SyncProfileType - Profile type of calendar
     *
     *  Function to get profile Type reuired from maesync point of view 
     */
    SyncProfileType getProfileType();
        
    /**
     * @param: None
     * @return: CalendarType the calendar type
     * 0 - Local calendar
     * 1 - Sync calendar
     * 2 - Smart calendar
     * 
     * This function is called to get the calendar type.
     */
    CalendarType getCalendarType();

    /**
     * @param bool fIsReadonly: represents whether calendar is readonly 
     * @return: bool (SUCCESS/FAILURE)
     *
     * This function is called to set whether calendar is readonly.
     */
    bool setCalendarReadOnly(bool fIsReadOnly);

    /**
     * @param: None
     * @return: bool TRUE/FALSE whether calendar is readonly. 
     * 
     * This function is called to know whether calendar is readonly.
     */
    bool IsReadOnly();

    /**
     * @param: bool isVisible: represents whether calendar is visible or not
     * @return: bool (SUCCESS/FAILURE)
     *
     * This function is called to set whether calendar is visible.
     */
    bool setCalendarShown(bool fIsVisible);

    /**
     * @param: None
     * @return: returns the visibility of the calendar
     * 
     * This function is called to know calendar visibility .
     */
    bool IsShown();

    /**
     * @param: CEvent* pEvent: pointer to calEvent
     * @param: int&  reference to error int to throw some error code. 
     * @return: bool 
     *
     * This function is called to modify the event entry record
     * in the database. Incase the Event is moved from one calendar
     * to the other then deleteEvent/addEvent APIs should be used
     * instead of modifyEvent.
     */
    bool modifyEvent(CEvent * pEvent,int &pErrorCode);


    /**
     * @param: CBdayEvent* pBday: pointer to CBdayEvent
     * @param: int * pErrorCode error code if any
     * @return: bool (SUCCESS/FAILURE)
     *
     * This function is called to modify the birthday record
     * in the database.
     */
    bool modifyBirthDay(CBdayEvent *pBday,int& pErrorCode);

    /**
     * @param: CJournal* pJournal: pointer to calJournal
     * @param: int * pErrorCode error code if any
     * @return: bool 
     *
     * This function is called to modify the journal entry record
     * in the database. Incase the Todo is moved from one calendar
     * to the other then deleteJournal/addJournal APIs should be used
     * instead of modifyJournal.
     */
    bool  modifyJournal(CJournal * pJournal,int& pErrorCode);

    /**
     * @param: CTodo* pTodo: pointer to calTodo 
     * @param: int * pErrorCode error code if any
     * @return: bool 
     *
     * This function is called to modify the todo entry record
     * in the database. Incase the Todo is moved from one calendar
     * to the other then deleteTodo/addTodo APIs should be used
     * instead of modifyTodo.
     */
    bool  modifyTodo(CTodo * pTodo,int& pErrorCode);

    /**
     * @param: CEvent* pEvent: calEvent pointer
     * @param: int * pErrorCode error code if any
     * @return: bool 
     *
     * This function is called when an event entry  is to be added.
     * it adds an event(provided by calEvent pointer) into database.
     * In successful scenario Id of the newly added event is set in pEvent , can be
     * retrieved using pEvent->getId().
     */
    bool  addEvent(CEvent * pEvent,int& pErrorCode);


    /**
     * @param: CBdayEvent *pBdayendar: CBdayEvent pointer
     * @param: int * pErrorCode error code if any
     * @return: bool (SUCCESS/FAILURE)
     *
     * This function is called when an birthday entry is to be added.
     * In successful scenario Id of the newly added event is set in pBday , can be
     * retrieved using pBday->getId().
     */
    bool addBirthDay(CBdayEvent * pBday,int& pErrorCode);

    /**
     * @param: CTodo* ptrTodo: calTodo pointer
     * @param: int * pErrorCode error code if any
     * @return: bool 
     * 
     * This function is called when a todo entry
     * is to be added. it adds a todo(provided by calTodo pointer) into database.
     * In successful scenario Id of the newly added todo is set in ptrTodo , can be
     * retrieved using ptrTodo->getId().
     */
    bool addTodo(CTodo * ptrTodo,int& pErrorCode);

    /**
     * @param ptrJournal: calJournal pointer
     * @param int * pErrorCode error code if any
     * @return: bool 
     * 
     * This function is called when a journal entry is to be added.
     * it adds a journal(provided by calJournal pointer) into database.
     * In successful scenario Id of the newly added Journal is set in ptrJournal , can be
     * retrieved using ptrJournal->getId().
     */
    bool addJournal(CJournal * ptrJournal,int& pErrorCode);

    /**
     * @param string sEventID: eventId to be deleted
     * @param int * pErrorCode error code if any
     * @return: bool
     *
     * This function is called when an event
     * is to be deleted.
     * it deletes an event(provided by eventId) from database.
     */
    bool deleteEvent(string sEventID,int& pErrorCode);


    /**
     * @param int iUid: Unique UID for birthday contacts from Ebook db.
     * @param int * pErrorCode error code if any
     * @return: bool (SUCCESS/FAILURE)
     *
     * Function used to delete birthday record from database.
     */
    bool deleteBirthDay(int iUid,int& pErrorCode);

    /**
     * @param string todoid: todoid to be deleted
     * @param int * pErrorCode error code if any
     * @return:bool  
     *
     * This function is called when a todo entry
     * is to be deleted.
     * it deletes a todo(provided by todoid) from database.
     */
    bool  deleteTodo(string sTodoID,int& pErrorCode);

    /**
     * @param string sJournalid: journalid to be deleted
     * @param int * pErrorCode error code if any
     * @return: NONE
     *
     * This function is called when a journal entry
     * is to be deleted.
     */
    bool deleteJournal(string sJournalID,int& pErrorCode);
    
    /**
     * @param string sId: id of the component to be deleted
     * @param int * pErrorCode error code if any
     * @return: NONE
     * 
     * Function deletes component from DB based on Component ID.  
     */
    void  deleteComponent(string  sId,int& pErrorCode);

    /**
     * @param  string etjId: id of event/todo  
     * @param int * pErrorCode error code if any
     * @return: vector of long ,cookie id is unique per alarm
     * 
     * Function used to get cookie vector from Databse for a particular 
     * event ID .
     */
     vector < long > getCookie(string ietjId,int& pErrorCode);
     
     /**
      * @param none
      * @return string calendar version number
      * 
      * Function to retrieve Calendar version
      */
     string getCalendarVersion();
     
     /**
      * @param string szCaeldnarVersion
      * @return none 
      * 
      * Function to set Caelndar version
       */
    void setCalendarVersion(string szCalendarVersion);
    /**
     * @param  perrorCode 
     * @return bool success/failure indicator 
     * Function to register Alarm for visible calendars 
     */
    bool registerAlarmForVisibleCalendar( int& perrorCode);

    /**
     * ~Calendar 
     * 
     * @param None 
     * @return None 
     *
     * This is destructor for the Calendar class.
     */
    ~CCalendar();
    /**
     * @param int iCompType  Component Type
     * @param int&  pErrorCode error code if any
     * @return vector <string> Vector of component id's
     *
     * Function is used to get the list of components id's
     * For a given calendar and component -type, all existing
     * IDs in DB are retrieved.
     */
     vector<string> getIdList(int iCompType,int& pErrorCode);

     /**
      * @param string Component Id
      * @param int * pErrorCode error code if any
      * @return CEvent * Pointer to CEvent class
      *
      * NOTE : Them memory for CEvent is allocated in this procedure
      * has to be freed by API user.
      * Function used to get the complete event details of particular
      * component ID
      */

    CEvent *getEvent(string  sComponentID,int& pErrorCode);


     /**
      * @param string szComponentId Component Id
      * @param int&  pErrorCode place holder
      * @return CTodo * Pointer to CTodo class
      * 
      * NOTE : Them memory for CTodo is allocated in this procedure
      * has to be freed by API user.
      * 
      * Function used to get the complete todo details of particular
      * component ID
      */
    CTodo *getTodo(string szComponentID,int& pErrorCode);


     /**
      * @param string szComponentId Id of component(Event/Todo/Journal)
      * @param int&  pErrorCode place holder
      * @return CJournal * Pointer to CJournal class
      * 
      * NOTE : Them memory for CJournal is allocated in this
      * procedure has to be freed by API user.
      * Function used to get the complete journal details of
      * particular component ID
      * 
      */
    CJournal *getJournal(string sComponentID,int& pErrorCode);
     /**
      * checkDuplicateEntry()
      * @param CComponent* pEntry  - pointer to CComponent 
      * @param int iType - Type of entry can be event todo or journal 
      * @param int&  pErrorCode -   
      * @return bool  - Duplicate is present or not 
      *
      * Function to check whether duplicate entry exist in DB
      */
     bool  checkDuplicateEntry(CComponent* pEntry,int iType , int& pErrorCode);


    /**
     * @param int&  pErrorCode place holder
     * @return vector<CEvent *> Vector to CEvent
     * 
     * Function used to get the all the events.
     * NOTE : Them memory for vector<CEvent*> is allocated in this
     * procedure has to be freed by API user.
     */
     vector < CEvent * >getDetailedEventList(int& pErrorCode);


    /**
     * @param int&  pErrorCode place holder
     * @param int * pErrorCode error code if any
     * @return vector<CTodo *> Vector to CTodo
     * 
     * Function used to get the all the todos for a given calendar.
     * NOTE : Them memory for vector<CTodo*> is allocated in this
     * procedure has to be freed by API user.
     */
     vector < CTodo * >getTodoListDetailed(int& pErrorCode);


    /**
     * @param int&  pErrorCode place holder
     * @return vector<CJournal *> Vector to CJournarl
     * 
     * Function to retrieve all journals for a given calendar
     * NOTE : Them memory for vector<CTodo*> is allocated in this procedure
     * has to be freed by API user.
     */
     vector < CJournal * >getJournalListDetailed(int& pErrorCode);


     /**
      * @param string Id of the birthday event in DB 
      * @param int&  pErrorCode place holder
      * @return TRUE/FALSE
      * 
      * Function will be called by Ebook API's in calendar package,
      * to delete the birthday's in local calendar database, if contact
      * is deleted in Global Address book
      */
    bool deleteBirthDay(string szUID,int& pErrorCode);

    /**
     * getComponents
     * 
     * @param : calid Calendar ID
     * @param :      1 -> EVENT
     *               2 -> TODO
     *               3 -> JOURNAL
     *               4 -> BDAY       
     * @param : stDate to do conditional fetching based on start date.
     *             -1 in case of no conditions
     * @param : endDate to do conditional fetching based on end date.
     *             -1 in case of no conditions
     * @param int&  pErrorCode place holder
     * @return : Vector for CComponent
     *  
     * NOTE : Them memory for vector <CComponent*> is allocated in this
     * procedure has to be freed by API user. This function is used
     * to get all the entries associated with particular component type
     */
    vector < CComponent * > getComponents(int iCalendarID,
                    int iStartDate, int iEndDate,int& pErrorCode);
    
    
    /**
     * getComponents
     * 
     * @param : int iCalendarId Calendar ID
     * @param : int type
     *               1 -> EVENT
     *               2 -> TODO
     *               3 -> JOURNAL
     *               4 -> BDAY
     *               
     *
     * @param : int iStDate to do conditional fetching based on 
     *         start date. -1 in case of no conditions
     * @param : int iEndDate to do conditional fetching based on end date.
     *         -1 in case of no conditions
     * @param : int iLimit limit for number of events fetched by the calenadr
     * 
     * @param int * pErrorCode error code:
     *          CALENDAR_OPERATION_SUCCESSFUL- fetching is successful and can be continued;
     *          CALENDAR_FETCH_NOITEMS- fetching is finished (no more items found in DB);
     *          other error code- fetching is failed.
     * @return : Vector for CComponent. If pErrorCode equals to CALENDAR_OPERATION_SUCCESSFUL,
     *           but vector is empty, no component (from offset to limit) meets the conditions,
     *           continue fetching
     * 
     * This function is used to get all the entries associated 
     * with particular component type progressively. It is an overloaded
     * API for get componets used to fetch Components in bursts of specified size
     */
    vector < CComponent * > getComponents(int iCalendarID, int iStartDate,
                    int iEndDate, int iLimit , int iOffset,int& pErrorCode);


    /**
     * @param int Calendar id
     * @param string component id
     * @param int component type
     * @param time_t Deleted time
     * @param int& place holder for  pErrorCode error code
     * @return SUCCESS/FAILURE
     * 
     * This function will be called when components are deleted, 
     * to update them in trash table
     **/
    int updateTrash (int iCalendarID, string iComponentID, int iComponentType,
        time_t iDeletedTime, int& pErrorCode);


    /**
     * @param: int Date Added items after this date  
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CEvent 
     *  
     * This function return all the events which are added after 
     * specified date.
     * Memory allocated for vector<CEvent*> has to be freed by API user 
     */
    vector < CEvent * > getAllAddedEvents(int iDate,int& pErrorCode);
    
    /**
     * @param: int Date Added items after this date  
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CTodo 
     *  
     * This function return all the Todos which are added after 
     * specified date 
     * Memory allocated for vector<CTodo*> has to be freed by API user 
     */
    vector < CTodo * > getAllAddedTodos(int iDate,int& pErrorCode);

    /**
     * @param: int Date Added items after this date  
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CJournal 
     *  
     * This function return all the Journals which are added after 
     * specified date 
     * Memory allocated for vector<CJournal*> has to be freed by API user 
     */
    vector < CJournal * > getAllAddedJournals(int iDate,int& pErrorCode);

    /**
     * @param: int Date Added items after this date  
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CComponent     
     *  
     * This function returna all the components which are added after 
     * specified date 
     * NOTE:Memory allocated for vector<CComponent*> inside this
     * procedure has to be freed by API user(using delete)
     */
    vector < CComponent * >getAllAddItems(int iDate,int& pErrorCode);

    /**
     * @param: int Date Modified items after this date  
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CEvent 
     *  
     * This function returns all the events which are Modified after 
     * specified date 
     * NOTE:Memory allocated for vector<CEvent*> inside this
     * procedure has to be freed by API user(using delete)
     */
    vector < CEvent * > getAllModifiedEvents(int iDate,int&  pErrorCode);
    
    /**
     * getAllModifiedTodos()
     * @param: int Date Modified items after this date  
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CTodo 
     *  
     * This function returns all the todos which are Modified after 
     * specified date 
     * NOTE:Memory allocated for vector<CTodo*> inside this
     * procedure has to be freed by API user(using delete)
     */
    vector < CTodo * > getAllModifiedTodos(int iDate,int&  pErrorCode);

    /**
     * @param: int Date Modified items after this date  
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CJournal 
     *  
     * NOTE:Memory allocated for vector<CJournal*> inside this
     * procedure has to be freed by API user(using delete)
     * This function returns all the journals which are Modified after 
     * specified date 
     */
    vector < CJournal * > getAllModifiedJournals(int iDate,int&  pErrorCode);
    
    /**
     * @param: int Date Modified items after this date  
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CComponent 
     *  
     * This function returns all the components which are Modified after 
     * specified date 
     * NOTE:Memory allocated for vector<CComponent*> inside this procedure
     * has to be freed by API user(using delete)
     */
     vector < CComponent * >getAllModifiedItems(int iDate,int& pErrorCode);

    /**
     * @param: vector of string id of Deleted items  
     * @param int iType specifies the component type to be fetched 
     * @param int& place holder for  pErrorCode error code
     * @return: vector of CComponent 
     *  
     * This function returns all the components which are Deleted after 
     * specified date 
     * NOTE:Memory allocated for vector<CComponent*> inside this
     * procedure has to be freed by API user (using delete)
     */
     vector < string >getAllDeletedItems(int iDate,int iType,int& pErrorCode);
    
    /**
     * @param string - filename of the ics file 
     * @param int& place holder for  pErrorCode error code
     * @return  string name of the file 
     * 
     * Export file format will all be in line with ICAL format
     */
    int exportCalendarEntries(string szFilename,int& pErrorCode);

    /**
     * @param int component id
     * @param int& place holder for  pErrorCode error code
     * @param int iType Component type 
     * @return  bool true/false
     * 
     * Function to set the impending alarm for a recursive event.
     * It is invoked on the onset of alarm.
     */
    bool setNextAlarm (string sComponentId,int iType,int& pErrorCode);

    /**
     * @param string szCaeldnarTune
     * @return none 
     * 
     * Function to set Caelndar tune
     */
    void setCalendarTune(string szCalendarTune);
    /**
     * @param NONE
     * @return string CaelndarTune 
     *
     * Function to retrieve calendar tune
     */
    string getCalendarTune();
            
    
    /**
     * @param : hashMap to store prop and param values
     * @param : sId id of the entry
     * @param : iAction action to be taken
     * @param int& place holder for  pErrorCode error code
     * @param : clearHashMap indicator for hashMap content deletion
     * @return : bool
     * 
     * This function adds/modifies parameters of an entry .
     */
    bool addParameters(map< string, vector < CParameters* > > hashMap,string sId,ACTION iAction,int& pErrorCode,bool clearHashMap = true);

    /**
     *  @param vector<Cproperties*> vXprop 
     *  @param string sId 
     *  @param ACTION iAction can be add/modify/delete
     *  @param int& place holder for  pErrorCode error code
     *  @return bool status of operation   
     *
     *  Function to add properties in to properties table.
     */

    bool addXProperties(vector < CProperties* > vXProp,string sId,ACTION iAction,int& pErrorCode);
    /**
     * @param sId input Id string for which mapped Id has to be found
     * @param bool fetchExternal - flag to fetch external Id 
     * @param int & reference to error code.
     * @return  string Id - local ID of the birthday event
     * 
     * Function to get local Id corresponding the the external Id 
     * fetchLocal- 1 returns local Id  corresponding to external Id 
     *           - 0 returns externalId corresponding to local Id 
     */
    string getExternalToLocalId(string sId,bool fetchLocal, int & pErrorCode);

    /**
     * @param  string sLocalId - local Id of Birthday event 
     * @param string externalId Id maintained by exteranl applications
     * @param int& pErrorCode - reference to errorcode 
     * @return boolean - success/failure  indicator 
     * 
     * Function to map externalId of Bday event with the internal ones
     */
    bool addMapTableInfo(string sLocalId,string sExternalId,int & pErrorCode);
    /**
     * @param: int Date Deleted items after this date  
     * @param int * pErrorCode error code if any
     * @return: vector of CComponent 
     *  
     * This function returns all the Events which are Deleted after 
     * specified date 
     */
    vector<string> getAllDeletedEvents (int iLastSyncDate,int& pErrorCode);
    /**
     * @param: int Date Deleted items after this date  
     * @param int * pErrorCode error code if any
     * @return: vector of CComponent 
     *  
     * This function returns all the components which are Deleted after 
     * specified date 
     */
    vector<string> getAllDeletedJournals(int iLastSyncDate,int& pErrorCode);

    /**
     * @param: int Date Deleted items after this date  
     * @param int * pErrorCode error code if any
     * @return: vector of CComponent 
     *  
     * This function returns all the tasks which are Deleted after 
     * specified date 
     */
    vector<string> getAllDeletedTodos(int iLastSyncDate,int& pErrorCode);
    /**
     * destroyExistingValidAlarms
     * @param pErrorCode - reference to error Code 
     * @return status of operation
     *
     * Function to deregister all the alarms which are 
     * already set for a  visible calendar  
     */
    bool destroyExistingValidAlarms(int& pErrorCode );
    /**
     * @param string table name  
     * @param entryid can be Id of event todo or Journal 
     * @param int& place holder for  pErrorCode error code
     * @return boolean ressult of operation 
     * 
     * This function is used to know whether there is any entry present in a table for particular ID 
     */
    bool checkEntryExist(string szTable, string iId,int& pErrorCode);

    /**
     * @param string sCompId Component Id 
     * @param int iType cand be E_EVENT,E_TODO or E_JOURNAL
     * @param int pErrorCode reference to error code 
     *
     * Function to retrieve Entry from DB , which can be event todo or Journal 
     */ 
    CComponent* getEntry(string sCompId , int iType, int &pErrorCode);

    /**
     * @param int * pErrorCode error code if any
     * @return detailed list of Event 
     *
     * This function is called when a Event list
     * is to be retrieved from DB it gives verbose list of all Event in DB
     * this member function is added from sync point of view 
     */
    vector <CEvent *> getDetailedBdayList(int &pErrorCode);

    /** 
     * getEvent
     * @param Component ID
     * @param int * pErrorCode error code if any
     * @return CEvent* Pointer to CEvent class
     * 
     * This function is used to get the elaborate details of particular entry
     */
    CEvent* getBirthDayEvent(string sCompId, int &pErrorCode);
    /* the foloing three APIS are Batch APIS */
    /**
     * @param vector<string> list of component Ids to be deleted 
     * @param ErrorCode
     * @return none 
     * 
     *  Function to delete components in batch 
     */
    void deleteComponents(vector<string> idList,int &ErrorCode);
    /**
     * @param vector<CComponent*>  CComponent to be modified
     * @param ErrorCode
     * @return  none
     * FIXME: even if one error is encountered in mofdify entire transaction is 
     * rolled back.
     * Function to modify Events in batch 
     */  
    void modifyComponents(vector<CComponent*> &compList,int &ErrorCode);
    /**
     * @param vector<CComponent*> vector of CComponent * to be added 
     * @param vector <string> duplicate Ids if at all they exist
     * @param Error code - can be CALENDAR_OPERATION_SUCCESSFUL/CALENDAR_ENTRY_DUPLICATED 
     * @return vector<string> added Component Ids in Calendar database.
     * 
     * To enhance the speed of operation error code when obtained is ignored in failure 
     * scenarios  ,  so that we donot rollback the entire transaction if adding one component fails.
     * what it means is the returned IdList contains empty strings in case there is any
     * error  while adding the component.
     * 
     * FIXME: will add a set of error codes, which can be passed to API user each one corresponding to 
     *  one evntry in component list,but this would mean changes in way 
     *  api is used, will integrate this in coming weeks.
     *
     * Function to add Events/tasks /Journals in batch
     */
    vector<string> addComponents(vector<CComponent*> &compList,vector <string> &duplicateIds,int &ErrorCode);
    /**
     * @param int errorcode 
     * @return vector CEvent*  
     *
     * Function returns all events in a particular calendar 
     */
    vector <CEvent*> getEvents(int & pErrorCode);
    /**
     * @param pErrorCode Reference to error code 
     * @return vector <CTodo*> detailed list of tasks 
     *
     * Function to retrieve list of todos per Calendar 
     */
    vector <CTodo*> getTodos(int & pErrorCode );
    /**
     * @param pErrorCode Reference to error code 
     * @return vector <CJournal*> detailed list of journals 
     *
     * Function to retrieve list of journals per Calendar 
     */
    vector <CJournal*> getJournals(int & pErrorCode );

    /**
     * toString
     */
    string toString();


  private:
    /* this function returns the instance Ids
     * on which the event is present 
     */
    vector<CCache*> getDeletedCacheTableInfo(string sCompId,int& pErrorCode);
    
    bool updateAttendeesForComponent(CComponentDetails *pCompDetails, 
		int &pErrorCode);
    bool deleteAttendeesForComponent(CComponentDetails *pCompDetails, 
		    int &pErrorCode);
    bool updateOrganizerForComponent(CComponentDetails *pCompDetails, 
		    int &pErrorCode);
    bool deleteOrganizerForComponent(CComponentDetails *pCompDetails, 
		    int &pErrorCode);
    bool updateXPropertiesForComponent(CComponentDetails *pEntry, 
		    int &pErrorCode);
    bool deleteXPropertiesForComponent (CComponentDetails *pEntry, 
		    int &pErrorCode);
    bool updateParametersForComponent(CComponentDetails *pEntry, 
		    int &pErrorCode);
    bool deleteParametersForComponent (CComponentDetails *pEntry, 
		    int &pErrorCode);

    bool deregisterAlarm (CComponentDetails *pCompDetails, 
		    int &pErrorCode);
    
    /**
     * @param  string szTable  table name 
     * @param  string szField column name 
     * @param string  szValue value to be checked 
     * 
     */
    bool checkFieldinTable(string szTable,string szField,string szValue);

    /**
     * @param int stDate date start in integer format
     * @param int end Date in integer format
     * @param vector <CComponent*> vector of CComponent pointers
     * @param int& place holder for  pErrorCode error code
     * @return  vector <CComponent*>  vector of CComponent pointers
     * 
     * Function is used to obtain recursive events in particular time range 
     * This is a private API not presented to end user
     */
    vector <CComponent *> getRecursiveEventsInRange( int iStartDate,
                int iEndDate, vector < CComponent * >vListEntry,int& pErrorCode);


    /**
     * @param: int iCompType specifies ComponentType (event/todo/journal)  
     * @param: int iChangeType specifies ChangeType (add/mod/delete)
     * @param: int iDate 
     * @param int& place holder for  pErrorCode error code
     * @return: vector of string Ids 
     *  
     * This function returns all the ids required as per the given component type,
     * change type and date 
     */
    vector < string > getChangeIds(int iComponentType, int iChangeType,
                       int iDate,int& pErrorCode);
    
    /**
     * @param CComponent *
     * @param int& place holder for  pErrorCode error code
     * @return boolean value to indicate success or failure
     *
     * Function is used to get Modified time from component-Details table 
     * this is a kind of work -around, better solution would be to move the 
     * domain ModifiedTime in to Component table 
     */
    bool entrySetModifiedTime(CComponent * pEntry,int& pErrorCode);


    /**
     * @param sqlite3_stmt* Sqlite pPreparedStmttement
     * @parm comp* template to CEvent, CTodo, CJournal
     * @param int& place holder for  pErrorCode error code
     * @returns bool true/false
     *
     * This function is used to do the prepared statement for common parameters
     */
    template < class comp > bool addCompDetails(sqlite3_stmt * pStmt, comp * cDet
                    ,int& pErrorCode);
    
    
    /** 
     * @param CEvent* Pointer to CEvent class
     * @param int& place holder for  pErrorCode error code
     * @return bool true or false
     *
     * This function is used to add event related parameters into database
     */
    bool addEventCompDetails(CEvent * pEvent,int& pErrorCode);
    
    /**
     * @param CTodo* Pointer to CTodo class
     * @param int& place holder for  pErrorCode error code
     * @return bool true or false
     *
     * This function is used to add todo related parameters into database
     */
    bool addTodoCompDetails(CTodo * ptrTodo,int& pErrorCode);

    /**
     * @param CJournal* Pointer to CJournal class
     * @param int& place holder for  pErrorCode error code
     * @return bool true or false
     *
     * This function is used to add journal related parameters into database
     */
    bool addJournalCompDetails(CJournal * ptrJournal,int& pErrorCode);

    /**
     * @param ACTION Enumerated data type
     * @param CComponent pointer
     * @param Calendar id
     * @param int& place holder for  pErrorCode error code
     * @return TRUE/FALSE
     * 
     * Function used to manage the cookies
     */
    bool manageCookie(const ACTION iAction, CComponent * pcomp,int& pErrorCode);
    /**
     * @param ptrAlarm pointer to Alarm
     * @param sId - id of component string  
     * @param iAction  type of Action 
     * @param int& place holder for  pErrorCode error code
     * @return bool  result of the operation 
     *
     * This function is used to Add alarm in Data base 
     */
    bool addAlarm(CAlarm * ptrAlarm, ACTION iAction, string sId,int& pErrorCode);
    
    /**
     * @param ptrRecur Pointer to Recurrence
     * @param sId       Event id - string 
     * @param int& place holder for  pErrorCode error code
     * @return TRUE/FALSE
     * 
     * Function to add values in to recurrence table 
     * 
     */
    bool addRecurrence(CRecurrence * ptrRecur,
                       string sId,int& pErrorCode);
    bool modifyRecurrence(CRecurrence * ptrRecur,
		                           string sId,int& pErrorCode);
    /*bool addToCacheTable(string sId, time_t startDate, time_t endDate,
		                                    int &pErrorCode);
    time_t getDateFromTime(time_t startDate);
    bool deleteCacheTable(time_t startDate);
    int getCacheTableCount(time_t inputDate);
    */
    
    
    /**
     * @param vector list of CAttendee pointers
     * @param string represents id of the component 
     * @param  int type can be event/todo or journal 
     * @param  iAction type can be Add / Modify
     * @param int& place holder for  pErrorCode error code
     * @return whether the operation is successfull or not 
     *
     * This function is called when an entry
     * is to be added in attendee table for event todo or journal
     * it adds an event(provided by CEvent pointer) into database
     */

    bool addAttendeeDetails(vector < CAttendee * >vAttendeeList, string sId,
                       int iType,ACTION iAction,int& pErrorCode);
    
    /**
     * @param ptrOrganizer COrganizer pointer
     * @param string sId Id of the component
     * @param  int iType can be event/todo or journal 
     * @param  iAction type can be Add / Modify
     * @param int& place holder for  pErrorCode error code
     * @return boolean value 
     *
     * This function is called when an event entry
     * is to be added in to organizer table .
     * it adds an event(provided by CEvent pointer) into database
     */
    bool addOrganizer(COrganizer * ptrOrganizer, string sId,
                       int iType,ACTION iAction,int& pErrorCode);

    /**
     * @param Template pointer can be CEvent* ,CTodo* ,CJournal* 
     * @param ACTION - enumeration for add/del/modify
     * @param int& place holder for  pErrorCode error code
     * @return int Add component status
     *
     * This function is called when an event entry
     * is to be added.
     * it adds an entry into Component table  
     * */
    template < class ent > int addComponent(ent * pEvent, ACTION iAction,int& pErrorCode);


    string szName; /*!< Calendar Name */

    CalendarColour iColor; /*!< Calendar color */

    bool fReadOnly; /*!< Calendar read only status */

    bool fIsVisible; /*!< Calendar visibility status */

    int iCalId; /*!< Calendar ID - Primary key in database */

    CalendarType iCalType; /*!< Calendar Type */
    
    string szCalendarTune; /*calendar Tune */
    
    string szCalendarVersion ; /*Calendar Version */
    
    SyncProfileType iProfileType ; /* sync profile type*/
                             
};

#endif
