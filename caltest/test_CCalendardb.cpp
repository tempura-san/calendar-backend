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

#include "test_CCalendardb.h"
#include "CCalendarDB.h"
#include "CalendarErrors.h"
#include "DbCalls.h"

/* CreateUpdateDBTestCase*/
void CCalendarDB_TS1 :: setUp()
{
    pCdb = CCalendarDB::Instance();
}
void CCalendarDB_TS1 :: tearDown()
{
    pCdb = NULL;
}
void CCalendarDB_TS1 :: test_createUpdateDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;

    pQ = "CREATE TABLE IF NOT EXISTS Cal(CalendarId INTEGER PRIMARY KEY, Name TEXT,Colour TEXT,IsVisible INTEGER,IsReadOnly INTEGER)";

    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute create cal",pCdb->updateDB(pPrepStmt, iSqliteError)==1);

    pQ =  "CREATE TABLE IF NOT EXISTS CompCac(CalendarId INTEGER, Id INTEGER PRIMARY KEY,Flags INTEGER,DateStart INTEGER,DateEnd INTEGER,Summary TEXT,Location TEXT,Description TEXT,Status INTEGER)";

    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute create compcac",pCdb->updateDB(pPrepStmt, iSqliteError)==1);

    pQ = "CREATE TABLE IF NOT EXISTS Comp(Id INTEGER, ComponentType INTEGER)";

    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute create comp",pCdb->updateDB(pPrepStmt, iSqliteError)==1);

}


/* Create Ten Tables Test Case*/
void CCalendarDB_TS1 :: test_createTenTables()
{

    double dBefore,dAfter;
    int iI=1;
    timeval stTim;
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec bef"<<dBefore;
    for(iI=1;iI<=10;iI++)
    {

	pQ=sqlite3_mprintf("CREATE TABLE IF NOT EXISTS Cal%d(CalendarId INTEGER PRIMARY KEY, Name TEXT,Colour TEXT,IsVisible INTEGER,IsReadOnly INTEGER)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);

	pCdb->updateDB(pPrepStmt, iSqliteError);

    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec aft"<<dAfter;


    std::cout<<"\ntime taken to create 10 DB tables\n"<<(dAfter-dBefore)<<"sec";
}

/* Create Hundred Tables Test Case*/
void CCalendarDB_TS1 :: test_createHunTables()
{

    double dBefore,dAfter;
    int iI=1;
    timeval stTim;
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec bef"<<dBefore;
    for(iI=1;iI<=100;iI++)
    {

	pQ=sqlite3_mprintf("CREATE TABLE IF NOT EXISTS Cal%d(CalendarId INTEGER PRIMARY KEY, Name TEXT,Colour TEXT,IsVisible INTEGER,IsReadOnly INTEGER)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->updateDB(pPrepStmt, iSqliteError);
    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec aft"<<dAfter;


    std::cout<<"\ntime taken to create 100 DB tables\n"<<(dAfter-dBefore)<<"sec";
}

/* Create FiveHundred Tables Test Case*/
void CCalendarDB_TS1 :: test_createFiveHunTables()
{

    double dBefore,dAfter;
    int iI=1;
    timeval stTim;
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec bef"<<dBefore;
    for(iI=1;iI<=500;iI++)
    {

	pQ=sqlite3_mprintf("CREATE TABLE IF NOT EXISTS Cal%d(CalendarId INTEGER PRIMARY KEY, Name TEXT,Colour TEXT,IsVisible INTEGER,IsReadOnly INTEGER)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->updateDB(pPrepStmt, iSqliteError);
    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec aft"<<dAfter;

    std::cout<<"\ntime taken to create 500 DB tables\n"<<(dAfter-dBefore)<<"sec";
}

/* Create Thosand  Tables Test Case*/
void CCalendarDB_TS1 :: test_createThouTables()
{

    double dBefore,dAfter;
    int iI=1;
    timeval stTim;
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec bef"<<dBefore;
    for(iI=1;iI<=1000;iI++)
    {	pQ=sqlite3_mprintf("CREATE TABLE IF NOT EXISTS Cal%d(CalendarId INTEGER PRIMARY KEY, Name TEXT,Colour TEXT,IsVisible INTEGER,IsReadOnly INTEGER)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->updateDB(pPrepStmt, iSqliteError);
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec aft"<<dAfter;

    std::cout<<"\ntime taken to create 1000 DB tables\n"<<(dAfter-dBefore)<<"sec";

}


/* Create Five Thousand  Tables Test Case*/
void CCalendarDB_TS1 :: test_createFiveThouTables()
{

    double dBefore,dAfter;
    int iI=1;
    timeval stTim;
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;

    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec bef"<<dBefore;
    for(iI=1;iI<=5000;iI++)
    {	pQ=sqlite3_mprintf("CREATE TABLE IF NOT EXISTS Cal%d(CalendarId INTEGER PRIMARY KEY, Name TEXT,Colour TEXT,IsVisible INTEGER,IsReadOnly INTEGER)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->updateDB(pPrepStmt, iSqliteError);
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec aft"<<dAfter;

    std::cout<<"\ntime taken to create 5000 DB tables\n"<<(dAfter-dBefore)<<"sec";

}

/* Create Ten Thousand  Tables Test Case*/
void CCalendarDB_TS1 :: test_createTenThouTables()
{

    double dBefore,dAfter;
    int iI=1;
    timeval stTim;
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec bef"<<dBefore;
    for(iI=1;iI<=10000;iI++)
    {	pQ=sqlite3_mprintf("CREATE TABLE IF NOT EXISTS Cal%d(CalendarId INTEGER PRIMARY KEY, Name TEXT,Colour TEXT,IsVisible INTEGER,IsReadOnly INTEGER)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->updateDB(pPrepStmt, iSqliteError);
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\nmicrosec aft"<<dAfter;

    std::cout<<"\ntime taken to create 10000 DB tables\n"<<(dAfter-dBefore)<<"sec";

}
/* InsertUpdateDBTestCase*/
void CCalendarDB_TS1 :: test_insertUpdateDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;

    pQ = "insert into Cal values(1,\"cal1\",\"red\",1,1)";
    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute insert into cal",pCdb->updateDB(pPrepStmt, iSqliteError)==1);

    pQ = "insert into Cal values(2,\"cal2\",\"blue\",1,1)";
    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute insert into cal",pCdb->updateDB(pPrepStmt, iSqliteError)==1);

    pQ = "insert into Comp values(1,1)";
    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute insert into comp",pCdb->updateDB(pPrepStmt, iSqliteError)==1);

    pQ = "insert into Comp values(2,2)";
    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute insert into comp",pCdb->updateDB(pPrepStmt, iSqliteError)==1);

    pQ = "insert into CompCac values(2,2,1,\"23456\",\"23456\",\"event1\",\"hyderabad\",\"EVENT CHECKING\",1)";
    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute insert into component",pCdb->updateDB(pPrepStmt, iSqliteError)==1);

    pQ = "insert into CompCac values(1,1,1,\"23456\",\"23456\",\"event1\",\"hyderabad\",\"EVENT CHECKING\",1)";
    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute insert into component",pCdb->updateDB(pPrepStmt, iSqliteError)==1);
}

/* InvalidInsertUpdateDBTestCase*/
void CCalendarDB_TS1 :: test_invalidInsertUpdateDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;

    pQ = "insert into Cal values(3,\"cal3\",\"pink\",1)";
    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute insert into cal",pCdb->updateDB(pPrepStmt, iSqliteError)==0);
}
/* Modify Update DB  Test Case*/
void CCalendarDB_TS1 :: test_modifyUpdateDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    pQ = "update CompCac set Summary=\"event1cal1\" where Id=1";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute modify",pCdb->updateDB(pPrepStmt, iSqliteError)==1);
}


/* Ten Modifications Test Case*/
void CCalendarDB_TS1 :: test_tenmodifications()
{
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter=0;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    for(int iI=1;iI<=10;iI++)
    {
	pQ=sqlite3_mprintf("update Cal1 set Name=\"BLACK\" where CalendarId=%d",iI);

	sqlite3_prepare(pCdb->getDb(), pQ, 100, &pPrepStmt, &pTail);

	pCdb->updateDB(pPrepStmt, iSqliteError);

    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 10 DB tables\n"<<(dAfter-dBefore)<<"sec";
}


/* Hun Modifications Test Case*/
void CCalendarDB_TS1 :: test_hunmodifications()
{
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter=0;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    for(int iI=1;iI<=100;iI++)
    {
	pQ=sqlite3_mprintf("update Cal1 set Name=\"BLACK\" where CalendarId=%d",iI);

	sqlite3_prepare(pCdb->getDb(), pQ, 100, &pPrepStmt, &pTail);

	pCdb->updateDB(pPrepStmt, iSqliteError);

    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 10 DB tables\n"<<(dAfter-dBefore)<<"sec";
}


/* Five Hun Modifications Test Case*/
void CCalendarDB_TS1 :: test_fivehunmodifications()
{
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter=0;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    for(int iI=1;iI<=500;iI++)
    {
	pQ=sqlite3_mprintf("update Cal1 set Name=\"BLACK\" where CalendarId=%d",iI);

	sqlite3_prepare(pCdb->getDb(), pQ, 100, &pPrepStmt, &pTail);

	pCdb->updateDB(pPrepStmt, iSqliteError);

    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 100 DB tables\n"<<(dAfter-dBefore)<<"sec";
}


/*  Thou Modifications Test Case */
void  CCalendarDB_TS1  :: test_thoumodifications()
{
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter=0;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    for(int iI=1;iI<=1000;iI++)
    {
	pQ=sqlite3_mprintf("update Cal1 set Name=\"BLACK\" where CalendarId=%d",iI);

	sqlite3_prepare(pCdb->getDb(), pQ, 100, &pPrepStmt, &pTail);

	pCdb->updateDB(pPrepStmt, iSqliteError);

    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 1000 DB tables\n"<<(dAfter-dBefore)<<"sec";
}

/*  Five Thou Modifications Test Case */
void  CCalendarDB_TS1  :: test_fivethoumodifications()
{
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter=0;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    for(int iI=1;iI<=5000;iI++)
    {
	pQ=sqlite3_mprintf("update Cal1 set Name=\"BLACK\" where CalendarId=%d",iI);

	sqlite3_prepare(pCdb->getDb(), pQ, 100, &pPrepStmt, &pTail);

	pCdb->updateDB(pPrepStmt, iSqliteError);

    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 5000 DB tables\n"<<(dAfter-dBefore)<<"sec";
}

/*  Thou Modifications Test Case */
void  CCalendarDB_TS1 :: test_tenthoumodifications()
{
    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter=0;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    for(int iI=1;iI<=10000;iI++)
    {
	pQ=sqlite3_mprintf("update Cal1 set Name=\"BLACK\" where CalendarId=%d",iI);

	sqlite3_prepare(pCdb->getDb(), pQ, 100, &pPrepStmt, &pTail);

	pCdb->updateDB(pPrepStmt, iSqliteError);

    }
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 10 DB tables\n"<<(dAfter-dBefore)<<"sec";
}
/*7th Modification in DB*/
void CCalendarDB_TS1  :: test_modification7()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    timeval stTim;
    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    pQ = "update Cal1 set Name=\"GREEN\" where CalendarId=7";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    pCdb->updateDB(pPrepStmt, iSqliteError);
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 7th tables\n"<<(dAfter-dBefore)<<"sec";
}


/* 67thModificationTestCase*/
void CCalendarDB_TS1 :: test_modification67()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    timeval stTim;
    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    pQ = "update Cal1 set Name=\"GREEN\" where CalendarId=67";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    pCdb->updateDB(pPrepStmt, iSqliteError);
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 10 DB tables\n"<<(dAfter-dBefore)<<"sec";
}


/* 473rdModificationTestCase*/
void CCalendarDB_TS1 :: test_modification473()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    timeval stTim;
    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    pQ = "update Cal1 set Name=\"GREEN\" where CalendarId=473";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    pCdb->updateDB(pPrepStmt, iSqliteError);
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 10 DB tables\n"<<(dAfter-dBefore)<<"sec";
}


/* 793rdModificationTestCase*/
void CCalendarDB_TS1 :: test_modification793()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    timeval stTim;
    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    pQ = "update Cal1 set Name=\"GREEN\" where CalendarId=793";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    pCdb->updateDB(pPrepStmt, iSqliteError);
    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;
    std::cout<<"\ntime taken to modify 10 DB tables\n"<<(dAfter-dBefore)<<"sec";
}

/* InvalidColModifyUpdateDBTestCase*/
void CCalendarDB_TS1 :: test_invalidColModifyUpdateDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    pQ = "update CompCac set Summary=\"event1cal1\" where Idd=1";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("Error:executed invalid modification",pCdb->updateDB(pPrepStmt, iSqliteError)==0);

}


/* DeleteUpdateDBTestCase*/
void CCalendarDB_TS1 :: test_deleteUpdateDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    pQ = "delete from CompCac where Id=2";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("failed to execute delete",pCdb->updateDB(pPrepStmt, iSqliteError)==1);
}



/* InvalidColDeleteUpdateDBTestCase*/
void CCalendarDB_TS1 :: test_invalidColDeleteUpdateDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    pQ = "delete from CompCac where Idd=1";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    CPPUNIT_ASSERT_MESSAGE("Error: executed delete with invalid value",pCdb->updateDB(pPrepStmt, iSqliteError)==0);
}



/* ValidGetRecordsTestCase*/
void CCalendarDB_TS1 :: test_validGetRecords()
{
    QueryResult *res;
    int iSqliteError;
    res = (QueryResult *) malloc (sizeof(QueryResult));
    res = pCdb->getRecords(const_cast<char *>("select * from CompCac where Id=1"), iSqliteError);

    for(int iI=0;iI<(10*(res->iRow));iI++)
    {
	printf ("result  %s\n", res->pResult[iI]);
    }

    CPPUNIT_ASSERT_MESSAGE("failed: ",!strcmp(res->pResult[9],"1"));
    CPPUNIT_ASSERT_MESSAGE("failed: ",!strcmp(res->pResult[10],"1"));
    CPPUNIT_ASSERT_MESSAGE("failed: ",!(strcmp(res->pResult[11],"1")));
    CPPUNIT_ASSERT_MESSAGE("failed: ",!(strcmp(res->pResult[12],"23456")));
    CPPUNIT_ASSERT_MESSAGE("failed: ",!(strcmp(res->pResult[13],"23456")));
    CPPUNIT_ASSERT_MESSAGE("failed: ",!(strcmp(res->pResult[14],"event1cal1")));
    CPPUNIT_ASSERT_MESSAGE("failed: ",!(strcmp(res->pResult[15],"hyderabad")));
    CPPUNIT_ASSERT_MESSAGE("failed: ",!(strcmp(res->pResult[16],"EVENT CHECKING")));



}

/* TimeForGetRecordsTestCase*/
void CCalendarDB_TS1 :: test_timeForGetRecords()
{
    QueryResult *res;
    int iSqliteError;
    res = (QueryResult *) malloc (sizeof(QueryResult));
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;
    res = pCdb->getRecords(const_cast<char *>("select * from Cal1 where IsVisible=1"), iSqliteError);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to get records\n"<<(dAfter-dBefore)<<"microsec";
    //std::cout<<endl<<res->iRow<<endl;
    //CPPUNIT_ASSERT_MESSAGE("failed:returned an empty iRow ",(res==NULL));	


}

/* InvalidGetRecordsTestCase*/
void CCalendarDB_TS1 :: test_invalidGetRecords()
{
    QueryResult *res;
    int iSqliteError;
    res = (QueryResult *) malloc (sizeof(QueryResult));
    res = pCdb->getRecords(const_cast<char *>("select * from CompCac where Id=3"), iSqliteError);

    CPPUNIT_ASSERT_MESSAGE("failed:returned an empty iRow ",(res==NULL));
}

/* NullGetRecordsTestCase*/
void CCalendarDB_TS1 :: test_nullGetRecords()
{
    QueryResult *res;
    int iSqliteError;
    res = (QueryResult *) malloc (sizeof(QueryResult));

    res = pCdb->getRecords(NULL, iSqliteError);

    CPPUNIT_ASSERT_MESSAGE("failed:  executed an empty query",(res==NULL));
}

/* CommitDBTestCase*/
void CCalendarDB_TS1 :: test_commitDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;

    pQ = "update EVENT set Title=\"commeventcal1\" where Id=2";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    pCdb->updateDB(pPrepStmt, iSqliteError);
    sqlite3_exec (pCdb->getDb(), "begin", NULL, 0,NULL);
    CPPUNIT_ASSERT_MESSAGE("failed to commit DB ",pCdb->commitDB("Some message !!")==1);
}
/* RollbackDBTestCase*/
void CCalendarDB_TS1 :: test_rollbackDB()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;

    pQ = "update ComponentCache set Title=\"rolleventcal1\" where Id=3";
    sqlite3_prepare(pCdb->getDb(),pQ,100,&pPrepStmt,&pTail);
    pCdb->updateDB(pPrepStmt, iSqliteError);
    sqlite3_exec (pCdb->getDb(), "begin", NULL, 0,NULL);

    CPPUNIT_ASSERT_MESSAGE("failed to rolback DB ",pCdb->rollbackDB()==1);
}

/* ValidInsertRowsTestCase*/
void CCalendarDB_TS1 :: test_validInsertRows()
{
    const char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    pQ = "insert into Cal values(3,\"cal3\",\"white\",0,1)";
    sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);

    CPPUNIT_ASSERT_MESSAGE("failed to execute insert iRow",pCdb->insertRows(pPrepStmt, iSqliteError) != -1);
}

/* InvalidInsertRowsTestCase*/
void CCalendarDB_TS1 :: test_invalidInsertRows()
{
    int iSqliteError;
    CPPUNIT_ASSERT_MESSAGE("tried to execute null insert iRow",pCdb->insertRows(NULL, iSqliteError) == -1);
}

/* Insert Ten Rows Test Case*/
void CCalendarDB_TS1  :: test_insertTenRows()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=1;iI<=10;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",1,1)",iI,iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 10 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

/* Insert Hundred Rows Test Case*/
void CCalendarDB_TS1 :: test_insertHunRows()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=1;iI<=100;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",1,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 100 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

/* Insert Five Hundred Rows Test Case*/
void CCalendarDB_TS1  :: test_insertFiveHunRows()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=1;iI<=500;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",1,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 500 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

/* Insert Thousand Rows Test Case*/
void CCalendarDB_TS1  :: test_insertThouRows()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=1;iI<=1000;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",1,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 1000 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}


/* Insert Five Thousand Rows Test Case*/
void CCalendarDB_TS1  :: test_insertFiveThouRows()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=1;iI<=5000;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",1,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 5000 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}


/* Insert Ten Thousand Rows Test Case*/
void CCalendarDB_TS1  :: test_insertTenThouRows()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=1;iI<=10000;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",1,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 10000 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}


/* Insert Ten RowsV Test Case*/
void CCalendarDB_TS1  :: test_insertTenRowsV()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=1;iI<=10;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",0,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 10 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

/* Insert Hundred RowsV Test Case*/
void CCalendarDB_TS1 :: test_insertHunRowsV()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=101;iI<=200;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",0,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 100 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

/* Insert Five Hundred Rows V Test Case*/
void CCalendarDB_TS1  :: test_insertFiveHunRowsV()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=501;iI<=1000;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",0,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 500 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

/* Insert Thousand Rows V Test Case*/
void CCalendarDB_TS1  :: test_insertThouRowsV()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=1001;iI<=2000;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",0,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 5000 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

/* Insert Five Thousand Rows V Test Case*/
void CCalendarDB_TS1  :: test_insertFiveThouRowsV()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=5001;iI<=10000;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",0,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 10000 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

/* Insert Ten Thousand Rows V Test Case*/
void CCalendarDB_TS1  :: test_insertTenThouRowsV()
{

    char *pQ = NULL;
    const char *pTail;
    sqlite3_stmt *pPrepStmt;
    int iSqliteError;
    double dBefore,dAfter;
    int iI=1;
    timeval stTim;


    sqlite3_exec(pCdb->getDb(),"begin",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dBefore=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec bef"<<dBefore;

    for(iI=10001;iI<=20000;iI++)
    {

	pQ=sqlite3_mprintf("insert into Cal1 values(%d,\"GOOGLE\",\"BLUE\",0,1)",iI);
	sqlite3_prepare(pCdb->getDb(),pQ,250,&pPrepStmt,&pTail);
	pCdb->insertRows(pPrepStmt, iSqliteError) ;
    }

    sqlite3_exec(pCdb->getDb(),"commit",NULL,0,NULL);
    gettimeofday(&stTim, NULL);
    dAfter=stTim.tv_sec+(stTim.tv_usec/1000000.0);
    std::cout<<"\n\nmicrosec aft"<<dAfter;

    std::cout<<"\n\ntime taken to insert into 1000 DB tables\n"<<(dAfter-dBefore)<<"microsec";

}

void CCalendarDB_TS1  :: test_sqliteErrorMapper()
{
    int pErrorCode = -1;
    pCdb->sqliteErrorMapper(SQLITE_NOTFOUND, pErrorCode);	
    CPPUNIT_ASSERT_MESSAGE("Error: SQLITE_NOTFOUND", (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL));
    pCdb->sqliteErrorMapper(SQLITE_OK, pErrorCode);	
    CPPUNIT_ASSERT_MESSAGE("Error: SQLITE_OK", (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL));
    pCdb->sqliteErrorMapper(SQLITE_DONE, pErrorCode);	
    CPPUNIT_ASSERT_MESSAGE("Error:SQLITE_DONE", (pErrorCode == CALENDAR_OPERATION_SUCCESSFUL));
    pCdb->sqliteErrorMapper(SQLITE_BUSY, pErrorCode);	
    CPPUNIT_ASSERT_MESSAGE("Error: SQLITE_BUSY", (pErrorCode == CALENDAR_DB_LOCKED));
    pCdb->sqliteErrorMapper(SQLITE_FULL, pErrorCode);	
    CPPUNIT_ASSERT_MESSAGE("Error: SQLITE_FULL", (pErrorCode == CALENDAR_DB_FULL));
    pCdb->sqliteErrorMapper(SQLITE_SCHEMA, pErrorCode);	
    CPPUNIT_ASSERT_MESSAGE("Error: SQLITE_SCHEMA", (pErrorCode == CALENDAR_SCHEMA_CHANGED));
    pCdb->sqliteErrorMapper(1000, pErrorCode);	
    CPPUNIT_ASSERT_MESSAGE("Error: Default", (pErrorCode == CALENDAR_DATABASE_ERROR));
}
void CCalendarDB_TS1 :: test_destroy()
{
    //	delete pCdb;
}

void CCalendarDB_TS1 :: test_sendDBusMessage()
{
    bool ret = pCdb->sendDBusMessage("Hello");
    CPPUNIT_ASSERT_MESSAGE("Error: test_sendDBusMessage", ret==true);
    ret=pCdb->closeDB();
    CPPUNIT_ASSERT_MESSAGE("Error: CloseDB", (ret == true));
}
