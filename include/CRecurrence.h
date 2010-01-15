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

#ifndef __RECURRENCE_H__
#define __RECURRENCE_H__

/* Headers */
#include<vector>
#include <string>
using std::string;
using std::vector;

#include "CRecurrenceRule.h"

/**
 * CRecurrence
 * @brief This contains the recurrence information
 */

class CRecurrence {

  public:

    /**
     * setRecurrenceRule
     * @param vector <CRecurrenceRule*> pointer to CRecurrenceRule object
     * @return boolean status of the operation
     * Function used to set CRecurrrencRule in to 
     * CRecurrence(this) class
     */
    bool setRecurrenceRule(vector < CRecurrenceRule * >VRRuleList);

    /**
     * getRecurrenceRule
     * @param NONE
     * @return vector <CRecurrenceRule*> pointer to CRecurrenceRule object
     * Function used to retrieve CRecurrrencRule from 
     * CRecurrence(this) class
     */
    vector < CRecurrenceRule * >getRecurrenceRule();

    /**
     * getErule
     * Function to get the recurrence rule of a recursive event.
     * @param none
     * @return vector string type of calendar recurrence rule
     * only exception rules are retrieived here 
     * 
     */
    vector < string > getErule();

    /**
     * getRrule()
     * @param none
     * @return vector string type of calendar recurrence rule
     *
     * This function will be used to get the recurrence rule of a recursive event
     * this list will include both recurrence rules and exception rules 
     * can be differentiated by type 
     */
    vector < string > getRrule();

    /**
     * CRecurrence(CRecurrence  & ref)
     * copy constructor for CRecurrence class
     */
    CRecurrence(CRecurrence & ref);

    /**
     * setRrule()
     * @param  string type of calendar recurrence rule    
     * @return bool to indicate operation is successful or not 
     *
     * This function will be used to set the recurrence rule of a recursive event,todo
     * or Journal
     */
    bool setRrule(vector < string > vRRuleList);

    /**
     * setErule()
     * @param  string type of calendar exception recurrence rule    
     * @return bool to indicate operation is successful or not 
     *
     * This function will be used to set the Exception recurrence rule of a recursive event,todo or Journal
     * Although not supposed to be useful from calendar might be significant from sync point of view.
     * 
     */
    bool setErule(vector < string > vERuleList);

    /**
     * setRDays()
     * @param  vector type of calendar recurrence dates    
     * @return bool to indicate operation is successful or not 
     *
     * This function will be used to set the Recursive days of a recursive event,todo or Journal
     * Although not supposed to be useful from calendar might be significant from sync point of view.
     * 
     */
    bool setRDays(vector < string > vRecRuleList);

    /**
     * getRDays()
     * @param  none
     * @return vector type of calendar recurrence dates    
     *
     * This function will be used to get the Recursive days of a recursive event,todo or Journal
     * Although not supposed to be useful from calendar might be significant from sync point of view.
     * 
     */
    vector < string > getRDays();

    /**
     * setEDays()
     * @param  vector type of calendar exception recurrence dates    
     * @return bool to indicate operation is successful or not 
     *
     * This function will be used to set the exception Recursive days of a recursive event,todo or Journal
     *  
     */
    bool setEDays(vector < string > vExceptionDateList);

    /**
     * getEDays()
     * @param  none
     * @return vector type of calendar exception recursive dates    
     *
     * This function will be used to get the exception days of a recursive event,todo or Journal
     * 
     * 
     */
    vector < string > getEDays();

    /**
     * setRecurId()
     * @param  int recurrence id    
     * @return bool to indicate operation is successful or not 
     *
     * This function will be used to set the exception Recursive days of a recursive event,todo or Journal
     *  
     */
    bool setRecurId(int iRId);

    /**
     * getRecurId()
     * @param  none
     * @return int recurrence id.
     *
     * This function will be used to set the exception Recursive days of a recursive event,todo or Journal
     *  
     */
    int getRecurId();
    /**
     * setRtype
     * @param : int Type of recurrence  .
     * @return : bool to indicate the result of operation.
     *  
     * This function is used to set the Recurrence type  value in Recurrence object.
     * It is used for recurrence events and todos. It can take values like 
     * 'BY_DAILY' =1
     * 'BY_WEEKLY' =2
     * 'BY_MONTHLY'=3 and so on.
     * For external events/todos the value should be set to 0.
     */
    bool setRtype(int iRType);
    /**
     *  getRtype
     *  @param : NONE.
     *  @return : Int Type of internal recurrence.
     *   
     *  This function is used to get the Recurrence type  value in Recurrence object.
     *  It is used for recurrence events and todos. It can take values like 
     *  'BY_DAILY' =1
     *  'BY_WEEKLY' =2
     *  'BY_MONTHLY'=3 and so on.
     *  For external events/todos the value should be set to 0.
     */
    int getRtype();

    /**
     * CRecurrence(string,int)
     * @param  vector <string> Rrule recurrence rule
     * @param  int recurrence id.
     * 
     * This is overloaded constructor for recurrence class
     *  
     */
    CRecurrence(vector < string > vRRuleList, int iRId);
    /**
     * generateInstanceTimes()
     * @param iviewBegin lower range of current view 
     * @param iviewEnd higher range of current view 
     * @param ieventSpan diff between date start and date end of original instance  
     * @return vector <time_t> values on which event has to happen
     *
     * Function used to retrieve the recurrence dates 
     * !!! DEPRECATED !!!
     */
    vector < time_t > generateInstanceTimes(time_t iViewBegin, 
            time_t iViewEnd,
            time_t iDateStart,
            int eventSpan,
            int is_day,
            string sTzid);

    /**
     * generateInstanceTimes()
     * @param iviewBegin lower range of current view 
     * @param iviewEnd higher range of current view 
     * @param ieventSpan diff between date start and date end of original instance  
     * @param instance_times values on which event has to happen
     * @param sort_times     true if instance times must be sorted
     *
     * Function used to retrieve the recurrence dates 
     */
    void generateInstanceTimes(time_t iViewBegin,
            time_t iViewEnd,
            time_t iDateStart,
            int eventSpan,
            int is_day,
            string sTzid,
            vector<time_t> &instance_times,
            bool sort_times = true);
    /**
     * checkInstances()
     * @param iviewBegin lower range of current view 
     * @param iviewEnd higher range of current view 
     * @param ieventSpan diff between date start and date end of original instance  
     * @return true if an instance found
     *
     * Function used to check if any instance of this event occurs in the given interval 
     */
    bool checkInstances(time_t iViewBegin, 
            time_t iViewEnd,
            time_t iDateStart,
            int eventSpan,
            int is_day,
            string sTzid);

    /**
     * CRecurrence()
     * 
     * This is default constructor for recurrence class
     *  
     */
    CRecurrence();

    /* overloaded assigment operator
     */
    CRecurrence & operator=(const CRecurrence & right);

    /**
     * ~CRecurrence()
     * 
     * This is destructor for recurrence class
     *  
     */
    ~CRecurrence();
    /**
     * toString
     */
    string toString();


  private:

    vector < CRecurrenceRule * >vRecrRuleList; /*!< Vector recurrence rule */

    vector < string > vExceptionDateList; /*!< Exception date list */

    vector < string > vRecRuleList; /*!< Recurrence rule list */

    int iRType; /*!< recurrence type */

    int iRecurId; /*!< Recurrence ID */


};

#endif
