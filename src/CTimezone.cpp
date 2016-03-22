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

#include "CTimezone.h"

#include "CCalendarDB.h"
#include "CalendarErrors.h"
#include "CalendarLog.h"
#include "Common.h"
#include "DbCalls.h"
#include <assert.h>
#include <sstream>
#include <vector>

CTimezone::CTimezone():
    StartStd(0), StartDst(0), OffsetStd(0), OffsetDst(0), haveDst(false)
{
};

CTimezone::~CTimezone()
{
};


// Uncomment the following line to log each column value during SQl result parsing
// #define LOG_SQLRESULT_PARSING


#ifdef LOG_SQLRESULT_PARSING

#define GET_TEXT_FROM_SQLRESULT(STR, SQLRESULT)                         \
                CAL_DEBUG_LOG(#STR"=%s", SQLRESULT?SQLRESULT:"NULL");   \
                _GET_TEXT_FROM_SQLRESULT(STR, SQLRESULT)

#define GET_INTEGER_FROM_SQLRESULT(INT, SQLRESULT)                      \
                CAL_DEBUG_LOG(#INT"=%s", SQLRESULT?SQLRESULT:"NULL");   \
                _GET_INTEGER_FROM_SQLRESULT(INT, SQLRESULT)

#else
#define GET_TEXT_FROM_SQLRESULT _GET_TEXT_FROM_SQLRESULT
#define GET_INTEGER_FROM_SQLRESULT _GET_INTEGER_FROM_SQLRESULT
#endif

#define _GET_TEXT_FROM_SQLRESULT(STR, SQLRESULT)             \
                STR = SQLRESULT ? SQLRESULT : ""

#define _GET_INTEGER_FROM_SQLRESULT(INT, SQLRESULT)          \
                INT = SQLRESULT ? atoi(SQLRESULT) : 0;

CTimezone *CTimezone::guessTimeZone(time_t StartStd,
                                    time_t StartDst,
                                    time_t OffsetStd,
                                    time_t OffsetDst,
                                    std::string RuleStd,
                                    std::string RuleDst,
                                    bool haveDst,
                                    int &pErrorCode)
{
    CTimezone *pTimeZone = 0;

    char *pQuery = 0;

    pErrorCode = CALENDAR_SYSTEM_ERROR; // We don't know what will happen. Let be pessimists...

    /*
     * Prepare query
     */
    if(haveDst) {
        // TODO move query to DbCalls.h
        pQuery = sqlite3_mprintf("SELECT * from TIMEZONE WHERE DSTFlag=1 "
                                 "AND DtStartStd=%ld "
                                 "AND DtStartDst=%ld "
                                 "AND TzOffsetStd=%ld "
                                 "AND TzOffsetDst=%ld "
                                 "AND RRuleStd='%s' "
                                 "AND RRuleDst='%s'"
                                 "LIMIT 1 ",
                                 (long)StartStd,
                                 (long)StartDst,
                                 (long)OffsetStd,
                                 (long)OffsetDst,
                                 RuleStd.c_str(),
                                 RuleDst.c_str());
    }
    else {
        // TODO move query to DbCalls.h
        // Query timezone with fixed offset
        pQuery = sqlite3_mprintf("SELECT * from TIMEZONE WHERE DSTFlag=0 AND TzOffsetStd=%ld LIMIT 1", (long)OffsetStd);
    }

    if(!pQuery) {
        CAL_ERROR_LOG("Failed to allocate SQL query");
        return 0;
    }

    CAL_DEBUG_LOG("SQL Query: %s", pQuery);

    pTimeZone = createFromSqlQuery(pQuery, pErrorCode);

    sqlite3_free(pQuery);

    return pTimeZone;
};

std::string CTimezone::toString()
{
    std::stringstream  s;

    s << "Location=" << Location
      << "#ID=" << Id
      << "#StartStd=" << StartStd << " (" << ctime(&StartStd) << ")"
      << "#StartDst=" << StartDst << " (" << ctime(&StartDst) << ")"
      << "#OffsetStd=" << OffsetStd
      << "#OffsetDst=" << OffsetStd
      << "#RuleStd=" << RuleStd
      << "#RuleDst=" << RuleDst
      << "#TzName=" << Name
      << "#DSTFlag" << haveDst;

    return s.str();
}

const std::string &CTimezone::getLocation() const
{
    return Location;
}

const std::string &CTimezone::getId() const
{
    return Id;
}

time_t CTimezone::getStartStd() const
{
    return StartStd;
}

time_t CTimezone::getStartDst() const
{
    return StartDst;
}

time_t CTimezone::getOffsetStd() const
{
    return OffsetStd;
}

time_t CTimezone::getOffsetDst() const
{
    return OffsetDst;
}

const std::string &CTimezone::getRuleStd() const
{
    return RuleStd;
}

const std::string &CTimezone::getRuleDst() const
{
    return RuleDst;
}

const std::string &CTimezone::getName() const
{
    return Name;
}

bool CTimezone::getHaveDst() const
{
    return haveDst;
}


CTimezone *CTimezone::createFromSqlQuery(char *pSqlQuery, int &pErrorCode)
{
    int iSqliteError;

    CTimezone *pTimeZone = 0;

    QueryResult *pQr = 0;
    CCalendarDB *pDb = CCalendarDB::Instance();

    ASSERTION(pSqlQuery);

    CAL_DEBUG_LOG("SQL=%s", pSqlQuery);

    pErrorCode = CALENDAR_SYSTEM_ERROR; // We don't know what will happen. Let be pessimists...

    if(!pDb) {
        pErrorCode = CALENDAR_DATABASE_ERROR;
        CAL_ERROR_LOG("Failed to get DB instance");
        return 0;
    }

    pQr = pDb->getRecords(pSqlQuery, iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError, pErrorCode);

    if(pQr) {
        // Fill run-time data from query
        CAL_DEBUG_LOG("SQL result:  %d row(s), %d column(s), data=%p", pQr->iRow, pQr->iColumn, pQr->pResult);

        if(pQr->iColumn == 10) {  // Check fields number
            char **data = pQr->pResult + pQr->iColumn; // set pointer to first data (e.i. skip column  names)

            pTimeZone = createFromSqlData(data);

            if(pTimeZone) {
                CAL_DEBUG_LOG("Got timezone: %s", pTimeZone->toString().c_str());
                pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
            }
            else {
                CAL_ERROR_LOG("Failed to allocate CTimezone query");
                pErrorCode = CALENDAR_SYSTEM_ERROR;
            }
        }
        else {
            CAL_ERROR_LOG("SQL Result have unexpected number of fileds (%d)", pQr->iColumn);
            pErrorCode = CALENDAR_DATABASE_ERROR;
        }

        sqlite3_free_table(pQr->pResult);
        delete pQr;
        pQr = 0;
    }
    else {
        CAL_DEBUG_LOG("Got empty SQL result");
    }

    return pTimeZone;
}

CTimezone *CTimezone::createFromSqlData(char **pSqlData)
{
    CTimezone *pTimeZone = new(nothrow) CTimezone;

    if(pTimeZone) {
        GET_TEXT_FROM_SQLRESULT(pTimeZone->Location, pSqlData[0]);        // Location TEXT
        GET_TEXT_FROM_SQLRESULT(pTimeZone->Id, pSqlData[1]);              // TzId TEXT
        GET_INTEGER_FROM_SQLRESULT(pTimeZone->StartStd, pSqlData[2])      // DtStartStd INTEGER
        GET_INTEGER_FROM_SQLRESULT(pTimeZone->StartDst, pSqlData[3]);     // DtStartDst INTEGER
        GET_INTEGER_FROM_SQLRESULT(pTimeZone->OffsetStd, pSqlData[4]);    // TzOffsetStd INTEGER
        GET_INTEGER_FROM_SQLRESULT(pTimeZone->OffsetDst, pSqlData[5]);    // TzOffsetDst INTEGER
        GET_TEXT_FROM_SQLRESULT(pTimeZone->RuleStd, pSqlData[6]);         // RRuleStd TEXT
        GET_TEXT_FROM_SQLRESULT(pTimeZone->RuleDst, pSqlData[7]);         // RRuleDst TEXT
        GET_TEXT_FROM_SQLRESULT(pTimeZone->Name, pSqlData[8]);            // TzName TEXT
        GET_INTEGER_FROM_SQLRESULT(pTimeZone->haveDst, pSqlData[9]);      // DSTFlag INTEGER
    }

    return pTimeZone;
}

CTimezone *CTimezone::getByLocation(std::string Location, int &pErrorCode)
{
    CTimezone *pTimeZone = 0;

    char *pQuery = sqlite3_mprintf("SELECT * FROM TIMEZONE WHERE Location='%s'", Location.c_str());

    pErrorCode = CALENDAR_SYSTEM_ERROR;

    if(pQuery) {
        pTimeZone = createFromSqlQuery(pQuery, pErrorCode);

        sqlite3_free(pQuery);
    }

    return pTimeZone;
}


string CTimezone::getLocation(int offsetstd, bool dstflag, time_t dstoffset, std::string szDaylight)
{
    QueryResult *pQr = 0;
    char *pQuery = 0;
    int iSqliteError = 0;
    CCalendarDB *pDb = 0;
    int pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    string szRet;
    std::vector <time_t> listtime ;
    std::string szRuleFromStart, szRuleFromEnd;

    pDb = CCalendarDB::Instance();

    if(pDb == 0) {
        CAL_ERROR_LOG("Failed to get DB instance");
        pErrorCode = CALENDAR_APP_ERROR;
        return szRet;
    }

    /* checks to see if daylight information is consistent in VCAL component*/
    if((dstflag  == false &&  !szDaylight.empty()) ||
            (dstflag == true && szDaylight.empty())) {
        CAL_DEBUG_LOG("Invalid timezone information "
                      "Adding event in local timezone");
        return szRet;
    }

    if(szDaylight.find("!") != std::string::npos) {
        szRuleFromStart = szDaylight.substr(szDaylight.find("!") + 1);
        szRuleFromEnd = szDaylight.substr(0, szDaylight.find("!"));
        CAL_DEBUG_LOG("Repeat search strings are %s and %s",
                      szRuleFromStart.c_str(), szRuleFromEnd.c_str());
    }

    if(dstflag) {
        if(!szRuleFromStart.empty() && !szRuleFromEnd.empty()) {
            pQuery = sqlite3_mprintf(SELECT_LOCATION_FROM_OFFSET_WITH_DST, TIMEZONE_FIELD_DSTFLAG,
                                     dstflag, TIMEZONE_FIELD_TZOFFSTD, offsetstd, TIMEZONE_FIELD_TZOFFDST,
                                     dstoffset, TIMEZONE_FIELD_RRULEDST, szRuleFromStart.c_str(),
                                     TIMEZONE_FIELD_RRULEDST, szRuleFromEnd.c_str());
        }
        else {
            pQuery = sqlite3_mprintf(SELECT_LOCATION_FROM_OFFSET, TIMEZONE_FIELD_DSTFLAG,
                                     dstflag, TIMEZONE_FIELD_TZOFFSTD, offsetstd, TIMEZONE_FIELD_TZOFFDST,
                                     dstoffset);
        }
    }
    else {
        pQuery = sqlite3_mprintf(SELECT_LOCATION_FROM_OFFSET, TIMEZONE_FIELD_DSTFLAG,
                                 dstflag, TIMEZONE_FIELD_TZOFFSTD, offsetstd, TIMEZONE_FIELD_TZOFFDST,
                                 dstoffset);
    }

    CAL_DEBUG_LOG("Query: %s", pQuery);
    ASSERTION(pQuery);

    pQr = pDb->getRecords(pQuery, iSqliteError);
    pDb->sqliteErrorMapper(iSqliteError, pErrorCode);
    sqlite3_free(pQuery);

    if(pErrorCode == CALENDAR_OPERATION_SUCCESSFUL) {
        if(pQr != 0) {
            if(pQr->pResult[1]) {
                szRet = pQr->pResult[1];
            }
            else {
                CAL_ERROR_LOG("Got NULL value as Location for offset std %d and dstflag %d", offsetstd, dstflag);
            }
        }
        else {
            CAL_ERROR_LOG("Timezone not found with the input details:"
                          "offset std %d and dstflag %d", offsetstd, dstflag);
        }

    }
    else {
        CAL_ERROR_LOG("Error occured while reading TIMEZONE table ErrorCode is %d", pErrorCode);
    }

    if(pQr != 0 &&  pQr->pResult != 0) {
        sqlite3_free_table(pQr->pResult);
    }

    delete pQr;
    pQr = 0;

    CAL_DEBUG_LOG("location is %s", szRet.c_str());

    return  szRet;
}
