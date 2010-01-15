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
#include <libical/ical.h>
#include "Common.h"

/**
 * setRecurrenceRule
 * @param vector CRecurrenceRule* pointers to CRecurrenceRule object
 * @return boolean status of the operation
 * Function used to set CRecurrrencRule in to 
 * CRecurrence(this) class
 */
bool CRecurrence::setRecurrenceRule(vector < CRecurrenceRule * >vRRule)
{
    if (vRRule.size() == 0) {
    return false;
    }
    /* already existing list of CRecurrence Rule objects should 
     * be added to the current one
     */
    unsigned int icount = 0;

    for (icount = 0; icount < vRRule.size(); icount++)
    this->vRecrRuleList.push_back(vRRule[icount]);

    return true;

}

 /**
  * checkInstances()
  * @param iviewBegin lower range of current view 
  * @param iviewEnd higher range of current view 
  * @param ieventSpan diff between date start and date end of original instance  
  * @return true or false
  *
  * Function used to check if any instance of this event occurs in the given interval 
  */
  bool CRecurrence::checkInstances(time_t iViewBegin, 
            time_t iViewEnd,
            time_t iDateStart,
            int eventSpan,
            int is_day,
            string sTzid)
{
    vector<time_t> instance_times;

    generateInstanceTimes(iViewBegin, iViewEnd, iDateStart, eventSpan, is_day, sTzid, instance_times, false);

    return (instance_times.size() > 0);
}

/**
 * generateInstanceTimes()
 * @param viewBegin lower range of current view 
 * @param viewEnd higher range of current view 
 * @param int iEventSpan diff between datestart and dateend of original instance  
 * @return vector <time_t> values on which event has to happen
 *
 * Function used to retrieve the recurrence dates 
 */
vector < time_t > CRecurrence::generateInstanceTimes(time_t iViewBegin,
                             time_t iViewEnd,
                             time_t iDateStart,
                             int ieventSpan,
			     int is_day,
			     string sTzid)
{
    CAL_DEBUG_LOG("CRecurrence::generateInstanceTimes : DEPRECATED version of function");

    vector<time_t> instances;
    generateInstanceTimes(iViewBegin, iViewEnd, iDateStart, ieventSpan, is_day, sTzid, instances);

    return instances;
}

void CRecurrence::generateInstanceTimes(time_t iViewBegin,
                                    time_t iViewEnd,
                                    time_t iDateStart,
                                    int ieventSpan,
                                    int is_day,
                                    string sTzid,
                                    vector<time_t> &instance_times,
                                    bool sort_times/* = true*/)
{
    vector < string > rrules;
    vector < string > erules;
    vector < string >::iterator siter;
    vector < time_t > etimes;
    vector < time_t > temp;
    int pErrorCode = 0 ;
    temp.clear();
    instance_times.clear();
    etimes.clear();
    rrules.clear();
    erules.clear();
    is_day = 0;

    
    CAL_DEBUG_LOG("CRecurrence::generateInstanceTimes : begin %s ",ctime(&iViewBegin));
    CAL_DEBUG_LOG("CRecurrence::generateInstanceTimes : end   %s ",ctime(&iViewEnd));
	
    /* If ViewEnd is 0 it implies that it is OK to 
     * expand recurrence 1 instance beyond view start 
     * so to keep the logic same making viewStart as
     * view end*/
    if (iViewEnd == 0)
	   iViewEnd = iViewBegin;
        
    /* return a empty list if there is no 
     * recurrence defined for the event yet this 
     * API is called 
     */
    CMulticalendar *mc = CMulticalendar::MCInstance();
    /* do this when timezone is not empty*/
    if (sTzid.empty() == 0)
	sTzid = sTzid.substr(1);
   
    icaltimezone *pTz = 0 ;
    pTz=icaltimezone_get_builtin_timezone(sTzid.c_str());
     
    icaltimetype timeIcal = icaltime_from_timet_with_zone( 
    		    (iDateStart ) ,is_day,pTz);
    
    rrules = this->getRrule();
    for (siter = rrules.begin(); siter != rrules.end(); siter++) {
        mc->getRecurrentTimes((*siter),
                              timeIcal,
                              ieventSpan,
                              iViewBegin,
                              iViewEnd,
                              pTz,
                              temp,
                              pErrorCode);
        instance_times.insert(instance_times.end(), temp.begin(), temp.end());
        temp.clear();
    }

    erules = this->getErule();
    if (erules.size() != 0) {
    for (siter = erules.begin(); siter != erules.end(); siter++) {
        CAL_DEBUG_LOG("exceptionis %s \n", (*siter).c_str());
        mc->getRecurrentTimes((*siter),
                              icaltime_from_timet_with_zone(iDateStart,is_day,pTz),
                              ieventSpan,
                              iViewBegin,
                              iViewEnd,
                              pTz,
                              temp,
                              pErrorCode);
        etimes.insert(etimes.end(), temp.begin(), temp.end());
        temp.clear();
    }

    } else
    CAL_DEBUG_LOG("SIZE of the exceptions list is zero\n");

    /*convert and copy all rdates 
     * to a vector of time_t
     */
    vector < string > recRuleList;
    recRuleList.clear();

    recRuleList = this->getRDays();
    if (recRuleList.size()) {
    unsigned int iter = 0;
    time_t temp = 0;
    vector < time_t > rtimesNoRule;
    rtimesNoRule.clear();
    for (iter = 0; iter < recRuleList.size(); iter++) {

        temp = (time_t) atoi(recRuleList[iter].c_str());
        rtimesNoRule.push_back(temp);
    }
    /*convert and append in to existing list */
    instance_times.insert(instance_times.end(), rtimesNoRule.begin(),
              rtimesNoRule.end());
    }

    /*convert and copy all exception dates 
     * to a vector of time_t
     */
    vector < string > exceptionDateList;
    exceptionDateList.clear();
    exceptionDateList = this->getEDays();
    if (exceptionDateList.size()) {
    unsigned int iter1 = 0;
    time_t temp = 0;
    vector < time_t > etimesNoRule;
    etimesNoRule.clear();
    for (iter1 = 0; iter1 < exceptionDateList.size(); iter1++) {

        temp = (time_t) atoi(exceptionDateList[iter1].c_str());
        etimesNoRule.push_back(temp);
    }

    etimes.insert(etimes.end(), etimesNoRule.begin(),
              etimesNoRule.end());
    }


    /* Check if datestart needs to be included 
     * before we filter out exception dates as 
     * datestart can also be one of exception */

    /* after finding all the dates we need to see if dt start is 
     * a part of the recurrence if not add it to the recurrence 
     * list */
    unsigned int i = 0;
    bool found = false;
    while (i < instance_times.size() )
    {	
	    if ( instance_times[i] == iDateStart )
	    {
		found = true;   
		break;
	    }
	    i++;
    }
    /* finding the dtstart and pushing it in to 
     * the recurrence list if it is not present */
    if (!found && (iViewBegin <= iDateStart && iDateStart <= iViewEnd)) {
       CAL_DEBUG_LOG(" \n %s \n",ctime(&iDateStart));
	   instance_times.push_back(iDateStart);
    }

    /* Removing Exception times */
    bool  timet_isday = false;
    vector < time_t >::iterator count1;
    vector < time_t >::iterator count2;
    for (count1 = etimes.begin(); count1 < etimes.end(); count1++)
    {    
	    timet_isday = false; 
	    struct icaltimetype exIcal;
	    memset (&exIcal, 0, sizeof (struct icaltimetype));
	    exIcal = icaltime_from_timet_with_zone((*count1) ,0,pTz);
	    if(exIcal.hour == 0 && exIcal.minute == 0  && exIcal.second == 0)
	    {
		    timet_isday = true; 
		    CAL_DEBUG_LOG("Exception time is date format not date+time format");
	    }
	    
	    for (count2 = instance_times.begin(); count2 < instance_times.end(); ) {

		    if (((*count1) == (*count2)) || 
		    (((*count1  <= *count2) && ( *count2 <= *count1+86399)) && timet_isday))  {
			    CAL_DEBUG_LOG("Found exception time %ld\n", (*count1));
			    count2 = instance_times.erase(count2);
			    
		    }
		    else
			    count2++;

	    }
    }


 /* sorting the vector of timestamps 
  Sorting is required in Next prev Logic */
  if (sort_times) {
    sort(instance_times.begin(),instance_times.end());
  }
}

/**
 * getRecurrenceRule
 * @param NONE
 * @return vector CRecurrenceRule* pointer to CRecurrenceRule object
 * Function used to retrieve CRecurrrencRule from 
 * CRecurrence(this) class
 */
vector < CRecurrenceRule * >CRecurrence::getRecurrenceRule()
{

    return (this->vRecrRuleList);

}

/**
 * setRecurId
 * Function to set the exception Recursive days of a recursive event,todo or Journal
 * 
 * @param  int recurrence id    
 * 
 * @return bool (TRUE/FALSE)
 */
bool CRecurrence::setRecurId(int iRecurId)
{
    if (iRecurId < 0)
    return false;

    this->iRecurId = iRecurId;
    return true;

}


/**
 * getRecurId
 * Function to set the exception Recursive days of a recursive event,todo or Journal
 * 
 * @param  none
 * 
 * @return int Recurrence ID
 */
int CRecurrence::getRecurId()
{
    return iRecurId;
}

/**
 * getErule
 * Function to get the recurrence rule of a recursive event.
 * @param none
 * @return vector string type of calendar recurrence rule
 * only exception rules are retrieived here 
 * 
 */
vector < string > CRecurrence::getErule()
{
    vector < string > stringrules;
    stringrules.clear();
    if (vRecrRuleList.size() == 0) {
    /* return empty list of strings */
    return stringrules;
    }
    vector < CRecurrenceRule * >::iterator iter;
    for (iter = vRecrRuleList.begin(); iter != vRecrRuleList.end(); iter++) {
        /**
         * this will call the getErule function in
         * CRecurrenceRule object
         */
    /* 1 is for exrule and 0 is for rrule */
    if ((*iter)->getRuleType() == 1)
        stringrules.push_back((*iter)->getRrule());
    }

    return stringrules;
}

/**
 * getRrule
 * Function to get the recurrence rule of a recursive event.
 * @param none
 * @return vector string type of calendar recurrence rule
 * only recursive rules are retrieived here 
 * 
 */
vector < string > CRecurrence::getRrule()
{
    vector < string > stringrules;
    stringrules.clear();
    if (vRecrRuleList.size() == 0) {
    /* return empty list of strings */
    return stringrules;
    }
    vector < CRecurrenceRule * >::iterator iter;
    for (iter = vRecrRuleList.begin(); iter != vRecrRuleList.end(); iter++) {
                /**
                 * this will call the getRrule function in
                 * CRecurrenceRule object
                 */
    /* 1 is for exrule and 0 is for rrule */
    if ((*iter)->getRuleType() == 0)
        stringrules.push_back((*iter)->getRrule());
    }

    return stringrules;
}


/**
 * setRrule
 * Function to set the recurrence rule of a recursive event,todo
 * or Journal
 * @param  vector string type of calendar recurrence rule    
 * @return bool to indicate operation is successful or not 
 */
bool CRecurrence::setRrule(vector < string > vRules)
{

    /*
     * 0- for recurrence RULE 
     * 1- for exception RULE
     */
    if (vRules.size() == 0)
    return false;
    vector < string >::iterator iter;
    for (iter = vRules.begin(); iter != vRules.end(); iter++) {
    CRecurrenceRule *temp = new CRecurrenceRule(0, (*iter));
    ASSERTION(temp);
    vRecrRuleList.push_back(temp);
    }
    return true;

}


/**
 * setErule
 * Function to set the Exception recurrence rule of a recursive event,todo or Journal
 * Although not supposed to be useful from calendar might be significant from sync point of view.
 * 
 * @param  vector <string> type of calendar exception recurrence rule    
 * 
 * @return bool to indicate operation is successful or not 
 */
bool CRecurrence::setErule(vector < string > vRules)
{


    /*
     * 0- for recurrence RULE 
     * 1- for exception RULE
     */
    if (vRules.size() == 0)
    return false;
    vector < string >::iterator iter;
    for (iter = vRules.begin(); iter != vRules.end(); iter++) {
    CRecurrenceRule *temp = new CRecurrenceRule(1, (*iter));
    ASSERTION(temp);
    vRecrRuleList.push_back(temp);
    }
    return true;

}

/**
 * setRDays
 * Function to set the Recursive days of a recursive event,todo or Journal
 * Although not supposed to be useful from calendar might be significant from sync point of view.
 * 
 * @param  vector type of calendar recurrence dates    
 * 
 * @return bool to indicate operation is successful or not 
 */
bool CRecurrence::setRDays(vector < string > vRecRuleList)
{
    if (vRecRuleList.size() != 0) {
    this->vRecRuleList = vRecRuleList;
    return true;
    }
    return false;

}


/**
 * getRDays()
 * Function to get the Recursive days of a recursive event,todo or Journal
 * Although not supposed to be useful from calendar might be significant from sync point of view.
 * 
 * @param  none
 * 
 * @return vector type of calendar recurrence dates    
 */

vector < string > CRecurrence::getRDays()
{
    return vRecRuleList;

}

/**
 * setEDays()
 * @param  vector type of calendar exception recurrence dates    
 * @return bool to indicate operation is successful or not 
 *
 * This function will be used to set the exception Recursive days of a recursive event,todo or Journal
 *  
 */
bool CRecurrence::setEDays(vector < string > vExceptionDateList)
{
    if (vExceptionDateList.size() != 0) {
    this->vExceptionDateList = vExceptionDateList;
    return true;
    } else
    return false;
}

/**
 * getEDays()
 * @param  none
 * @return vector type of calendar exception recursive dates    
 *
 * This function will be used to get the exception days of a recursive event,todo or Journal
 */

vector < string > CRecurrence::getEDays()
{

    return vExceptionDateList;

}

/**
 * overloaded assignment operator 
 */
CRecurrence & CRecurrence::operator=(const CRecurrence & right)
{

    //prevent self assignment 
    if (&right != this) {

    /* we cannot assign the values 
     * because it is a list of pointers 
     * to create a copy of object we need to 
     * copy objects explicitly
     */
    unsigned int iter;
    for (iter = 0; iter < right.vRecrRuleList.size(); iter++) {
        CRecurrenceRule *temp =
        new CRecurrenceRule(right.vRecrRuleList[iter]->
                    getRuleType(),
                    right.vRecrRuleList[iter]->getRrule());
    	ASSERTION(temp);
        vRecrRuleList.push_back(temp);

    }
    vExceptionDateList = right.vExceptionDateList;
    vRecRuleList = right.vRecRuleList;
    iRecurId = right.iRecurId;
    iRType = right.iRType;
    }
    return *this;
}

/**
 * CRecurrence:: CRecurrence(CRecurrence  & ref)
 * copy constructor for CRecurrence class
 */
CRecurrence::CRecurrence(CRecurrence & ref)
{
    vector < CRecurrenceRule * >::iterator iter;
    for (iter = ref.getRecurrenceRule().begin(); iter != ref.getRecurrenceRule().end(); iter++) {
        CRecurrenceRule *temp = new CRecurrenceRule((*iter)->getRuleType(), (*iter)->getRrule());
        ASSERTION(temp);
        vRecrRuleList.push_back(temp);
    }
    iRecurId = ref.iRecurId;
    vExceptionDateList = ref.vExceptionDateList;
    vRecRuleList = ref.vRecRuleList;
    iRType = ref.iRType;

}

/**
 * CRecurrence
 * This is overloaded constructor for recurrence class
 * 
 * @param  Rrule recurrence rule
 * @param  int recurrence id.
 */
CRecurrence::CRecurrence(vector < string > vRRule, int iRecurId):iRecurId
    (iRecurId)
{
    if (vRRule.size() != 0) {

    this->setRrule(vRRule);
    }
    iRType = 0;
}

/**
 * CRecurrence()
 * This is default constructor for recurrence class
 */
CRecurrence::CRecurrence():iRecurId(-1)
{
    iRType = 0;
}

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
bool CRecurrence::setRtype(int iRType)
{
    this->iRType = iRType;
    return true;
}

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
int CRecurrence::getRtype()
{
    return iRType;
}

/**
 * ~CRecurrence()
 * This is destructor for recurrence class
 */
CRecurrence::~CRecurrence()
{
    unsigned int iIter;

    for (iIter = 0; iIter < vRecrRuleList.size(); iIter++) {
    delete(vRecrRuleList[iIter]);
    vRecrRuleList[iIter] = 0;
    }
    vExceptionDateList.clear();
    vRecRuleList.clear();

}

string CRecurrence::toString()
{
    string szRet;
    std::stringstream ss;
    ss << "Recurrence type=";
    ss << iRType ;
    ss << ",Recurrence ID=";
    ss << iRecurId;

    if((vRecRuleList).size()>0)
    {
	ss << ",Rule=";
	vector<string>::iterator it;
	for(it=vRecRuleList.begin();it!=vRecRuleList.end();it++){
	    ss << *it;
	    ss << SEMI_COLON;
	}
    }
    if((vExceptionDateList).size()>0)
    {
	ss << ",ExceptionDate=";
	vector<string>::iterator it;
	for(it=vExceptionDateList.begin();it!=vExceptionDateList.end();it++){
	    ss << *it;
	    ss << SEMI_COLON;
	}
    }
    if((vRecrRuleList).size()>0)
    {
	ss << ",RecRuleList=";
	vector<CRecurrenceRule *>::iterator it;
	for(it=vRecrRuleList.begin();it!=vRecrRuleList.end();it++){
	    ss << *it;
	    ss << SEMI_COLON;
	}
    }

    szRet.append(ss.str());
    return szRet;

}
