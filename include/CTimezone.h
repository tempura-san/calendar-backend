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

#ifndef __CTIMEZONE_H__
#define __CTIMEZONE_H__

#include <string>

class CTimezone
{
public:
    /**
    * Destructor
    */
    ~CTimezone();

    static CTimezone *guessTimeZone(time_t StartStd,
                                    time_t StartDst,
                                    time_t OffsetStd,
                                    time_t OffsetDst,
                                    std::string RuleStd,
                                    std::string  RuleDst,
                                    bool haveDst,
                                    int &pErrorCode);

    static CTimezone *getByLocation(std::string Location, int &pErrorCode);
    static std::string getLocation(int offsetstd, bool dstflag, time_t dstoffset, std::string szDaylight);


    const std::string &getLocation() const;
    const std::string &getId() const;
    time_t getStartStd() const;
    time_t getStartDst() const;
    time_t getOffsetStd() const;
    time_t getOffsetDst() const;
    const std::string &getRuleStd() const;
    const std::string &getRuleDst() const;
    const std::string &getName() const;
    bool getHaveDst() const;

protected:

    /**
      * Default Constructor
      */
    CTimezone();
private:
    /*
     #define CREATE_TIMEZONE "CREATE TABLE IF NOT EXISTS TIMEZONE (Location TEXT,TzId TEXT,DtStartStd INTEGER,DtStartDst INTEGER,TzOffsetStd INTEGER,TzOffsetDst INTEGER,RRuleStd TEXT,RRuleDst TEXT,TzName TEXT,DSTFlag INTEGER )"
    */

    std::string Location;   // Location TEXT
    std::string Id;         // TzId TEXT
    time_t StartStd;        // DtStartStd INTEGER
    time_t StartDst;        // DtStartDst INTEGER
    time_t OffsetStd;       // TzOffsetStd INTEGER
    time_t OffsetDst;       // TzOffsetDst INTEGER
    std::string RuleStd;    // RRuleDst TEXT
    std::string  RuleDst;   // RRuleDst TEXT
    std::string Name;       // TzName TEXT
    bool haveDst;           // DSTFlag INTEGER

    std::string toString();

    static CTimezone *createFromSqlQuery(char *pSqlQuery, int &pErrorCode);
    static CTimezone *createFromSqlData(char **pSqlData);
};

#endif // ifdef __CTIMEZONE_H__
