#include "CCalendarProcs.h"
#include <assert.h>
#include <CalendarLog.h>
#include <CalendarErrors.h>

#include <CComponent.h>
#include <Common.h>

#include <time.h>

#define DAYMONTH_MULTIPLIER 100

static const int FETCH_SIMPLE_EVENTS_AND_TASKS = 1;
static const int FETCH_REPEATING_EVENTS = 2;

class SQLiteQuery
{
public:
    SQLiteQuery(char *query)
    : pQuery(query)
    , pResult(0)
    , iRow(0)
    , iColumn(0)
    {
    }

    ~SQLiteQuery()
    {
        if (pQuery)
        {
            sqlite3_free(pQuery);
        }

        if (pResult)
        {
            sqlite3_free_table(pResult);
        }

    }

    operator const char * () const
    {
        return pQuery;
    }

    int execute(CCalendarDB *pDb)
    {
        char *pErr_msg = 0;

        int iRet = sqlite3_exec(pDb->getDb(), pQuery, 0, 0, &pErr_msg);

        if (pErr_msg) {
            CAL_ERROR_LOG("SQLiteQuery::execute:  SQL error(%d): %s while executing \"%s\"", iRet, pErr_msg, pQuery);
            sqlite3_free(pErr_msg);
        }
        else
        {
            CAL_DEBUG_LOG("SQLiteQuery::execute \"%s\", result %d",  pQuery, iRet);
        }

        return iRet;
    }

    int getRecords(CCalendarDB *pDb)
    {
        char *pErr_msg = 0;

        CAL_DEBUG_LOG("SQLiteQuery::execute %s",  pQuery);

        int iRet = sqlite3_get_table(pDb->getDb(), pQuery, &pResult, &iRow, &iColumn, &pErr_msg);

        if (pErr_msg) {
            CAL_ERROR_LOG("SQLiteQuery::getRecords:  SQL error(%d): %s while executing \"%s\"", iRet, pErr_msg, pQuery);
            sqlite3_free(pErr_msg);
        }
        else
        {
            CAL_DEBUG_LOG("SQLiteQuery::getRecords \"%s\" finished with code %d, got %d rows (%d columns)",
                           pQuery, iRet, iRow, iColumn);
        }

        return iRet;
    }

    int getRowCount() const
    {
        return iRow;
    }

    int getColumnCount() const
    {
        return iColumn;
    }

    /**
     * Get whole SQL result row
     * @param row row index
     * @return pointer to (const char *[iColumn]), 0 if no data or row index is invalid
     */
    const char **operator() (int row) const
    {
        const char ** retval = 0;

        if ((pResult != 0) && (row < iRow))
        {
            retval =  (const char**)(pResult + ((row + 1) * iColumn));
        }
        else
        {
            CAL_ERROR_LOG("SQL data have no row #%d. SQL Result is %p and have %d rows", row, pResult, iRow);
        }

        return retval;
    }


    const char *operator() (int row, int col) const
    {
        const char * retval = 0;

        if ((pResult != 0) && (row < iRow) && (col < iColumn))
        {
            retval = pResult[(row + 1) * iColumn + col];
        }
        else
        {
            CAL_ERROR_LOG("No SQL data at (%d, %d). SQL Result is %p, %dx%d", row, col, pResult, iRow, iColumn);
        }

        return retval;
    }

private:
    char *pQuery;
    char **pResult;
    int iRow;
    int iColumn;
};

CCalendarProcs::CCalendarProcs(CCalendarDB *pDb)
: m_pDb(pDb)
{
    assert(pDb != 0);
};

void CCalendarProcs::dropInstances(int CompId, int &Error)
{
};

void CCalendarProcs::updateInstances(int CompId, const std::vector<time_t> &InstanceTimes, int &Error)
{
};

void CCalendarProcs::getInstances(int CompId, std::vector<time_t> &InstanceTimes, int &Error)
{
};

//////////////////////////
// Birthady events cache
//////////////////////////

int CCalendarProcs::initBDay()
{
    CAL_DEBUG_LOG("Init Bithday cache");
    return SQLITE_OK;
}

int CCalendarProcs::addBDay(std::string CompId, time_t bday_date)
{
    int daymonth, year;

    time2monthday(bday_date, daymonth, year);

    SQLiteQuery query(sqlite3_mprintf("INSERT INTO Birthdays (Id, DayMonth, Year) values (%Q,%d,%d)",
                                       CompId.c_str(), daymonth, year));

    int sql_error = query.execute(m_pDb);

    if (sql_error == SQLITE_CONSTRAINT)
    {
        CAL_DEBUG_LOG("Failed to add Birthday onto cache (error %d), trying to update existing one", sql_error);

        SQLiteQuery query(sqlite3_mprintf("UPDATE Birthdays SET DayMonth=%d, Year=%d WHERE Id=%Q",
                                         daymonth, year, CompId.c_str()));
        sql_error = query.execute(m_pDb);

        if (sql_error != SQLITE_OK)
        {
            CAL_ERROR_LOG("Failed to add Birthday onto cache (error %d)", sql_error);
        }
    }
    else if (sql_error != SQLITE_OK)
    {
        CAL_ERROR_LOG("Failed to add Birthday onto cache (error %d)", sql_error);
    }

    return sql_error;
}

int CCalendarProcs::getBDays(time_t iStart, time_t iEnd, std::vector< std::string > &CompIds)
{
    int start_daymonth, start_year;
    int end_daymonth, end_year;

    CompIds.clear();

    // Validate dates
    if (iStart > iEnd)
    {
        CAL_ERROR_LOG("Wrong range specified: start > end");
        return -1;
    }

    time2monthday(iStart, start_daymonth, start_year);
    time2monthday(iEnd, end_daymonth, end_year);


    char *query_str = 0;

    if (end_year == start_year)
    {
        // Range is within one year
        query_str = sqlite3_mprintf("SELECT Id FROM Birthdays WHERE Year<=%d"
                                      " AND DayMonth>=%d AND DayMonth<=%d",
                                      end_year,
                                      start_daymonth,
                                      end_daymonth);
    }
    else if ((end_year - start_year) == 1)
    {
        query_str = sqlite3_mprintf("SELECT Id FROM Birthdays WHERE  Year<=%d"
                                      " AND ((Year<%d AND (DayMonth>=%d OR DayMonth<=%d)) OR "
                                            "(Year=%d AND DayMonth<=%d))",
                                      end_year,
                                      end_year, start_daymonth, end_daymonth,
                                      end_year, end_daymonth);
    }
    else
    {
        CAL_ERROR_LOG("Range is more than one year. Select everything");
        query_str = sqlite3_mprintf("SELECT Id FROM Birthdays WHERE (Year<%d) "
                                                    " OR (Year=%d AND DayMonth<=%d)",
                                    end_year,
                                    end_year, end_daymonth);
    }

    SQLiteQuery query(query_str);

    int sql_error = query.getRecords(m_pDb);

    if (sql_error == SQLITE_OK)
    {
        for(int i=0; i < query.getRowCount(); i++)
        {
            const char *id = query(i, 0);
            if (id != 0)
            {
                CompIds.push_back(id);
            }
            else
            {
                sql_error = -2;
                CAL_ERROR_LOG("No more data (row = %d)", i);

                break;
            }
        }
//         CAL_ERROR_LOG("Failed to fetch Birthday cache data (error %d)", sql_error);
    }
    else
    {
        CAL_ERROR_LOG("Failed to fetch Birthday cache data (error %d)", sql_error);
    }

    return sql_error;
}

int CCalendarProcs::delBDay(std::string CompId)
{
    SQLiteQuery query(sqlite3_mprintf("DELETE FROM Birthdays WHERE Id=%Q", CompId.c_str()));
    return query.execute(m_pDb);
}

bool CCalendarProcs::time2monthday(time_t t, int &daymonth, int &year)
{
    bool retval = false;

    struct tm *date = gmtime(&t);


    if (date)
    {
        daymonth = date->tm_mday + date->tm_mon * DAYMONTH_MULTIPLIER;
        year = date->tm_year + 1900;

        retval = true;
    }
    else
    {
        CAL_DEBUG_LOG("Failed to convert '%s' ", ctime(&t));
    }

    return retval;
}

int CCalendarProcs::dropBDays()
{
    SQLiteQuery query(sqlite3_mprintf("DELETE FROM Birthdays"));
    return query.execute(m_pDb);
}


// Scheme of Compoments table
// Idx:Name:Type
// 0  Id INTEGER
// 1  CalendarId INTEGER,
// 2  ComponentType INTEGER
// 3  Flags INTEGER
// 4  DateStart INTEGER
// 5  DateEnd INTEGER
// 6  Summary TEXT
// 7  Location TEXT
// 8  Description TEXT
// 9  Status INTEGER
// 10 Uid TEXT
// 11 Until INTEGER
// 12 AllDay INTEGER
// 13 CreatedTime INTEGER
// 14 ModifiedTime INTEGER
// 15 Tzid TEXT
// 16 TzOffset INTEGER


CEvent* CCalendarProcs::createComponentFromTableRecord(const char ** pSqlRowData, int columns)
{
    if (columns < 17)
    {
        CAL_ERROR_LOG("Wrong column number(%d), expected is 17", columns);
        return 0;
    }

    CEvent *event = 0; // TODO rename

    /*this function is responsible to retrieve the values stored in 
     * Component table and uses the set function to add values in 
     * to event object*/
    event = new CEvent();

    if (!event) {
        CAL_DEBUG_LOG("Memory allocation error");
        return 0;
    }

    // 0  Id INTEGER
    event->setId(pSqlRowData[0]);

    // 1  CalendarId INTEGER,
    event->setCalendarId(atoi(pSqlRowData[1]));

    // 2  ComponentType INTEGER
    event-> setType(atoi(pSqlRowData[2]));

    // 3  Flags INTEGER
    event-> setFlags(atoi (pSqlRowData[3]));

    // 4  DateStart INTEGER
    event-> setDateStart(atoi (pSqlRowData[4]));

    // 5  DateEnd INTEGER
    event-> setDateEnd(atoi (pSqlRowData[5]));

    // 6  Summary TEXT
    if (pSqlRowData[6]) {
        event->setSummary(pSqlRowData[6]);
    }

    // 7  Location TEXT
    if (pSqlRowData[7]) {
        event->setLocation(pSqlRowData[7]);
    }

    // 8  Description TEXT
    if (pSqlRowData[8]) {
        event->setDescription(pSqlRowData[8]);
    }

    // 9  Status INTEGER
    event-> setStatus(atoi (pSqlRowData[9]));

    // 10 Uid TEXT
    if (pSqlRowData[10]) 
    {
        event->setGUid(pSqlRowData[10]);
    }

    // 11 Until INTEGER
    event-> setUntil(atoi(pSqlRowData[11]));

    // 12 AllDay INTEGER
    event->setAllDay(atoi(pSqlRowData[12]));

    // 13 CreatedTime INTEGER
    event-> setCreatedTime(atoi(pSqlRowData[13]));

    // 14 ModifiedTime INTEGER
    event-> setLastModified(atoi(pSqlRowData[14]));

    // 15 Tzid TEXT
    event->setTzid(pSqlRowData[15]);

    // 16 TzOffset INTEGER
    // * Ignore *

    /* Flags has the following values 
     * HAS_RECURRENCE -- 3 
     * HAS_ALARM ---- 4
     * HAS_RECURRENCE_ALARM - 5 defined in common.h
     */

    if ((event->getFlags() == HAS_RECURRENCE) ||
        (event->getFlags() == HAS_RECURRENCE_ALARM))
    {
        event->getRecurrenceProperties();
    }

    if ((event->getFlags() == HAS_ALARM) ||
        (event->getFlags() == HAS_RECURRENCE_ALARM))
    {
        event->getAlarmProperties();
    }

    return event;
}

int CCalendarProcs::getBDays(time_t iStart, time_t iEnd, std::vector< CBdayEvent * > & Comps)
{
    int start_daymonth, start_year;
    int end_daymonth, end_year;

    Comps.clear();

    // Validate dates
    if (iStart > iEnd)
    {
        CAL_ERROR_LOG("Wrong range specified: start > end");
        return -1;
    }

    time2monthday(iStart, start_daymonth, start_year);
    time2monthday(iEnd, end_daymonth, end_year);


    char *query_str = 0;

    if (end_year == start_year)
    {
        // Range is within one year
        query_str = sqlite3_mprintf("SELECT * FROM Components "
                                              "WHERE Id in (SELECT Id FROM Birthdays"
                                                                     " WHERE Year<=%d"
                                                                      " AND DayMonth>=%d AND DayMonth<=%d)",
                                      end_year,
                                      start_daymonth,
                                      end_daymonth);
    }
    else if ((end_year - start_year) == 1)
    {
        query_str = sqlite3_mprintf("SELECT * FROM Components "
                                            "WHERE Id in (SELECT Id FROM Birthdays WHERE  Year<=%d"
                                                " AND ((Year<%d AND (DayMonth>=%d OR DayMonth<=%d)) OR "
                                                      "(Year=%d AND DayMonth<=%d)))",
                                      end_year,
                                      end_year, start_daymonth, end_daymonth,
                                      end_year, end_daymonth);
    }
    else
    {
/*        CAL_ERROR_LOG("Range is more than one year. Select everything");
        query_str = sqlite3_mprintf("SELECT Id FROM Birthdays WHERE (Year<%d) "
                                                    " OR (Year=%d AND DayMonth<=%d)",
                                    end_year,
                                    end_year, end_daymonth);*/
        return -1;
    }

    SQLiteQuery query(query_str);

    int sql_error = query.getRecords(m_pDb);

    if (sql_error == SQLITE_OK)
    {
        for(int i=0; i < query.getRowCount(); i++)
        {
            const char **row = query(i);
            if (row != 0)
            {
                CBdayEvent *event = (CBdayEvent*)createComponentFromTableRecord(row, query.getColumnCount());
                if (event)
                {
                    Comps.push_back(event);
                }
                else
                {
                    CAL_ERROR_LOG("Got NULL CBdayEvent pointer(row)", i);
                }
            }
            else
            {
                sql_error = -2;
                CAL_ERROR_LOG("No more data (row = %d)", i);

                break;
            }
        }
//         CAL_ERROR_LOG("Failed to fetch Birthday cache data (error %d)", sql_error);
    }
    else
    {
        CAL_ERROR_LOG("Failed to fetch Birthday cache data (error %d)", sql_error);
    }

    return sql_error;
}

int CCalendarProcs::getComponentsAllCalendars(int iStDate, int iEndDate, int iLimit, int iOffset, vector< CComponent * > &vComponents, int iQueryType)
{
    int retval = CALENDAR_APP_ERROR;
    int iSqliteError = 0;

    vComponents.clear();

    /* Implementation in getComponents function will be similar for getting 
     * events /todo / Bdays  (or) fetching all components other than query 
     * part. So all the functialities are written together in a single 
     * function for better code optimization
     **/
    CAL_DEBUG_LOG("stDate is:%d", iStDate);
    CAL_DEBUG_LOG("endDate is: %d", iEndDate);
    CAL_DEBUG_LOG("query type: %d", iQueryType);



    // Prepare query to fetch BDays using cache
    char *bd_query_str = 0;

    int start_daymonth, start_year;
    int end_daymonth, end_year;

    time2monthday(iStDate, start_daymonth, start_year);
    time2monthday(iEndDate, end_daymonth, end_year);

    if (end_year == start_year)
    {
        // Range is within one year
        bd_query_str = sqlite3_mprintf("SELECT * FROM Components WHERE "
                                              " ComponentType=%d "
                                              " AND Id in (SELECT Id FROM Birthdays"
                                                                     " WHERE Year<=%d"
                                                                      " AND DayMonth>=%d AND DayMonth<=%d)",
                                      E_BDAY,
                                      end_year,
                                      start_daymonth,
                                      end_daymonth);
    }
    else if ((end_year - start_year) == 1)
    {
        bd_query_str = sqlite3_mprintf("SELECT * FROM Components WHERE "
                                            " ComponentType=%d "
                                            " AND Id in (SELECT Id FROM Birthdays WHERE  Year<=%d"
                                                " AND ((Year<%d AND (DayMonth>=%d OR DayMonth<=%d)) OR "
                                                      "(Year=%d AND DayMonth<=%d)))",
                                      E_BDAY,
                                      end_year,
                                      end_year, start_daymonth, end_daymonth,
                                      end_year, end_daymonth);
    }
    else
    {
        bd_query_str = sqlite3_mprintf("SELECT * FROM Components WHERE ComponentType=%d", E_BDAY);
    }


    SQLiteQuery bd_query(bd_query_str);

    char *query_str = 0;
    switch(iQueryType){
        case FETCH_SIMPLE_EVENTS_AND_TASKS:


            query_str =
            sqlite3_mprintf("SELECT * from Components WHERE "
                            "(    (ComponentType=%d  AND DateStart<%d AND DateEnd>=%d) "
                            "  OR (ComponentType=%d AND DateStart>=%d AND DateStart <= %d )) "
                            "AND (Flags!=%d AND Flags != %d) "
                            "AND calendarId in (select calendarid from calendars where IsVisible =1 )"
                            "ORDER BY CalendarId LIMIT %d OFFSET %d ",
                        E_EVENT, iEndDate, iStDate,
                        E_TODO,  iStDate, iEndDate,
                        HAS_RECURRENCE, HAS_RECURRENCE_ALARM,
                        iLimit, iOffset);
            break;

        case FETCH_REPEATING_EVENTS:
            query_str =
            sqlite3_mprintf("SELECT * from Components WHERE "
                                    "(ComponentType=%d) AND (Flags = %d OR Flags = %d) "
                                    " AND (Until>%d OR Until=-1) "
                                    " AND calendarId in (select calendarid from calendars where IsVisible =1) "
                                    " UNION %s "
                                    " ORDER BY CalendarId LIMIT %d OFFSET %d ",
                            E_EVENT, HAS_RECURRENCE, HAS_RECURRENCE_ALARM, 
                            iStDate,
                            (const char*)bd_query,
                            iLimit, iOffset);

            break;

        default:

        query_str =
            sqlite3_mprintf("SELECT * from Components WHERE "
                            "(   (ComponentType=%d AND DateStart<%d AND DateEnd>=%d) "
                            " OR (ComponentType=%d AND DateStart>=%d AND DateStart<=%d )) "
                            "AND (Flags!=%d AND Flags!=%d) "
                            "AND calendarId in (select calendarid from calendars where IsVisible =1 ) "
                            "UNION "
                            "SELECT * from Components WHERE "
                            "(ComponentType=%d) AND (Flags=%d OR Flags=%d) AND (Until>%d OR Until=-1) "
                            "AND calendarId in (select calendarid from calendars where IsVisible =1) "
                            "UNION "
                            "%s"
                            " ORDER BY CalendarId LIMIT %d OFFSET %d ",
                E_EVENT, iEndDate, iStDate,
                E_TODO,  iStDate,  iEndDate,
                HAS_RECURRENCE, HAS_RECURRENCE_ALARM,
                E_EVENT, HAS_RECURRENCE, HAS_RECURRENCE_ALARM, iStDate,
                (const char*)bd_query,
                iLimit, iOffset);
        break;
    }

    SQLiteQuery query(query_str);

    iSqliteError = query.getRecords(m_pDb);

    if (iSqliteError == SQLITE_OK)
    {
        if (query.getRowCount() > 0)
        {
            for(int i=0; i < query.getRowCount(); i++)
            {
                const char **row = query(i);
                if (row != 0)
                {
                    CEvent *entry = createComponentFromTableRecord(row, query.getColumnCount());
                    if (entry)
                    {
                        vComponents.push_back(entry);
                    }
                    else
                    {
                        CAL_ERROR_LOG("Got NULL vComponents pointer(row = %d)", i);
                    }
                }
                else
                {
                    CAL_ERROR_LOG("No more data (row = %d)", i);

                    break;
                }
            }
            retval = CALENDAR_OPERATION_SUCCESSFUL;
        }
        else {
            retval = CALENDAR_FETCH_NOITEMS;
        }
    }
    else
    {
        m_pDb->sqliteErrorMapper(iSqliteError, retval);
    }

    CAL_DEBUG_LOG("size of list is %d, retval %d", vComponents.size(), retval);

    return retval;
}
