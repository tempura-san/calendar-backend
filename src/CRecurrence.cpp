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

#include "CRecurrence.h"

#include "CalendarLog.h"
#include "CMulticalendar.h"

#include <cassert>
#include <sstream>
using namespace std;

#include <libical/ical.h>

bool CRecurrence::checkInstances(time_t iViewBegin, time_t iViewEnd,
        time_t iDateStart, int iEventDuration, int is_day, string sTzid)
{
    vector<time_t> instance_times;
    generateInstanceTimes(iViewBegin, iViewEnd, iDateStart, iEventDuration,
            is_day, sTzid, instance_times, false);

    return (!instance_times.empty());
}

vector<time_t> CRecurrence::generateInstanceTimes(time_t iViewBegin,
        time_t iViewEnd, time_t iDateStart, int iEventDuration, int is_day,
        string sTzid)
{
    CAL_DEBUG_LOG("DEPRECATED FUNCTION: CRecurrence::generateInstanceTimes()");

    vector<time_t> instance_times;
    generateInstanceTimes(iViewBegin, iViewEnd, iDateStart, iEventDuration,
            is_day, sTzid, instance_times);

    return instance_times;
}

void CRecurrence::generateInstanceTimes(time_t iViewBegin, time_t iViewEnd,
        time_t iDateStart, int iEventDuration, int is_day, string sTzid,
        vector<time_t> &instance_times, bool sort_times)
{
    (void) is_day;

    vector<string> rules;
    vector<time_t> etimes;
    vector<time_t> temp;
    vector<string>::iterator siter;
    vector<time_t>::iterator titer;
    int pErrorCode = 0;

    CAL_DEBUG_LOG("CRecurrence::generateInstanceTimes() : view begin %s",
            ctime(&iViewBegin));
    CAL_DEBUG_LOG("CRecurrence::generateInstanceTimes() : view end %s",
            ctime(&iViewEnd));

    if (iViewEnd == 0) {
        iViewEnd = iViewBegin;
    }

    CMulticalendar *mc = CMulticalendar::MCInstance();

    /* do this when time zone is not empty */
    // TODO check why this is necessary
    if (!sTzid.empty()) {
        sTzid = sTzid.substr(1);
    }

    // get the Olson time zone from the passed string
    icaltimezone *pTz = icaltimezone_get_builtin_timezone(sTzid.c_str());
    icaltimetype timeIcal = icaltime_from_timet_with_zone(iDateStart, 0, pTz);

    // add recurrences (based on rules) of the event to the result vector
    rules = getRrule();
    for (siter = rules.begin(); siter != rules.end(); ++siter) {
        CAL_DEBUG_LOG("Found recurrence rule '%s'", (*siter).c_str());
        mc->getRecurrentTimes((*siter), timeIcal, iEventDuration, iViewBegin,
                iViewEnd, pTz, temp, pErrorCode);
        // TODO check for error code - ATM CMultiCalendar::getRecurrentTimes()
        //      does not put anything there
        instance_times.insert(instance_times.end(), temp.begin(), temp.end());
        temp.clear();
    }

    // add exceptions (based on rules) of the event to the exception vector
    rules = this->getErule();
    for (siter = rules.begin(); siter != rules.end(); ++siter) {
        CAL_DEBUG_LOG("Found exception rule '%s'", (*siter).c_str());
        mc->getRecurrentTimes((*siter),
                icaltime_from_timet_with_zone(iDateStart, 0, pTz),
                iEventDuration, iViewBegin, iViewEnd, pTz, temp, pErrorCode);
        // TODO check for error code - ATM CMultiCalendar::getRecurrentTimes()
        //      does not put anything there
        etimes.insert(etimes.end(), temp.begin(), temp.end());
        temp.clear();
    }

    // add recurrences (based on dates) of the event to the result vector
    rules = this->getRDays();
    if (!rules.empty()) {
        for (siter = rules.begin(); siter != rules.end(); ++siter) {
            CAL_DEBUG_LOG("Found recurrence date '%s'", (*siter).c_str());
            // TODO check for conversion error and do not add element if it
            //      could not be converted
            temp.push_back(static_cast<time_t>(atoi((*siter).c_str())));
        }
        instance_times.insert(instance_times.end(), temp.begin(), temp.end());
        temp.clear();
    }

    // add exceptions (based on dates) of the event to the exception vector
    rules = this->getEDays();
    if (!rules.empty()) {
        for (siter = rules.begin(); siter != rules.end(); ++siter) {
            CAL_DEBUG_LOG("Found exception date '%s'", (*siter).c_str());
            // TODO check for conversion error and do not add element if it
            //      could not be converted
            temp.push_back(static_cast<time_t>(atoi((*siter).c_str())));
        }
        etimes.insert(etimes.end(), temp.begin(), temp.end());
        temp.clear();
    }

    // add the start date if not already present and in range
    for (titer = instance_times.begin(); titer != instance_times.end();
            ++titer) {
        if ((*titer) == iDateStart) {
            break;
        }
    }
    if ((titer != instance_times.end()) && (iViewBegin <= iDateStart)
            && (iDateStart <= iViewEnd)) {
        CAL_DEBUG_LOG("Adding start date '%s'", ctime(&iDateStart));
        instance_times.push_back(iDateStart);
    }

    // remove the exception times from the result vector
    for (titer = etimes.begin(); titer < etimes.end(); ++titer) {
        bool timet_isday = false;

        struct icaltimetype exIcal;
        memset(&exIcal, 0, sizeof(struct icaltimetype));

        exIcal = icaltime_from_timet_with_zone((*titer), 0, pTz);
        if ((exIcal.hour == 0) && (exIcal.minute == 0)
                && (exIcal.second == 0)) {
            timet_isday = true;
            CAL_DEBUG_LOG("Exception time is date format not date+time format");
        }

        vector<time_t>::iterator it;
        for (it = instance_times.begin(); it < instance_times.end();) {
            // times match if they match directly or if the exception falls into
            // an all-day event
            if (((*titer) == (*it))
                    || (timet_isday
                            && (((*titer) <= (*it))
                                    && ((*it) <= ((*titer) + 60 * 60 * 24 - 1))))) {
                CAL_DEBUG_LOG("Found exception time %s", ctime(&(*titer)));
                it = instance_times.erase(it);
            } else {
                ++it;
            }
        }
    }

    // sort the result vector if demanded
    if (sort_times) {
        sort(instance_times.begin(), instance_times.end());
    }
}

bool CRecurrence::setRecurrenceRule(vector<CRecurrenceRule *> vRRule)
{
    if (vRRule.empty()) {
        return false;
    }

    vRecrRuleList.insert(vRecrRuleList.end(), vRRule.begin(), vRRule.end());
    return true;
}

vector<CRecurrenceRule *> CRecurrence::getRecurrenceRule()
{
    return vRecrRuleList;
}

bool CRecurrence::setRecurId(int iRecurId)
{
    if (iRecurId < 0) {
        return false;
    }

    this->iRecurId = iRecurId;
    return true;
}

int CRecurrence::getRecurId()
{
    return iRecurId;
}

vector<string> CRecurrence::getErule()
{
    vector<string> stringrules;
    if (vRecrRuleList.empty()) {
        return stringrules;
    }

    vector<CRecurrenceRule *>::iterator iter;
    for (iter = vRecrRuleList.begin(); iter != vRecrRuleList.end(); ++iter) {
        if ((*iter)->getRuleType() == EXCEPTION_RULE) {
            stringrules.push_back((*iter)->getRrule());
        }
    }

    return stringrules;
}

vector<string> CRecurrence::getRrule()
{
    vector<string> stringrules;
    if (vRecrRuleList.empty()) {
        return stringrules;
    }

    vector<CRecurrenceRule *>::iterator iter;
    for (iter = vRecrRuleList.begin(); iter != vRecrRuleList.end(); ++iter) {
        if ((*iter)->getRuleType() == RECURRENCE_RULE) {
            stringrules.push_back((*iter)->getRrule());
        }
    }

    return stringrules;
}

bool CRecurrence::setRrule(vector<string> vRules)
{
    if (vRules.empty()) {
        return false;
    }

    vector<string>::iterator iter;
    for (iter = vRules.begin(); iter != vRules.end(); ++iter) {
        CRecurrenceRule *temp = new CRecurrenceRule(RECURRENCE_RULE, (*iter));
        assert(temp);
        vRecrRuleList.push_back(temp);
    }
    return true;
}

bool CRecurrence::setErule(vector<string> vRules)
{
    if (vRules.empty()) {
        return false;
    }

    vector<string>::iterator iter;
    for (iter = vRules.begin(); iter != vRules.end(); ++iter) {
        CRecurrenceRule *temp = new CRecurrenceRule(EXCEPTION_RULE, (*iter));
        assert(temp);
        vRecrRuleList.push_back(temp);
    }
    return true;
}

bool CRecurrence::setRDays(vector<string> vRecRuleList)
{
    if (!vRecRuleList.empty()) {
        this->vRecRuleList = vRecRuleList;
        return true;
    } else {
        return false;
    }
}

vector<string> CRecurrence::getRDays()
{
    return vRecRuleList;
}

bool CRecurrence::setEDays(vector<string> vExceptionDateList)
{
    if (!vExceptionDateList.empty()) {
        this->vExceptionDateList = vExceptionDateList;
        return true;
    } else {
        return false;
    }
}

vector<string> CRecurrence::getEDays()
{
    return vExceptionDateList;
}

CRecurrence & CRecurrence::operator=(const CRecurrence & right)
{
    // prevent self assignment
    if (&right != this) {
        // clear existing elements before deep copy
        vRecrRuleList.clear();

        vector<CRecurrenceRule *> vRRule =
                const_cast<CRecurrence &>(right).getRecurrenceRule();

        vector<CRecurrenceRule *>::const_iterator iter;
        for (iter = vRRule.begin(); iter != vRRule.end(); ++iter) {
            CRecurrenceRule *temp = new CRecurrenceRule(**iter);
            assert(temp);
            vRecrRuleList.push_back(temp);
        }
        vExceptionDateList = right.vExceptionDateList;
        vRecRuleList = right.vRecRuleList;
        iRecurId = right.iRecurId;
        iRType = right.iRType;
    }
    return *this;
}

bool CRecurrence::setRtype(int iRType)
{
    if ((iRType < E_NONE) || (iRType > E_COMPLEX)) {
        return false;
    }

    this->iRType = static_cast<RecurrenceType_t>(iRType);
    return true;
}

int CRecurrence::getRtype()
{
    return iRType;
}

CRecurrence::CRecurrence(CRecurrence & ref)
{
    vector<CRecurrenceRule *> vRRule = ref.getRecurrenceRule();
    vector<CRecurrenceRule *>::iterator iter;
    for (iter = vRRule.begin(); iter != vRRule.end(); ++iter) {
        CRecurrenceRule *temp = new CRecurrenceRule(**iter);
        assert(temp);
        vRecrRuleList.push_back(temp);
    }
    iRecurId = ref.iRecurId;
    vExceptionDateList = ref.vExceptionDateList;
    vRecRuleList = ref.vRecRuleList;
    iRType = ref.iRType;
}

CRecurrence::CRecurrence(vector<string> vRRule, int iRecurId) :
        iRType(E_NONE), iRecurId(iRecurId)
{
    (void) setRrule(vRRule);
}

CRecurrence::CRecurrence() :
        iRType(E_NONE), iRecurId(-1)
{
}

CRecurrence::~CRecurrence()
{
    vector<CRecurrenceRule *>::iterator iter;
    for (iter = vRecrRuleList.begin(); iter != vRecrRuleList.end(); ++iter) {
        delete (*iter);
        *iter = 0;
    }
}

string CRecurrence::toString()
{
    std::stringstream ss;
    ss << "Recurrence type=" << iRType;
    ss << ",Recurrence ID=" << iRecurId;

    if (!vRecRuleList.empty()) {
        ss << ",Rule=[";
        vector<string>::iterator it;
        for (it = vRecRuleList.begin(); it != vRecRuleList.end(); ++it) {
            if (it != vRecRuleList.begin()) {
                ss << ",";
            }
            ss << *it;
        }
        ss << "]";
    }
    if (!vExceptionDateList.empty()) {
        ss << ",ExceptionDate=[";
        vector<string>::iterator it;
        for (it = vExceptionDateList.begin(); it != vExceptionDateList.end();
                ++it) {
            if (it != vExceptionDateList.begin()) {
                ss << ",";
            }
            ss << *it;
        }
        ss << "]";
    }
    if (!vRecrRuleList.empty()) {
        ss << ",RecRuleList=[";
        vector<CRecurrenceRule *>::iterator it;
        for (it = vRecrRuleList.begin(); it != vRecrRuleList.end(); ++it) {
            if (it != vRecrRuleList.begin()) {
                ss << ",";
            }
            ss << "[" << (*it)->toString() << "]";
        }
        ss << "]";
    }

    return ss.str();
}
