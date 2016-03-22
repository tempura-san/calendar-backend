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

#include "CUtility.h"
#include "Common.h"

#include <iomanip>

// initialise static instance pointer to NULL
CUtility *CUtility::pUt = 0;

time_t CUtility::getDateFromTime(time_t startDate)
{
    time_t reqDate = 0;
    struct tm pDate;

    if(startDate == 0) {
        return reqDate;
    }

    memset(&pDate, 0, sizeof(struct tm));

    // use default/local timezone for conversion
    if(time_get_remote(startDate, 0, &pDate) != 0) {
        return reqDate;
    }

    pDate.tm_hour = 0;
    pDate.tm_min = 0;
    pDate.tm_sec = 0;
    reqDate = time_mktime(&pDate, 0);

    return reqDate;
}

vector<string> CUtility::parseIds(string szIds)
{
    vector < string > listId;

    string token;
    istringstream iss(szIds);

    if(!szIds.empty()) {
        while(getline(iss, token, ',')) {
            if(!token.empty()) {
                listId.push_back(token);
            }
        }
    }

    return listId;
}

bool CUtility::stringReplaceOne(string &toChange, const char *changeThis,
                                const char *toThis)
{
    bool retval = false;
    size_t subStart = 0;

    if((subStart = toChange.find(changeThis)) != string::npos) {
        toChange.replace(subStart, strlen(changeThis), toThis);
        retval = true;
    }

    return retval;
}

string CUtility::encodeQuotedPrintable(string szInput)
{
    ostringstream oss;
    int linechars = 0;

    // nothing to encode, so return empty string
    if(szInput.empty()) {
        return "";
    }

    // insert initial soft line break
    oss << "=\n";

    // omit base for 'hex' used in encoding
    oss.unsetf(ios::showbase);

    for(string::iterator it = szInput.begin(); it != szInput.end(); ++it) {
        if((*it >= 33 && *it <= 126) &&     // printable ASCII range
                (*it != '=') &&                 // no '=' for QP
                (*it != ',') &&                 // no COMMA (RFC2445/4.1.1)
                (*it != ':') &&                 // no COLON (RFC2445/4.1.1)
                (*it != ';')) {                 // no SEMICOLON (RFC2445/4.1.1)

            // no encoding, simply add character itself
            oss << *it;
            linechars++;
        }
        else {
            // encode and insert using uppercase hexadecimals
            if(*it == '\n') {
                // special handling for line breaks
                oss << "=0D=0A";
                linechars += 6;
            }
            else {
                // as 'hex' operates on int only, use integer promotion here
                int ci = (*it & 0xFF);
                oss << "=" << uppercase << hex << setw(2) << setfill('0') << ci;
                linechars += 3;
            }
        }

        // RFC1521 / 5.1 / Rule #5 - lines must not exceed 76 characters
        if(linechars > 70) {
            // insert soft line break and reset output character count
            oss << "=\n";
            linechars = 0;
        }
    }

    return oss.str();
}

vector<time_t> CUtility::getRecurrentTimes(string szRRule,
        icaltimetype iDtStart, time_t iViewEnd, icaltimezone *pTz,
        int &pErrorCode)
{
    (void) pErrorCode;

    struct icalrecurrencetype recur;
    time_t utc_tim = 0;
    vector<time_t> listtime;
    icalrecur_iterator *ritr;
    bool loopexit = false;

    unsigned int pos = szRRule.find(UNTIL_ICAL, 0);

    if(pos != string::npos) {
        // find the six zeros and then substitute them with 245959  on the fly
        pos = szRRule.find(T_ICAL, pos + SIX);

        if(pos != string::npos) {
            szRRule.replace(pos + 1, SIX, DAY_ICAL);
        }
    }

    recur = icalrecurrencetype_from_string(szRRule.c_str());
    ritr = icalrecur_iterator_new(recur, iDtStart);

    if(ritr) {
        struct icaltimetype next;
        next = icalrecur_iterator_next(ritr);

        while((!icaltime_is_null_time(next)) && (!loopexit)) {
            utc_tim = icaltime_as_timet_with_zone(next, pTz);

            if((utc_tim > iViewEnd) && (iViewEnd != 0)) {
                loopexit = true;
            }
            else {
                CAL_DEBUG_LOG("instance time  = %s", ctime(&utc_tim));
                listtime.push_back(utc_tim);
            }

            next = icalrecur_iterator_next(ritr);
        }

        icalrecur_iterator_free(ritr);
    }

    return listtime;

}

string CUtility::getSystemTimeZone()
{
    char *zone = new char[DEFAULT_SIZE];
    string szZone;
    int nchars = 0;

    bzero(zone, DEFAULT_SIZE);
    nchars = time_get_timezone(zone, DEFAULT_SIZE);

    if(nchars > 0 && nchars < DEFAULT_SIZE) {
        szZone = zone;
        CAL_DEBUG_LOG("Current timezone is %s\n", zone);
    }
    else {
        CAL_DEBUG_LOG("Error: got nchars = %d", nchars);
    }

    delete[] zone;
    return szZone;
}

icaltimezone *CUtility::getSystemTimeZoneAsIcalTimeZone()
{
    string szZone(getSystemTimeZone());
    unsigned int i;

    if((i = szZone.find(":", 0)) != string::npos) {
        szZone = szZone.substr(i + 1);
    }

    CAL_DEBUG_LOG("Timezone after parsing is %s", szZone.c_str());

    if(szZone.empty()) {
        CAL_ERROR_LOG("Something is terribly wrong with the environment");
        return 0;
    }

    icaltimezone *pTz = icaltimezone_get_builtin_timezone(szZone.c_str());

    if(!pTz) {
        CAL_ERROR_LOG("Requested timezone '%s' not found", szZone.c_str());
    }

    return pTz;
}

CUtility *CUtility::Instance()
{
    if(pUt == 0) {
        pUt = new CUtility();
        ASSERTION(pUt);
    }

    return pUt;
}

CUtility::CUtility()
{
}

CUtility::~CUtility()
{
}
