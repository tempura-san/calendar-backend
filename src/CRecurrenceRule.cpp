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
 * header files
 */
#include "CRecurrenceRule.h"
#include "CalendarLog.h"
#include <sstream>
const int ARRAY_DEFAULT_VALUE = 32639;
/**
 * overloaded Constructor
 * type 0-rrule ,1 -exrule
 *
 */
CRecurrenceRule::CRecurrenceRule(int iType, string szRRule)
{
    this->szRule = szRRule;
    this->iType = (RuleType) iType;
    vBySecond.clear();
    vByMinute.clear();
    vByHour.clear();
    vByWeekDay.clear();
    vByMonthDay.clear();
    vByYearDay.clear();
    vByWeekNumber.clear();
    vByMonth.clear();
    vBySetPos.clear();
    iFreq = NO_RECURRENCE;
    iCount = 0;
    interval = 0;
    iUntil = -1;
    weekStart = ICAL_SUNDAY_WEEKDAY;

}


/**
 * Default constructor
 */
CRecurrenceRule::CRecurrenceRule()
{
    vBySecond.clear();
    vByMinute.clear();
    vByHour.clear();
    vByWeekDay.clear();
    vByMonthDay.clear();
    vByYearDay.clear();
    vByWeekNumber.clear();
    vByMonth.clear();
    vBySetPos.clear();
    iFreq = NO_RECURRENCE;
    iCount = 0;
    interval = 0;
    iUntil = -1;
    weekStart = ICAL_SUNDAY_WEEKDAY;
    iType = RECURRENCE_RULE;


}

/**
 * Copy Constructor
 */
CRecurrenceRule::CRecurrenceRule(CRecurrenceRule &ref)
{

    vBySecond = ref.vBySecond;
    vByMinute = ref.vByMinute;
    vByHour = ref.vByHour;
    vByWeekDay = ref.vByWeekDay;
    vByMonthDay = ref.vByMonthDay;
    vByYearDay = ref.vByYearDay;
    vByWeekNumber = ref.vByWeekNumber;
    vByMonth = ref.vByMonth;
    vBySetPos = ref.vBySetPos;
    iFreq = ref.iFreq;
    iCount = ref.iCount;
    interval = ref.interval;
    iUntil = ref.iUntil;
    weekStart = ref.weekStart;
    iType = ref.iType;
    szRule = ref.szRule;

}

/**
 * overloaded assignment operator
 */
CRecurrenceRule &CRecurrenceRule::operator=(CRecurrenceRule &right)
{

//prevent self assignment
    if(&right != this) {
        vBySecond = right.vBySecond;
        vByMinute = right.vByMinute;
        vByHour = right.vByHour;
        vByWeekDay = right.vByWeekDay;
        vByMonthDay = right.vByMonthDay;
        vByYearDay = right.vByYearDay;
        vByWeekNumber = right.vByWeekNumber;
        vByMonth = right.vByMonth;
        vBySetPos = right.vBySetPos;
        iFreq = right.iFreq;
        iCount = right.iCount;
        interval = right.interval;
        iUntil = right.iUntil;
        weekStart = right.weekStart;
        iType = right.iType;
        szRule = right.szRule;
    }

    return *this;
}


/**
 * Destructor of the class
 */
CRecurrenceRule::~CRecurrenceRule()
{



}

/**
 * convertArrayToVector()
 * @param array of short
 * @return vector of short;
 *
 * function to convert array to vector
 */

vector < short >CRecurrenceRule::convertArrayToVector(short array[],
        int iLengthMax)
{

    vector < short >temp;
    int i = 0;
    temp.clear();

    while(i < iLengthMax) {
        /**
         * After parsing Recurrence rule using ICAL API
         * all arrays are populated with default value of
         * 32639 , to save memory consumption  we have filtered out
         * default values
         */
        if(array[i] == ARRAY_DEFAULT_VALUE) {
            i = iLengthMax;
        }
        else

        {
            temp.push_back(array[i]);
        }

        i++;
    }

    return temp;
}



/**
 * rruleParser()
 * @param string : recurrence rule, can be recurrence rule or exception
 *            rule
 * @return     : bool status of the operation
 *
 * Function used to parse the recurrence rule and populate
 * the fields in to private members of this class
 * All other set methods defined in this class will be called inside this
 * procedure
 */
bool CRecurrenceRule::rruleParser(string szRule)
{
    /*******************************************************************************
        struct icalrecurrencetype {
            icalrecurrencetype_FREQUENCY freq;


            /. * until and count are mutually exclusive. *../
            struct icaltimetype until;
            int count;

            short interval;

            icalrecurrencetype_weekday week_start;

            /...* The BY* parameters can each take a list of values. Here I
             * assume that the list of values will not be larger than the
             * range of the value -- that is, the client will not name a
             * value more than once.

             * Each of the lists is terminated with the value
             * ICAL_RECURRENCE_ARRAY_MAX unless the the list is full.
             *.../

            short by_second[ICAL_BY_SECOND_SIZE];
            short by_minute[ICAL_BY_MINUTE_SIZE];
            short by_hour[ICAL_BY_HOUR_SIZE];
            short by_day[ICAL_BY_DAY_SIZE];
            short by_month_day[ICAL_BY_MONTHDAY_SIZE];
            short by_year_day[ICAL_BY_YEARDAY_SIZE];
            short by_week_no[ICAL_BY_WEEKNO_SIZE];
            short by_month[ICAL_BY_MONTH_SIZE];
            short by_set_pos[ICAL_BY_SETPOS_SIZE];
        };
    ************************************************************************/


    struct icalrecurrencetype rruleParsed;
    rruleParsed = icalrecurrencetype_from_string(szRule.c_str());
    this->iFreq = (FREQUENCY) rruleParsed.freq;
    this->iUntil = icaltime_as_timet(rruleParsed.until);
    this->iCount = rruleParsed.count;
    this->interval = rruleParsed.interval;
    /**
     * currently libical is used later
     * this dependency has to be removed and
     * Funambol API should be used
     */
    this->vBySecond =
        convertArrayToVector(rruleParsed.by_second, ICAL_BY_SECOND_SIZE);
    this->vByMinute =
        convertArrayToVector(rruleParsed.by_minute, ICAL_BY_MINUTE_SIZE);
    this->vByHour =
        convertArrayToVector(rruleParsed.by_hour, ICAL_BY_HOUR_SIZE);
    this->vByWeekDay =
        convertArrayToVector(rruleParsed.by_day, ICAL_BY_DAY_SIZE);
    this->vByMonthDay =
        convertArrayToVector(rruleParsed.by_month_day,
                             ICAL_BY_MONTHDAY_SIZE);
    this->vByYearDay =
        convertArrayToVector(rruleParsed.by_year_day,
                             ICAL_BY_YEARDAY_SIZE);
    this->vByWeekNumber =
        convertArrayToVector(rruleParsed.by_week_no, ICAL_BY_WEEKNO_SIZE);
    this->vByMonth =
        convertArrayToVector(rruleParsed.by_month, ICAL_BY_MONTH_SIZE);
    this->vBySetPos =
        convertArrayToVector(rruleParsed.by_set_pos, ICAL_BY_SETPOS_SIZE);


    return true;


}
bool CRecurrenceRule::checkRuleIsComplex()
{
    if((vByMonthDay.size() > 1) ||
            (vByYearDay.size() > 1) ||
            (vByWeekNumber.size() > 1) ||
            (vByMonth.size() > 1) ||
            (vBySetPos.size() > 0) ||
            ((vByWeekDay.size() > 1) && (vByWeekDay.size() != 5))) {
        return true;
    }

    return false;
}

/**
 * setMonthDay
 * @param vector<short> : month days 0-31
 * @return bool status of operation
 * Function to add Month day in recurrence rule
 *
 */
bool CRecurrenceRule::setMonthDay(vector < short >vMonthDays)
{
    vByMonthDay = vMonthDays;
    return true;
}

/**
 * getMonthDay
 * @param NONE
 * @return vector<short> : month days 0-31
 * Function to retrieve list of month days
 */
vector < short >CRecurrenceRule::getMonthDay()
{
    return vByMonthDay;
}

/**
 * setRrule
 * @param : string rrule - recurrence rule / exception rule
 * @return : bool whether  the operation is successful or not
 * Function to set recurrence rule in class place holder
 */
bool CRecurrenceRule::setRrule(string szRRule)
{
    this->szRule = szRRule;

    return true;
}

/**
 * getRrule()
 * @param : NONE
 * @return : string recurrence rule
 * Function used to retrieve recurrence values from class  members
 */
string CRecurrenceRule::getRrule()
{

    return (this->szRule);
}

/**
 * setFrequency
 * @param  FREQUENCY : ivalue
 * @return bool: status
 * to set FREQUENCY of occurrence
 */
bool CRecurrenceRule::setFrequency(FREQUENCY iValue)
{
    this->iFreq = iValue;
    return true;
}

/**
 * getFREQUENCY
 * @param NONE
 * @return FREQUENCY of occurrence
 * function returns FREQUENCY parameter set in the class
 */
FREQUENCY CRecurrenceRule::getFrequency()
{
    return (this->iFreq);
}

/**
 * setUntil(time_t until)
 * @param time_t : until value in rrule as time_t
 * @return bool : status of operation performed
 *
 * function used to set unitl time in CRecurrencRule object
 */
bool CRecurrenceRule::setUntil(time_t iUntil)
{

    this->iUntil = iUntil;
    return true;
}

/**
 * getUntil()
 * @param : NONE
 * @return unitl  :time_t  status of operation performed
 *
 * function used to set until time in CRecurrencRule object
 */
time_t CRecurrenceRule::getUntil()
{
    return (this->iUntil);
}

/**
 * setCount
 * @param : integer Count of occurances
 * @return : boolean status of operation performed
 *
 * Function used to set MAX count of instances
 */
bool CRecurrenceRule::setCount(int iCount)
{

    this->iCount = iCount;
    return true;
}

/**
 * getCount
 * @param : NONE
 * @return : integer count of occurances
 *
 * Function used to retrieve  MAX count of instances
 */
int CRecurrenceRule::getCount()
{

    return (this->iCount);

}

/**
 * setSecond
 * @param vector<short>: seconds list on which the events should occur
 * @return boolean: staus of the operation
 *
 * Function used to set seconds in list of seconds appearing in
 * Recurrence rule
 */

bool CRecurrenceRule::setSecond(vector < short >vSec)
{
    this->vBySecond = vSec;
    return true;

}

/**
 * getSecond
 * @param NONE
 * @return vector<short>: seconds list on which the events should occur
 *
 * Function used to get seconds list  in
 * CRecurrenceRule Class
 */
vector < short >CRecurrenceRule::getSecond()
{
    return (this->vBySecond);

}

/**
 * setMinute
 * @param vector<short>: Minute list on which the events should occur
 * @return boolean: staus of the operation
 *
 * Function used to set Minutes appearig in Recurrence rule to
 * CRecurrenceRule class
 *
 */
bool CRecurrenceRule::setMinute(vector < short >vMinute)
{
    this->vByMinute = vMinute;
    return true;
}


/**
 * getMinute
 * @return vector<short>: Minute list on which the events should occur
 * @param NONE
 *
 * Function used to get Minutes in CRecurrenceRule class
 *
 */
vector < short >CRecurrenceRule::getMinute()
{

    return (this->vByMinute);
}

/**
 * setHour
 * @param vector<short>: Hour list on which the events should occur
 * @return boolean: staus of the operation
 *
 * Function used to set Hours  appearing in  Recurrence rule to CRecurrenceRule
 * class
 */
bool CRecurrenceRule::setHour(vector < short >vHour)
{
    this->vByHour = vHour;
    return true;
}

/**
 * getHour
 * @param NONE
 * @return vector<short>: Hour list on which the events should occur
 *
 * Function used to get Hours   in   CRecurrenceRule
 * class
 */
vector < short >CRecurrenceRule::getHour()
{
    return (this->vByHour);
}

/**
 * setWeekDay
 * @param vector<short>: WeekDay list on which the events should occur
 * @return boolean: staus of the operation
 *
 * Function used to set WeekDays SU.MO,TU etc  appearing in
 * Recurrence rule to CRecurrenceRule class
 *
 */
bool CRecurrenceRule::setWeekDay(vector < short >vByWeekDay)
{
    this->vByWeekDay = vByWeekDay;
    return true;
}

/**
 * getWeekDay
 * @param NONE
 * @return vector<short>: WeekDay list on which the events should occur
 *
 * Function used to get WeekDays SU.MO,TU etc in
 *  CRecurrenceRule class
 *
 */
vector < short >CRecurrenceRule::getWeekDay()
{
    return (this->vByWeekDay);
}

/**
 * setYearDay
 * @param vector<short>: YearDay list on which the events should occur
 * @return boolean: staus of the operation
 *
 * Function used to set YearDay appearing in  Recurrence rule to CRecurrenceRule
 * class Eg:  RRULE:FREQ=YEARLY;INTERVAL=3;COUNT=10;BYYEARDAY=1,100,200
 */
bool CRecurrenceRule::setYearDay(vector < short >vByYearDay)
{
    this->vByYearDay = vByYearDay;
    return true;
}

/**
 * getYearDay
 * @param NONE
 * @return vector<short>: YearDay list on which the events should occur
 *
 * Function used to Retrieve YearDay appearing in  CRecurrenceRule class
 */
vector < short >CRecurrenceRule::getYearDay()
{
    return (this->vByYearDay);
}

/**
 * setWeekNumber
 * @param vector<short>: WeekNumeber list on which the events should occur
 * @return boolean: staus of the operation
 *
 * Function used to set WeekNumber  appearing in
 * Recurrence rule to CRecurrenceRule class
 */
bool CRecurrenceRule::setWeekNumber(vector < short >vByWeekNumber)
{
    this->vByWeekNumber = vByWeekNumber;
    return true;

}

/**
 * getWeekNumber
 * @param NONE
 * @return vector<short>: WeekNumeber list on which the events should occur
 *
 * Function used to get WeekNumber  appearing in
 * CRecurrenceRule class
 */
vector < short >CRecurrenceRule::getWeekNumber()
{

    return (this->vByWeekNumber);
}

/**
 * setMonth
 * @param vector<short>: Month list on which the events should occur
 * @return boolean: staus of the operation
 *
 * Function used to set Month list   appearing in
 * Recurrence rule to CRecurrenceRule class
 */
bool CRecurrenceRule::setMonth(vector < short >vMonth)
{
    this->vByMonth = vMonth;
    return true;

}

/**
 * getMonth
 * @param NONE
 * @return vector<short>: Month list on which the events should occur
 *
 * Function used to get Month list   appearing in
 * CRecurrenceRule class
 */
vector < short >CRecurrenceRule::getMonth()
{

    return (this->vByMonth);
}

/**
 * setPos
 * @param vector<short>: Position list on which the events should occur
 * @return boolean: staus of the operation
 *
 * Function used to set Position list   appearing in
 * Recurrence rule to CRecurrenceRule class
 * eg :
 *    The BYSETPOS rule part specifies a COMMA character (US-ASCII decimal
 *    44) separated list of values which corresponds to the nth occurrence
 *    within the set of events specified by the rule. Valid values are 1 to
 *    366 or -366 to -1. It MUST only be used in conjunction with another
 *    BYxxx rule part. For example "the last work day of the month" could
 *    be represented as:
 *    RRULE:FREQ=MONTHLY;BYDAY=MO,TU,WE,TH,FR;BYSETPOS=-1
 */
bool CRecurrenceRule::setPos(vector < short >vPosition)
{
    this->vBySetPos = vPosition;
    return true;
}

/**
 * getPos
 * @param NONE
 * @return vector<short>: Position list on which the events should occur
 *
 * Function used to get Position list from CRecurrenceRule class
 */
vector < short >CRecurrenceRule::getPos()
{
    return (this->vBySetPos);
}

/**
 * setPos
 * @param int : weekstart  offset day from which week starts in Calendar
 *        in our calendar it is either SUNDAY or MONDAY
 * @return boolean: staus of the operation
 *
 * Function used to set week start  appearing in
 * Recurrence rule to CRecurrenceRule class
 * Eg:
 *      DTSTART;TZID=US-Eastern:19970805T090000
 *      RRULE:FREQ=WEEKLY;INTERVAL=2;COUNT=4;BYDAY=TU,SU;WKST=MO
 *      ==> (1997 EDT)Aug 5,10,19,24
 *     changing only WKST from MO to SU, yields different results...
 *      DTSTART;TZID=US-Eastern:19970805T090000
 *      RRULE:FREQ=WEEKLY;INTERVAL=2;COUNT=4;BYDAY=TU,SU;WKST=SU
 *       ==> (1997 EDT)August 5,17,19,31
 */
bool CRecurrenceRule::setWeekStart(icalrecurrencetype_weekday start)
{
    this->weekStart = start;
    return true;

}

/**
 * getPos
 * @param int : weekstart  offset day from which week starts in Calendar
 *        in our calendar it is either SUNDAY or MONDAY
 * @return boolean: staus of the operation
 *
 * Function used to get week start day from CRecurrenceRule class
 */
icalrecurrencetype_weekday CRecurrenceRule::getWeekStart()
{
    return (this->weekStart);
}

/**
 * setInterval
 * @param short : interval between two instances
 * @return      : bool status of the operation
 * Function used to set the interval param
 */
bool CRecurrenceRule::setInterval(short interval)
{
    this->interval = interval;
    return true;
}

/**
 * getInterval
 * @param NONE
 * @return short: interval between instances
 *
 * Function used to retrieve interval parameter
 */
short CRecurrenceRule::getInterval()
{
    return (this->interval);
}

/**
 * setRuleType
 * @param int type of recurrence rule 0 - RecurrenceRule ;1 -Exception Rule
 * @return boolean
 *
 *function used to decide the rule type
 */
bool CRecurrenceRule::setRuleType(RuleType iType)
{
    this->iType = iType;
    return true;
}


/**
 * getRuleType
 * @param NONE
 * @return int type of recurrence rule 0 - RecurrenceRule ;1 -Exception Rule
 * function used to retrieve rule type from CRecurrenceRule object
 *
 */
RuleType CRecurrenceRule::getRuleType()
{

    return this->iType;
}

string CRecurrenceRule::toString()
{
    string szRet;
    std::stringstream ss;
    ss << "Count=";
    ss << iCount ;
    ss << " ,Interval=";
    ss << interval;
    ss << ",Until=";
    ss << iUntil;
    ss << ",Rule=";
    ss << getRrule();
    ss << ",WeekStart=";
    ss << getWeekStart();
    ss << ",Freq=";
    ss << getFrequency();

    if((vBySecond).size() > 0) {
        ss << ",BySec=";
        vector<short>::iterator it;

        for(it = vBySecond.begin(); it != vBySecond.end(); it++) {
            ss << *it;
            ss << SEMI_COLON;
        }
    }

    if((vByMinute).size() > 0) {
        ss << ",ByMin=";
        vector<short>::iterator it;

        for(it = vByMinute.begin(); it != vByMinute.end(); it++) {
            ss << (*it);
            ss << SEMI_COLON;
        }
    }

    if((vByHour).size() > 0) {
        ss << ",ByHour=";
        vector<short>::iterator it;

        for(it = vByHour.begin(); it != vByHour.end(); it++) {
            ss << (*it);
            ss << SEMI_COLON;
        }
    }

    if((vByWeekDay).size() > 0) {
        ss << ",WeekDay=";
        vector<short>::iterator it;

        for(it = vByWeekDay.begin(); it != vByWeekDay.end(); it++) {
            ss << (*it);
            ss << SEMI_COLON;
        }
    }

    if((vByMonthDay).size() > 0) {
        ss << ",MonthDay=";
        vector<short>::iterator it;

        for(it = vByMonthDay.begin(); it != vByMonthDay.end(); it++) {
            ss << (*it);
            ss << SEMI_COLON;
        }
    }

    if((vByYearDay).size() > 0) {
        ss << ",YearDay=";
        vector<short>::iterator it;

        for(it = vByYearDay.begin(); it != vByYearDay.end(); it++) {
            ss << (*it);
            ss << SEMI_COLON;
        }
    }

    if((vByWeekNumber).size() > 0) {
        ss << ",WeekNo=";
        vector<short>::iterator it;

        for(it = vByWeekNumber.begin(); it != vByWeekNumber.end(); it++) {
            ss << (*it);
            ss << SEMI_COLON;
        }
    }

    if((vBySetPos).size() > 0) {
        ss << ",Pos=";
        vector<short>::iterator it;

        for(it = vBySetPos.begin(); it != vBySetPos.end(); it++) {
            ss << *it;
            ss << SEMI_COLON;

        }
    }

    szRet.append(ss.str());
    return szRet;

}
