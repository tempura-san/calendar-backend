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
 * Header files used in this file
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <sqlite3.h>
#include <dirent.h>
#include <clockd/libtime.h>
#include "CCalendarDB.h"
#include "Common.h"
#include "CalendarErrors.h"
#include "CCalendarProcs.h"

extern "C" {
#include <sys/vfs.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
}
// Uncomment the following line to have persistent journal.
// From http://www.sqlite.org/pragma.html:
//   "The PERSIST journaling mode prevents the rollback journal from being deleted 
//   at the end of each transaction. Instead, the header of the journal is overwritten with zeros"
//#define PERSIST_JOURNALING_MODE

//! SQL Journal location
// Comment it out to store journal along with database file
// NOTE: Make sence of PERSIST_JOURNALING_MODE is defined only
//#define JOURNAL_LOCATION "/var/tmp"

// number ot times SQL will wait for a second in busy condition
#define SQL_MAX_ATTEMPTS 10

//#include "CUtility.h"
/**
 * static variables used here  
 */
static const char* CALENDAR =  "/.calendar";
static const char* CALENDARDB = "/calendardb";
static const char* CALENDAR_DBUS_LISTENER_SERVICE =  "com.nokia.calendar";
static const char* CALENDAR_DBUS_OBJECT_PATH =  "/com/nokia/calendar";
static const char* CALENDAR_DBUS_LISTENER_SIGNAL = "dbChange";

/* max tables in databse*/
//Aparna
const unsigned int MIN_SPACE_AVAIL = 16 *1024;

/* list of tables */
static const char* tableList[] = {
    CREATE_CAL,
    CREATE_ENTRY,
    CREATE_COMPONENT,
    CREATE_RECURSIVE,
    CREATE_ALARM,
    CREATE_TRASH,
    CREATE_XPROP,
    CREATE_PARAM,
    CREATE_IDMAP,
    CREATE_INSTANCES,
    CREATE_TIMEZONE,
    CREATE_BIRTHDAYS
};

static const int MAX_TABLES = sizeof(tableList) / sizeof(tableList[0]);

/* indexTableList - list of tables on which we 
 * plan to create indexes to retrieve data faster 
 */
static const char* indexTableList[] = {
    INDEX_CALENDAR,
    INDEX_COMPONENT,
    INDEX_COMPONENT_DETAILS,
    INDEX_RECURSIVE,
    INDEX_ALARM,
    INDEX_TRASH,
    INDEX_INSTANCES,
    INDEX_TIMEZONE,
    INDEX_BIRTHDAYS_IDS,
    INDEX_BIRTHDAYS_DATE
};

/* indexes we plan to create on these tables */
static const int MAX_INDEXES = sizeof(indexTableList) / sizeof(indexTableList[0]);

static const char* triggerList[] = {
	DELETE_TRIGGER
};

static const int MAX_TRIGGER = sizeof(triggerList) / sizeof(triggerList[0]);

//just a random integer using for semaphore
static const int SEM_KEY=21107007;

/**
 * Initialising CCalendarDB global pointer to 0
 */
CCalendarDB *CCalendarDB::pCalendarDb = 0;
union semun {
   int val;
   struct semid_ds *buf;
   ushort * array;
} argument;

/**
 * Dummy copy constructor 
 */

CCalendarDB::CCalendarDB(CCalendarDB & calendardb)
{
}

/**
 * Dummy assignment operator
 */
CCalendarDB & CCalendarDB::operator=(CCalendarDB & calendardb)
{
    return *this;
}

/**
* CCalendarDB
* @param None 
* @return None
*  This function is constructor for CCalendarDB class and will be called 
*  when ever object of this class is created.As CCalendarDB class is 
*  defined as singleton class constructor is declared as private member.     
*/
CCalendarDB::CCalendarDB()
{
    //private constructor for singleton class calDB
	pBus = 0;
	pDb = 0;
	CAL_DEBUG_LOG("Inside  Private constructor ");
	
}

void CCalendarDB::sem_v()
{   
    struct sembuf operations[1]; 
    int retval;

    operations[0].sem_num = 0;
    operations[0].sem_op = 1; //increment
    operations[0].sem_flg = SEM_UNDO;
    retval = semop(sem_id, operations, 1);
        CAL_DEBUG_LOG(" Semaphore unlocked %d ",retval );
    if(retval)    {
        CAL_DEBUG_LOG("\n CCalendarDB::sem_v error %d \n",retval);
    }
}

void CCalendarDB::sem_p()
{   
    struct sembuf operations[1]; 
    int retval;

    operations[0].sem_num = 0;
    operations[0].sem_op = -1; // decrement
    operations[0].sem_flg = SEM_UNDO;
    retval = semop(sem_id, operations, 1);
    CAL_DEBUG_LOG(" Semaphore locked , %d ",retval );

    if(retval)    {
      CAL_DEBUG_LOG("\n CCalendarDB::sem_v error %d \n",retval);
    }
}

/**
 * Instance
 * @param None
 * @return CCalendarDB*
 * This function is used to create instance of CCalendarDB class, since CCalendarDB
 * is declared as singleton class-(constructor is declared as private ) 
 *  when ever object of this class needs to be created Instance is to be called.    
 */
CCalendarDB *CCalendarDB::Instance()
{
    if (pCalendarDb == 0) {
    pCalendarDb = new CCalendarDB;
    pCalendarDb->initDB();
    }
    if(pCalendarDb->getDb() == 0) {
	    pCalendarDb->initDB();
    }
    return pCalendarDb;
}

void CCalendarDB::InstanceDestroy()
{
	if (pCalendarDb) {
		delete pCalendarDb;
		pCalendarDb = 0;
	}
}

/**
 * This is the default destructor for CCalendarDB class
 */
CCalendarDB::~CCalendarDB()
{
	CAL_DEBUG_LOG("Inside Destructor for CCalendarDB");
}


bool CCalendarDB::initializeDBus()
{

    DBusError error;

    /* Get a connection to the session bus */
    dbus_error_init(&error);
    pBus = dbus_bus_get(DBUS_BUS_SESSION, &error);

    if (!pBus) {
    CAL_DEBUG_LOG("Failed to connect to DBUS daemon %s\n",
              error.message);
    return false;
    }

    return true;
}

/**
 * sendDBusMessage
 * Send the DBusMessage over the session bus
 *
 * @param string Message to send
 *
 * @return TRUE/FALSE
 */
bool CCalendarDB::sendDBusMessage(string szSendMessage)
{
    DBusMessage *dbusMessage = 0;
    //DBusError error;
    bool ret;

    if (pBus == 0) {
    ret = initializeDBus();
    if (!ret) {
       CAL_ERROR_LOG("Failed to connect dbus\n");
        return false;
    }
    }

    dbusMessage = dbus_message_new_signal(CALENDAR_DBUS_OBJECT_PATH,
                      CALENDAR_DBUS_LISTENER_SERVICE,
                      CALENDAR_DBUS_LISTENER_SIGNAL);
    CMulticalendar *pMc = CMulticalendar::MCInstance();
//    CUtility *pUt = CUtility::Instance();

    const char *dbusMsg = szSendMessage.c_str();
    const char *appName = (pMc->getApplicationName()).c_str();
    //const char *appName = (pUt->getApplicationName()).c_str();

   CAL_DEBUG_LOG("Message is %s\n", dbusMsg);

    dbus_message_append_args(dbusMessage, DBUS_TYPE_STRING, &dbusMsg,
                 DBUS_TYPE_STRING, &appName,
                 DBUS_TYPE_INVALID);

    /* Send signal */
    dbus_connection_send(pBus, dbusMessage, 0);

    /* Unref message */
    dbus_message_unref(dbusMessage);
   CAL_DEBUG_LOG("Dbus message sent\n");

    return true;
}


int busy_handler(void* pointer, int attempt)
{
    CAL_ERROR_LOG("SQL is busy (attempt %d)", attempt);
    if(attempt >= SQL_MAX_ATTEMPTS) {
        CAL_ERROR_LOG("SQL is busy - aborting the operation");
        return 0;
    }
    sleep(1);
    return 1;
}


/**
 * initDB 
 * @param None
 * @return int database intialization status
 * This r/share/applications/tsclient.desktop' unction Creates database file in ~/.calenadar/calenadrdb if it is not present.
 * If DB file already exists then it opens the file for DB transactions.
 * Also responsible for creation of all DB tables if they are not present in DB.  
 */
int CCalendarDB::initDB()
{

    int iRet = -1;

    string szCaldb;
    int iCount = 0;

    // init semaphores
    sem_id = semget(SEM_KEY, 1, 0666 | IPC_CREAT|  IPC_EXCL);
    if (sem_id != -1) { // got the semaphore
        /* Initialize the semaphore. */
        argument.val = 1;  // initial value=1 - semaphore is enabled
        if(semctl(sem_id, 0, SETVAL, argument) < 0) {
             CAL_DEBUG_LOG("Cannot set semaphore value.\n");
             return NULLID;
        }
        else {
            CAL_DEBUG_LOG("Semaphore %d initialized vith value %d.\n",
                sem_id, semctl(sem_id, 0, GETVAL) );
        }
    }
    else{ // unable to create
        if(errno == EEXIST) { // exists
            if ((sem_id = semget(SEM_KEY,0, 0)) == -1) { // unable to get
                CAL_DEBUG_LOG("Cannot get semaphore: error %d", errno); 
                 return NULLID;
            }
            else { // got it
                CAL_DEBUG_LOG("Semaphore %d  with value %d opened by second process", 
                    sem_id, semctl(sem_id, 0, GETVAL) );
            }
        }
    }


    /*Initialise the time API */
    //Register for device time/time-zone change notification
    time_get_synced();

    const char *home_env = (const char *) getenv("HOME");

    if (home_env != 0 && strcmp(home_env, "/root") != 0)
    {
        szCaldb = home_env;
    }
    else
    {
        CAL_ERROR_LOG("HOME is %s. Fallback to /home/$USER", home_env ? home_env : "not set");

        string user;

        const char *user_env = (const char *)getenv("USER");

        if (user_env != 0 && strcmp(user_env, "root") != 0)
        {
            user = user_env;
        }
        else
        {
            CAL_ERROR_LOG("USER is %s. Fallback to 'user'", user_env ? user_env : "not set");
            user = "user";
        }

        szCaldb = "/home/";
        szCaldb.append(user);
    }

    szCaldb.append(CALENDAR);

    //mkdir(szCaldb.c_str(), S_IRWXU);
    mkdir(szCaldb.c_str(),0777);

    szCaldb.append(CALENDARDB);
    CAL_DEBUG_LOG("Calendar DB is '%s'", szCaldb.c_str());


    // Validate database file
    sem_p();
    if (!validateDbFile(szCaldb))
    {
        // Store corrupted file as backup
        moveToBackup(szCaldb);
    }
    sem_v();

    iRet = sqlite3_open(szCaldb.c_str(), &pDb);
   
    sqlite3_busy_handler(pDb, busy_handler, NULL);

    if (iRet) {
    CAL_DEBUG_LOG("Can't open database: %s", sqlite3_errmsg(pDb));
    sqlite3_close(pDb);
    return NULLID;
    } else {
    chmod(szCaldb.c_str(), S_IRUSR|S_IWUSR);
    CAL_DEBUG_LOG("DB is created");
    }

    /* first create all the table if they donot exist, 
     * for each iteration one of the table is created 
     */
    for (iCount = 0; iCount< MAX_TABLES;iCount++)
    {
        iRet = execSQL(tableList[iCount]);
        if (iRet != SQLITE_OK) {
            return iRet;
        }
    }


    /*After creating all tables we have to create index 
     * on frequently used columns
     */
    for (iCount = 0; iCount< MAX_INDEXES;iCount++) {
        iRet = execSQL(indexTableList[iCount]);
        if (iRet != SQLITE_OK) {
            return iRet;
        }
    }
    /* Create triggers
    *
    */

    for (iCount = 0; iCount< MAX_TRIGGER;iCount++){
        iRet = execSQL(triggerList[iCount]);
        if (iRet != SQLITE_OK) {
            return iRet;
        }
    }

    execSQL("PRAGMA cache_size=100");
    execSQL("PRAGMA synchronous=off");
    execSQL("PRAGMA journal_mode=memory");

#ifdef PERSIST_JOURNALING_MODE
    execSQL("PRAGMA journal_mode = PERSIST");

#ifdef JOURNAL_LOCATION
    string szJournalLink(szCaldb + "-journal");
    string szJournalFile(string(JOURNAL_LOCATION) + string(CALENDARDB) + "-journal");

    sem_p();

    // Create journal file
    unlink(szJournalFile.c_str());

    int fd = open(szJournalFile.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0770);

    if (fd > 0)
    {
        CAL_DEBUG_LOG("Created journal file '%s", szJournalFile.c_str());
        close(fd);

        unlink(szJournalLink.c_str());

        if (symlink(szJournalFile.c_str(), szJournalLink.c_str()) == 0)
        {
            CAL_DEBUG_LOG("Created a symlink %s->%s", szJournalFile.c_str(), szJournalLink.c_str());
        }
        else
        {
            CAL_DEBUG_LOG("Failed to create symlynk %s->%s", szJournalFile.c_str(), szJournalLink.c_str());
        }
    }
    else
    {
        CAL_DEBUG_LOG("Failed to create journal file '%s", szJournalFile.c_str());
    }
    sem_v();

#endif //#ifdef JOURNAL_LOCATION

#endif //#ifdef PERSIST_JOURNALING_MODE
	
	// create the default calendar
    int iSqliteError;
    char* pQuery = sqlite3_mprintf(SELECT_TAB,CALENDARS_TABLE);
    QueryResult *pQr = getRecords(pQuery,iSqliteError);
    sqlite3_free(pQuery);

    if (pQr == 0) {
        execSQL(CREATE_DEFAULT_CALENDAR);
        execSQL(CREATE_PRIVATE_CALENDAR);
    }
    else {
        sqlite3_free_table(pQr->pResult);
        delete pQr;
    }


    if (!checkTimezoneTable())
    {
        int error;
        fillTimezoneTable(error);
    }

    CCalendarProcs procs(this);
    procs.initBDay();

    return iRet;
}

/**
 * getDb
 * @param None
 * @return sqlite3* Pointer to DB
 *  This function returns DB pointer( here sqlite3*) 
 */

sqlite3 *CCalendarDB::getDb()
{
    return pDb;
}

/**
 * sqliteErrorMapper
 * @param iSqliteError - error obtained from sqlite 
 * @param pErrorCode - error message thrown to external world
 * @return void 
 * 
 * Function to Map sqlite error to external error 
 */
void CCalendarDB::sqliteErrorMapper(int iSqliteError, int& pErrorCode )
{

/****************************************************************************************
 * ********************** Error codes supported by sqlite3*******************************/
/* #define SQLITE_OK           0   * Successful result 					*/
/* #define SQLITE_ERROR        1   * SQL error or missing database 			*/
/* #define SQLITE_INTERNAL     2   * Internal logic error in SQLite 			*/
/* #define SQLITE_PERM         3   * Access permission denied 				*/
/* #define SQLITE_ABORT        4   * Callback routine requested an abort 		*/
/* #define SQLITE_BUSY         5   * The database file is locked 			*/
/* #define SQLITE_LOCKED       6   * A table in the database is locked 			*/
/* #define SQLITE_NOMEM        7   * A malloc() failed 					*/
/* #define SQLITE_READONLY     8   * Attempt to write a readonly database 		*/
/* #define SQLITE_INTERRUPT    9   * Operation terminated by sqlite3_interrupt()	*/
/* #define SQLITE_IOERR       10   * Some kind of disk I/O error occurred 		*/
/* #define SQLITE_CORRUPT     11   * The database disk image is malformed 		*/
/* #define SQLITE_NOTFOUND    12   * NOT USED. Table or record not found 		*/
/* #define SQLITE_FULL        13   * Insertion failed because database is full 		*/
/* #define SQLITE_CANTOPEN    14   * Unable to open the database file 			*/
/* #define SQLITE_PROTOCOL    15   * NOT USED. Database lock protocol error 		*/
/* #define SQLITE_EMPTY       16   * Database is empty 					*/
/* #define SQLITE_SCHEMA      17   * The database schema changed 			*/
/* #define SQLITE_TOOBIG      18   * String or BLOB exceeds size limit 			*/
/* #define SQLITE_CONSTRAINT  19   * Abort due to constraint violation 			*/
/* #define SQLITE_MISMATCH    20   * Data type mismatch 				*/
/* #define SQLITE_MISUSE      21   * Library used incorrectly				*/
/* #define SQLITE_NOLFS       22   * Uses OS features not supported on host 		*/
/* #define SQLITE_AUTH        23   * Authorization denied 				*/
/* #define SQLITE_FORMAT      24   * Auxiliary database format error 			*/
/* #define SQLITE_RANGE       25   * 2nd parameter to sqlite3_bind out of range		*/
/* #define SQLITE_NOTADB      26   * File opened that is not a database file 		*/
/* #define SQLITE_ROW         100  * sqlite3_step() has another row ready 		*/
/* #define SQLITE_DONE        101  * sqlite3_step() has finished executing      	*/
/* ***********************end-of-error-codes*********************************************/
  
	/* in future this can be extended to 
	 * support error codes described above 
	 * in case the need arises
	 */
	CAL_DEBUG_LOG ("The sqlite error we got is %d",iSqliteError);
	switch (iSqliteError){
		case SQLITE_NOTFOUND:
			pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
			break;
		case SQLITE_OK:
			pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
			break;
		case SQLITE_DONE:
			pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
			break ;
		case SQLITE_BUSY:
			pErrorCode = CALENDAR_DB_LOCKED;
			break;
		case SQLITE_FULL:
			pErrorCode = CALENDAR_DB_FULL;
			break;
		case SQLITE_SCHEMA:
			pErrorCode = CALENDAR_SCHEMA_CHANGED;
			break;
		default :
			pErrorCode = CALENDAR_DATABASE_ERROR;
			break;
	}
	if (pErrorCode != CALENDAR_OPERATION_SUCCESSFUL)
	{
		struct statfs file_statistics ;
		
		string szCaldb;
		szCaldb.append((char *) getenv("HOME"));
		szCaldb.append(CALENDAR);
    		memset (&file_statistics, 0, sizeof (struct statfs));

		if(statfs(szCaldb.c_str(), &file_statistics) == 0) {
			if ( file_statistics.f_bavail * file_statistics.f_bsize < MIN_SPACE_AVAIL){

				CAL_DEBUG_LOG("NO memory blocks available in MMC 1 ");
				pErrorCode =CALENDAR_DISK_FULL;
			}
		}
	}
}

/**
 * insertRows
 * @param preparedStmt: Pointer to prepared statement 
 * @return (int) Id of the inserted Row (so that functions at higher levels can make a note of ID added)
 *  This function returns ID of the new row inserted.
 *  Function is used to add new rows in all tables.(EVETN ,TODO and JOURNAL)    
 */


int CCalendarDB::insertRows(sqlite3_stmt * pPreparedStmt,int& iSqliteError)
{
    int iRet = -1;
    int iId = -1;
    CAL_DEBUG_LOG("insert Rows (%p,%d) is called",pPreparedStmt,iSqliteError);

    if (pPreparedStmt == 0)
    return NULLID;

    iRet = sqlite3_step(pPreparedStmt);
    iSqliteError = iRet;
    iId = sqlite3_last_insert_rowid(pDb);
    CAL_DEBUG_LOG("insert returns %d code and %d id\n",iRet,iId);

    sqlite3_reset(pPreparedStmt);
    if (iRet != SQLITE_DONE) {
    sqlite3_finalize(pPreparedStmt);
    return NULLID;
    }
    CAL_DEBUG_LOG("insert done\n");
    sqlite3_finalize(pPreparedStmt);
    return iId;
}

/**
 * updateDB
 * @param (preparedStmt) Pointer to prepared statement 
 * @return (bool) status of the DB transaction
 * Function is used to modify rows in all tables.(EVETN ,TODO and JOURNAL)    
 */


bool CCalendarDB::updateDB(sqlite3_stmt * pPreparedStmt,int& iSqliteError)
{
    int iRet = -1;
    CAL_DEBUG_LOG("Prepared statement is %p", pPreparedStmt);
    if (pPreparedStmt == 0)
    return false;

    iRet = sqlite3_step(pPreparedStmt);
    iSqliteError = iRet;
    CAL_DEBUG_LOG("Return value from commit  %d\n", iRet);
    sqlite3_reset(pPreparedStmt);
    if (iRet != SQLITE_DONE) {
    sqlite3_finalize(pPreparedStmt);
    return false;
    }

    sqlite3_finalize(pPreparedStmt);
    return true;
}

/**
 * rollbackDB
 * @param None
 * @return (bool) status of the rollback operation
 * Function is used to rollback in case of inconsistency in DB transaction?    
 */
bool CCalendarDB::rollbackDB()
{
    int iRet = -1;
    char *szErrMsg = 0;

    if (!sqlite3_get_autocommit(pDb)) {
	    iRet = sqlite3_exec(pDb, ROLLBACK, 0, 0, &szErrMsg);
	    if (szErrMsg) {
		    CAL_ERROR_LOG("SQL error: %s", szErrMsg);
		    sqlite3_free(szErrMsg);
	    }
	    if (iRet != SQLITE_OK){
		    /* unlocking the semaphore  in failure scenario*/
		    sem_v();
		    return false;
	    }
    }
    CAL_DEBUG_LOG("Autocommit enabled \n");
    /* unlocking the semaphore  in rollback scenario*/	
    sem_v();
    return true;
}

/**
 * commitDB
 * @param None
 * @return (bool) status of the commit operation
 * Function is used to commit DB transaction?    
 */
bool CCalendarDB::commitDB(string szMessage)
{
    int iRet = -1;
    char *szErrMsg = 0;
    bool ret;


    if (!sqlite3_get_autocommit(pDb)) {
	    iRet = sqlite3_exec(pDb, COMMIT, 0, 0, &szErrMsg);
	    if (szErrMsg) {
		    CAL_ERROR_LOG("SQL error: %s\n", szErrMsg);
		    sqlite3_free(szErrMsg);
	    }

	    if (iRet != SQLITE_OK){
		    /* before returning do a roll back */
		    this -> rollbackDB(); 
		    return false;
	    }


    /* Broadcast the DBUS signal with the above information */

    if (!szMessage.empty()) {
        ret = sendDBusMessage(szMessage);
    }
	sem_v();
    return true;
    }

    CAL_DEBUG_LOG("Autocommit is Enabled\n");
    return false;
}

 /**
   * @param: none
   * @return: int SUCCESS/FAILURE
   * 
   * Function to set the autocommit off in sqlite database.
   */
int CCalendarDB::setAutocommitOff()
{
	
    sem_p();
    /* it is always good practice to 
     * rollback any unfinished transaction 
     * start a new transaction */
    if (!sqlite3_get_autocommit(pDb)) {
    	rollbackDB();
    	return FAILURE;
    }
    /* if DB responds with sqlite Busy 
     * then keep on retrying with a span 
     * of 200 ms, the default SQLITE_BUSY 
     * time out is adjusted to be 200 milli seconds
     * total time transaction waits before it aquires 
     * a lock in worst case is 100 * 200  ms ie 20 seconds 
     * Which should be sufficient in most cases 
     */
    int iRetryNumber = 0;
    bool loopexit = false;
    int iResult = -1;
    char *szErrMsg = 0;

    do {
        iResult = sqlite3_exec(pDb, "BEGIN IMMEDIATE", 0, 0, &szErrMsg);
        if (szErrMsg) {
          CAL_ERROR_LOG("SQL error: %s", szErrMsg);
          sqlite3_free(szErrMsg);
             }
        if (iResult == SQLITE_BUSY)
         iRetryNumber ++ ;
        else
         loopexit = true;
    }while((iRetryNumber < 100 ) && !loopexit);    
    
    return SUCCESS;
}
/**
 * closeDB
 * @param None
 * @return (bool)  status of the DB close 
 * Function is used to close DB file.    
 */

bool CCalendarDB::closeDB()
{
    int iRet = -1;
    CAL_DEBUG_LOG("Close database\n");
    iRet = sqlite3_close(pDb);
    if(SQLITE_BUSY == iRet) {
	sqlite3_stmt *pStmt = 0;
	/* closing any prepared statements that 
	 * might be left unfinalized. 
	 * Sqlites prepared  statements will be finalized by it , if the 
	 * error code is SQLITE_BUSY it means our own prepared 
	 * statement might be left unfinalizaed so finalizing any such 
	 * prepared statements. 
	 */
	while ((pStmt = sqlite3_next_stmt(pDb, 0)) != 0) 
		sqlite3_finalize(pStmt);

	if (SQLITE_OK == sqlite3_close(pDb)){
		pDb = 0;
		return true;
       	}

	}
	else if(SQLITE_OK == iRet)
	{
		pDb = 0;
		return true;
	} 
	else
		/* donot do anything here */	
			;
	
	CAL_ERROR_LOG("Error in closing DB,error code is %d",iRet);

	return false;

}

/**
 * getRecords
 * @param (query) sql statement in form of query.
 * @return (QueryResult) result of query is obtained in QueryResult structure 
 * Function is used to get result from a query.
 * 
 */
QueryResult *CCalendarDB::getRecords(char *pQuery, int &iSqliteError)
{
    int iRet = -1;
    char *pErr_msg = 0;
    QueryResult *pQr = 0;

    if (pQuery == 0)
    return 0;

    pQr = (QueryResult *) new QueryResult();
    pQr->pResult = 0;

    iRet =
    sqlite3_get_table(pDb, pQuery, &pQr->pResult, &pQr->iRow,
              &pQr->iColumn, &pErr_msg);
    if (pErr_msg) {
        CAL_ERROR_LOG("SQL error : %s", pErr_msg);
        sqlite3_free(pErr_msg);
    }
    iSqliteError = iRet;

    if (((pQr->iRow == 0) && (pQr->iColumn == 0)) || (pQr->pResult ==0) ) {
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return 0;
    }
    return pQr;
}

/**
 * execSQL
 * @param (pQquery) sql statement in form of query.
 * @return SQL error code
 * Function can be used in general SQL statements execution, i.e. for table creation
 */

int CCalendarDB::execSQL(const char *pQuery)
{
    char *pErr_msg = 0;
    CAL_DEBUG_LOG("CCalendarDB::execSQL( %s )",  pQuery);

    if(!pQuery)
        return SQLITE_OK;

    sem_p();

    int iRet = sqlite3_exec(pCalendarDb->getDb(), pQuery, 0, 0, &pErr_msg);

    if (pErr_msg) {
       CAL_ERROR_LOG("SQL error during executing : %s",  pErr_msg);
       sqlite3_free(pErr_msg);
    }
    sem_v();


     return iRet;
 }

bool CCalendarDB::validateDbFile(const std::string& szDbFilename)
{
    bool valid = FALSE;

    int sql_error;
    char *pErr_msg = 0;
    sqlite3 * db;

    // try to open
    sql_error = sqlite3_open(szDbFilename.c_str(), &db);

    CAL_DEBUG_LOG("CHECKDB: Database '%s' file is opened with code = %d", szDbFilename.c_str(), sql_error);

    switch (sql_error) {
        case SQLITE_CANTOPEN:
        case SQLITE_NOTADB:
        case SQLITE_CORRUPT:
            CAL_ERROR_LOG("CHECKDB: Failed to open batabase %s (error=%d)", szDbFilename.c_str(), sql_error);
            db = 0;
            break;
    }

    if (db) {
        // try to execute integrity check pragma

        QueryResult *pQr = new QueryResult();

        sql_error = sqlite3_get_table(db, "PRAGMA integrity_check", 
                        &pQr->pResult, &pQr->iRow, &pQr->iColumn, &pErr_msg);

        if (pErr_msg) {
            CAL_ERROR_LOG("SQL error : %s", pErr_msg);
            sqlite3_free(pErr_msg);
        }
        else if (sql_error != SQLITE_OK) {
            CAL_ERROR_LOG("SQL error : %d", sql_error);
        }
        else if (((pQr->iRow == 0) && (pQr->iColumn == 0)) || (pQr->pResult == 0) ) {
            CAL_ERROR_LOG("SQL error : empty table");
        }
        else {
            string ok = "ok";
            // here is the main "if"
            if( (pQr->iRow == 1) &&  (ok == pQr->pResult[1]) ) {
                // just one row with "ok" - everything is fine
                valid = TRUE;
                CAL_DEBUG_LOG("CHECKDB:SQL database is valid");
            }
            else {
                // print validation error codes
                for(int i=0; i<=pQr->iRow; i++)
                    CAL_ERROR_LOG("%s", pQr->pResult[i] );
            }
        }

        // clean up
        sqlite3_free_table(pQr->pResult);
        delete pQr;

        // now we need to close the database
        sql_error = sqlite3_close(db);
    }

    if(!valid)  {
        CAL_ERROR_LOG("CHECKDB:SQL database is corrupted");
    }

    return valid;
};

bool CCalendarDB::moveToBackup(const std::string& szDbFilename)
{
    std::string szBackFilename;

    time_t now;
    time(&now);

    struct tm *now_tm  = localtime(&now);

    if (now) {
        std::stringstream s;

        s << szDbFilename << "-backup-"  \
            << now_tm->tm_year + 1900 << "-" \
            << std::setw(2) << std::setfill('0') << now_tm->tm_mon+1 << "-" \
            << std::setw(2) << std::setfill('0') << now_tm->tm_mday << "_" \
            << std::setw(2) << std::setfill('0') << now_tm->tm_hour << ":" \
            << std::setw(2) << std::setfill('0') << now_tm->tm_min << ":" \
            << std::setw(2) << std::setfill('0') << now_tm->tm_min;

        szBackFilename = s.str();
    } else {
        std::string szBackFilename = szDbFilename + "-backup";
    }

    int err;

    if ((err = rename(szDbFilename.c_str(), szBackFilename.c_str())) != 0) {
        CAL_ERROR_LOG("Failed to rename %s to %s: error=%d", 
                       szDbFilename.c_str(), 
                       szBackFilename.c_str(),
                       err);
    } else {
        CAL_DEBUG_LOG("'%s' is successfully renamed to '%s'",
                       szDbFilename.c_str(),
                       szBackFilename.c_str());
    }

    return (err == 0);
};

bool CCalendarDB::checkTimezoneTable()
{
    bool retval;
    int error;

    QueryResult *pResult = getRecords("SELECT Location FROM TIMEZONE", error);

    if (error == SQLITE_OK && 
        pResult != 0 &&
        pResult->iRow > 0)
    {
        CAL_DEBUG_LOG("TIMEZONE table have some data");
        retval = true;
    }
    else
    {
        CAL_DEBUG_LOG("TIMEZONE is not present or have no data(err=%d, result=%p, %d rows)", 
                      error,
                      pResult,
                      pResult ? pResult->iRow : 0);
        retval = false;
    }

    if (pResult && pResult->pResult)
    {
        sqlite3_free_table(pResult->pResult);
    }

    delete pResult;

    return retval;
}

bool CCalendarDB::fillTimezoneTable(int & iSqliteError)
{
    icalarray *zones;
    icaltimezone *zone;
    icalcomponent *component;
    icalcomponent *comp =0 ;
    icalcomponent *comp2 = 0;
    icalproperty *prop;
    struct icaltimetype dtstartStd;
    struct icaltimetype dtstartDst;
    struct icalrecurrencetype rruleStd, rruleDst;
    // struct icaldatetimeperiodtype rdateStd, rdateDst;
    int  prev_offsetStd = 0;
    // int offsetStd = 0;
    int  prev_offsetDst = 0;
    // int offsetDst = 0;

    int dstflag=0;
    // time_t rawtime;
    time_t daylightStart = 0;
    time_t stdStart = 0; 
    // struct tm *time_stdate;
    // struct tm *time_rdate;
    // char* szQuery = 0;
    // char* szErr_msg = 0;
    // int iRet = 0;
    string Loc;
    bool retDb = false;

    // purge old timezone data
    execSQL("delete from timezone");

    zones = icaltimezone_get_builtin_timezones();
    /* get all the timezones from libical and iterate
    */

    setAutocommitOff();

    for (unsigned i = 0; i < zones->num_elements; i++) {
        string ruleStd;
        string ruleDst;
        string location;
        string tzid ;
        string tznameStd;
        string tznameDst;
        ruleStd.clear();
        ruleDst.clear();
        location.clear();
        tzid.clear();
        tznameStd.clear();
        tznameStd.clear();

        bool flag = false;
        zone = (icaltimezone *)icalarray_element_at(zones, i);
        location = (char*)icaltimezone_get_location(zone);
        tzid = (char*)icaltimezone_get_tzid(zone);
        component = icaltimezone_get_component(zone);
        comp = icalcomponent_get_first_component (component, ICAL_XSTANDARD_COMPONENT);

        stdStart = 0 ;
        prev_offsetStd =0 ;
        daylightStart =0 ;
        prev_offsetDst = 0;
        icaltimezone* pTz = icaltimezone_get_builtin_timezone("UTC");

        if (!pTz) {
            CAL_ERROR_LOG("time zone is incorrect %s", location.c_str());
        }

        if (comp) {
            prop = icalcomponent_get_first_property (comp, ICAL_ANY_PROPERTY);
            while (prop) {
                switch (icalproperty_isa (prop)) {
                    case ICAL_TZNAME_PROPERTY:
                        tznameStd = (char*)icalproperty_get_tzname (prop);
                        break;

                    case ICAL_DTSTART_PROPERTY:
                        dtstartStd = icalproperty_get_dtstart (prop);
                        stdStart = icaltime_as_timet_with_zone(dtstartStd, pTz);
                        break;

                    case ICAL_TZOFFSETTO_PROPERTY:
                        prev_offsetStd = icalproperty_get_tzoffsetto (prop);
                        break;

                    case ICAL_RRULE_PROPERTY:
                        rruleStd = icalproperty_get_rrule (prop);
                        ruleStd=icalrecurrencetype_as_string(&rruleStd);
                        break;
                    default:
                        break;
                };

                prop = icalcomponent_get_next_property (comp, ICAL_ANY_PROPERTY);
            }
        }

        comp2 = icalcomponent_get_next_component (component, ICAL_XDAYLIGHT_COMPONENT);

        if (comp2) {
            flag = true;
            prop = icalcomponent_get_first_property (comp2, ICAL_ANY_PROPERTY);
            while (prop) {
                switch (icalproperty_isa (prop)) {
                    case ICAL_TZNAME_PROPERTY:
                        tznameDst = (char*)icalproperty_get_tzname (prop);
                        break;


                    case ICAL_DTSTART_PROPERTY:
                        dtstartDst = icalproperty_get_dtstart (prop);
                        daylightStart = icaltime_as_timet_with_zone(dtstartDst, pTz);
                        break;

                    case ICAL_TZOFFSETTO_PROPERTY:
                        prev_offsetDst = icalproperty_get_tzoffsetto (prop);
                        break;

                    case ICAL_RRULE_PROPERTY:
                        rruleDst = icalproperty_get_rrule (prop);
                        ruleDst = icalrecurrencetype_as_string(&rruleDst);      
                        break;


                    default:
                        break;
                };

                prop = icalcomponent_get_next_property (comp2, ICAL_ANY_PROPERTY);
            }

        }

        dstflag = flag ? 1 : 0;

        retDb = insertTimezoneInfo(tzid,
                                   location,
                                   stdStart,
                                   daylightStart,
                                   prev_offsetStd,
                                   prev_offsetDst,
                                   dstflag,
                                   tznameStd,
                                   ruleStd,
                                   ruleDst);

        ruleStd.clear();
        ruleDst.clear();
        location.clear();
        tzid.clear() ;
        tznameStd.clear();
        tznameDst.clear();
    }

    commitDB("");

    return SUCCESS;
}

bool CCalendarDB::insertTimezoneInfo(string tzId,
                                     string location,
                                     time_t dtstartStd,
                                     time_t dtstartDst,
                                     int offsetStd,
                                     int offsetDst,
                                     int dstflag,
                                     string tzname,
                                     string rruleStd,
                                     string rruleDst)
{
    // int ret = NULLID;

    char* pQuery = 0;
    const char *pTail = 0;
    sqlite3_stmt *pPreparedStmt = 0;
    int iCol = 1;
    int iSqliteError = 0;
    int pErrorCode;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    CAL_DEBUG_LOG("Inserting timezone %s", location.c_str());

    pQuery = sqlite3_mprintf(INSERT_TIMEZONE, TIMEZONE_FIELD_LOCATION, TIMEZONE_FIELD_TZID,
            TIMEZONE_FIELD_DTSTSTD, TIMEZONE_FIELD_DTSTDST, TIMEZONE_FIELD_TZOFFSTD, TIMEZONE_FIELD_TZOFFDST,
            TIMEZONE_FIELD_RRULESTD, TIMEZONE_FIELD_RRULEDST, TIMEZONE_FIELD_TZNAME,
            TIMEZONE_FIELD_DSTFLAG);

    ASSERTION(pQuery);
    sqlite3_prepare(getDb(), pQuery, strlen(pQuery),
            &pPreparedStmt, &pTail);
    sqlite3_free(pQuery);
    if (location.length())
        sqlite3_bind_text(pPreparedStmt, iCol++, location.c_str(), location.length(),
            SQLITE_TRANSIENT);
    else
        sqlite3_bind_text(pPreparedStmt, iCol++, NULL, 0, SQLITE_TRANSIENT);

    if (tzId.length())
        sqlite3_bind_text(pPreparedStmt, iCol++, tzId.c_str(), tzId.length(),
            SQLITE_TRANSIENT);
    else
        sqlite3_bind_text(pPreparedStmt, iCol++, NULL, 0, SQLITE_TRANSIENT);

    sqlite3_bind_int(pPreparedStmt, iCol++, (int)dtstartStd);
    sqlite3_bind_int(pPreparedStmt, iCol++, (int)dtstartDst);
    sqlite3_bind_int(pPreparedStmt, iCol++, offsetStd);
    sqlite3_bind_int(pPreparedStmt, iCol++, offsetDst);
    if (rruleStd.length()) {
        sqlite3_bind_text(pPreparedStmt, iCol++, rruleStd.c_str(), rruleStd.length(),
            SQLITE_TRANSIENT);
    } else {
        sqlite3_bind_text(pPreparedStmt, iCol++, NULL, 0, SQLITE_TRANSIENT);
    }

    if (rruleDst.length()) {
        sqlite3_bind_text(pPreparedStmt, iCol++, rruleDst.c_str(), rruleDst.length(),
            SQLITE_TRANSIENT);
    } else {
            sqlite3_bind_text(pPreparedStmt, iCol++, NULL, 0, SQLITE_TRANSIENT);
    }

    if(tzname.length() ) {
        sqlite3_bind_text(pPreparedStmt, iCol++, tzname.c_str(), tzname.length(), 
                    SQLITE_TRANSIENT);
    } else {
        sqlite3_bind_text(pPreparedStmt, iCol++, NULL, 0, SQLITE_TRANSIENT);
    }

    sqlite3_bind_int(pPreparedStmt, iCol++, dstflag);

    insertRows(pPreparedStmt, iSqliteError);
    sqliteErrorMapper(iSqliteError, pErrorCode);

    if (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL){
        CAL_DEBUG_LOG("Insert Success");
        return true;
    }
    CAL_DEBUG_LOG("Insert failed");
    return false;
}
