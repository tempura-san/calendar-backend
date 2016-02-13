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

#ifndef __CALDB_H__
#define __CALDB_H__

/* SQlite header file */
#include <sqlite3.h>

/* Headers for DBUS */
#include <dbus/dbus.h>
#include <string>
using namespace std;
using std::string;

/* Structure to hold the data which was fetched from database and
 * to pass them to the higher level*/
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
 * CCalendarDB class
 * @brief This class contains core API's to interact with SQLite database. It will perform 
 * the query operations in the database
 */
class CCalendarDB {

  public:
     /**
      * @param: none
      * @return: int SUCCESS/FAILURE
      * 
      * Function to set the autocommit off in sqlite database.
      */
    int setAutocommitOff();
    /**
     * initDB 
     * @param None
     * @return: int database intialization status
     * This function Creates database file in ~/.calenadar/calenadrdb if it is not present.
     * If DB file already exists then it opens the file for DB transactions.
     * Also responsible for creation of all DB tables if they are not present in DB.  
     */
    int initDB();

    /**
     * getDb
     * @param: None
     * @return :sqlite3* Pointer to DB
     *  This function returns DB pointer( here sqlite3*) 
     */
    sqlite3 *getDb();

    /**
     * insertRows
     * @param: preparedStmt: Pointer to prepared statement 
     * @param: int& iSqliteError - place holder for error code from sqlite 
     * @return: int : Id of the inserted Row (so that functions at higher levels can make a note of ID added)
     * This function returns ID of the new row inserted.
     * Function is used to add new rows in all tables.(EVETN ,TODO and JOURNAL)     
     */
    int insertRows(sqlite3_stmt* preparedStmt,int& iSqliteError);

    /**
     * updateDB
     * @param: preparedStmt: Pointer to prepared statement 
     * @param: int& iSqliteError - place holder for error code from sqlite 
     * @return: bool : status of the DB transaction
     * Function is used to modify rows in all tables.(EVETN ,TODO and JOURNAL)       
     */
    bool updateDB(sqlite3_stmt * preparedStmt,int& iSqliteError);
    /**
     * sqliteErrorMapper
     * @param iSqliteError - error obtained from sqlite 
     * @param pErrorCode - error message thrown to external world
     * @return void 
     * 
     * Function to Map sqlite error to external error 
     */
    void sqliteErrorMapper(int iSqliteError, int& pErrorCode );


    /**
     * rollbackDB
     * @param: None
     * @return: bool : status of the rollback operation
     * Function is used to rollback in case of inconsistency in DB transaction?      
     */
    bool rollbackDB();

    /**
     * commitDB
     * @param: None
     * @return: bool : status of the commit operation
     * Function is used to commit DB transaction?    
     */
    bool commitDB(string szMessage);

    /**
     * closeDB
     * @param: None
     * @return: bool : status of the DB close 
     * Function is used to close DB file.    
     */
    bool closeDB();

    /**
     * getRecords
     * @param: query: sql statement in form of query.
     * @param: int& reference to error code.
     * @return: QueryResult : result of query is obtained in QueryResult structure 
     * Function is used to get result from a query.
     */
    QueryResult *getRecords(char *pQuery, int& iSqliteError);

    /**
     * Instance
     * @param: None
     * @return: CCalendarDB*
     * This function is used to create instance of CCalendarDB class, since CCalendarDB
     * is declared as singleton class-(constructor is declared as private ) 
     * when ever object of this class needs to be created Instance is to be called. 
     */
    static CCalendarDB *Instance();


    bool sendDBusMessage (string szSendMessage);

    bool initializeDBus ();

    //Destructor
    ~CCalendarDB();
    void InstanceDestroy();

    /**
    * execSQL
    * @param (pQquery) sql statement in form of query.
    * @return SQL error code
    * Function can be used in general SQL statements execution, i.e. for table creation
    */
    int execSQL(const char *pQuery);

  private:

    /**
     * CCalendarDB
     * @param: None 
     * @return: None
     * This function is constructor for CCalendarDB class and will be called 
     * when ever object of this class is created.   
     */
     CCalendarDB();

    //Copy constructor dummy so that 
    //multiple instances are not created
     CCalendarDB(CCalendarDB & calendardb);

    // Overloading for assignment operator
     CCalendarDB & operator=(CCalendarDB & calendardb);

    sqlite3 * pDb; /*!< SQLite database pointer */
    DBusConnection *pBus; /*!< DBus Connection pointer */
    static CCalendarDB *pCalendarDb; /*!<Static CCalendarDB pointer for singleton usage */
    // semaphore increment
    void sem_p(void);
    //semaphore decrement
    void sem_v(void);
    //semaphore id
    int sem_id;

    /**
     * Validate and backup Database file if corrupted
     * @param  szDbFilename filename if SQlite3 databsae storage file to check. 
     * @return TRUE if DB is found alive. FALSE if DB is treared as corrupted
     */
    static bool validateDbFile(const std::string& szDbFilename);

    /**
     * Make a backup copy of Database file
     * @param szDbFilename file to backup
     * @return SUCCESS/FAILURE
     */
    static bool moveToBackup(const std::string& szDbFilename);


    /**
     * Check TIMEZONE table presence
     * @return TRUE if TIMEZONE table exists and have some data
     */
    bool checkTimezoneTable();

    /**
     * Fill TIMEZONE table using build-in timzone information
     * @param iSqliteError
     * @return SUCCESS/FAILURE
     */
    bool fillTimezoneTable(int& iSqliteError);

    /**
     * Add one timezone record into TIMEZONE table
     * @param tzId 
     * @param location 
     * @param dtstartStd 
     * @param dtstartDst 
     * @param offsetStd 
     * @param offsetDst 
     * @param dstflag 
     * @param tzname 
     * @param rruleStd 
     * @param rruleDst 
     * @return SUCCESS/FAILURE
     */
    bool insertTimezoneInfo(string tzId,
                            string location,
                            time_t dtstartStd,
                            time_t dtstartDst,
                            int offsetStd,
                            int offsetDst,
                            int dstflag,
                            string tzname,
                            string rruleStd,
                            string rruleDst);

};

#endif
