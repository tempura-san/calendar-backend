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
#include "CalendarLog.h"
#include "CProperties.h"
extern "C" {
#include <libical/ical.h>
}

CUtility *CUtility::pUt = 0;
//aparna
//TODO: Need to check the time zone conversions
time_t CUtility::getDateFromTime(time_t startDate)
{
    time_t reqDate = 0;

    if(startDate == 0) {
        return reqDate;
    }

    struct tm pDate;

    memset(&pDate, 0, sizeof(struct tm));

    if(time_get_remote(startDate, 0, &pDate) != 0) {
        return reqDate;
    }

    pDate.tm_hour = 0;
    pDate.tm_min = 0;
    pDate.tm_sec = 0;
    reqDate = time_mktime(&pDate, 0);

    //TODO: Remove next 2 lines
    char *tmp = ctime(&reqDate);
    CAL_DEBUG_LOG("The date is-----:%s-----------------\n", tmp);
    return reqDate;
}
//aparna
vector <string> CUtility::parseIds(string szIds)
{
    vector <string> listId;
    int iCounter = 0;
    int iLength = 0;
    string szSeparator = ",";
    string szSepId;

    if(szIds.empty()) {
        return listId;
    }

    iLength = szIds.length();


    for(iCounter = 0; iCounter < iLength; iCounter++) {
        if(szIds.compare(iCounter, 1, szSeparator) != 0) {
            szSepId.push_back(szIds[iCounter]);
        }
        else {
            if(!szSepId.empty()) {
                listId.push_back(szSepId);
                szSepId.clear();
            }
        }
    }

    CAL_DEBUG_LOG("---------The number of elements is:%d\n", listId.size());
    return listId;
}

bool CUtility::stringReplaceOne(string &toChange, const char *changeThis,
                                const char *toThis)
{
    bool retval = false;
    size_t subStart = 0;
    string orig(changeThis);

    if((subStart = toChange.find(changeThis)) != string::npos) {
        retval = true;
        toChange.replace(subStart, orig.length(), toThis);
    }

    return retval;
}

/* FIXME: Need to remove this procedure if it always returns false*/
bool CUtility::isEncodingRequired(string szInput, bool syncing)
{

    return syncing;

    /*  No need to encode based on ASCII value
     *  rather encode every time data is synced
     *  Commmented to fix 120530.
    	for(iter = 0; iter < iLength; iter++){
    		char cInput = szInput[iter];
    		if(cInput & 0x80) {
    			found_non_ascii = true;
    			break;
    		}
    	}
    	return found_non_ascii;
    */
}


/* function to encode data in quoted printable
 */
string  CUtility::encodeQuotedPrintable(string szInput)
{
    string szOutput;
    char tmp[256];
    int iPos = 0;
    unsigned int iLength = szInput.length();
    unsigned int iter = 0;

    CAL_DEBUG_LOG("encodeQuotedPrintable(%s)\n", szInput.c_str());
    int limit = 70;
    szOutput.clear();

    szOutput.append("=\n");

    for(iter = 0; iter < iLength; iter++) {
        char cInput = szInput[iter];

        if((cInput > 36 && cInput < 91 && cInput != 61 && cInput != 44
                && cInput != 59) ||
                (cInput > 96 && cInput < 127)) {
            /*
             * No encoding required. Add as is
             * szOutput.push_back(cInput);
             */
            iPos += 1;
            //CAL_DEBUG_LOG("No encoding %c \n", cInput);
            szOutput.push_back(cInput);
        }
        else {
            /*everything else encode to hex value and insert
             */
            sprintf(tmp, "%04hX", cInput);
            tmp[1] = '=';
            szOutput.append(tmp + 1);
            iPos += 3;
        }

        if(iPos > limit) {
            iPos = 0;
            szOutput.append("=\n");
        }
    }

    return szOutput;
}

std::vector < time_t > CUtility::getRecurrentTimes(string szRRule,
        icaltimetype iDtStart,
        time_t iViewEnd, icaltimezone *pTz, int &pErrorCode)

{

    struct icalrecurrencetype recur;
    time_t utc_tim = 0;
    vector < time_t > listtime;
    bool loopexit = false;
    unsigned int pos = 0;
    pos = szRRule.find(UNTIL_ICAL, 0);

    if(pos != string::npos) {
        /* Find the six zeros and then substitute them with
         *          * 245959  on the fly */
        pos = szRRule.find(T_ICAL, pos + SIX);

        if(pos != string::npos) {
            szRRule.replace(pos + 1, SIX, DAY_ICAL);
        }
    }

    recur = icalrecurrencetype_from_string(szRRule.c_str());

    CAL_DEBUG_LOG("*********Date start from ICS *********** ");
    CAL_DEBUG_LOG(" Year is:    %d ", iDtStart.year);
    CAL_DEBUG_LOG(" Month is:   %d ", iDtStart.month);
    CAL_DEBUG_LOG(" Day is:     %d ", iDtStart.day);
    CAL_DEBUG_LOG(" Hour is:    %d ", iDtStart.hour);
    CAL_DEBUG_LOG(" Minute  is: %d ", iDtStart.minute);
    CAL_DEBUG_LOG(" Second is : %d ", iDtStart.second);
    CAL_DEBUG_LOG("**************************************** ");
    icalrecur_iterator *ritr = 0;
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

void CUtility::releasePropertiesVector(std::vector < CProperties * > vec)
{
    std::vector < CProperties * >::iterator iIter;
    CAL_DEBUG_LOG("Freeing the property List");

    for(iIter = vec.begin(); iIter < vec.end(); iIter++) {
        delete(*iIter);
        (*iIter) = 0;
    }

    vec.clear();
}


icaltimezone *CUtility::getSystemTimeZoneAsIcalTimeZone()
{
    string szZone(getSystemTimeZone());
    unsigned int i = 0;

    if((i = szZone.find(":", 0)) != string::npos) {
        szZone = szZone.substr(i + 1);
    }

    CAL_DEBUG_LOG(" Timezone after parsing is %s", szZone.c_str());

    if(szZone.empty()) {
        CAL_DEBUG_LOG("Something is terribly wrong with the environment");
        return 0;
    }

    icaltimezone *pTz = icaltimezone_get_builtin_timezone(szZone.c_str());

    if(!pTz) {
        CAL_DEBUG_LOG("Requested timezone not found in the system %s", szZone.c_str());
    }

    return pTz;
}


std::string CUtility::getSystemTimeZone()
{
    char *zone = new char[DEFAULT_SIZE];
    string szZone;
    int   nchars = 0;

    ASSERTION(zone);
    memset(zone, 0, DEFAULT_SIZE);
    nchars = time_get_timezone(zone, DEFAULT_SIZE);
    CAL_DEBUG_LOG("*** nchars = %d", nchars);

    if(nchars > 0 && nchars < DEFAULT_SIZE) {
        szZone = zone;
        CAL_DEBUG_LOG("Current timezone is %s\n", zone);
    }

    delete [] zone;
    return szZone;
}

/**
 * @param : None
 * @return : string name of the application
 *
 * This function sets the given name as the application name.
 */
string CUtility::getApplicationName()
{
    return AppName;
}

/**
 * @param: string name of the application
 * @return: void
 *
 * This function sets the given name as the application name.
 */
void CUtility::setApplicationName(string ApplicationName)
{
    if(AppName.empty()) {
        AppName.append(ApplicationName);
    }
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
