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

#ifndef __RECURRENCE_RULE_H__
#define __RECURRENCE_RULE_H__

/* Headers */
#include<libical/ical.h>
#include "DbCalls.h"
#include<vector>
#include <string>
using std::string;
using std::vector;

/* Enum definitions */
enum FREQUENCY{

    SECONDLY_RECURRENCE = 0,
    MINUTELY_RECURRENCE = 1,
    HOURLY_RECURRENCE = 2,
    DAILY_RECURRENCE = 3,
    WEEKLY_RECURRENCE = 4,
    MONTHLY_RECURRENCE = 5,
    YEARLY_RECURRENCE = 6,
    NO_RECURRENCE = 7
};

typedef enum {
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
} WeekDay;

typedef enum {
    RECURRENCE_RULE = 0,
    EXCEPTION_RULE
} RuleType;

/**
 * CRecurrenceRule class
 * @brief This class contains the information about Recurrence rule for particular component
 */
class CRecurrenceRule {

  public:

    /**
     * Copy Constructor 
     */
    CRecurrenceRule(CRecurrenceRule & ref);

    /**
     * overloaded assignment operator
     */
    CRecurrenceRule & operator=(CRecurrenceRule & right);

    /**
     * Default constructor 
     */
    CRecurrenceRule();

    /**
     * Destructor of the class
     */
    ~CRecurrenceRule();

    /**
     * setRuleType
     * @param RuleType  type of recurrence rule 0 - RecurrenceRule ;1 -Exception Rule 
     * @return boolean
     *     
     *function used to decide the rule type
     */
    bool setRuleType(RuleType iType);

    /**
     * getRuleType
     * @param NONE
     * @return RuleType type of recurrence rule 0 - RecurrenceRule ;1 -Exception Rule 
     * 
     * function used to retrieve rule type from CRecurrenceRule object
     * 
     */
    RuleType getRuleType();

    /**
     * @param string szRule  recurrence rule, can be recurrence rule or exception 
     *            rule
     * @return      bool status of the operation 
     *
     * Function used to parse the recurrence rule and populate 
     * the fields in to private members of this class
     * All other set methods defined in this class will be called inside this 
     * procedure 
     */
    bool rruleParser(string szRule);

    /**
     * @param : string szRRule - recurrence rule / exception rule 
     * @return : bool whether  the operation is successful or not 
     * 
     * Function to set recurrence rule in class place holder 
     */
    bool setRrule(string szRRule);

    /**
     * @param : NONE
     * @return : string recurrence rule 
     * 
     * Function used to retrieve recurrence values from class  members
     */
    string getRrule();

    /**
     * @param  FREQUENCY : ivaluea frequebncy can be daily,monthly etc, valid values 
     *                     defined in FREQUENCY enumeration.
     * @return bool: status
     *  
     * to set FREQUENCY of occurrence
     */
    bool setFrequency(FREQUENCY iValue);

    /**
     * @param NONE
     * @return FREQUENCY of occurrence 
     * 
     * function returns FREQUENCY parameter set in the class
     */
    FREQUENCY getFrequency();

    /**
     * @param time_t : until value in rrule as time_t
     * @return bool : status of operation performed
     *
     * function used to set unitl time in CRecurrencRule object 
     */
    bool setUntil(time_t iUntil);

    /**
     * @param : NONE
     * @return unitl  :time_t  status of operation performed
     *
     * function used to set until time in CRecurrencRule object 
     */
    time_t getUntil();

    /**
     * @param : integer Count of occurances 
     * @return : boolean status of operation performed
     *
     * Function used to set MAX count of instances 
     */
    bool setCount(int iCount);

    /**
     * @param : NONE 
     * @return : integer count of occurances 
     *
     * Function used to retrieve  MAX count of instances 
     */
    int getCount();

    /**
     * @param vector<short>: seconds list on which the events should occur
     * @return boolean: staus of the operation
     *
     * Function used to set seconds in list of seconds appearing in 
     * Recurrence rule 
     */
    bool setSecond(vector < short > vSec);

    /**
     * @param NONE 
     * @return vector<short>: seconds list on which the events should occur
     *
     * Function used to get seconds list  in 
     * CRecurrenceRule Class  
     */
    vector < short >getSecond();

    /**
     * @param vector<short>: Minute list on which the events should occur
     * @return boolean: staus of the operation
     *
     * Function used to set Minutes appearig in Recurrence rule to 
     * CRecurrenceRule class
     * 
     */
    bool setMinute(vector < short > vMinute);

    /**
     * @return vector<short>: Minute list on which the events should occur
     * @param NONE
     *
     * Function used to get Minutes in CRecurrenceRule class
     * 
     */
    vector < short >getMinute();

    /**
     * @param vector<short>: Hour list on which the events should occur
     * @return boolean: staus of the operation
     *
     * Function used to set Hours  appearing in  Recurrence rule to CRecurrenceRule 
     * class 
     */
    bool setHour(vector < short > vHour);

    /**
     * @param NONE
     * @return vector<short>: Hour list on which the events should occur
     *
     * Function used to get Hours   in   CRecurrenceRule 
     * class 
     */
    vector < short >getHour();

    /**
     * @param vector<short>: WeekDay list on which the events should occur
     * @return boolean: staus of the operation
     *
     * Function used to set WeekDays SU.MO,TU etc  appearing in 
     * Recurrence rule to CRecurrenceRule class
     * 
     */
    bool setWeekDay(vector < short > vByWeekDay);

    /**
     * @param NONE
     * @return vector<short>: WeekDay list on which the events should occur
     *
     * Function used to get WeekDays SU.MO,TU etc in 
     *  CRecurrenceRule class
     * 
     */
    vector < short >getWeekDay();

    /**
     * @param vector<short>: MonthDay list on which the events should occur
     * @return boolean: staus of the operation
     *
     * Function used to set Month Days appearing in  Recurrence rule to CRecurrenceRule 
     */
    bool setMonthDay(vector < short > vByMonthDay);

    /**
     * @param NONE
     * @return vector<short>: YearDay list on which the events should occur
     *
     * Function used to Retrieve MonthDay appearing in  CRecurrenceRule class
     */
    vector < short >getMonthDay();

    /**
     * @param vector<short>: YearDay list on which the events should occur
     * @return boolean: staus of the operation
     *
     * Function used to set YearDay appearing in  Recurrence rule to CRecurrenceRule 
     * class Eg:  RRULE:FREQ=YEARLY;INTERVAL=3;COUNT=10;BYYEARDAY=1,100,200 
     */
    bool setYearDay(vector < short > vByYearDay);

    /**
     * @param NONE
     * @return vector<short>: YearDay list on which the events should occur
     *
     * Function used to Retrieve YearDay appearing in  CRecurrenceRule class
     */
    vector < short >getYearDay();

    /**
     * @param vector<short>: WeekNumeber list on which the events should occur
     * @return boolean: staus of the operation
     *
     * Function used to set WeekNumber  appearing in 
     * Recurrence rule to CRecurrenceRule class
     */
    bool setWeekNumber(vector < short > vByWeekNumber);

    /**
     * @param NONE
     * @return vector<short>: WeekNumeber list on which the events should occur
     *
     * Function used to get WeekNumber  appearing in 
     * CRecurrenceRule class
     */
    vector < short >getWeekNumber();

    /**
     * @param vector<short>: Month list on which the events should occur
     * @return boolean: staus of the operation
     *
     * Function used to set Month list   appearing in 
     * Recurrence rule to CRecurrenceRule class
     */
    bool setMonth(vector < short > vMonth);

    /**
     * @param NONE
     * @return vector<short>: Month list on which the events should occur
     *
     * Function used to get Month list   appearing in 
     * CRecurrenceRule class
     */
    vector < short >getMonth();

    /**
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
    bool setPos(vector < short > vPosition);

    /**
     * @param NONE
     * @return vector<short>: Position list on which the events should occur
     *
     * Function used to get Position list from CRecurrenceRule class
     */
    vector < short >getPos();

    /**
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
    bool setWeekStart(icalrecurrencetype_weekday start);

    /**
     * @ int : weekstart  offset day from which week starts in Calendar 
     *        in our calendar it is either SUNDAY or MONDAY        
     * @return boolean: staus of the operation
     *
     * Function used to get week start day from CRecurrenceRule class
     */
    icalrecurrencetype_weekday getWeekStart();

    /**
     * @param short : interval between two instances  
     * @return      : bool status of the operation
     * Function used to set the interval param
     */
    bool setInterval(short interval);

    /**
     * @param NONE
     * @return short: interval between instances 
     *
     * Function used to retrieve interval parameter
     */

    short getInterval();
    /**
     * toString
     */
    string toString();


    /**
     * overloaded Constructor 
     * type 0-rrule ,1 -exrule 
     */
    CRecurrenceRule(int iType, string szRRule);

    /**
     * Function used for checking whether rule is complex
     */
    bool checkRuleIsComplex();
  private:
    /**
     * @param array of short 
     * @return vector of short;
     *
     * function to convert array to vector 
     */

    vector < short > convertArrayToVector(short array[], int iLengthMax);
    
    FREQUENCY iFreq; /*!< Frequency */
    
    string szRule; /*!< RRule or Exception Rule */ 

    int iCount; /*!<integer value */

    short interval; /*!< interval appearing in rrule */ 

    time_t iUntil; /*!< Until value */

    RuleType iType; /*!< Rule type */


    /**
     * week start as per rfc Ical is 
     * week day  from which recuurenc
     * has to start. Our Calendar app supports SU
     * or MO for week start. 
     */

    icalrecurrencetype_weekday weekStart;

    //week start as per rfc Ical is 
    //week day  from which recuurenc
    // has to start. Our Calendar app supports SU
    // or MO for week start. 
    vector < short >vBySecond; /*!< (0-60) second values */

    vector < short >vByMinute; /*!< (0-60) Minute values */

    vector < short >vByHour; /*!< (0-24) Hour values */

    vector < short >vByWeekDay; /*!< (SU,MO,TU,WE,TH,FR,ST) WeekDay values */

    vector < short >vByMonthDay; /*!< (1-31) Month values */

    vector < short >vByYearDay; /*!< (0-365) Year day values */

    vector < short >vByWeekNumber; /*!< (1-52) Week Number values */

    vector < short >vByMonth; /*!< (1-12) Month values */
    
    vector < short >vBySetPos; /*!< (1-366) SetPosition values */
};

#endif
