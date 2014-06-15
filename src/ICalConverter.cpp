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
 * header files included in this file
 */
#include <string>
#include "ICalConverter.h"
#include "Common.h"
#include "CAlarm.h"
#include "CalendarErrors.h"
#include "VCalConverter.h"
#include <libical/icalderivedparameter.h>
#include "CUtility.h"
#include <sstream>
#include "CRecurrenceRule.h"
#include "CTimezone.h"

#define ICAL_PRODID "//Nokia Corporation//Maemo5 Calendar PR1.2//EN"

using namespace std;
using std::string;

struct zoneInfor{
    string location;
    string RruleDst;
    time_t dstStart;
};
/**
 * static variables used in this file */
 
static const int MAX_SUPPORTED_YEAR = 2037;

/** 
 * While importing we need to pick timezone information 
 * from different places for ics and vcs files 
 * For ICS format  TZID will be specified in DTSTART
 * For VCS format  TZID will be specified in CALENDAR component 
 *
 */
string vCalTimeZone ;

bool ical_value_is_valid (icalproperty *pProp)
{
    icalvalue *pVal = 0;
    pVal = icalproperty_get_value (pProp);

    return (icalvalue_is_valid (pVal));
}

/* function to fetch location based on offset */

string getLocation(int offsetstd,bool dstflag, time_t dstoffset,string szDaylight ){
    return CTimezone::getLocation(offsetstd, dstflag, dstoffset, szDaylight);
}

static string guessLocationFromVTimezone(string szVTimezone);

void addEncodingQuotedPrintable(icalproperty *pProp)
{
   icalparameter *pParam = 0;
   // add ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8
   pParam = icalparameter_new(ICAL_X_PARAMETER);
   if(pParam) {
        icalparameter_set_xname(pParam, "ENCODING");
        icalparameter_set_xvalue(pParam, "QUOTED-PRINTABLE");
        icalproperty_add_parameter(pProp, pParam);
   }
   pParam = icalparameter_new(ICAL_X_PARAMETER);
   if(pParam) {
        icalparameter_set_xname(pParam, "CHARSET");
        icalparameter_set_xvalue(pParam, "UTF-8");
        icalproperty_add_parameter(pProp, pParam);
   }
}
    
static bool 
stringReplaceAll (
        string        &toChange,
        const char   *changeThis,
        const char   *toThis);

static bool 
stringRemoveAll (
        string        &toChange,
        const char   *removeThis);

/**
 * constructor
 */
ICalConverter::ICalConverter()
{
     icalerror_set_error_state( ICAL_MALFORMEDDATA_ERROR,ICAL_ERROR_NONFATAL );
     icalerror_set_error_state( ICAL_BADARG_ERROR,ICAL_ERROR_NONFATAL );
     bSyncing = true;
}

/**
 * destructor
 */
ICalConverter::~ICalConverter()
{
    icalmemory_free_ring();
}

/**
 * Function to find the localtion from time offset of GMT 
 * from offset and daylight information we find the 
 * nearest possible match for city name from the 
 * timezone configuration table stored in the DB 
 *  
 */
void ICalConverter::findLocationFromOffset(string szDaylight)
{

/*
 if daylight is empty then return the city name 
 based on offset. 
 vCalTimeZone = TZ: +
 */
    time_t offset = 0 ;
    time_t offset_dst = 0;
    bool is_negative = false;
    
    /* vCalTimeZone = + 05:30   - 02:02 */
    /**
     * Calculating offset from VCalTimeZone value in VCS format */
    
    if ( vCalTimeZone.find("+") != string::npos)
        vCalTimeZone = vCalTimeZone.substr(vCalTimeZone.find("+") + 1);
    else {  
        if ( vCalTimeZone.find("-") != string::npos){
        	is_negative = true;
        	vCalTimeZone = vCalTimeZone.substr(vCalTimeZone.find("-") + 1);
        }
    }
    
    string szMinutes;

    if ( vCalTimeZone.find(":",0) != string::npos) {
        szMinutes  = vCalTimeZone.substr(vCalTimeZone.find(":") + 1);
            offset = atoi(szMinutes.c_str()) * 60;
    }

    offset = offset + atoi(vCalTimeZone.c_str()) * 3600 ;
    CAL_DEBUG_LOG("offset is %ld",offset);

    if (is_negative )
        offset = 0 - offset;

    /* now that we have the offset value  find the 
     * Daylight start date 
     */    
    
    if (szDaylight.empty())
    {
    /* picks first  city which has no DST and same offset 
     * */
     vCalTimeZone = getLocation(offset,0,0,szDaylight);

    }
    else{
        /* pick a city which has DST similiar to the one 
         * given in the VcalComponent*/
        VCalConverter vCalTemp;
        vector<string> szTokens;
        int icount = 0;
        
        szTokens = vCalTemp.getTokens(szDaylight,SEMI_COLON);

        CAL_DEBUG_LOG("Printing Daylight tokens : ");
        while (icount < (int)szTokens.size()){
        	CAL_DEBUG_LOG(" token %d is %s",icount,szTokens[icount].c_str());
        	++icount;
        }
        
        if (szTokens.size() >=3){

        	is_negative = false;
        	string dstStr = szTokens[1];

        	if ( dstStr.find("+") != string::npos)
        		dstStr = dstStr.substr(dstStr.find("+"));
        	else {
        		if ( dstStr.find("-") != string::npos){
        			is_negative = true;
        			dstStr = dstStr.substr(dstStr.find("-") + 1);
        		}
        	}

        	string szMinutes;

        	if ( dstStr.find(":",0) != string::npos) {
        		szMinutes  = dstStr.substr(dstStr.find(":") + 1);
        		offset_dst = atoi(szMinutes.c_str()) * 60;
        	}

        	offset_dst = offset_dst + atoi(dstStr.c_str()) * 3600 ;
    		if (is_negative )
        		offset_dst = 0 - offset_dst;


        }
        
        string szStartDst;
	
	if (szTokens.size() > 2 )
            szStartDst =  szTokens[2];
	else
	{
		CAL_DEBUG_LOG("Invalid size of tokens %d",szTokens.size());
	}
	/* This Api will find the daylight information in current year */
        findZoneStartTime(szStartDst, offset);	
        CAL_DEBUG_LOG("Daylight pattern follows the rule %s", szStartDst.c_str());

	/* pick a city from db corresponding to daylight obtained here */
	vCalTimeZone = getLocation(offset,1,offset_dst,szStartDst);

	if(vCalTimeZone.empty()){
		/* try to pick up city/timezone by offset only, ignoring daylight start rule */
			vCalTimeZone = getLocation(offset,1,offset_dst,"!");
	}
        /* If by any means input daylight string is incorrect then 
	 * ignore DST and pick a city with same offset */
	  
	if(vCalTimeZone.empty()){
	    /* Ignore DST and pick a city in case incorrect 
	     * data is sent via sync  , if  vCalTimeZone is 
	     * empty here also then we cannot do much ....
	     * import the event to local timezone*/
	     szStartDst.clear();
	     vCalTimeZone = getLocation(offset,0,0,szStartDst);	
         }
	

    }

    CAL_DEBUG_LOG("City Picked from the timezone configuration table is %s",vCalTimeZone.c_str());

}



/**
 * Function to find effective DST offset */
void ICalConverter::findEffectiveDSToffset(string szLine,string &szDaylight)
{
/* szLine contains DTSTART and
 * szDaylight Contains the daylight information 
 */
    time_t tempDateStart = 0;
    VCalConverter vCalTemp;
    vector<string> szTokens;
    int icount = 0;
    
    szLine = szLine.substr(szLine.find(":")+ 1);
    CAL_DEBUG_LOG("line after printing %s",szLine.c_str());
    tempDateStart = icaltime_as_timet(icaltime_from_string(szLine.c_str()));
    szTokens = vCalTemp.getTokens(szDaylight,SEMI_COLON);
    CAL_DEBUG_LOG("Printing the tokens");
    
    while (icount < (int)szTokens.size()){
    CAL_DEBUG_LOG(" token %d is %s",icount,szTokens[icount].c_str());    
        ++icount;
    }
    if ((int)szTokens.size() >= 4)
    {
        time_t  daylightStart = 0;
        time_t   daylightEnd = 0;
        
        daylightStart = icaltime_as_timet(icaltime_from_string(szTokens[2].c_str()));
        daylightEnd = icaltime_as_timet(icaltime_from_string(szTokens[3].c_str()));
        CAL_DEBUG_LOG("DST START %ld END %ld DTSTART %ld",daylightStart,daylightEnd,tempDateStart);
        if ((daylightStart  < tempDateStart) && 
        	( daylightEnd > tempDateStart))
        vCalTimeZone = "GMT" + szTokens[1];
               
    } 
    CAL_DEBUG_LOG("Effective timeZone   %s",vCalTimeZone.c_str());

    
        
}
/**
 * @param  szSpaceString 
 * @return NONE 
 * 
 * Function to strip all the white spaces 
 */
void ICalConverter::deleteWhiteSpaces(std::string &szSpaceString )
{
    string szTemp;
    string szLine;
    std::istringstream streamDescriptor(szSpaceString);
    size_t startpos = string::npos;

    /* getting line by line and adding \n
     * in the end*/
    while (getline(streamDescriptor, szLine)) {
     /* stripping  the empty spaces 
     */
        startpos = szLine.find_first_not_of(WHITE_SPACE);
        
        if(( string::npos != startpos ) )  
            szLine = szLine.substr( startpos );  

        if (!szLine.empty())
        szTemp = szTemp + szLine + NEWLINE;
    }
    szSpaceString = szTemp;
        CAL_DEBUG_LOG("szSpaceString is : %s\n",szSpaceString.c_str() );
   
}

/**
 * @param  string the input string value
 * @return  float 
 *
 * This function is used to convert string to float.
 */
float ICalConverter::stringToFloat(string szValue)
{
    float bRet;
    istringstream ss(szValue);
    ss >> bRet;
    return bRet;
}

/**
 * @param strIcalComp string
 * @param strVCalAlarmDate string
 * @param int iType
 * @return string
 *
 * This function converts alarm date to string format.
 */
string ICalConverter::addAlarmDateToString(string strIcalComp, 
                                   string strVCalAlarmDate,
        			   entryType iType )
{
    unsigned int startPos = 0;
    string temp;
    if (((startPos = strIcalComp.find(END_EVENT, 0)) != string::npos) || 
    ((startPos = strIcalComp.find(END_TODO, 0)) != string::npos)) {
    temp = strIcalComp.substr(0, startPos);
    }
    if (((startPos = strIcalComp.find(END_EVENT, 0)) != string::npos) || 
    ((startPos = strIcalComp.find(END_TODO, 0)) != string::npos)) {
    temp =
        temp + strVCalAlarmDate.substr(0, strVCalAlarmDate.size() - 1);
    }
    /* only for event and journal this procedure will be called */
    if (iType == E_EVENT)
    temp = temp + NEWLINE + END_EVENT + NEWLINE + END_CALENDAR;
    else
    temp = temp + NEWLINE + END_TODO + NEWLINE + END_CALENDAR;
    return temp;
}

/**
 * @param strIcalComp string
 * @param rrules vector <string>
 * @param flag int
 * @return string
 *
 * Utility function used for parsing recurrence rules
 */
string ICalConverter::addRuleToString(string strIcalComp, vector < string > rrules,
               int flag)
{
    unsigned int startPos = 0;
    string strMatch ;
    string temp;
    string rrule;
    switch (flag) {
        case E_EVENT:
            strMatch.append(END_EVENT);
            break;
        case E_TODO:
            strMatch.append(END_TODO);
            break;
    default :
       break;
        
    }
    if ((startPos = strIcalComp.find(strMatch.c_str(), 0)) != string::npos) {
    temp = strIcalComp.substr(0, startPos);
    for (unsigned int i = 0; i < rrules.size(); i++) {
        rrule = rrules.at(i);
        if (rrule.empty() == 0)
        temp = temp + rrule + NEWLINE;
    }
    }
    temp = temp + strMatch + NEWLINE + END_CALENDAR + NEWLINE;
    return temp;
}

/**
 * getIcalAlarm()
 * @param cAlarm CAlarm object
 * @return string
 *
 * Function to get the Alarm string from Alarm component
 */
string ICalConverter::getIcalAlarm(CAlarm * cAlarm,string sZone,bool isAllday)
{
    string strVcal;
    icalcomponent *pAcomp = 0;
    icalproperty *pProp = 0;
    icaltimetype ical_ctime;
    memset (&ical_ctime, 0, sizeof(icaltimetype));
    time_t ctime;
    
    if (cAlarm == NULL)
    return strVcal;
    
    pAcomp = icalcomponent_new_valarm();
    struct icaltriggertype trigger;
    ctime = (time_t) cAlarm->getTrigger();

    ical_ctime = icaltime_from_timet_with_zone(ctime,0, 
            icaltimezone_get_builtin_timezone(sZone.c_str()));
    if (isAllday)
    ical_ctime.is_utc = 0;
    trigger.time = ical_ctime;

    
    pProp = icalproperty_new_trigger(trigger);
    icalcomponent_add_property(pAcomp, pProp);
    icalproperty_free(pProp);

    struct icaldurationtype duration;
    duration.is_neg = 0;
    duration.days = 0;
    duration.weeks = 0;
    duration.hours = 0;
    duration.seconds = 0;
    duration.minutes = cAlarm->getDuration();
    pProp = icalproperty_new_duration(duration);
    icalcomponent_add_property(pAcomp, pProp);
    icalproperty_free(pProp);

    int repeat = cAlarm->getRepeat();
    pProp = icalproperty_new_repeat(repeat);
    icalcomponent_add_property(pAcomp, pProp);
    icalproperty_free(pProp);

    pProp = icalproperty_new_action(ICAL_ACTION_AUDIO);
    icalcomponent_add_property(pAcomp, pProp);
    icalproperty_free(pProp);

    strVcal = icalcomponent_as_ical_string(pAcomp);
    icalcomponent_free(pAcomp);
    return strVcal;
}
/**
 * @param szContents
 * @return string
 *
 * This function appends ctrl+M to each line as required for pc suite sync
 */
string ICalConverter::appendControlM(string szContents)
{
    string szLine;
    string szRet;
    szContents = this->validateContents(szContents);
    std::istringstream iss(szContents);
    while (getline(iss, szLine)) {

    unsigned int loc = szLine.length();

    if (loc != 0) {
        szLine = szLine + "\r";
        szLine = szLine + NEWLINE;

        szRet = szRet + szLine;
    }
    }
    return szRet;
}

/**
 * @param pOrg COrganizer object
 * @return icalproperty object
 *
 * This function returns the ical organizer property from the given COrganizer object.
 */
icalproperty *ICalConverter::getIcalOrganizer(COrganizer * pOrg)
{
    string szOrg;
    string szMailTo;
    icalparameter *tempIcalParam = 0;
    icalproperty *pProp = 0;
    CAL_DEBUG_LOG("\n the ORG");

    szMailTo = pOrg->getMailTo();

    pProp = icalproperty_new_organizer(szMailTo.c_str());

    if (!(pOrg->getCommonName()).empty()) {

    tempIcalParam =
        icalparameter_new_cn((char *) pOrg->getCommonName().c_str());
    if (tempIcalParam)
        icalproperty_add_parameter(pProp, tempIcalParam);

    }
    if (!(pOrg->getDirectoryParameter()).empty()) {

    tempIcalParam = icalparameter_new_dir((char *) pOrg->
        getDirectoryParameter().c_str());
    if (tempIcalParam)
        icalproperty_add_parameter(pProp, tempIcalParam);
    }
    if (!(pOrg->getSentBy()).empty()) {

    tempIcalParam =
        icalparameter_new_sentby((char *) pOrg->getSentBy().c_str());
    if (tempIcalParam)
        icalproperty_add_parameter(pProp, tempIcalParam);
    }
    if (!(pOrg->getLanguage()).empty()) {

    tempIcalParam = icalparameter_new_language((char *) pOrg->
        getLanguage().c_str());
    if (tempIcalParam)
        icalproperty_add_parameter(pProp, tempIcalParam);
    }
    return pProp;
}
/**
 * @param iCUType CalendarUserType
 * @return icalparameter_cutype
 * 
 * This function returns the mapping for CalendarUserType from local databse format to 
 * ICAL format.
 */
icalparameter_cutype ICalConverter::
getIcalCalendarUserType(CalendarUserType iCalCUType)
{
    icalparameter_cutype retVal;
    switch (iCalCUType) {
    case INDIVIDUAL:
        retVal =  ICAL_CUTYPE_INDIVIDUAL;
        break;
    case GROUP:
        retVal =  ICAL_CUTYPE_GROUP;
        break;
    case RESOURCE:
        retVal = ICAL_CUTYPE_RESOURCE;
        break;
    case ROOM:
        retVal = ICAL_CUTYPE_ROOM;
        break;
    case UNKNOWN_USER:
        retVal =  ICAL_CUTYPE_UNKNOWN;
        break;
    default:
        retVal =  ICAL_CUTYPE_INDIVIDUAL;
        break;
    }
    return retVal;

}

/**
 * @param iRole ParticipantRole
 * @return icalparameter_role
 * 
 * This function returns the mapping for ParticipantRole from local databse format to 
 * ICAL format.
 */
icalparameter_role ICalConverter::getIcalPartRole(ParticipantRole iRole)
{
    icalparameter_role retVal;    

    switch (iRole) {
    case CHAIR:
        retVal =  ICAL_ROLE_CHAIR;
        break;
    case REQ_PARTICIPANT:
        retVal =  ICAL_ROLE_REQPARTICIPANT;
        break;
    case OPT_PARTICIPANT:
        retVal = ICAL_ROLE_OPTPARTICIPANT;
        break;
    case NON_PARTICIPANT:
        retVal =  ICAL_ROLE_NONPARTICIPANT;
        break;
    default:
        retVal =  ICAL_ROLE_OPTPARTICIPANT;
        break;
    }
    return retVal;

}

/**
 * @param iPartStat ParticipantStatus
 * @return icalparameter_partstat
 * 
 * This function returns the mapping for ParticipantStatus from local databse format to 
 * ICAL format.
 */
icalparameter_partstat ICalConverter::
getIcalPartStatus(ParticipantStatus iPartStat)
{
    icalparameter_partstat retVal;    
    switch (iPartStat) {
    case NEEDS_ACTION:
        retVal =  ICAL_PARTSTAT_NEEDSACTION;
        break;
    case ACCEPTED:
        retVal =  ICAL_PARTSTAT_ACCEPTED;
        break;
    case DECLINED:
        retVal = ICAL_PARTSTAT_DECLINED;
        break;
    case TENTATIVE:
        retVal = ICAL_PARTSTAT_TENTATIVE;
        break;
    case DELEGATED:
        retVal = ICAL_PARTSTAT_DELEGATED;
        break;
    case COMPLETED:
        retVal = ICAL_PARTSTAT_COMPLETED;
        break;
    case IN_PROCESS:
        retVal = ICAL_PARTSTAT_INPROCESS;
        break;
    default:
        retVal = ICAL_PARTSTAT_NEEDSACTION;
        break;
    }
    return retVal;
}
#if 0
icalparameter_rsvp ICalConverter::getIcalRSVP(bool fRSVP)
{
    if (fRSVP == 1)
    return ICAL_RSVP_TRUE;

    return ICAL_RSVP_FALSE;
}
#endif
int ICalConverter::getIcalStatus(statusParam iStatus)
{
    int retVal;
    switch (iStatus) {
    case TENTATIVE_STATUS:
        retVal = ICAL_STATUS_TENTATIVE;
        break;
    case CONFIRMED_STATUS:
        retVal =  ICAL_STATUS_CONFIRMED;
        break;
    case COMPLETED_STATUS:
        retVal = ICAL_STATUS_COMPLETED;
        break;
    case NEEDSACTION_STATUS:
        retVal = ICAL_STATUS_NEEDSACTION;
        break;
    case CANCELLED_STATUS:
        retVal = ICAL_STATUS_CANCELLED;
        break;
    case INPROCESS_STATUS:
        retVal = ICAL_STATUS_INPROCESS;
        break;
    case DRAFT_STATUS:
        retVal = ICAL_STATUS_DRAFT;
        break;
    case FINAL_STATUS:
        retVal = ICAL_STATUS_FINAL;
        break;
    default:
        retVal = ICAL_STATUS_NEEDSACTION;
        break;
    }
    return retVal;
}

/**
 * getIcalAttendee()
 * @param : pAtten CAttendee pointer
 * @return : icalproperty icalproperty pointer
 */
icalproperty *ICalConverter::getIcalAttendee_ical(CAttendee * pAtten)
    
{
    string szAtten;
    string szMailTo;
    icalparameter *pParam = 0;
    icalproperty *pProp = 0;

    szMailTo = pAtten->getMailTo();
    pProp = icalproperty_new_attendee(szMailTo.c_str());
    if (pAtten->getCalendarUserType() != -1) {
    icalparameter_cutype icalCUType;
    icalCUType =
        getIcalCalendarUserType(pAtten->getCalendarUserType());
    pParam = icalparameter_new_cutype(icalCUType);
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
    if (!(pAtten->getMember()).empty()) {

    pParam = icalparameter_new_member((char *) pAtten->getMember().
        c_str());
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
    if (pAtten->getRole() != -1) {

    icalparameter_role icalRole;
    icalRole = getIcalPartRole(pAtten->getRole());
    pParam = icalparameter_new_role(icalRole);

    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
    icalparameter_rsvp icalRSVP;
    if (pAtten->getRSVP()==1)
    icalRSVP=ICAL_RSVP_TRUE;
    else
    icalRSVP=ICAL_RSVP_FALSE;

    pParam = icalparameter_new_rsvp(icalRSVP);
    if (pParam)
    icalproperty_add_parameter(pProp, pParam);

    pParam = NULL;
    if (!(pAtten->getDelegator()).empty()) {

    pParam = icalparameter_new_delegatedfrom((char *) pAtten->
        getDelegator().c_str());
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
    if (!(pAtten->getDelegatees()).empty()) {

    pParam = icalparameter_new_delegatedto((char *) pAtten->
        getDelegatees().c_str());
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
    if (!(pAtten->getCommonName()).empty()) {

    pParam =
        icalparameter_new_cn((char *) pAtten->getCommonName().
            c_str());
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);

    }
    pParam = NULL;
    if (!(pAtten->getDirectoryParameter()).empty()) {

    pParam = icalparameter_new_dir((char *) pAtten->
        getDirectoryParameter().c_str());
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
    if (!(pAtten->getSentBy()).empty()) {

    pParam = icalparameter_new_sentby((char *) pAtten->
        getSentBy().c_str());
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
    if (!(pAtten->getLanguage()).empty()) {

    pParam = icalparameter_new_language((char *) pAtten->
        getLanguage().c_str());
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
    if (pAtten->getParticipationStatus() != -1) {

    icalparameter_partstat icalPartStat;
    icalPartStat =
        getIcalPartStatus(pAtten->getParticipationStatus());

    pParam = icalparameter_new_partstat(icalPartStat);
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    return pProp;
}

/**
 *  * getIcalAttendee()
 *   * @param : pAtten CAttendee pointer
 *    * @return : icalproperty icalproperty pointer
 *     */
icalproperty *ICalConverter::getIcalAttendee_vcal(CAttendee * pAtten)
{
    string szAtten;
    string szMailTo;
    icalparameter *pParam = 0;
    icalproperty *pProp = 0;
    VCalConverter *pVCal = 0;

    szMailTo = pAtten->getMailTo();

    pProp = icalproperty_new_attendee(szMailTo.c_str());
    if (pAtten->getRole() != -1) {

/*    icalparameter_role icalRole; */
    string szRole = pVCal->getVcalPartRole(pAtten->getRole());
    pParam = icalparameter_new_from_string(szRole.c_str());
    if (pParam)
        icalproperty_add_parameter(pProp, pParam);
    }
    pParam = NULL;
/*    icalparameter_rsvp icalRSVP; */
    pParam = pVCal->getVcalRSVP(pAtten->getRSVP());
    if (pParam)
    icalproperty_add_parameter(pProp, pParam);

    pParam = NULL;
    return pProp;
}

icalparameter_reltype ICalConverter::getIcalRelType(RelationshipType iRel)
{
    icalparameter_reltype retVal;    

    switch (iRel) {
    case PARENT:
        retVal = ICAL_RELTYPE_PARENT;
        break;
    case CHILD:
        retVal = ICAL_RELTYPE_CHILD;
        break;
    case SIBLING:
        retVal = ICAL_RELTYPE_SIBLING;
        break;
    default:
        retVal =  ICAL_RELTYPE_PARENT;
        break;
    }
    return retVal;
}
icalparameter *ICalConverter::getTZIDParam()
{
    icalparameter *pParam = 0;
    char *zone = new char[DEFAULT_SIZE];
    int   nchars;
    ASSERTION(zone);
    memset(zone, 0, DEFAULT_SIZE);
    nchars = time_get_timezone(zone, DEFAULT_SIZE);
    CAL_DEBUG_LOG("*** nchars = %d", nchars);
    if (nchars > 0 && nchars < DEFAULT_SIZE) {
        string szZone(zone);
        unsigned int loc;
    
    CAL_DEBUG_LOG("Current timezone is %s\n", zone);
        loc = szZone.find(FW_SLASH, 0);
        if (loc != string::npos)
            szZone.replace(loc, 1, ICS_CALNAME_SEPERATOR);
	parseTimeZone(szZone);

        pParam = icalparameter_new_tzid(szZone.c_str());
    }

    delete [] zone;
    return pParam;
}


char* ICalConverter::getCurrentTimeZone()
{

    char *zone = new char[DEFAULT_SIZE];
    ASSERTION(zone);
    memset(zone, 0, DEFAULT_SIZE);
    time_get_timezone(zone, DEFAULT_SIZE);
    CAL_DEBUG_LOG("Current timezone is %s\n", zone);
    return zone ;
}


time_t ICalConverter::getTimeDiff(time_t originalTime )
{
    char *zone = new char[DEFAULT_SIZE];
    time_t timeDiff = 0;
    ASSERTION(zone);
    memset(zone, 0, DEFAULT_SIZE);
    time_get_timezone(zone, DEFAULT_SIZE);
    CAL_DEBUG_LOG("Current timezone is %s\n", zone);
    timeDiff = time_get_time_diff(
        	originalTime,zone,":GMT");
    delete [] zone;
    return timeDiff;
}

template<class T>
void ICalConverter::exportCreatedTimeFromLocal(icalcomponent *pEntcomp, T *pComp, FileType iType)
{
    time_t ctime;    
    icalproperty *pProp = 0;
    icaltimetype ical_ctime;
    memset (&ical_ctime, 0, sizeof(icaltimetype));
    /* gets the time diff between the current zone and 
     * GMT*/
    //time_t time_diff = getTimeDiff();
    ctime = pComp->getCreatedTime(); 
    /*exporting created time */
    if (ctime) {
    ical_ctime = icaltime_from_timet_with_zone(ctime, 0,
        	icaltimezone_get_builtin_timezone("UTC"));
        if (iType == ICAL_TYPE) {
        pProp = icalproperty_new_created(ical_ctime);
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
        }
    }
}

template <class T>
void ICalConverter::exportDateStampFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;
    icaltimetype ical_datestamp;
    time_t datestamp;
    string szZone = pComp->getTzid();
    parseTimeZone(szZone);
    //time_t time_diff = getTimeDiff();
    datestamp = pComp->getDateStamp();
    memset (&ical_datestamp, 0, sizeof(icaltimetype));
    /*exporting date stamp */
    if (datestamp) {
        ical_datestamp = icaltime_from_timet_with_zone(datestamp, 0,
        icaltimezone_get_builtin_timezone(szZone.c_str()));
        pProp = icalproperty_new_dtstamp(ical_datestamp);
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
    }
}

template<class T>
void ICalConverter::exportSummaryFromLocal(icalcomponent *pEntcomp, T *pComp, FileType iType)
{
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    /*exporting summary */
    if (!(pComp->getSummary()).empty()) {
        string summary;

        switch(iType) {
        case ICAL_TYPE:
        	/*
        	 * iCal supports UTF-8 so no need for QP encode here.
        	 */
        	summary = pComp->getSummary();
        	break;
        case VCAL_TYPE:
        default:
        	/*
			 * If QP encode is not required we can let libical
			 * handle the content. Otherwise we should not
			 * paste the content here, because libical changes the format
			 * of the encoded string if put here.
			 */
        	CUtility *pUt = 0;
			pUt = CUtility::Instance();
        	if(!pUt->isEncodingRequired(pComp->getSummary(), bSyncing)) {
				summary = pComp->getSummary();
        	}
        	break;
        }

		pProp = icalproperty_new_summary(summary.c_str());
		hashMap = pComp->getHashMap();
		if (pProp) {
			icalcomponent_add_property(pEntcomp, pProp);
			it = hashMap.find(SUMMARY);
			if (it != hashMap.end()) {
				for (unsigned int i = 0; i < (*it).second.size(); i++) {
					pParam = 0;
					ParamType paramVal;
					string paramName;
					paramName = (*it).second[i]->getParamName();
					paramVal = (*it).second[i]->getParamValue();
					if (paramName == LANGUAGE) {
						pParam = icalparameter_new_language(paramVal.
						       szString.c_str());
					}
					else if (paramName == ALTER) {
						pParam = icalparameter_new_altrep(paramVal.
						       szString.c_str());
					}
					if(pParam)
						icalproperty_add_parameter(pProp, pParam);
				}
			}
			if(summary.length() == 0) {
				addEncodingQuotedPrintable(pProp);
			}
		}
        icalproperty_free(pProp);
    }
}


template<class T>
void ICalConverter::exportLocationFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    /*exporting location */
    if (!(pComp->getLocation()).empty()) {
        string location;
        CUtility *pUt = 0;
        pUt = CUtility::Instance();

        //If quoted printable encode is not required we can
        //let libical handle the content else we should not 
        //paste the content here cos libical changes the format
        //of encoded string if put here
    if(!pUt->isEncodingRequired(pComp->getLocation(), bSyncing))
        location = pComp->getLocation();
        pProp = icalproperty_new_location(location.c_str());
        hashMap = pComp->getHashMap();
        if (pProp) {
            icalcomponent_add_property(pEntcomp, pProp);
            it = hashMap.find(LOCATION);
            if (it != hashMap.end()) {
                for (unsigned int i = 0; i < (*it).second.size(); i++) {
                    pParam = 0;
                    ParamType paramVal;
                    string paramName;
                    paramName = (*it).second[i]->getParamName();
                    paramVal = (*it).second[i]->getParamValue();
                    if (paramName == LANGUAGE) {
                        pParam =  icalparameter_new_language(paramVal.
                               szString.c_str());
                    }
                    else if (paramName == ALTER) {
                        pParam =  icalparameter_new_altrep(paramVal.
                             szString.c_str());
                    }
                    if(pParam)
                        icalproperty_add_parameter(pProp, pParam);
                }
            }
            if(location.length() != pComp->getLocation().length() ) {
                addEncodingQuotedPrintable(pProp);
            }
        }
        icalproperty_free(pProp);
    }
}
template <class T>
void ICalConverter::exportDescriptionFromLocal(icalcomponent *pEntcomp,  T *pComp, FileType iType)
{
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    /*exporting description */
    if (!(pComp->getDescription()).empty()) {
    	string description;

		switch(iType) {
		case ICAL_TYPE:
			/*
			 * iCal supports UTF-8 so no need for QP encode here.
			 */
			description = pComp->getDescription();
			break;
		case VCAL_TYPE:
		default:
			/*
			 * If QP encode is not required we can let libical
			 * handle the content. Otherwise we should not
			 * paste the content here, because libical changes the format
			 * of the encoded string if put here.
			 */
			CUtility *pUt = 0;
			pUt = CUtility::Instance();
			if(!pUt->isEncodingRequired(pComp->getDescription(), bSyncing)) {
				description = pComp->getDescription();
			}
			break;
		}

		pProp = icalproperty_new_description(description.c_str());
        hashMap = pComp->getHashMap();
        if (pProp) {
            icalcomponent_add_property(pEntcomp, pProp);
            it = hashMap.find(DESCRIPTION);
            if (it != hashMap.end()) {
                for (unsigned int i = 0; i < (*it).second.size(); i++) {
                    pParam = 0;
                    ParamType paramVal;
                    string paramName;
                    paramName = (*it).second[i]->getParamName();
                    paramVal = (*it).second[i]->getParamValue();
                    if (paramName == LANGUAGE) {
                        pParam =  icalparameter_new_language(paramVal.
                               szString.c_str());
                    }
                    else if (paramName == ALTER) {
                        pParam =  icalparameter_new_altrep(paramVal.
                             szString.c_str());
                    }
                    if(pParam)
                        icalproperty_add_parameter(pProp, pParam);
                }
            }
            if(description.length() == 0) {
                addEncodingQuotedPrintable(pProp);
            }
        }
        icalproperty_free(pProp);
    }
}

template<class T>
void ICalConverter::exportUidFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    string szId;
    icalproperty *pProp = 0;    
    /*exporting guid - id is taken as uid */
    szId = pComp->getId();
    pProp = icalproperty_new_uid(szId.c_str());
    if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
    }
    icalproperty_free(pProp);
}

template<class T> void ICalConverter::exportDueFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icaltimetype ical_dtstart;
    icalproperty *pProp = 0;    
    time_t dtstart;
    string szZone = pComp->getTzid();
    parseTimeZone(szZone);
    memset (&ical_dtstart, 0, sizeof(icaltimetype));
    /*exporting due */
    dtstart = pComp->getDue();
    ical_dtstart = icaltime_from_timet_with_zone(dtstart, 0,
       	icaltimezone_get_builtin_timezone(szZone.c_str()));
    ical_dtstart.is_utc=0; 

   pProp = icalproperty_new_due(ical_dtstart);
   icalcomponent_add_property(pEntcomp, pProp);
   icalproperty_free(pProp);
}

template<class T>
void ICalConverter::exportDateStartFromLocal(icalcomponent *pEntcomp, T *pComp,FileType iType)
{
    icaltimetype ical_dtstart;
    icalproperty *pProp = 0;    
    time_t dtstart = 0;
    string szZone = pComp->getTzid();
    parseTimeZone(szZone);
    memset (&ical_dtstart, 0, sizeof(icaltimetype));
    if(pComp->getType() == E_TODO) {
        dtstart = pComp->getDateStamp();
    }
    else {
        dtstart = pComp->getDateStart();
    }

    CAL_DEBUG_LOG("Time diff in Exporting in %ld",dtstart - pComp->getDateStart());
    CAL_DEBUG_LOG("Zone of event is  %s",szZone.c_str());
    /*exporting datestart */

     if(!pComp->getAllDay() && (pComp->getType() != E_TODO)) {
	     ical_dtstart = icaltime_from_timet_with_zone(dtstart, 0,
			     icaltimezone_get_builtin_timezone("UTC"));
     }
     else {
	     ical_dtstart = icaltime_from_timet_with_zone(dtstart, (iType == ICAL_TYPE),
			     icaltimezone_get_builtin_timezone(szZone.c_str()));
	     /* alldays and tasks should always be sent in floating 
	      * time so that in other devices they are treated as 
	      * alldays 
	      */
	     ical_dtstart.is_utc = 0;
     }

    limitDateRange(ical_dtstart,true);

    /*check if the DST is in use 
     * if so update the same in ical_dtstart property*/    
        pProp = icalproperty_new_dtstart(ical_dtstart);
    /* only in ICAL format TZID has to be specified for non allday events 
     * in VCAL format this one need not be specified as we specify the 
     * timezone information in TZ: field
     */
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
}

string ICalConverter::exportTimeZone(time_t dtstart,string szZone,FileType iType)
{
    icaltimetype ical_dtstart;
    string strTimeZone;
    char *pTimeComp = 0;
    
    memset (&ical_dtstart, 0, sizeof(icaltimetype));
    parseTimeZone(szZone);
    icaltimezone* pTZ = icaltimezone_get_builtin_timezone(szZone.c_str());

    /******************************************************/
    /*   this part is for ICAL time zone component        */
    /******************************************************/
    if (iType == ICAL_TYPE )
    {
    	strTimeZone = "VERSION:2.0";
        ical_dtstart = icaltime_from_timet_with_zone(dtstart, 0,pTZ);
        icalcomponent* tzComp = icaltimezone_get_component (pTZ);
        pTimeComp = icalcomponent_as_ical_string(tzComp);
        if (pTimeComp)
           strTimeZone = strTimeZone + "\n"+ pTimeComp;
        
        CAL_DEBUG_LOG("TimeZone Component %s",strTimeZone.c_str());
        return strTimeZone;
    }

    /******************************************************/
    /*   this part is for VCAL time zone component        */
    /******************************************************/
    strTimeZone = "VERSION:1.0";
    if (dtstart) {
        struct icalrecurrencetype rruleStd,rruleDst;
        struct icaltimetype dtstartStd;
        struct icaltimetype dtstartDst;
        string ruleStd;
        string ruleDst;
        string tzid ;
        string tznameStd;
        string tznameDst;
        int  prev_offsetStd = 0;
        int  prev_offsetDst = 0;
        time_t stdStart = 0 ;
        time_t daylightStart =0 ;

        icalcomponent* comp1 = 0;
        icalcomponent* comp2 = 0;
        icalproperty* prop = 0;

        memset (&dtstartStd, 0, sizeof (struct icaltimetype));
        memset (&dtstartDst, 0, sizeof (struct icaltimetype));

        ical_dtstart = icaltime_from_timet_with_zone(dtstart, 0,pTZ);
        icalcomponent* tzComp = icaltimezone_get_component (pTZ);
        CAL_DEBUG_LOG("##################TIMEZONE INFO ##################");
        CAL_DEBUG_LOG("TimeZone Component %s", icalcomponent_as_ical_string(tzComp));
        CAL_DEBUG_LOG("##################TIMEZONE INFO ##################");
        
        comp1 = icalcomponent_get_first_component (tzComp, ICAL_XSTANDARD_COMPONENT);
    if (comp1 == 0){
    	return strTimeZone;
    }

        prop = icalcomponent_get_first_property (comp1, ICAL_ANY_PROPERTY);
        while (prop) {
            switch (icalproperty_isa (prop)) {
        	case ICAL_TZNAME_PROPERTY:
        	    tznameStd = (char*)icalproperty_get_tzname (prop);
        	    break;
        	case ICAL_DTSTART_PROPERTY:
        	    dtstartStd = icalproperty_get_dtstart (prop);
        	    CAL_DEBUG_LOG("*********Date start STD  *********** ");
        	    CAL_DEBUG_LOG(" Year is:    %d ", dtstartStd.year);
        	    CAL_DEBUG_LOG(" Month is:   %d ", dtstartStd.month);
        	    CAL_DEBUG_LOG(" Day is:     %d ", dtstartStd.day);
        	    CAL_DEBUG_LOG(" Hour is:    %d ", dtstartStd.hour);
        	    CAL_DEBUG_LOG(" Minute  is: %d ", dtstartStd.minute);
        	    CAL_DEBUG_LOG(" Second is : %d ", dtstartStd.second);
        	    CAL_DEBUG_LOG("*********Date end STD *********** ");
        	    stdStart = icaltime_as_timet_with_zone(dtstartStd, pTZ);
        	    CAL_DEBUG_LOG("standard start time : %s " ,ctime(&stdStart));
        	    break;
        	case ICAL_TZOFFSETTO_PROPERTY:
        	    prev_offsetStd = icalproperty_get_tzoffsetto (prop);
        	    break;
        	case ICAL_RRULE_PROPERTY:
        	    rruleStd = icalproperty_get_rrule (prop);
        	    ruleStd=icalrecurrencetype_as_string(&rruleStd);
        	    break;
        	default:
        	    break;
            }
            prop = icalcomponent_get_next_property (comp1, ICAL_ANY_PROPERTY);
        }
        strTimeZone += "\nTZ:";
        strTimeZone += getTimeInHHMM (prev_offsetStd);
        strTimeZone += "\nDAYLIGHT:";
        
        comp2 = icalcomponent_get_first_component (tzComp, ICAL_XDAYLIGHT_COMPONENT);
        if (comp2){
        strTimeZone += "TRUE;";
        prop = icalcomponent_get_first_property (comp2, ICAL_ANY_PROPERTY);
        while (prop) {
            switch (icalproperty_isa (prop)) {
        	case ICAL_TZNAME_PROPERTY:
        	    tznameDst = (char*)icalproperty_get_tzname (prop);
        	    break;
        	case ICAL_DTSTART_PROPERTY:
        	    dtstartDst = icalproperty_get_dtstart (prop);
        	    CAL_DEBUG_LOG("*********Date start DST  *********** ");
        	    CAL_DEBUG_LOG(" Year is:    %d ", dtstartDst.year);
        	    CAL_DEBUG_LOG(" Month is:   %d ", dtstartDst.month);
        	    CAL_DEBUG_LOG(" Day is:     %d ", dtstartDst.day);
        	    CAL_DEBUG_LOG(" Hour is:    %d ", dtstartDst.hour);
        	    CAL_DEBUG_LOG(" Minute  is: %d ", dtstartDst.minute);
        	    CAL_DEBUG_LOG(" Second is : %d ", dtstartDst.second);
        	    CAL_DEBUG_LOG("*********Date end DST *********** ");
        	    daylightStart = icaltime_as_timet_with_zone(dtstartDst, pTZ);
        	    CAL_DEBUG_LOG("daylight start time : %s " ,ctime(&daylightStart));
        	    break;
        	case ICAL_TZOFFSETTO_PROPERTY:
        	    prev_offsetDst = icalproperty_get_tzoffsetto (prop);
        	    break;
        	case ICAL_RRULE_PROPERTY:
        	    rruleDst = icalproperty_get_rrule (prop);
        	    ruleDst = icalrecurrencetype_as_string(&rruleDst);
        	    break;
        	default:
        	    break;
            };

            prop = icalcomponent_get_next_property (comp2, ICAL_ANY_PROPERTY);
        }
            strTimeZone += getTimeInHHMM (prev_offsetDst) + ";";
          
        string daylightStart;
        string daylightEnd;
       
       daylightStart = calculateMonthdayFromRule(dtstart, dtstartStd,ruleStd,szZone);
       daylightEnd = calculateMonthdayFromRule( dtstart,dtstartDst,ruleDst,szZone);

      strTimeZone += daylightEnd + ";" + daylightStart + ";"+ tznameStd + ";" + tznameDst;
        }
        else {
            
        strTimeZone += "FALSE";
        
        }

    }
    
    CAL_DEBUG_LOG("**** TimeZone string to be added in VCAL *****\n%s", strTimeZone.c_str());
    return strTimeZone;
}


/**
 * Find the daylight start time from daylight string 
 * from daylight start time we figure out the 
 * Repeat pattern in current year for Daylight information.
 *    
 * 
 **/
void  ICalConverter:: findZoneStartTime(string &szTime, int correction)
{

/* DAYLIGHT:TRUE;-06;19960407T025959;19961027T010000;EST;EDT
 */
		 
   struct icaltimetype startTime;
   struct tm currentTm;
   memset (&currentTm, 0, sizeof (struct tm));
   time_t offset_time = 0 ;
   memset (&startTime, 0, sizeof (struct icaltimetype));
   short month_day= 0,week_day_required = 0 ,position = 0;
   startTime = icaltime_from_string(szTime.c_str());
   time_t current_utc_offset = 0; 
   string szZone;
   szZone  =  CMulticalendar::MCInstance()->getSystemTimeZone();
   CAL_DEBUG_LOG("local zone is %s",szZone.c_str());
   CAL_DEBUG_LOG("Input string is %s", szTime.c_str());
   current_utc_offset = time_get_time_diff(offset_time,szZone.c_str(), "UTC");
   
   if (startTime.is_utc == 1)
   { 
       /* Convert the time to UTC based on the offset */ 
	   CAL_DEBUG_LOG("inside UTC ");
       offset_time =  icaltime_as_timet_with_zone(startTime,
  	   icaltimezone_get_builtin_timezone("UTC")) + correction;
    
        /* convert it back to icaltimetype */
       startTime = icaltime_from_timet_with_zone(offset_time,0,icaltimezone_get_builtin_timezone("UTC"));
       startTime.is_utc = 0;
	   
   }
   else
          offset_time = icaltime_as_timet_with_zone(startTime,
	     icaltimezone_get_builtin_timezone("UTC")) -current_utc_offset;
    CAL_DEBUG_LOG("********* startTime  *************");
    CAL_DEBUG_LOG(" Year is:    %d ", startTime.year);
    CAL_DEBUG_LOG(" Month is:   %d ", startTime.month);
    CAL_DEBUG_LOG(" Day is:     %d ", startTime.day);
    CAL_DEBUG_LOG(" Hour is:    %d ", startTime.hour);
    CAL_DEBUG_LOG(" Minute  is: %d ", startTime.minute);
    CAL_DEBUG_LOG(" Second is : %d ", startTime.second);
    CAL_DEBUG_LOG(" Is UTC :    %d ",startTime.is_utc);
    CAL_DEBUG_LOG("**********************************");
   /* 
    * since local time adds the GMT offset while computation 
    * we need to remove the offset before convertion 
    */
   offset_time = offset_time + current_utc_offset;
   time_get_remote(offset_time,szZone.c_str(), &currentTm);
   
   CAL_DEBUG_LOG("*** struct tm value of daylight start time *****");
   CAL_DEBUG_LOG( " Year = 1900 +  %d" , currentTm.tm_year);
   CAL_DEBUG_LOG( " Month = %d",currentTm.tm_mon);
   CAL_DEBUG_LOG( " Month day = %d",currentTm.tm_mday);
   CAL_DEBUG_LOG( " Hour is = %d",currentTm.tm_hour);
   CAL_DEBUG_LOG( " Minute is = %d",currentTm.tm_min);
   CAL_DEBUG_LOG( " Week day  = %d",currentTm.tm_wday);
   CAL_DEBUG_LOG("***********************************");
   
   /*
    * Convert the start time to string */
   /* Find week day and its position from first
    * */
    month_day = currentTm.tm_mday ;
    week_day_required = currentTm.tm_wday ;

    /**
    position of week day from start of the month
    **/ 
    while ( month_day >= 0 ){
       month_day = month_day - 7; 
       position ++ ;
    }
    /**  Find total number of weekday ( su.Mo,TU ... in that month */
    short month = 0;
    short monthEnd = 0;
    short week_day_count = 0;
    short year = currentTm.tm_year;
    
    month = currentTm.tm_mon + 1;
    if ( month == 1  ||
       	 month == 3  ||
         month == 5  ||
         month == 7  ||
         month == 8  ||
         month == 10 ||
         month == 12 )
         monthEnd = 31; 
    else if ( month == 2 ){
        /* check if it is a leap year */
        if (year%4 == 0)
        	monthEnd = 29;
        else
        	monthEnd = 28;
    }
    else
        monthEnd = 30;
    /* reset the monthday to 
     * required value*/ 
    month_day = currentTm.tm_mday;

    while(month_day <= monthEnd )
    {
	    month_day += 7;
	    ++week_day_count ;
    }
    /* Total possible week days */
    week_day_count += position;
    CAL_DEBUG_LOG("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
    CAL_DEBUG_LOG("Required Week day is %d", week_day_required);
    CAL_DEBUG_LOG(" Number of week days  %d", week_day_count);
    CAL_DEBUG_LOG("position of week day from start of month %d",position);
    CAL_DEBUG_LOG("Position of week day from end of month %d",( week_day_count - position));
    CAL_DEBUG_LOG("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
    /* now we have the week  position form start of month and 
     * and week day so constructing the query parameter 
     *   FREQ=YEARLY;BYDAY=1SU;BYMONTH=4
     */
    
      string WeekDay;
      switch (week_day_required)
      {
	case 0: 
	      WeekDay = "SU";
	      break;
	case 1:
	      WeekDay = "MO";
	      break;
	      
	case 2:
	      WeekDay = "TU";
	      break;
	case 3:
	      WeekDay = "WE";
	      break;
	case 4:
	      WeekDay = "TH";
	      break;
	case 5:
	      WeekDay = "FR";
	      break;
	case 6: 
	      WeekDay = "SA";
	      break;
	default:
	      CAL_DEBUG_LOG("Week day incorrect ????");
	      break;
      }
      /* Exclamatory mark separates the two set of 
       * values possible for repeat pattern*/
      char* search_string = 0;
      asprintf( &search_string ,"FREQ=YEARLY;BYDAY=%d%s;BYMONTH=%d!FREQ=YEARLY;BYDAY=%d%s;BYMONTH=%d",
		      position,WeekDay.c_str(),month,
		     (position - week_day_count),WeekDay.c_str(),month);

      if (!search_string)
      {
	     CAL_DEBUG_LOG("FATAL:asprintf failed to allocate memory "); 
             ASSERTION(false);
      } 
      szTime = search_string ;
      CAL_DEBUG_LOG("Search string is %s", szTime.c_str());
      free (search_string);
      search_string = 0;
      
}

string ICalConverter::calculateMonthdayFromRule(time_t dtstart,struct  icaltimetype &dtstartStd,string ruleStd,string szZone)
{
    
    struct tm tm_dt_start;
    time_t time_standard;

    memset (&tm_dt_start, 0, sizeof (struct tm));
    // copy the daylight start hour and minute 

    time_get_remote(dtstart,szZone.c_str(),&tm_dt_start);
    tm_dt_start.tm_hour = dtstartStd.hour;
    tm_dt_start.tm_min = dtstartStd.minute;
    tm_dt_start.tm_sec = 0;
    tm_dt_start.tm_mon  = dtstartStd.month -1 ;
    tm_dt_start.tm_mday = 1;
    tm_dt_start.tm_isdst = -1;
    /* convert it to time_t and again back to struct tm 
     * so that you will get the week day */
    time_standard = time_mktime(&tm_dt_start, szZone.c_str());
    time_get_remote(time_standard,szZone.c_str(),&tm_dt_start);

    CRecurrenceRule * pRrule= new  CRecurrenceRule(0,ruleStd);
    pRrule->rruleParser(ruleStd);

    /* recurrence rule WK day is */
    short weekDayFromRrule = icalrecurrencetype_day_day_of_week((pRrule->getWeekDay())[0]);
    short offsetWeekDay  = icalrecurrencetype_day_position((pRrule->getWeekDay())[0]); 

    short monthDay = 1 ;
    short firstWeekDayInMonth = tm_dt_start.tm_wday + 1 ; 
    /* get the week day from here */
    CAL_DEBUG_LOG("Week day is %d", tm_dt_start.tm_wday);
    CAL_DEBUG_LOG("Week day offset is %d",offsetWeekDay);
    CAL_DEBUG_LOG("actual day is %d",weekDayFromRrule);

    /**
      Find the first occurence of week day in  in month 
      in which event starts
     **/
    if ( firstWeekDayInMonth  == weekDayFromRrule  ) {
        /**
          first day itself is first occurence
         * */
    }
    else {
        /* it comes here it means the weekDayFromRule
         * is greater than the tm_wday
         */
        short temp ;
        temp = weekDayFromRrule % 7;
        if ( temp < firstWeekDayInMonth)
        	monthDay = monthDay +  ( 7 - firstWeekDayInMonth ) + temp ;
        else
        	monthDay = monthDay + firstWeekDayInMonth + (7 -temp);

    }

    /* Month day will have the 
     * first occurence day */
    short month = tm_dt_start.tm_mon + 1;
    short year = tm_dt_start.tm_year + 1900;
    short monthEnd  = 30;
    /* 
     * Month   days 
     * 1 jan --- 31  
     * 2 feb --- 28/29 
     * 3  march  -- 31
     * 4 april -- 30
     * 5 may  -- 31
     * 6 june -- 30
     * 7july -- 31
     * 8 august -- 31
     * 9 september-- 30
     * 10 october -- 31
     * 11 November -- 30
     * 12 december -- 31
     */


    if ( month == 1  ||
        	month == 3  ||
        	month == 5  ||
        	month == 7  ||
        	month == 8  ||
        	month == 10 ||
        	month == 12 )
        monthEnd = 31; 
    else if ( month == 2 ){
        /* check if it is a leap year */
        if (year %4 == 0)
        	monthEnd = 29;
        else
        	monthEnd = 28;
    }
    else
        monthEnd = 30;

    if (offsetWeekDay > 0 )
    {
        //find the week day from first 
        do {
        	monthDay += 7;
        	--offsetWeekDay;
        }while (offsetWeekDay -1 > 0);
        CAL_DEBUG_LOG("Month day offset taken from first %d",monthDay);
    }
    else
    {
        // find the week day from last 

        short dayArray[5]= { 0 , 0 , 0 , 0 , 0};
        short i = -1 ;   	
        do {
        	i++ ;
        	dayArray[i] = monthDay;
        	monthDay += 7;
        }while (monthEnd > monthDay);
        /* here offsetWeekDay is -ve */
        monthDay = dayArray[i+1+offsetWeekDay]; 	
        CAL_DEBUG_LOG("Month day offset taken from last %d",monthDay);
    }

    /**
      once we have the month day 
      we need to keep every thing else 
      same as before  for current year

     **/
    char * pTemp = 0;
    asprintf( &pTemp , "%04d%02d%02dT%02d%02d%02d",year,month,monthDay,
        	dtstartStd.hour, dtstartStd.minute, dtstartStd.second);

    string sDaylightTime ;
    if (pTemp){
        sDaylightTime = pTemp ;
        free(pTemp);
        pTemp = 0;
    }
    /* before returning clean up the
     * memory 
     */
    delete pRrule;
    pRrule = 0;

    return sDaylightTime ;
}



void ICalConverter::exportEventDateEndFromLocal(icalcomponent *pEntcomp, CEvent *pEvent,FileType iType)
{
    icaltimetype ical_dtend;
    icalproperty *pProp = 0;    
    time_t dtend = 0;
    dtend = pEvent->getDateEnd();
    string szZone = pEvent->getTzid();
    parseTimeZone(szZone);
    memset (&ical_dtend, 0, sizeof(icaltimetype));
    CAL_DEBUG_LOG("Time diff in Exporting in %ld",dtend - pEvent->getDateEnd());
    /*exporting dateend */
    if ((pEvent->getAllDay()) &&(pEvent->getDateStart() != pEvent->getDateEnd()))
        dtend = dtend + 1;

    
    if(!pEvent->getAllDay()) {
        ical_dtend = icaltime_from_timet_with_zone(dtend, 0, icaltimezone_get_builtin_timezone("UTC"));
    }
    else {
        ical_dtend = icaltime_from_timet_with_zone(dtend, (iType == ICAL_TYPE), icaltimezone_get_builtin_timezone(szZone.c_str()));
		/*
		 * allday events and tasks should always be sent
		 * in floating time so that in other devices they remain as
		 * alldays
		 */
		ical_dtend.is_utc= 0;
    }
    limitDateRange(ical_dtend,true);
    pProp = icalproperty_new_dtend(ical_dtend);
    /* only in ICAL format TZID has to be specified for non allday events 
     * in VCAL format this one need not be specified as we specify the 
     * timezone information in TZ: field
     */
#if 0
    if ((!pEvent->getAllDay()) && (iType == ICAL_TYPE)){
        /* For allday event no need to add the 
         * timezone as allday event should be always in local time 
         * import / export */

        unsigned int loc = 0 ;	
        loc = szZone.find(FW_SLASH, 0);
        if (loc != string::npos)
        	szZone.replace(loc, 1, ICS_CALNAME_SEPERATOR);

        pParam = icalparameter_new_tzid(szZone.c_str());
        if (pParam)
        	icalproperty_add_parameter(pProp, pParam);
    }
#endif
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
}

template<class T>
void ICalConverter::exportAttendeesFromLocal(icalcomponent *pEntcomp, T *pComp, FileType iType)
{
    vector < CAttendee * >pRetAttenList;    
    icalproperty *pProp = 0;    
    /*exporting attendees */
    pRetAttenList.clear();
    pRetAttenList = pComp->getAttendees();
    for (unsigned int iCount = 0; iCount < pRetAttenList.size();
         iCount++) {
        if (pRetAttenList[iCount]) {
        switch (iType){
        case ICAL_TYPE: 
            pProp = getIcalAttendee_ical(pRetAttenList[iCount]);
            break;

        case VCAL_TYPE:
            pProp = getIcalAttendee_vcal(pRetAttenList[iCount]);
            break;
        default:
            break;
        }

        if (pProp) {
            icalcomponent_add_property(pEntcomp, pProp);
            icalproperty_free(pProp);
        }
        }
    }
}
template<class T>
void ICalConverter::exportAttachmentsFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    vector < string > vAttachList;    
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    /*exporting attachments */
    vAttachList.clear();
    vAttachList = pComp->getAttachments();
    CAL_DEBUG_LOG("ATTACH SIZE = %d\n", vAttachList.size());
    for (unsigned int iCount = 0; iCount < vAttachList.size();
         iCount++) {
        icalattach *icalAtt = 0;
        CAL_DEBUG_LOG("ATTACH = %s\n", vAttachList[iCount].c_str());

        icalAtt = icalattach_new_from_url(vAttachList[iCount].c_str());
        hashMap = pComp->getHashMap();
        if (icalAtt)
        pProp = icalproperty_new_attach(icalAtt);
        if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
        it = hashMap.find(ATTACH);
        if (it != hashMap.end()) {
            for (unsigned int i = 0; i < (*it).second.size(); i++) {
            ParamType paramVal;
            string paramName;
            paramVal = (*it).second[i]->getParamValue();
            paramName = (*it).second[i]->getParamName();
            if (paramName == FMTYPE)
                pParam =
                icalparameter_new_fmttype(paramVal.
                              szString.c_str());
            icalproperty_add_parameter(pProp, pParam);
            }
        }
        }
    icalattach_unref(icalAtt);
        icalproperty_free(pProp);
    }
}

template<class T>
void ICalConverter::exportPriorityFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    int priority = 0;    
    icalproperty *pProp = 0;    
    /* exporting priority */
    priority = pComp->getPriority();
    if (priority != -1) {
        pProp = icalproperty_new_priority(priority);
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
    }
}
template<class T>
void ICalConverter::exportSequenceFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    int sequence = 0;
    icalproperty *pProp = 0;    
    /* exporting sequence */
    sequence = pComp->getSequence();
    if (sequence != -1) {
        pProp = icalproperty_new_sequence(sequence);
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
    }
}

template<class T>
void ICalConverter::exportCommentsFromLocal(icalcomponent *pEntcomp, T *pComp, FileType iType)
{
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    /*exporting comments */
    if (!(pComp->getComments()).empty()) {
    	string comments;

		switch(iType) {
		case ICAL_TYPE:
			/*
			 * iCal supports UTF-8 so no need for QP encode here.
			 */
			comments = pComp->getComments();
			break;
		case VCAL_TYPE:
		default:
			/*
			 * If QP encode is not required we can let libical
			 * handle the content. Otherwise we should not
			 * paste the content here, because libical changes the format
			 * of the encoded string if put here.
			 */
			CUtility *pUt = 0;
			pUt = CUtility::Instance();
			if(!pUt->isEncodingRequired(pComp->getComments(), bSyncing)) {
				comments = pComp->getComments();
			}
			break;
		}
        if (pProp) {
            icalcomponent_add_property(pEntcomp, pProp);
            it = hashMap.find(COMMENT);
            if (it != hashMap.end()) {
                for (unsigned int i = 0; i < (*it).second.size(); i++) {
                    pParam = 0;
                    ParamType paramVal;
                    string paramName;
                    paramName = (*it).second[i]->getParamName();
                    paramVal = (*it).second[i]->getParamValue();
                    if (paramName == LANGUAGE) {
                        pParam =  icalparameter_new_language(paramVal.
                               szString.c_str());
                    }
                    else if (paramName == ALTER) {
                        pParam =  icalparameter_new_altrep(paramVal.
                             szString.c_str());
                    }
                    if(pParam)
                        icalproperty_add_parameter(pProp, pParam);
                }
            }
            if(comments.length() == 0) {
                addEncodingQuotedPrintable(pProp);
            }
        }
        icalproperty_free(pProp);
    }
}

template<class T>
void ICalConverter::exportUrlFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;    
    /*exporting URL */
    if (!(pComp->getUrl()).empty()) {
        pProp = icalproperty_new_url((pComp->getUrl()).c_str());
        if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
        }
        icalproperty_free(pProp);
    }
}

template<class T>
void ICalConverter::exportCategoriesFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    /*exporting categories */
    if (!(pComp->getCategories()).empty()) {
        pProp = icalproperty_new_categories((pComp->getCategories()).c_str());
    hashMap = pComp->getHashMap();
        if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
        it = hashMap.find(CATEGORIES);
        if (it != hashMap.end()) {
            for (unsigned int i = 0; i < (*it).second.size(); i++) {
            ParamType paramVal;
            string paramName;
            paramVal = (*it).second[i]->getParamValue();
            paramName = (*it).second[i]->getParamName();
            if (paramName == LANGUAGE)
                pParam =
                icalparameter_new_language(paramVal.
                               szString.c_str());

            icalproperty_add_parameter(pProp, pParam);
            }
        }
        }

        icalproperty_free(pProp);
    }
}
template <class T>
void ICalConverter::exportGeoFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalgeotype geo;
    string szGeo;
    icalproperty *pProp = 0;    
    /*exporting geo */
    if (!(pComp->getGeo()).empty()) {
        unsigned int found;
        szGeo = pComp->getGeo();
        if ((found = szGeo.find(";")) != string::npos) {

        string szLat = szGeo.substr(0, found);
        string szLon = szGeo.substr(found);
        geo.lat = stringToFloat(szLat);
        geo.lon = stringToFloat(szLon);

        pProp = icalproperty_new_geo(geo);
        if (pProp) {
            icalcomponent_add_property(pEntcomp, pProp);
        }
        icalproperty_free(pProp);
        }
    }
}
template<class T>
void ICalConverter::exportClassFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;    
    /*exporting class */
    if (!(pComp->getClas()).empty()) {

        icalproperty_class icalClass = ICAL_CLASS_NONE;
        string szClass = pComp->getClas();
        if (szClass == PUBLIC)
        icalClass = ICAL_CLASS_PUBLIC;
        if (szClass == PRIVATE)
        icalClass = ICAL_CLASS_PRIVATE;
        if (szClass == CONFIDENTIAL)
        icalClass = ICAL_CLASS_CONFIDENTIAL;

        pProp = icalproperty_new_class(icalClass);
        if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
        }
        icalproperty_free(pProp);
    }
}
void ICalConverter::exportEventTranspFromLocal(icalcomponent *pEntcomp, CEvent *pEvent, FileType iType)
{
    //icalproperty *pProp = 0;    
    VCalConverter *pVCal = new VCalConverter();
    ASSERTION(pVCal);
    /*exporting Transp */
    if (!(pEvent->getTransparency()).empty()) {

        icalproperty_transp icalTransp = ICAL_TRANSP_NONE;
        icalproperty *transp = 0;
        string szTransp = pEvent->getTransparency();
        if (iType == VCAL_TYPE)
        transp = pVCal->getVcalTransp(szTransp);

        else {
        if (szTransp == TRANSPERANCY)
            icalTransp = ICAL_TRANSP_TRANSPARENT;
        if (szTransp == OPAQUE)
            icalTransp = ICAL_TRANSP_OPAQUE;

        transp = icalproperty_new_transp(icalTransp);
        }
        if (transp)
        icalcomponent_add_property(pEntcomp, transp);

        icalproperty_free(transp);
    }
    delete pVCal;
}
template<class T>
void ICalConverter::exportRelatedFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    hashMap.clear();
    /*exporting Related to */
    if (!(pComp->getRelated()).empty()) {
        pProp = icalproperty_new_relatedto((pComp->getRelated()).c_str());
    hashMap = pComp->getHashMap();
        if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
        it = hashMap.find(RELATEDTO);
        if (it != hashMap.end()) {
            for (unsigned int i = 0; i < (*it).second.size(); i++) {
            ParamType paramVal;
            string paramName;
            paramVal = (*it).second[i]->getParamValue();
            paramName = (*it).second[i]->getParamName();
            if (paramName == RELTYPE) {
                icalparameter_reltype icalrel =
                getIcalRelType((RelationshipType) paramVal.
                           i);
                pParam = icalparameter_new_reltype(icalrel);
            }
            icalproperty_add_parameter(pProp, pParam);
            }
        }
        }
        icalproperty_free(pProp);
    }
}
template<class T>
void ICalConverter::exportOrganizerFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;    
    /*exporting organizer */
    COrganizer *pOrg = pComp->getOrganizer();
    if (pOrg) {

        pProp = getIcalOrganizer(pOrg);
        if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
        }
    }
}
template<class T>
void ICalConverter::exportXPropFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    vector < CProperties * >vPropList;
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    hashMap.clear();
    /*exporting x-prop */
    vPropList.clear();
    vPropList = pComp->getXProperties();
    hashMap.clear();
    hashMap = pComp->getHashMap();
    for (unsigned int iCount = 0; iCount < vPropList.size(); iCount++) {
        PropType propVal;
        string propName = vPropList[iCount]->getPropName();

        // Ignore X-NOEXPORT- properties during export
        if (propName.find("X-NOEXPORT-") == 0)
        {
            CAL_DEBUG_LOG("Skip '%s'", propName.c_str());
            continue;
        };

        propVal = vPropList[iCount]->getPropValue();
        pProp = icalproperty_new_x(propVal.szString.c_str());
        icalproperty_set_x_name(pProp,
                    propName.c_str());

        it = hashMap.find(vPropList[iCount]->getPropName());
        if (it != hashMap.end()) {
        for (unsigned int i = 0; i < (*it).second.size(); i++) {
            ParamType paramVal;
            if ((*it).second[i] != NULL) {
            paramVal = (*it).second[i]->getParamValue();
            pParam = icalparameter_new_x(paramVal.szString.c_str());
            icalparameter_set_xname(pParam,
                        (*it).second[i]->
                        getParamName().c_str());
            icalproperty_add_parameter(pProp, pParam);
            }
        }
        }
        icalcomponent_add_property(pEntcomp, pProp);
    icalproperty_free(pProp);
    pProp = 0;
    }
}
template<class T>
void ICalConverter::exportResourcesFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    hashMap.clear();
    /*exporting resources */
    if (!(pComp->getResources()).empty()) {
        pProp =
        icalproperty_new_resources(pComp->getResources().c_str());
        hashMap = pComp->getHashMap();
        if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
        it = hashMap.find(RESOURCES);
        if (it != hashMap.end()) {
            for (unsigned int i = 0; i < (*it).second.size(); i++) {
            ParamType paramVal;
            string paramName;
            paramVal = (*it).second[i]->getParamValue();
            paramName = (*it).second[i]->getParamName();
            if (paramName == LANGUAGE)
                pParam =
                icalparameter_new_language(paramVal.
                               szString.c_str());
            if (paramName == ALTER)
                pParam =
                icalparameter_new_altrep(paramVal.
                             szString.c_str());

            icalproperty_add_parameter(pProp, pParam);
            }
        }
        }
        icalproperty_free(pProp);
    }
}
template<class T>
void ICalConverter::exportContactsFromLocal(icalcomponent *pEntcomp, T *pComp)
{
    icalproperty *pProp = 0;    
    icalparameter *pParam = 0;
    map < string, vector < CParameters * > >hashMap;
    map < string, vector < CParameters * > >::iterator it;
    hashMap.clear();
    /*exporting contacts */
    if (!(pComp->getContact()).empty()) {
        pProp = icalproperty_new_contact(pComp->getContact().c_str());
        hashMap = pComp->getHashMap();
        if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
        it = hashMap.find(CONTACT);
        if (it != hashMap.end()) {
            for (unsigned int i = 0; i < (*it).second.size(); i++) {
            ParamType paramVal;
            string paramName;
            paramVal = (*it).second[i]->getParamValue();
            paramName = (*it).second[i]->getParamName();
            if (paramName == LANGUAGE)
                pParam =
                icalparameter_new_language(paramVal.
                               szString.c_str());
            if (paramName == ALTER)
                pParam =
                icalparameter_new_altrep(paramVal.
                             szString.c_str());

            icalproperty_add_parameter(pProp, pParam);
            }
        }
        }
        icalproperty_free(pProp);
    }
}

void ICalConverter::exportTodoPercentCompleteFromLocal(icalcomponent *pEntcomp, CTodo *pTodo)
{
    int percentComplete = 0;
    icalproperty *pProp = 0;    
    /* exporting percent complete */
    percentComplete = pTodo->getPercentComplete();
    if (percentComplete != -1) {
        pProp = icalproperty_new_percentcomplete(percentComplete);
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
    }
}
void ICalConverter::exportTodoCompletedFromLocal(icalcomponent *pEntcomp, CTodo *pTodo)
{
    icalproperty *pProp = 0;    
    icaltimetype ical_completed;
    time_t completed;
    memset (&ical_completed, 0, sizeof(icaltimetype));
    completed = pTodo->getCompleted();
    string szZone = pTodo->getTzid();
    parseTimeZone(szZone);
    /*exporting completed */
    if (pTodo->getStatus() == COMPLETED_STATUS) {
        ical_completed = icaltime_from_timet_with_zone(completed, 0, 
        	icaltimezone_get_builtin_timezone(szZone.c_str()));
        pProp = icalproperty_new_completed(ical_completed);
        icalcomponent_add_property(pEntcomp, pProp);
        icalproperty_free(pProp);
    }
}


void ICalConverter::exportEntryStatusFromLocal(icalcomponent *pEntcomp, CComponent *pEntry, FileType iType)
{
    icalproperty *pProp = 0;    
    /*exporting status */
    if (pEntry->getStatus() != -1) {
    int iStatus = pEntry->getStatus();
    int icalStatus;
    if (iType == VCAL_TYPE) {
        string szStatus;
        VCalConverter *pVCal = new VCalConverter();
        szStatus = pVCal->getVcalStatus((statusParam) iStatus);
        pProp = icalproperty_new_from_string(szStatus.c_str());
    delete pVCal;
    } else {
        icalStatus = getIcalStatus((statusParam) iStatus);
        pProp =
        icalproperty_new_status((icalproperty_status) icalStatus);
    }
    if (pProp) {
        icalcomponent_add_property(pEntcomp, pProp);
    }
    icalproperty_free(pProp);
    }
}
void ICalConverter::exportEntryLastModifiedFromLocal(icalcomponent *pEntcomp, CComponent *pEntry)
{
    icalproperty *pProp = 0;    
    icaltimetype ical_ltime;
    time_t ltime;
    memset (&ical_ltime, 0, sizeof(icaltimetype));
    /*exporting last modified */
    ltime = pEntry->getLastModified();
    if (ltime) {
    ical_ltime = icaltime_from_timet_with_zone(ltime, 0,
        	icaltimezone_get_builtin_timezone("UTC"));
    pProp = icalproperty_new_lastmodified(ical_ltime);
    icalcomponent_add_property(pEntcomp, pProp);
    icalproperty_free(pProp);
    }
}

template<class T>
string ICalConverter::exportAlarmFromLocal(T *pComp, FileType iType)
{
    string strICalAlarm;
    /* exporting alarm */
    CAlarm *cAlarm = pComp->getAlarm();
    string sZone = pComp->getTzid();
    parseTimeZone(sZone);
    bool isAllday = false;
    if(( pComp->getType() == E_TODO) || 
        (pComp->getAllDay() == 1))
	    isAllday = true;

    if (cAlarm) {
        VCalConverter *vCalConverter = new VCalConverter();
        ASSERTION(vCalConverter);
	 
        strICalAlarm = getIcalAlarm(cAlarm,sZone,isAllday);
        if ((iType == VCAL_TYPE) && (strICalAlarm.empty() == 0))
            strICalAlarm = vCalConverter->iCalToVCalAlarm(strICalAlarm);
        delete vCalConverter;
        vCalConverter = 0;
    }
    return strICalAlarm;
}

void ICalConverter::exportEventProperties (icalcomponent *pEntcomp, CEvent *pEvent, FileType iType)
{
 
    /*exporting created time*/
    exportCreatedTimeFromLocal    (pEntcomp, pEvent, iType); 
    /*exporting date stamp*/
    exportDateStampFromLocal     (pEntcomp, pEvent);
    /*exporting summary*/
    exportSummaryFromLocal    (pEntcomp, pEvent, iType);
    /*exporting location */
    exportLocationFromLocal    (pEntcomp, pEvent);
    /*exporting description */
    exportDescriptionFromLocal    (pEntcomp, pEvent, iType);
    /*exporting guid - id is taken as uid */
    exportUidFromLocal        (pEntcomp, pEvent);
    /*exporting datestart */
    exportDateStartFromLocal    (pEntcomp, pEvent,iType);
    /*exporting dateend */
    exportEventDateEndFromLocal (pEntcomp, pEvent,iType);
    /*exporting attendees */
    exportAttendeesFromLocal      (pEntcomp, pEvent, iType);
    /*exporting attachments */
    exportAttachmentsFromLocal    (pEntcomp, pEvent);
    /* exporting priority */
    exportPriorityFromLocal    (pEntcomp, pEvent);
    /* exporting sequence */
    exportSequenceFromLocal    (pEntcomp, pEvent);
    /*exporting comments */
    exportCommentsFromLocal    (pEntcomp, pEvent, iType);
    /*exporting URL */
    exportUrlFromLocal        (pEntcomp, pEvent);
    /*exporting categories */
    exportCategoriesFromLocal    (pEntcomp, pEvent);
    /*exporting geo */
    exportGeoFromLocal        (pEntcomp, pEvent);
    /*exporting class */
    exportClassFromLocal    (pEntcomp, pEvent);
    /*exporting Transp */
   exportEventTranspFromLocal    (pEntcomp, pEvent, iType);
    /*exporting Related to */
    exportRelatedFromLocal    (pEntcomp, pEvent);
    /*exporting organizer */
    exportOrganizerFromLocal    (pEntcomp, pEvent);
    /*exporting x-prop */
    exportXPropFromLocal    (pEntcomp, pEvent);
    /*exporting resources */
    exportResourcesFromLocal    (pEntcomp, pEvent);
    /*exporting contacts */
    exportContactsFromLocal    (pEntcomp, pEvent);
}
void ICalConverter::exportTodoProperties (icalcomponent *pEntcomp, CTodo *pTodo, FileType iType)
{
    /* exporting priority */
    exportPriorityFromLocal        (pEntcomp, pTodo);
    /* exporting percent complete */
    exportTodoPercentCompleteFromLocal    (pEntcomp, pTodo);
    /*exporting location */
    exportLocationFromLocal        (pEntcomp, pTodo);
    /*exporting attachments */
    exportAttachmentsFromLocal        (pEntcomp, pTodo);
    /*exporting summary */
    exportSummaryFromLocal        (pEntcomp, pTodo, iType);
    /*exporting description */
    exportDescriptionFromLocal        (pEntcomp, pTodo, iType);
    /*exporting guid - id is taken as uid */
    exportUidFromLocal        	(pEntcomp, pTodo);
    /* exporting sequence */
    exportSequenceFromLocal        (pEntcomp, pTodo);
    /*exporting comments */
    exportCommentsFromLocal        (pEntcomp, pTodo, iType);
    /*exporting categories */
    exportCategoriesFromLocal        (pEntcomp, pTodo);
    /*exporting attendees */
    exportAttendeesFromLocal        (pEntcomp, pTodo, iType);
    /*exporting geo */
    exportGeoFromLocal        	(pEntcomp, pTodo);
    /*exporting organizer */
    exportOrganizerFromLocal        (pEntcomp, pTodo);
    /*exporting class */
    exportClassFromLocal        (pEntcomp, pTodo);
    /*exporting URL */
    exportUrlFromLocal        	(pEntcomp, pTodo);
    /*exporting x-prop */
    exportXPropFromLocal        (pEntcomp, pTodo);
    /*exporting Related to */
    exportRelatedFromLocal        (pEntcomp, pTodo);
    /*exporting completed */
    exportTodoCompletedFromLocal    (pEntcomp, pTodo);
    /*exporting created time */
    exportCreatedTimeFromLocal        (pEntcomp, pTodo, iType);
    /*exporting date stamp */
    exportDateStampFromLocal        (pEntcomp, pTodo);
    //due = pTodo->getDue();        
    /* We should use getDueDate : this calendar issue */
    /*exporting due*/
    exportDueFromLocal                  (pEntcomp,pTodo);
    /* exporting Dtstart*/
    exportDateStartFromLocal        (pEntcomp, pTodo, iType);
    /*exporting resources */
    exportResourcesFromLocal        (pEntcomp, pTodo);
    /*exporting contacts */
    exportContactsFromLocal        (pEntcomp, pTodo);
}
void ICalConverter::exportJournalProperties (icalcomponent *pEntcomp, CJournal *pJournal, FileType iType)
{
    /*exporting created time */
    exportCreatedTimeFromLocal     (pEntcomp, pJournal, iType);
    /*exporting location */
    exportLocationFromLocal    (pEntcomp, pJournal);
    /*exporting attachments */
    exportAttachmentsFromLocal    (pEntcomp, pJournal);
    /*exporting summary */
    exportSummaryFromLocal    (pEntcomp, pJournal, iType);
    /*exporting description */
    exportDescriptionFromLocal    (pEntcomp, pJournal, iType);
    /*exporting date stamp */
    exportDateStampFromLocal    (pEntcomp, pJournal);
    /*exporting guid - id is taken as uid */
    exportUidFromLocal        (pEntcomp, pJournal);
    /*exporting organizer */
    exportOrganizerFromLocal    (pEntcomp, pJournal);
    /*exporting attendees */
    exportAttendeesFromLocal    (pEntcomp, pJournal, iType);
    /*exporting x-prop */
    exportXPropFromLocal    (pEntcomp, pJournal);
    /*exporting URL */
    exportUrlFromLocal        (pEntcomp, pJournal);
    /*exporting contacts */
    exportContactsFromLocal    (pEntcomp, pJournal);
    /*exporting Related to */
    exportRelatedFromLocal    (pEntcomp, pJournal);
    /* exporting sequence */
    exportSequenceFromLocal    (pEntcomp, pJournal);
    /*exporting comments */
    exportCommentsFromLocal    (pEntcomp, pJournal, iType);
    /*exporting datestart */
    exportDateStartFromLocal    (pEntcomp, pJournal,iType);
    /*exporting categories */
    exportCategoriesFromLocal    (pEntcomp, pJournal);
    /*exporting class */
    exportClassFromLocal    (pEntcomp, pJournal);
}
/**
 * localToIcalVcal
 * @param pEntry: CComponent object
 * @param pErrorCode: Errorcode for the function
 * @return string, ical format string from local calendar database
 *
 * This function takes CComponent object as parameter
 * converts this into ical format and
 * returns vcalendar string
 */
string ICalConverter::localToIcalVcal(CComponent * pEntry, FileType iType,
                      int &pErrorCode)
{
    string strIcalComp;
    string strTimeZone;
    bool globalAllDayFlag = false;

    if (pEntry == NULL) {
    pErrorCode = CALENDAR_INVALID_ARG_ERROR;
    return strIcalComp;
    }

    icalcomponent *m_pCalcomp = NULL;
    icalcomponent *pEntcomp = NULL;
    icalproperty *pProp = 0;
    CEvent *pEvent = NULL;
    CTodo *pTodo = NULL;
    CJournal *pJournal = NULL;
    struct icaltimetype ical_ctime;
    memset (&ical_ctime, 0, sizeof (struct icaltimetype));

    m_pCalcomp = icalcomponent_new_vcalendar();

    if (!m_pCalcomp) {
    strIcalComp = "";
    pErrorCode = CALENDAR_MEMORY_ERROR;
    return strIcalComp;
    }

    icalcomponent *pComp = NULL;
    vector < string > rrules;
    string strICalAlarm;
    string strRDate ;
    string strERDate;

    /* VERSION */
    switch(iType) {
		case ICAL_TYPE:
			pProp = icalproperty_new_version("2.0");
			break;
		case VCAL_TYPE:
		default:
			pProp = icalproperty_new_version("1.0");
			break;
	}
    icalcomponent_add_property(m_pCalcomp, pProp);
    icalproperty_free(pProp);

    /* PRODID */
    pProp = icalproperty_new_prodid (ICAL_PRODID);
    icalcomponent_add_property (m_pCalcomp, pProp);
    icalproperty_free(pProp);

    if ((pEntry->getType() == E_EVENT) || 
        (pEntry->getType() == E_BDAY)) {
    pEvent = (CEvent *) pEntry;
    /*Bday event is exported as event */
    pEvent->setType(E_EVENT);

    pEntcomp = icalcomponent_new_vevent();

    if (!pEntcomp) {
        strIcalComp = "";
        pErrorCode = CALENDAR_ICAL_PARSE_ERROR;
    icalcomponent_free(m_pCalcomp);
        return strIcalComp;
    }

    pComp = pEntcomp;
    /* exporting event properties */
    exportEventProperties (pEntcomp, pEvent, iType);

    /* exporting timezone component */
    strTimeZone = exportTimeZone (pEvent->getDateStart(), pEvent->getTzid(), iType);

    /*exporting event recurrence*/
    CRecurrence *cRecur = pEvent->getRecurrence();
    if (cRecur) {
        VCalConverter *vCalConverter = new VCalConverter();
    ASSERTION(vCalConverter);
        /* RRULE */
        vector < CRecurrenceRule * >list = cRecur->getRecurrenceRule();
        CRecurrenceRule *cRRule = NULL;

    string delimeter;
        for (unsigned int rule_index = 0; rule_index < list.size();
         rule_index++) {
        cRRule = (CRecurrenceRule *) list.at(rule_index);
        string rule;
        string rrule;
        if (iType == ICAL_TYPE) {
            rule = cRRule->getRrule();
        delimeter = COMMA;
        } else {
            rule = vCalConverter->iCalToVCalRule(
        	    cRRule->getRrule(),
        	    pEvent->getDateStart());
        delimeter = SEMI_COLON;
        }
        if (rule.empty() == 0) {
            if (cRRule->getRuleType() == 0) {    /* rrule */
            rrule.append(RRULE_COL).append(rule);
            } else {    /* exrule */
            rrule.append(EXRULE_COL).append(rule);
            }
            rule.clear();
        }
        cRRule = NULL;
        rrules.push_back(rrule);
        }
        delete vCalConverter;
    
        /* RDATEs and EXDATEs*/
    string szZone = pEntry->getTzid();
    parseTimeZone(szZone);
        vector < string > rdays = cRecur->getRDays();
    if(!pEntry->getAllDay()) {
        szZone = "UTC";
    }
    /* RDATE */
        if (rdays.size() != 0)
        strRDate = RDATE_COL;
    time_t time_t_rdate = 0;
    struct icaltimetype ical_rdate;
    memset (&ical_ctime, 0, sizeof (struct icaltimetype));
    for (unsigned int rday = 0; rday < rdays.size(); rday++) {
    		time_t_rdate = (time_t)atoi( rdays.at(rday).c_str());
    		ical_rdate = icaltime_from_timet_with_zone (time_t_rdate, 0,
    						icaltimezone_get_builtin_timezone(szZone.c_str()));
		if( pEntry->getAllDay())
			ical_rdate.is_utc=0;
    		CAL_DEBUG_LOG("RDATE %s", icaltime_as_ical_string (ical_rdate));
    		strRDate = strRDate + icaltime_as_ical_string (ical_rdate) + delimeter;
    }
    strRDate = strRDate.substr(0, strRDate.size() - 1) + "\n\n";
    /* EXDATE */
    vector < string > erdays = cRecur->getEDays();
    if (erdays.size() != 0)
    		strERDate = EXDATE_COL;
    time_t time_t_exdate = 0;
    struct icaltimetype ical_exdate;
    for (unsigned int erday = 0; erday < erdays.size(); erday++) {
    		time_t_exdate = (time_t)atoi( erdays.at(erday).c_str());
    		ical_exdate = icaltime_from_timet_with_zone (time_t_exdate, 0,
    						icaltimezone_get_builtin_timezone(szZone.c_str()));	
		if( pEntry->getAllDay())
			ical_rdate.is_utc=0;
    		CAL_DEBUG_LOG("EXDATE %s", icaltime_as_ical_string (ical_exdate));
    		strERDate = strERDate + icaltime_as_ical_string (ical_exdate) + delimeter;
    }
    strERDate = strERDate.substr(0, strERDate.size() - 1) + "\n\n";
    }
    /* Alarm */
    strICalAlarm = exportAlarmFromLocal (pEvent, iType);
    /* if the event is allday then donot export the 
     * TZ information in VCAL formt as the allday event has to 
     * be added as allday event in any timezone*/
    globalAllDayFlag = pEvent->getAllDay();
    
    }
    if (pEntry->getType() == E_TODO) {

        pTodo = (CTodo *) pEntry;
        strTimeZone = exportTimeZone (pTodo->getDateStart(), pTodo->getTzid(), iType);
        pEntcomp = icalcomponent_new_vtodo();
        if (!pEntcomp) {
    	    strIcalComp = "";
    	    pErrorCode = CALENDAR_ICAL_PARSE_ERROR;
    	    icalcomponent_free(m_pCalcomp);
    	    return strIcalComp;
        }
        pComp = pEntcomp;
        /* exporting todo properties */    
        exportTodoProperties (pEntcomp, pTodo, iType);
        /* Alarm */
        strICalAlarm = exportAlarmFromLocal (pTodo, iType);
    }
    if (pEntry->getType() == E_JOURNAL) {
    	pJournal = (CJournal *) pEntry;
    	pEntcomp = icalcomponent_new_vjournal();
    	if (!pEntcomp) {
    			strIcalComp = "";
    			pErrorCode = CALENDAR_ICAL_PARSE_ERROR;
    			icalcomponent_free(m_pCalcomp);
    			return strIcalComp;
    	}

    pComp = pEntcomp;
    /* exporting journal properties */
    exportJournalProperties (pEntcomp, pJournal, iType);
    }
    
    /*exporting status */
    exportEntryStatusFromLocal    (pEntcomp, pEntry, iType);
    /*exporting last modified */
    exportEntryLastModifiedFromLocal    (pEntcomp, pEntry);
    
    if (pComp) {
    icalcomponent_add_component(m_pCalcomp, pComp);
    strIcalComp = icalcomponent_as_ical_string(m_pCalcomp);
    if (iType == VCAL_TYPE) {
    time_t ctime = pEntry->getCreatedTime(); 
    if (ctime) {
        ical_ctime = icaltime_from_timet_with_zone(ctime, 0,
            icaltimezone_get_builtin_timezone("UTC"));
    }
        VCalConverter *pVCal = new VCalConverter();
        strIcalComp =
        pVCal->
        getVcalCreatedTime(string
                   (icaltime_as_ical_string(ical_ctime)),
                   strIcalComp);
    delete pVCal;
    }

    }
    if ( pEntry->getType() != E_JOURNAL){
         if (rrules.size())
            strIcalComp = addRuleToString(strIcalComp, rrules, pEntry->getType());
        if (strICalAlarm.empty() == 0) 
            strIcalComp = addAlarmDateToString(strIcalComp, strICalAlarm, (entryType)pEntry->getType());

        if (strRDate.empty()== 0) 
            strIcalComp = addAlarmDateToString(strIcalComp, strRDate,(entryType) pEntry->getType());

        if (strERDate.empty() == 0) 
            strIcalComp = addAlarmDateToString(strIcalComp, strERDate,(entryType) pEntry->getType());
    }
    
    /*** Specify the time zone information if the export is VCAL Type **************/
    /*    VERSION:1.0 
     *    And also if the event is not allday           		    *
     *    We will always export in VCAL format while doing sync with pcsuite        *
     *    or mobile         					            *
     *                                                                           *
    *******************************************************************************/
    //if ((iType == VCAL_TYPE) && (globalAllDayFlag== false) && (pEntry->getType()!=E_TODO))  {
    if(iType == VCAL_TYPE) {
        strIcalComp.replace(strIcalComp.find("VERSION:1.0",0),11, strTimeZone.c_str());
    }
    else if(iType == ICAL_TYPE) {
    	strIcalComp.replace(strIcalComp.find("VERSION:2.0",0),11, strTimeZone.c_str());
    }
    //}

   replaceWithEncodedString(strIcalComp, (CComponentDetails *)pEntry);
   strIcalComp = this->appendControlM(strIcalComp);
   /* probably it is a good idea to add TZ information 
    *  in ICAL format too  */
        

    CAL_DEBUG_LOG(" ICALCOMPONENT is %s",strIcalComp.c_str());
    icalcomponent_free(pEntcomp);
    icalcomponent_free(m_pCalcomp);

    return strIcalComp;
}

void ICalConverter::replaceWithEncodedString(string &szIcalComp, 
        			CComponentDetails *pEntry)
{
    string des = "DESCRIPTION;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:";
    string summary = "SUMMARY;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:";
    string loc = "LOCATION;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:";
    string comments = "COMMENT;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:";
        string replace;
    string szText;
    string szEncoded;
    CUtility *pUt = 0;
    pUt = CUtility::Instance();

    
    if(szIcalComp.find(summary) != string::npos) {
        szText = pEntry->getSummary();
        if(!szText.empty()) {
                szEncoded = pUt->encodeQuotedPrintable(szText);
                replace.append(summary);
                replace.append(szEncoded);
            pUt->stringReplaceOne(szIcalComp, summary.c_str(), 
        		replace.c_str());
        }
    }
    szText.clear();
    szEncoded.clear();
    replace.clear();
    

    if(szIcalComp.find(loc) != string::npos) {
        szText = pEntry->getLocation();
        if(!szText.empty()) {
                szEncoded = pUt->encodeQuotedPrintable(szText);
                replace.append(loc);
                replace.append(szEncoded);
            pUt->stringReplaceOne(szIcalComp, loc.c_str(), replace.c_str());
        }
    }
    szText.clear();
    szEncoded.clear();
    replace.clear();

    
    if(szIcalComp.find(des) != string::npos) {
        szText = pEntry->getDescription();
        if(!szText.empty()) {
                szEncoded = pUt->encodeQuotedPrintable(szText);
                replace.append(des);
                replace.append(szEncoded);
            pUt->stringReplaceOne(szIcalComp, des.c_str(), replace.c_str());
        }
    }
    szText.clear();
    szEncoded.clear();
    replace.clear();


    if(szIcalComp.find(comments) != string::npos) {
        szText = pEntry->getComments();
        if(!szText.empty()) {
                szEncoded = pUt->encodeQuotedPrintable(szText);
                replace.append(comments);
                replace.append(szEncoded);
            pUt->stringReplaceOne(szIcalComp, comments.c_str(), 
        	    replace.c_str());
        }
    }
}
    
/**
 * initTime
 * @param icaltimetype structure
 * @return tm structure
 *
 * this function takes icaltimetype structure and coverts it into tm structure
 */
struct tm ICalConverter::initTime(icaltimetype time_ical)
{
    struct tm tm1;
    memset (&tm1, 0, sizeof (struct tm));
    
    if(icaltime_is_null_time(time_ical))
    return tm1;

    tm1.tm_year = time_ical.year - NINETEEN_HUNDRED;
    tm1.tm_mon = time_ical.month - ONE;
    tm1.tm_mday = time_ical.day;
    tm1.tm_hour = time_ical.hour;
    tm1.tm_min = time_ical.minute;
    tm1.tm_sec = time_ical.second;
    /* tm_isdst will make further calls to 
     * decide daylight correction based on system time settings
     */
    tm1.tm_isdst = -1;

    return tm1;
}

/**
 * getRecurrenceDates()
 */
vector < string > ICalConverter::getRecurrenceDates(string strIcalComp,
                            icalcomponent * pComp,
        	            CEvent* pEvent,
                            icalproperty_kind kind,
			    int type)
{
    vector < string > dateList;
    vector < string > list;
    string strRuleType;
    string sZone = pEvent->getTzid();
    parseTimeZone(sZone);
    
    RuleType iRuleType;
    switch (kind) {
        case ICAL_RDATE_PROPERTY:
            strRuleType = RDATE;
            iRuleType = RECURRENCE_RULE;
            break;
        case ICAL_EXDATE_PROPERTY:
            strRuleType = EXDATE;
            iRuleType = EXCEPTION_RULE;

            break;
        default:
            break;
    }
    if (strIcalComp.find(strRuleType) != string::npos ) {
    unsigned int sPos = 0;
    int count = 0;
    while ((sPos =
        strIcalComp.find(strRuleType, sPos)) != string::npos) {
        sPos += 5;
        count += 1;

    }
    if (strIcalComp.find(strRuleType) != string::npos ) {
    string szTemp ;
    int i = 1;
    VCalConverter *vCalConv = new VCalConverter();
    ASSERTION(vCalConv);
    while (count >= i) {
        string date = get_line_in_ical(strIcalComp, strRuleType, i);
    /* EXDATE */
    if (date.size() == strRuleType.size()) {
            i += 1;
    	continue;
    }
        date =
        date.substr(strRuleType.size() + 1,
                date.size() - strRuleType.size() - 1);

        size_t start_val = date.find(':');
        if (start_val != string::npos)
            date = date.substr(start_val+1);

        /* remove spaces */
        size_t i1=0;
        while((i1=date.find(' ',i1))!=string::npos) 
            date.erase(i1,1);

    icalvalue* pVal = icalvalue_new_from_string(ICAL_PERIOD_VALUE, date.c_str());
    int temp = icalvalue_is_valid(pVal);
        if (pComp && temp) {
        date = vCalConv->iCalToVCalDate(date);
        }
	if (type == ICAL_TYPE)
		list = vCalConv->getTokens(date, ",");
	else
		list = vCalConv->getTokens(date, SEMI_COLON);
        for (unsigned int k = 0; k < list.size(); k++) {
        /* convert each of these value to 
         * time_t and then put in a string
         */
        struct icaltimetype tempExDate;
        time_t exceptionTime = 0;
        string Zone = sZone;
        szTemp = list.at(k);
        tempExDate = icaltime_from_string(szTemp.c_str());
        if (!icaltime_is_valid_time(tempExDate))
    	  break;
        if (tempExDate.is_utc == 1)
        	Zone = "UTC";
        exceptionTime = icaltime_as_timet_with_zone(tempExDate,
        		icaltimezone_get_builtin_timezone(Zone.c_str()));
        CAL_DEBUG_LOG("Exception Date: %s",  ctime(&exceptionTime));
        char *data= 0;
        asprintf(&data,"%ld",exceptionTime);
        if(data){ 	
        	CAL_DEBUG_LOG("Exception Date in string: %s",  data);
        	dateList.push_back(data);
        	free(data);
        	data = 0;
        }

        }
        i += 1;
    }
    delete vCalConv;
    vCalConv = 0;

    }
    }
    return dateList;
}

/**
 * getRecurrenceRule()
 */
CRecurrenceRule *ICalConverter::getRecurrenceRule(string strIcalComp,
                          int index,
                          string strRuleType,
                          FileType iType)
{
    CRecurrenceRule *cRRule = NULL;
    if (strIcalComp.empty() == 0) {
    string rrule = get_line_in_ical(strIcalComp, strRuleType, index);
    CAL_DEBUG_LOG("Rrrule is %s\n", rrule.c_str());
    if (rrule.size() == strRuleType.size()){
        return cRRule;
    }
    rrule =
        rrule.substr(strRuleType.size() + 1,
             rrule.size() - strRuleType.size() - 1);
    CAL_DEBUG_LOG("Rrrule is %s\n", rrule.c_str());
    if (iType == VCAL_TYPE) {    /* VCAL RRULE */
        VCalConverter *vCalConverter = new VCalConverter();
    ASSERTION(vCalConverter);
        rrule = vCalConverter->vCalToICalRule(rrule);
        delete vCalConverter;
    }
    if (rrule.empty() == 0) {
        cRRule = new CRecurrenceRule();
    ASSERTION(cRRule);
        cRRule->setRrule(rrule);
    }
    }
    return cRRule;
}

/**
 * getRecurrence
 */
vector <
    CRecurrenceRule * >ICalConverter::getRecurrence(string strIcalComp,
                            icalcomponent * pComp,
                            icalproperty_kind kind,
			    FileType iType)
{
    CRecurrenceRule *cRRecur = NULL;
    icalproperty *pProp = 0;
    icalvalue    *pVal = 0;
    string rrule;
    vector < CRecurrenceRule * >recList;
    string strRuleType;
    RuleType iRuleType = RECURRENCE_RULE;

    if (kind == ICAL_RRULE_PROPERTY) {
	strRuleType = RRULE;
	iRuleType = RECURRENCE_RULE;
    } else if (kind == ICAL_EXRULE_PROPERTY) {
	strRuleType = EXRULE;
	iRuleType = EXCEPTION_RULE;
    }
    if (pComp) {
	int count = icalcomponent_count_properties(pComp, kind);
	int i = 1;

	pProp = icalcomponent_get_first_property(pComp, kind);
	pVal = icalproperty_get_value (pProp);
	if (pProp && icalvalue_is_valid (pVal)) {
	    cRRecur =
		getRecurrenceRule(strIcalComp, i, strRuleType, iType);

	    if (cRRecur){
		cRRecur->setRuleType(iRuleType);
		recList.push_back(cRRecur);
	    }
	}

	pProp = NULL;
	while (count != i && count > 1) {
	    pProp = icalcomponent_get_next_property(pComp, kind);
	    if (pProp) {
		cRRecur =
		    getRecurrenceRule(strIcalComp, (i = i + 1),
			    strRuleType, iType);

		if (cRRecur){
		    cRRecur->setRuleType(iRuleType);
		    recList.push_back(cRRecur);
		}
		pProp = NULL;
	    }
	}
    } else if ((int) strIcalComp.find(strRuleType) != -1) {
	unsigned int sPos = 0;
	int count = 0;
	while ((sPos = strIcalComp.find(strRuleType, sPos)) != string::npos) {
	    sPos += 5;
	    count += 1;
	}
	int i = 0;
	while (count != i) {
	    cRRecur =
		getRecurrenceRule(strIcalComp, (i = i + 1), strRuleType,
			iType);
	    if (cRRecur) {
		cRRecur->setRuleType(iRuleType);
		recList.push_back(cRRecur);
	    }
	}

    }
    return recList;
}

/**
 * getAlarm()
 */
template <class T> CAlarm *ICalConverter::getAlarm(string strIcalComp, icalcomponent * pComp,T* pEntry)
{
CAlarm *alarm = NULL;
icalcomponent *pRececomp = 0;
icalproperty *pProp = 0;
struct icaltriggertype trigger;
/*struct icaldurationtype duration;*/
int repeat = 0;
/* initialize trigger time to start date of entry */
time_t st_time = 0;
string szZone = pEntry->getTzid();
parseTimeZone(szZone);
bool free_needed = false;
if (!pComp) {
    /* pComp will be null if the alarm is VCAL ALARM 
     * so parsing the string to get proper Alarm */
    VCalConverter *pvCalConverter = new VCalConverter();
    ASSERTION(pvCalConverter);
    strIcalComp = get_line_in_ical(strIcalComp, A_ALARM, 1);
    string strAlarm = pvCalConverter->vCalToICalAlarm(strIcalComp);
    string strComp;
    strComp = BEGIN_CALENDAR;
    strComp = strComp + "\n" + strAlarm + END_CALENDAR;

    pRececomp =
    icalcomponent_new_from_string((char *) strComp.c_str());
    if (!pRececomp){
        delete pvCalConverter;
        	pvCalConverter = 0;	
        return 0;
    }
    free_needed = true;
    
    pComp =
    icalcomponent_get_first_component(pRececomp,
    ICAL_VALARM_COMPONENT);
    
    delete pvCalConverter;
    pvCalConverter = 0;
}
        /* here we handle ICS ALARMS */
if (pComp) {
    alarm = new CAlarm();
    ASSERTION(alarm);
    bool flag = true;
    pProp =
    icalcomponent_get_first_property(pComp, ICAL_TRIGGER_PROPERTY);
	if (!icalproperty_kind_is_valid(ICAL_TRIGGER_PROPERTY) || 
		!ical_value_is_valid(pProp))
		flag = false;
    if (pProp && flag) {
        trigger = icalproperty_get_trigger(pProp);
        if( !icaltime_is_null_time ( trigger.time ))
        {

        	string szTempZone = szZone;
        	if(trigger.time.is_utc == 1)
        		szTempZone = "UTC";
        	st_time =icaltime_as_timet_with_zone(trigger.time,
        			icaltimezone_get_builtin_timezone(szTempZone.c_str()));
	        	
        		
        }
        else if (!icaldurationtype_is_null_duration(trigger.duration))
        {
            time_t trigger_offset = 0;	
            CAL_DEBUG_LOG("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
            CAL_DEBUG_LOG("Duration field in ical is  : %s ",
        		icaldurationtype_as_ical_string(trigger.duration));
            CAL_DEBUG_LOG("Printing the contents of duration ");
            CAL_DEBUG_LOG(" is_neg : %d", trigger.duration.is_neg);
            CAL_DEBUG_LOG(" days : %d",trigger.duration.days );
            CAL_DEBUG_LOG(" hours: %d",trigger.duration.hours );
            CAL_DEBUG_LOG(" minutes : %d",trigger.duration.minutes);
            CAL_DEBUG_LOG(" seconds : %d",trigger.duration.seconds);
            CAL_DEBUG_LOG("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
            trigger_offset  = trigger.duration.days * 86400 + trigger.duration.hours * 3600 + 
        	    trigger.duration.minutes * 60 + trigger.duration.seconds;
            if(trigger.duration.is_neg)
        	    trigger_offset = 0 - trigger_offset;
            /* st_time is actual trigger time */
            st_time = pEntry->getDateStart()+ trigger_offset;

        }
        else
        {
        	/* unknown trigger format */
        	CAL_DEBUG_LOG("Trigger format not recognised ");
        }
    }

    /* Invalid alarm value is provided so
     * delete existing alarm and  return NULL
     */ 
    if (st_time == 0){
    	delete alarm;
    	alarm = 0;
	    CAL_DEBUG_LOG("Alarm value is null");
	    return 0;
    }
         	    

    alarm->setTrigger(st_time);
    /* it is incorrect to fetch the snooze duration and 
     * fill it here  we need to make a decision based on
     * the trigger time 
     * */
    /* currently we don't have a place holder for snooze duration 
     * it is being used by alarm flag 
     * need seperate place holder for alarm flag*/
    time_t pre_offset = 0;
    pre_offset = pEntry->getDateStart() - alarm->getTrigger();
    if (pEntry->getType() == E_EVENT)
    {
	if (pre_offset == 0)
        	alarm->setDuration(E_AM_ETIME);
        else if (pre_offset == 300)
        	alarm->setDuration(E_AM_5MIN);
        else if (pre_offset == 900)
        	alarm->setDuration(E_AM_15MIN);
        else if (pre_offset == 1800)
        	alarm->setDuration(E_AM_30MIN);
        else if (pre_offset == SEC_IN_HR)
        	alarm->setDuration(E_AM_1HR);
        else if (pre_offset == 3 * SEC_IN_HR)
        	alarm->setDuration(E_AM_3HR);
        else if (pre_offset == 24 * SEC_IN_HR){
        	alarm->setDuration(E_AM_DAYBEFORE);
		alarm->setTrigger( pEntry->getDateStart() - 24*SEC_IN_HR);
        }else{ 
		alarm->setDuration(E_AM_EXACTDATETIME);
		alarm->setTimeBefore(pre_offset);	
        }
    }
    else if (pEntry->getType() == E_TODO)
    {
        CAL_DEBUG_LOG ("Synced task With Due date, alarm is set to %s", ctime(&st_time));
        alarm->setTrigger(st_time);
        alarm->setDuration(E_AM_EXACTDATETIME);
        alarm->setTimeBefore(0);
    }


    pProp =
        icalcomponent_get_first_property(pComp, ICAL_REPEAT_PROPERTY);
    if (pProp) {
        repeat = icalproperty_get_repeat(pProp);
        alarm->setRepeat(repeat);
    }
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_ACTION_PROPERTY);
    if (pProp) {
        //We handle only audio alarms so we expect there is only 
        //audio alarm
        alarm->setAction(ICAL_ACTION_AUDIO);
    }
}
    /* we need to free it if only we 
     * have created it by using new */
    if(free_needed)
        icalcomponent_free(pRececomp);	

    return alarm;
}

/**
 * getIntFromIcaltimetype
 * @param icaltime
 * @return integer value
 *
 * This functioni used to get int equivalent for icaltime
 */
int ICalConverter::getIntFromIcaltimetype(struct icaltimetype ical_time)
{

    struct tm tm1 = initTime(ical_time);
    time_t st_time = time_mktime(&tm1, NULL);
    int time1 = st_time;
    return time1;
}

/**
 * floatToString()
 * @param: float value 
 * @return: string
 *
 * This function converts given float value to string
 */
string floatToString(float value)
{
    string szString;
    stringstream ss;
    ss << value;
    ss >> szString;
    return szString;
}

/**
 * getTranspFromIcal
 * @param: iTransp icalproperty_transp, transparency in ICAL format
 * @return: returns string format of transparency mapping to calendar-backend
 *
 * This function returns the mapping for transparency from ICAL format to local 
 * calendar database.
 */
string ICalConverter::getTranspFromIcal(icalproperty_transp iTransp)
{
    if (iTransp == ICAL_TRANSP_TRANSPARENT)
    return TRANSPERANCY;

    else if (iTransp == ICAL_TRANSP_OPAQUE)
    return OPAQUE;

    return OPAQUE;
}

/**
 * getOrganizerFromIcal()
 * @param: icalproperty pointer
 * @return: COrganizer pointer
 *
 * This function converts the ical organizer to COrganizer.
 */
COrganizer *ICalConverter::getOrganizerFromIcal(icalproperty * pProp)
{

    COrganizer *pOrg = 0;
    icalparameter *pParam = 0;

    bool flag = ical_value_is_valid (pProp);
    if (pProp && flag) {
    pOrg = new COrganizer();
    ASSERTION(pOrg);
    if(icalproperty_get_organizer(pProp))
    pOrg->setMailTo(icalproperty_get_organizer(pProp));

    pParam =
        icalproperty_get_first_parameter(pProp, ICAL_CN_PARAMETER);
    if (pParam) {
        pOrg->setCommonName((char *) icalparameter_get_cn(pParam));
        pParam = NULL;
    }

    pParam =
        icalproperty_get_first_parameter(pProp, ICAL_DIR_PARAMETER);
    if (pParam) {
        pOrg->setDirectoryParameter((char *)
                    icalparameter_get_dir(pParam));
        pParam = NULL;
    }

    pParam =
        icalproperty_get_first_parameter(pProp, ICAL_SENTBY_PARAMETER);
    if (pParam) {
        pOrg->setSentBy((char *) icalparameter_get_sentby(pParam));
        pParam = NULL;
    }

    pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_LANGUAGE_PARAMETER);
    if (pParam) {
        pOrg->setLanguage((char *) icalparameter_get_language(pParam));
        pParam = NULL;
    }
    return pOrg;
    } else
    return NULL;
}

/**
 * getParticipationRoleFromIcal()
 * @param: icalparameter_role icalRole
 * @return: int
 *
 * This function returns the mapping for Participation Role from ICAL format to local 
 * calendar database.
 */
int ICalConverter::
getParticipationRoleFromIcal(icalparameter_role icalRole)
{

    switch (icalRole) {
        case ICAL_ROLE_CHAIR:
            return CHAIR;
        case ICAL_ROLE_REQPARTICIPANT:
            return REQ_PARTICIPANT;
        case ICAL_ROLE_OPTPARTICIPANT:
            return OPT_PARTICIPANT;
        case ICAL_ROLE_NONPARTICIPANT:
            return NON_PARTICIPANT;
        default:
            return REQ_PARTICIPANT;
    }

}

/**
 * getParticipationStatusFromIcal()
 * @param: icalparameter_partstat icalpartstat
 * @return: int
 *
 * This function returns the mapping for Participation Status from ICAL format to local 
 * calendar database.
 */
int ICalConverter::
getParticipationStatusFromIcal(icalparameter_partstat icalPartStat)
{

    switch (icalPartStat) {
        case ICAL_PARTSTAT_NEEDSACTION:
            return NEEDS_ACTION;
        case ICAL_PARTSTAT_ACCEPTED:
            return ACCEPTED;
        case ICAL_PARTSTAT_DECLINED:
            return DECLINED;
        case ICAL_PARTSTAT_TENTATIVE:
            return TENTATIVE;
        case ICAL_PARTSTAT_DELEGATED:
            return DELEGATED;
        case ICAL_PARTSTAT_COMPLETED:
            return COMPLETED;
        case ICAL_PARTSTAT_INPROCESS:
            return IN_PROCESS;
        default:
            return NEEDS_ACTION;
    }
}

/**
 * getRSVPFromIcal()
 * @param: icalparameter_rsvp icalRSVP
 * @return: bool
 *
 * This function returns the mapping for RSVP from ICAL format to local 
 * calendar database.
 */
bool ICalConverter::getRSVPFromIcal(icalparameter_rsvp icalRSVP)
{
    if (icalRSVP == ICAL_RSVP_TRUE)
    return true;
    else
    return false;

}

/**
 * getCUTypeFromIcal()
 * @param: icalparameter_cutype icalCUType
 * @return: int
 *
 * This function returns the mapping for CUType from ICAL format to local 
 * calendar database.
 */
int ICalConverter::getCUTypeFromIcal(icalparameter_cutype icalCUType)
{
    switch (icalCUType) {
        case ICAL_CUTYPE_INDIVIDUAL:
            return INDIVIDUAL;
        case ICAL_CUTYPE_GROUP:
            return GROUP;
        case ICAL_CUTYPE_RESOURCE:
            return RESOURCE;
        case ICAL_CUTYPE_ROOM:
            return ROOM;
        case ICAL_CUTYPE_UNKNOWN:
            return UNKNOWN_USER;
        case ICAL_CUTYPE_NONE:
            return USER_NONE;
        default:
            return INDIVIDUAL;
    }

}
int ICalConverter::getStatusFromIcal(icalproperty_status iStatus)
{

    switch (iStatus) {
        case ICAL_STATUS_TENTATIVE:
            return TENTATIVE_STATUS;
        case ICAL_STATUS_CONFIRMED:
            return CONFIRMED_STATUS;
        case ICAL_STATUS_COMPLETED:
            return COMPLETED_STATUS;
        case ICAL_STATUS_NEEDSACTION:
            return NEEDSACTION_STATUS;
        case ICAL_STATUS_CANCELLED:
            return CANCELLED_STATUS;
        case ICAL_STATUS_INPROCESS:
            return INPROCESS_STATUS;
        case ICAL_STATUS_DRAFT:
            return DRAFT_STATUS;
        case ICAL_STATUS_FINAL:
            return FINAL_STATUS;
        default:
            return 0;
    }
}

/**
 * getClasFromIcal()
 * @param: icalproperty_class iClass
 * @return: string
 *
 * This function returns the mapping for Class from ICAL format to local 
 * calendar database.
 */
string ICalConverter::getClasFromIcal(icalproperty_class iClass)
{
    if (iClass == ICAL_CLASS_PUBLIC)
    return PUBLIC;
    else if (iClass == ICAL_CLASS_PRIVATE)
    return PRIVATE;
    else if (iClass == ICAL_CLASS_CONFIDENTIAL)
    return CONFIDENTIAL;
    else
    return PUBLIC;

}

/**
 * getAttendeeFromIcal()
 * @param: icalproperty pointer
 * @return: CAttendee pointer
 *
 * This function converts the ical attendee to CAttendee.
 */
CAttendee *ICalConverter::getAttendeeFromIcal(icalproperty * pProp,
                          FileType iType)
{
    CAttendee *pAtten = 0;
    icalparameter *pParam = 0;
    VCalConverter *pVCal = 0;

    if (pProp) {
    string szAtten;
    const char* ptemp = icalproperty_as_ical_string(pProp);
    if(ptemp)
        szAtten = ptemp;
    
    pAtten = new CAttendee();
    ASSERTION(pAtten);
    char * AttValue = 0;
    AttValue = (char *) icalproperty_get_attendee(pProp);
    if (AttValue)
       pAtten->setMailTo(AttValue);
   CAL_DEBUG_LOG("setting Mail to" );
    if (iType == ICAL_TYPE) {
        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_DELEGATEDFROM_PARAMETER);
        if (pParam) {
        pAtten->setDelegator((char *)
                     icalparameter_get_delegatedfrom
                     (pParam));
        pParam = NULL;
        }
        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_DELEGATEDTO_PARAMETER);
        if (pParam) {
        pAtten->setDelegatees((char *)
                      icalparameter_get_delegatedto
                      (pParam));
        pParam = NULL;
        }
        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_MEMBER_PARAMETER);
        if (pParam) {
        pAtten->
            setMember((char *) icalparameter_get_member(pParam));
        pParam = NULL;
        }
    }
    pParam =
        icalproperty_get_first_parameter(pProp, ICAL_ROLE_PARAMETER);
    if (pParam) {
        icalparameter_role icalRole;
        int iRole;
        if (iType == VCAL_TYPE) {
        iRole = pVCal->getParticipationRoleFromVcal(szAtten);

        } else {
        icalRole = icalparameter_get_role(pParam);
        iRole = getParticipationRoleFromIcal(icalRole);
        }
        pAtten->setRole((ParticipantRole) iRole);
        pParam = NULL;
    }
    pParam =
        icalproperty_get_first_parameter(pProp, ICAL_RSVP_PARAMETER);
    if (pParam) {
        icalparameter_rsvp icalRSVP;
        bool fRSVP;
        if (iType == VCAL_TYPE)
        fRSVP = pVCal->getRSVPFromVcal(szAtten);
        else {
        icalRSVP = icalparameter_get_rsvp(pParam);
        fRSVP = getRSVPFromIcal(icalRSVP);
        }
        pAtten->setRSVP(fRSVP);
        pParam = NULL;
    }
    if (iType == ICAL_TYPE) {
        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_PARTSTAT_PARAMETER);
        if (pParam) {
        icalparameter_partstat icalPartStat;
        int iPartStat;

        icalPartStat = icalparameter_get_partstat(pParam);
        iPartStat = getParticipationStatusFromIcal(icalPartStat);
        pAtten->
            setParticipationStatus((ParticipantStatus) iPartStat);
        pParam = NULL;
        }
        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_CUTYPE_PARAMETER);
        if (pParam) {
        icalparameter_cutype icalCUType;
        int iCUType;

        icalCUType = icalparameter_get_cutype(pParam);
        iCUType = getCUTypeFromIcal(icalCUType);
        pAtten->setCalendarUserType((CalendarUserType) iCUType);
        pParam = NULL;
        }
        pParam =
        icalproperty_get_first_parameter(pProp, ICAL_CN_PARAMETER);
        if (pParam) {
        pAtten->
            setCommonName((char *) icalparameter_get_cn(pParam));
        pParam = NULL;
        }

        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_DIR_PARAMETER);
        if (pParam) {
        pAtten->setDirectoryParameter((char *)
                          icalparameter_get_dir
                          (pParam));
        pParam = NULL;
        }

        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_SENTBY_PARAMETER);
        if (pParam) {
        pAtten->
            setSentBy((char *) icalparameter_get_sentby(pParam));
        pParam = NULL;
        }

        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_LANGUAGE_PARAMETER);
        if (pParam) {
        pAtten->setLanguage((char *)
                    icalparameter_get_language(pParam));
        pParam = NULL;
        }
    }

    return pAtten;
    } else
    return NULL;
}




string ICalConverter::validateContents(string szContents)
{
    std::istringstream validatess(szContents);
    string szLine;
    string temp;
    string szRet;

    while (getline(validatess, szLine)) {
    unsigned int loc = szLine.find('\r', 0);

    if (loc != string::npos) {
        szLine = szLine.substr(0, (szLine.size() - 1));
    }

    if ((szLine.find("BEGIN:V", 0) != string::npos)
        || (szLine.find("END:V", 0) != string::npos)) {
        temp = "";
        for (unsigned int i = 0; i < szLine.length(); i++) {
        if (szLine[i] != ' ')
            temp += szLine[i];
        }
        szLine = temp;
    }
    szRet = szRet + NEWLINE + szLine;
    }
    return szRet;


}

bool ICalConverter::checkCount(string szContents, string szBegin,
                   string szEnd, unsigned int &iCount)
{
    std::istringstream validatess(szContents);
    string szLine;
    int ib = 0;
    int ie = 0;
    bool found = false;
    /* count of events / todos/ journals in ics file */
    iCount = 0;

    CAL_DEBUG_LOG("Input strings are %s , %s ", szBegin.c_str(),
          szEnd.c_str());

    while (getline(validatess, szLine)) {
    if (szLine.find(szBegin, 0) != string::npos){
        found = true;
        ib++;
    }
    if (szLine.find(szEnd, 0) != string::npos)
        ie++;
    }
    
    if (ib != ie) {
    CAL_DEBUG_LOG("if part returning false");
    return false;
    } else {
    CAL_DEBUG_LOG("else part ib is %d  \n", ib);
    iCount = ib;
    }

    return true;

}

string ICalConverter::strip(string szContents)
{
    string szRet;
    string szLine;
    
    
    std::istringstream validatess(szContents);
    /* getting line by line and adding \n
     * in the end*/
    
    
    while (getline(validatess, szLine)) {
    if ((szLine.compare(BEGIN_CALENDAR) != 0)
        && (szLine.compare(END_CALENDAR) != 0))
       szRet = szRet + NEWLINE + szLine;
    }
    /* deleting the white spaces from 
     * the import string */
    this->deleteWhiteSpaces(szRet);
       
    return szRet;

}

static bool 
stringReplaceAll (
        string        &toChange,
        const char   *changeThis,
        const char   *toThis)
{
    bool              retval = false;
    size_t            subStart;
    const string      orig (changeThis);

    while ((subStart = toChange.find (changeThis)) != string::npos) {
        retval = true;
        toChange.replace (subStart, orig.length(), toThis);
    }

    return retval;
}

static inline bool
stringIsRRule (
        const string &toCheck)
{
    return     toCheck.compare (0, 6, RRULE_COL) == 0 	|| 
        toCheck.compare (0, 7, EXRULE_COL) == 0 || 
        toCheck.compare (0, 6, RDATE_COL) == 0 	||
        toCheck.compare (0, 7, EXDATE_COL) == 0;
}

static void
stringAddEncodingQuotedPrintable (
        string      &theString)
{
    size_t   endOfName;
    string   namePart;

    endOfName = theString.find (":");
    if (endOfName == string::npos) {
        CAL_DEBUG_LOG ("The ':' not found.");
        return;
    }

    namePart = theString.substr (0, endOfName);

    if (namePart.find ("ENCODING=QUOTED-PRINTABLE") != string::npos) {
        CAL_DEBUG_LOG ("The encoding already set.");
        return;
    }

    theString.insert (endOfName, ";ENCODING=QUOTED-PRINTABLE");
}

/**
 * Function to parse the VCAL timezone field 
 */
void ICalConverter::fillvCalTimeZone(string &szContents )
{
        vCalTimeZone = szContents.substr(szContents.find(":") + 1);
        CAL_DEBUG_LOG("Timezone in VCalendar format is %s", vCalTimeZone.c_str());

}

/**
 * Function to convert attach  
 */
void ICalConverter::convertAttach(string &szAttach)
{
    /* In vcal format adding only the value */
    if (szAttach.find(":") != string::npos){
    szAttach= szAttach.substr(szAttach.find(":"));
    szAttach = "ATTACH" + szAttach;
    }
    
}

/**
 * icalVcalToLocal
 * @param strIcalComp: ical string to be converted
 * @param Filetype 
 * @param pErrorCode: Errorcode for the function
 * @return CComponent, CComponent object pointer
 *
 * This function takes ical string as parameter
 * converts the ical formatted string into local calendar database entry
 * returns the CComponent object
 */
vector <CComponent *>
ICalConverter::icalVcalToLocal(string szCont,
                              FileType iType,
                              int &pErrorCode)
{
    FileType               iFType;
    string                 szContents;
    CComponent            *pEntry = 0;
    unsigned int           iDummy;
    VCalConverter          pVCal;
    bool                   printableEncodingNeeded;
    vector <CComponent *>  vCompList;

    if (szCont.empty()) {
        pErrorCode = CALENDAR_INVALID_ARG_ERROR;
        return vCompList;
    }

    /*content validation */
    //szCont = validateContents(strIcalComp);
    CAL_DEBUG_LOG("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" );
    CAL_DEBUG_LOG("String Input to function :\n %s",szCont.substr(0,1024).c_str());
    CAL_DEBUG_LOG("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" );

    /*
     * Fix the broken iCalendar content provided by PCSuite
     */
    CAL_DEBUG_LOG("Fix QUOTED-PRINTABLE");
    stringReplaceAll (szCont, ";QUOTED-PRINTABLE", ";ENCODING=QUOTED-PRINTABLE");

    /*
     * Unfolding the continuation lines.
     */
    CAL_DEBUG_LOG("Cleanup \\r");
    stringRemoveAll (szCont, "\r");

    /* =\n represents a soft line break 
     * in quotes printable algo */
    CAL_DEBUG_LOG("Cleanup '\\n '");
    stringRemoveAll (szCont, "\n ");

    CAL_DEBUG_LOG("Cleanup '=\\n'");
    stringRemoveAll (szCont, "=\n");

    CAL_DEBUG_LOG("Cleanup \\n\\t");
    stringRemoveAll (szCont, "\n\t");

    CAL_DEBUG_LOG("Contents After Replcaing  =\n %s", szCont.substr(0,1024).c_str());

    CAL_DEBUG_LOG("Check events, jou and todos");

    if (/*!(checkCount(szCont, BEGIN_CALENDAR, END_CALENDAR, iDummy))
        	||*/ !(checkCount(szCont, BEGIN_EVENT, END_EVENT, iDummy))
        	|| !(checkCount(szCont, BEGIN_JOURNAL, END_JOURNAL, iDummy))
        	|| !(checkCount(szCont, BEGIN_TODO, END_TODO, iDummy))) {
        /*err code */
        CAL_DEBUG_LOG ("CALENDAR_INVALID_ICSFILE");
        pErrorCode = CALENDAR_INVALID_ICSFILE;
        return vCompList;
    }

    /* check type */
    iFType = pVCal.checkContentsType(szCont);
    CAL_DEBUG_LOG("File type is %d",iFType);
    /*stripping function */
    szContents = strip(szCont);

     
    std::istringstream iss(szContents);
    string szLine;
    string szEntry;
    string szDaylight;
    bool daylight_present = false;
    entryType entry_type = E_SPARE;
    while (getline(iss, szLine)) {
//         CAL_DEBUG_LOG("Line is: %s", szLine.c_str());
        /*
         * We first assume that this line does not need an encoding
         * property and if we change it we revoke this assumption.
         */
        printableEncodingNeeded = false;
        /*
         * If this is a start line we will know the type of the
         * entry.
         */
        if (szLine == BEGIN_EVENT) {
        	entry_type = E_EVENT;
        	CAL_DEBUG_LOG ("event started");
        } else if (szLine == BEGIN_TODO) {
        	entry_type = E_TODO;
        } else if (szLine == BEGIN_JOURNAL) {
        	entry_type = E_JOURNAL;
        } else if (szLine == BEGIN_TIMEZONE) {
            entry_type = E_VTIMEZONE;
            CAL_DEBUG_LOG ("timezone started");
        }
        /* fill the global timezone in vcal format */

        if ((iFType == VCAL_TYPE)  &&
        		(szLine.find("TZ:")!= string::npos))
        	fillvCalTimeZone(szLine);
        
        /*Make copy of line with daylight offset */ 
        
        if (szLine.find("DAYLIGHT:TRUE")!= string::npos){
        	daylight_present = true;
        	szDaylight = szLine;
        }

        /* convert VCAL Attach to ICal attach so that 
         * the parser can recognise the property*/
        
        if ((iFType == VCAL_TYPE)&& (szLine.find("ATTACH") != string::npos))
        	convertAttach(szLine);
        	
        
#if 0 
        /* Commented by Aditya:  = at the end of line specifies a soft 
         * line break hence it shouldn't be overwritten*/
        /*
         * For Each line replace the incorrect encoded pattern
         * with the correct one. Line feed carriage return should be 
         * '=0D=0A' coming from outlook but some other value is being
         * sent so replacing the same with the encoded value.
         */
        if (stringReplaceAll (szLine, "=0D=0A=", "=0D=0A "))
        	printableEncodingNeeded = true;
        /* , coming from outside have to be 
         * prepended with back slash 
         */
#endif 
        
        if (!stringIsRRule(szLine)) {
        	stringReplaceAll (szLine, "\\,", ",");
        	unsigned int startpos = 0 ;
        	while ((startpos = szLine.find("\\,", startpos))!= string ::npos )
        	{
        		szLine.replace(startpos,2 ,",");
        		startpos++;
        	}
        	
        	startpos = 0;
        	while ((startpos = szLine.find(",", startpos))!= string ::npos )
        	{
        		szLine.replace(startpos,1 ,"\\,");
        		startpos = startpos+ 2;
        	}
        	
        	
        }

        /*
         * If this string needs the 'quoted-printable' encoding.
         */
        if (printableEncodingNeeded) {
        	stringAddEncodingQuotedPrintable (szLine);
        }
        /*
         * Adding this new line to the entry.
         */
        if (entry_type != E_SPARE) {
        	if(szLine.empty())
        		szEntry = szLine;
        	else
        		szEntry += NEWLINE + szLine;

             /* If the properties like description are spread across multiple lines    
          * then we should parse them properly 
          *
          * This is clearly wrong because:
          *
          * 1) You can not decide that the line is a continuation line when the 
          *    line does not start with these strings. What if the line is an
          *    unrecognize property? This code will glue it with the previous
          *    line thus ruining it.
          *
          *    Of course calling the icalproperty_new_from_string() should 
          *    provide a good test but I saw events with perfectly normal 
          *    property lines that was unrecognized by icalproperty_new_from_string()
          *    and merged with the previous line.
          *
          * 2) If a continuation line has a ',' in it and the property is not
          *    'quoted printable' we change the ',' to  '=2C' and we have to
          *    add ';ENCODING=QUOTED-PRINTABLE' to the property name but we can
          *    not go back to the previous line to add it. That's why we have 
          *    to unfold (merge the continuation line with the previous line)
          *    before processing the given line.
          *
          * 3) It is clearly not efficient to call the icalproperty_new_from_string()
          *    function here.
          *
          * 4) When the continuation line is glued to the previous line no space
          *    character should be added.
          *
          * This is why I added the stringReplaceAll (szCont, "\n ", "");
          * to this function. This will properly join the continuation lines
          * with the previous line. According to the standards the continuation
          * lines must start with a white character.
          *
          * Please note that if this test fails and ruins the previous line it
          * can cause a crash in the ical library e.g. when the previous line
          * is a date string. (Check the icaltime_from_string() asserts and the
          * NB#99638).
          */    
        #if 0		
        	icalproperty* prop = 0;
        	if ((szLine.find("END:",0)==string::npos) &&
                    (szLine.find("BEGIN:",0)== string::npos))
        		prop = icalproperty_new_from_string(szLine.c_str());

        	if (prop ||  (szLine.find(A_ALARM,0) !=string::npos) ||
        			(szLine.find(EXDATE_COL,0)!= string::npos ) ||
        			(szLine.find(RRULE,0)!= string::npos ) ||
        			(szLine.find(RDATE_COL,0)!= string::npos ) ||
        			(szLine.find(EXRULE_COL,0)!= string::npos ) ||
        			(szLine.find (ATTACH,0)!= string::npos)  ||
        			(szLine.find("BEGIN:",0)!= string::npos) ||
        			(szLine.find("END:",0)!=string::npos))
        	{
        		szEntry = szEntry + "\n" + szLine;
        		if (prop)
        			icalproperty_free(prop);
        	} else {
        		szEntry = szEntry +" "+  szLine;
        	}

        #endif 
        }

        if (szLine == END_EVENT ||
            szLine == END_TODO ||
            szLine == END_JOURNAL) {
        	CAL_DEBUG_LOG ("Actual entry is:\n---8<---8<---\n%s\n---8<---8<---\n", szEntry.c_str());

        	switch (entry_type) {
        		case E_SPARE:
        			CAL_DEBUG_LOG ("END without BEGIN: giving up.");
            			pErrorCode = CALENDAR_INVALID_ICSFILE;
        			goto finalize;

        		case E_EVENT:
        			CAL_DEBUG_LOG ("Event found.");
        			break;

        		case E_TODO:
        			CAL_DEBUG_LOG ("Todo found.");
        			break;

        		case E_JOURNAL:
        			CAL_DEBUG_LOG ("Journal found.");
        			break;

        		default:
        			CAL_DEBUG_LOG ("Unhandled entry_type: %d", entry_type);
        			break;
        	}

        	/* Before adding the event in to local timezone
        	 * convert the GMT+OFFSET name to corresponding 
        	 * city name */
        	
        	if((iFType == VCAL_TYPE)  && ( !vCalTimeZone.empty())) 
        		findLocationFromOffset(szDaylight);

        	/* 
        	 * Creating a new entry and adding it to the list what
        	 * we return.
        	 */
        	pEntry = this->toLocal (szEntry, iFType, pErrorCode);
        	if (!pEntry) {
        		CAL_DEBUG_LOG ("Entry not recognized: error %d", pErrorCode);
        		goto finalize;
        	}
        			
        	vCompList.push_back(pEntry);
        	daylight_present = false;
        	vCalTimeZone.clear();
        	szDaylight.clear();
        	
        	/*
        	 * Resetting the this parser.
        	 */
        	entry_type = E_SPARE;
        	szEntry = "";
        } else if (szLine == END_TIMEZONE) {
            if (entry_type == E_VTIMEZONE) {
                vCalTimeZone = guessLocationFromVTimezone(szEntry);
            } else {
                CAL_ERROR_LOG ("VTimezone tags missmatch");
                pErrorCode = CALENDAR_INVALID_ICSFILE;
                goto finalize;
            }

            entry_type = E_SPARE;
            szEntry = "";
        }
    } // While reading all the lines.

    if (vCompList.size() == 0) {
            pErrorCode = CALENDAR_INVALID_ICSFILE;
        CAL_DEBUG_LOG ("WARNING: Found no events/todo/journal.");
    }

finalize:
    return vCompList;
}

CParameters *ICalConverter::getParameters(icalparameter * pParam)
{
    CParameters *pParameter = 0;

    if(icalparameter_get_xname(pParam)&&icalparameter_get_xvalue(pParam)){
        pParameter = new CParameters();
        ASSERTION(pParameter);
        pParameter->setParamName(icalparameter_get_xname(pParam));
        ParamType paramVal;
        paramVal.szString = icalparameter_get_xvalue(pParam);
        pParameter->setParamValue(paramVal);
    }
    return pParameter;

}

CProperties *ICalConverter::getXProperty(icalproperty * pProp)
{
    CProperties *pProperty = 0;
    PropType propVal;
    if (!ical_value_is_valid (pProp))        
        return pProperty;
    pProperty = new CProperties();
    ASSERTION(pProperty);
    
    pProperty->setPropName(icalproperty_get_x_name(pProp));
    /**
     *  this kind of check is needed 
     * because libical is not able to parse 
     * X_properties if it doesn't have X- for its parameters 
     */
    /* no strdup required as the  value is changed to string class 
     * and the szString will be destroyed once the 
     * automatic variable propVal goes out of scope */

    if (icalproperty_get_value(pProp) && icalproperty_get_x(pProp)){
        propVal.szString = (char *) icalproperty_get_x(pProp);
        pProperty->setPropValue(propVal);
    }
    else {

        delete pProperty;
        pProperty = 0;
    }

    return pProperty;
}

vector <
    CParameters *
    >ICalConverter::getRelationshipParamFromICal(icalparameter * pParam)
{
    CParameters *pParameter = 0;
    vector < CParameters * >vParamList;
    pParameter = new CParameters();
    ASSERTION(pParameter);
    icalparameter_reltype icalrel;
    ParamType value;
    icalrel = icalparameter_get_reltype(pParam);
    switch (icalrel) {
        case ICAL_RELTYPE_PARENT:
            value.i = PARENT;
            break;
        case ICAL_RELTYPE_CHILD:
            value.i = CHILD;
            break;
        case ICAL_RELTYPE_SIBLING:
            value.i = SIBLING;
            break;
        default:
            value.i = PARENT;
            break;
    }
    pParameter->setParamValue(value);
    pParameter->setParamName(RELTYPE);

    vParamList.push_back(pParameter);
    return vParamList;
}

/**
 * @param string Value of property
 * @param decoding type - 0 for quoted printable
 * Function to decode the text fields in ICS 
 */
char* ICalConverter::decodeParameterTextFields(string encodedString, 
        			   icalparameter_encoding  encodingType)
{

    size_t size = encodedString.length();
    /* decoded string will always be  greater than encoded one 
     * have to check the algo to see if double size is ok 
     * but it seems safe practise*/
    char* decodedString =  new char[(size*sizeof(char) * 2)];
    /* decoding algorithm 
     * depends on the type of encoding used here 
     * we support only 
     * Quoted-Printable 
     * Base64
     */
   memset(decodedString, 0, size*sizeof(char)*2); 
    if( encodingType != ICAL_ENCODING_BASE64)  
        decode_quoted_printable(decodedString, (char*)encodedString.c_str(), &size);
    else
           decode_base64(decodedString,(char*)encodedString.c_str(),&size);
    
        
    return decodedString;    
}



CParameters *ICalConverter::getLanguageFromIcal(icalparameter * pParam)
{
    CParameters *pParameter = 0;
    ParamType value;

    pParameter = new CParameters();
    ASSERTION(pParameter);
    pParameter->setParamName(LANGUAGE);
    value.szString = (char *) icalparameter_get_language(pParam);
    pParameter->setParamValue(value);

    return pParameter;
}

CParameters *ICalConverter::getFMTypeFromIcal(icalparameter * pParam)
{
    CParameters *pParameter = 0;
    ParamType value;

    pParameter = new CParameters();
    ASSERTION(pParameter);
    pParameter->setParamName(FMTYPE);
    value.szString = (char *) icalparameter_get_fmttype(pParam);
    pParameter->setParamValue(value);

    return pParameter;
}

CParameters *ICalConverter::getAlterFromIcal(icalparameter * pParam)
{
    CParameters *pParameter = 0;
    ParamType value;

    pParameter = new CParameters();
    ASSERTION(pParameter);
    pParameter->setParamName(ALTER);
    value.szString = (char *) icalparameter_get_altrep(pParam);
    pParameter->setParamValue(value);

    return pParameter;
}

vector <
    CParameters * >ICalConverter::getLangAlterParam(icalproperty * pProp)
{
    icalparameter *pParam = 0;
    vector < CParameters * > vLangAlter;
    vLangAlter.clear();
    pParam =
    icalproperty_get_first_parameter(pProp, ICAL_LANGUAGE_PARAMETER);
    if (pParam) {
    CParameters *pLang = 0;
    pLang = getLanguageFromIcal(pParam);
    if (pLang)
        vLangAlter.push_back(pLang);
    }
    pParam =
    icalproperty_get_first_parameter(pProp, ICAL_ALTREP_PARAMETER);
    if (pParam) {
    CParameters *pAlter = 0;
    pAlter = getAlterFromIcal(pParam);
    if (pAlter)
        vLangAlter.push_back(pAlter);
    }

    return vLangAlter;
}

/**
 * Private function to limit the Date Range 
 * for event coming from sync 
 */
void ICalConverter::limitDateRange(struct icaltimetype &s_time,bool isExport)
{
	struct tm local_tm;
	memset (&local_tm, 0, sizeof (struct tm));
	time_get_local(&local_tm);

	if (!isExport){
		/* If event time is > 2037 such event is added on current year 
		 * if event time is < 1970 it is approximated to 1970 
		 * this is inline with what we do for birthdays 
		 * Note: because of timezone changing 20370101 and 19691231 
		 * are also valid dates (bug#132166) 
		 */   
		if ( s_time.year > 2037 && (s_time.year != 2038 || s_time.month != 1 || s_time.day != 1))
			s_time.year = local_tm.tm_year+ 1900;
		else if (s_time.year < 1970 && (s_time.year != 1969 || s_time.month != 12 || s_time.day != 31)) 
			s_time.year = 1970;	    
	}
	else
	{
		/* In export scenario donot export dates beyond 
		 * 2037 december 31 as this is the max supported date 
		 * on device.
		 * Note: because of timezone changing 20370101 and 19691231 
		 * are also valid dates (bug#132166) 
		 */


		if ( s_time.year > 2037 && (s_time.year != 2038 || s_time.month != 1 || s_time.day != 1)){
			s_time.year = 2037;
			s_time.month = 12;
			s_time.day = 31;
			/* keep the time fields same */
		}
		else if (s_time.year < 1970 && (s_time.year != 1969 || s_time.month != 12 || s_time.day != 31)) 
			s_time.year = 1970;	    

	}

}

/**
 * importEventDateStartAndDateEnd:
 * @timeDiff: The place where we put the start_time_zone/local_time_zone diff.
 * returns: True if the date start is at 00:00:00 (midnight).
 *
 * Reads and sets the start time for the CEvent object from an icalcomponent.
 * The timeDiff is an in/out parameter for the event/local time zone 
 * difference.
 * 
 * Returns true if the start time modified by the time zone difference is at
 * midnight (so it can be considered as a part of an allday event).
 */
bool 
ICalConverter::importEventDateStartAndDateEnd (
        		icalcomponent    *pComp, 
        		CEvent           *pEvent, 
        		FileType 	iType,
        		int& 		pErrorCode)
{
    time_t                  datestart = 0;
    time_t                  end_time = 0;
    icalproperty           *pPropDateStart = 0;
    icalproperty           *pPropDateEnd = 0;
    struct icaltimetype     s_time;
    struct icaltimetype     e_time;
    bool                    isAllday = false;
    string 			startTimeZone;
    /* start timezone is initalized to local timezone */
    CMulticalendar *mc = CMulticalendar::MCInstance();
    string currentTimeZone = mc ->getSystemTimeZone();
    startTimeZone = currentTimeZone;
    parseTimeZone(startTimeZone);

    memset (&s_time, 0, sizeof (struct icaltimetype));
    memset (&e_time, 0, sizeof (struct icaltimetype));
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

	/* 
	 * importing datestart and date end 
	 */
	pPropDateStart = icalcomponent_get_first_property(pComp, ICAL_DTSTART_PROPERTY);
	pPropDateEnd = icalcomponent_get_first_property(pComp, ICAL_DTEND_PROPERTY);
	if(!pPropDateStart || !pPropDateEnd ||
	 	!(ical_value_is_valid (pPropDateStart)) ||
	                         !(ical_value_is_valid(pPropDateEnd))){
			pErrorCode = CALENDAR_ICS_COMPONENT_INVALID;
			CAL_DEBUG_LOG("CALENDAR_ICS_COMPONENT_INVALID");
			return false;
	}
	
	if (!icalproperty_kind_is_valid(ICAL_DTSTART_PROPERTY)||
		!icalproperty_kind_is_valid(ICAL_DTEND_PROPERTY))
	    return false;
    icalparameter *pParam = 0;
    /* TZID will be specified only 
     * in ICAL format in VCALENDAR
     * format we have to use global timezone from 
     * vCalTimeZone  
     */ 

    pParam = icalproperty_get_first_parameter (
    				pPropDateStart,
    				ICAL_TZID_PARAMETER);
    if ((pParam) &&
    				(iType == ICAL_TYPE))	{
    		const char *tzid = icalparameter_get_tzid(pParam);
    		string szIcsTimeZone = tzid;
    		unsigned int i = szIcsTimeZone.find("-", 0);
    		if (i != string::npos) {
    				szIcsTimeZone.replace(i, 1, "/");
    				CAL_DEBUG_LOG("ICAL Time zone is %s  " ,szIcsTimeZone.c_str());
    		}

    		/* check if any valid timezone exist with this name 
    		 * if so overwrite the default timezone*/

    		if (icaltimezone_get_builtin_timezone(szIcsTimeZone.c_str()))
            {
                CAL_DEBUG_LOG("Found build-in TZ info for '%s'", szIcsTimeZone.c_str());
    				startTimeZone = szIcsTimeZone;
            }
            else if (!vCalTimeZone.empty())
            {
                startTimeZone = vCalTimeZone;
                CAL_DEBUG_LOG("Use guessed timezone '%s'", startTimeZone.c_str());
            }


    }
    else if (iType == VCAL_TYPE )
    {
    		/* use the global vCalTimeZone to calculate the timediff */
    		/* first we need to convert zone to :GMT+- format 
    		*/
    		if (!vCalTimeZone.empty())
    				startTimeZone = vCalTimeZone;
    		CAL_DEBUG_LOG("VCal time zone is %s", startTimeZone.c_str());

    }
    else{
    		CAL_DEBUG_LOG("TZID not specified in ICAL event format  "
    						"So we use the local timezone to import the event ");
    }

    s_time = icalproperty_get_dtstart(pPropDateStart);
    e_time = icalproperty_get_dtend(pPropDateEnd);

    /* check if any one of these is in UTC if so 
     * convert them to Components timezone */
    CAL_DEBUG_LOG("is start time UTC : %d",s_time.is_utc  );
    CAL_DEBUG_LOG("is end time UTC : %d",e_time.is_utc  );
    CAL_DEBUG_LOG("start timezone: %p",s_time.zone  );
    CAL_DEBUG_LOG("end   timezone: %p",e_time.zone  );

    /* check if the date range is exceeded 
     * event before 1970 is adjusted to 1970 
     * and event after 2037 is added on the current
     * year
     * This is consistent with what we follow for Bdays 
     */
    limitDateRange(s_time,false);
    limitDateRange(e_time,false);

    if (s_time.is_utc)
    		s_time = icaltime_convert_to_zone(s_time,
    				icaltimezone_get_builtin_timezone(startTimeZone.c_str()));

    if (e_time.is_utc)
    		e_time = icaltime_convert_to_zone(e_time,
    				icaltimezone_get_builtin_timezone(startTimeZone.c_str()));

    /* now both start time and end time are in components timezone */

    /*
     * following 4 formats are supported which indicate that the event is 
     * allday
     *
     *  1. DTSTART:20081226T000000   DTEND:20081227T000000
     *  2. DTSTART:20081226T000000   DTEND:20081226T000000
     *  3. DTSTART:20081226T000000   DTEND:20081226T235959 
     *  4. DTSTART:20081226T235900   DTEND:20081226T235900 
     *
     *  
     *  Export is always made in the following format 
     *  DTSTART:20081226T000000   DTEND:20081227T000000
     *
     * */
    	if (((s_time.hour == 23)         && (s_time.minute == 59) && (s_time.second == 0)) &&
    		    ((e_time.hour == 23)   && (e_time.minute == 59) && (e_time.second == 0)))
    	{ 
    	    /* phone sends anniversary allday event in this 
    	     * format so we need to consider it as allday event
    	     */
    	    isAllday = true;
    	    CAL_DEBUG_LOG("Modified The start and end time to Allday event ");
    	    s_time.hour = s_time.minute =  0;
    	    e_time.hour = e_time.minute =  0;
    	}
    	CAL_DEBUG_LOG("*********Date start from ICS *********** ");
    	CAL_DEBUG_LOG(" Year is:    %d ", s_time.year);
    	CAL_DEBUG_LOG(" Month is:   %d ", s_time.month);
    	CAL_DEBUG_LOG(" Day is:     %d ", s_time.day);
    	CAL_DEBUG_LOG(" Hour is:    %d ", s_time.hour);
    	CAL_DEBUG_LOG(" Minute  is: %d ", s_time.minute);
    	CAL_DEBUG_LOG(" Second is : %d ", s_time.second);
    	CAL_DEBUG_LOG("*********Date end from ICS *********** ");
    	CAL_DEBUG_LOG(" Year is:    %d ", e_time.year);
    	CAL_DEBUG_LOG(" Month is:   %d ", e_time.month);
    	CAL_DEBUG_LOG(" Day is:     %d ", e_time.day);
    	CAL_DEBUG_LOG(" Hour is:    %d ", e_time.hour);
    	CAL_DEBUG_LOG(" Minute  is: %d ", e_time.minute);
    	CAL_DEBUG_LOG(" Second is : %d ", e_time.second);
    	CAL_DEBUG_LOG("**************************************** ");
    	/* simple check to see if it is allday 
    	*/
    	if ((s_time.hour == 0) 	&& (s_time.minute == 0) && (s_time.second == 0)) {
    		isAllday = true;
    		CAL_DEBUG_LOG("Event DateStart seems like Allday ");
    }

    /* anniversary events from s60 phones are sent in a wierd way 
     * so handling support for those too */
    if (((e_time.hour == 0)  && (e_time.minute == 0) && (e_time.second == 0) &&  isAllday) ||
    	((e_time.hour == 23)  && (e_time.minute == 59) && (e_time.second == 59) &&  isAllday) )
    		isAllday = true;
    else
    		isAllday = false;


    /* set date-start parmeter  from icaltime type */
    datestart = icaltime_as_timet_with_zone(s_time,
    				icaltimezone_get_builtin_timezone(startTimeZone.c_str()));
    pEvent->setDateStart(datestart);

    
    /* set date-end parameter from icaltime type  */
    end_time = icaltime_as_timet_with_zone(e_time,
    				icaltimezone_get_builtin_timezone(startTimeZone.c_str()));

    if (( (end_time - pEvent->getDateStart())% 86400 == 0)
    				&& ( isAllday == true) 
    				&& (end_time - pEvent->getDateStart() != 0 ))
    		end_time = end_time -1;
		    
    pEvent->setDateEnd(end_time);

    pPropDateStart = NULL;
    pPropDateEnd = NULL;
    
    /* set the timezone parameter  and allday flag */
    
    startTimeZone = ":" + startTimeZone;
    pEvent->setTzid(startTimeZone);
    pEvent->setAllDay(isAllday);

    return true;
}

/**
 * importEventDateEnd:
 *
 * Reads and sets the end time for the CEvent object from an icalcomponent.
 * The timeDiff is the time zone difference between the component and the local
 * clock.
 */
bool 
ICalConverter::importEventDateEnd (
        icalcomponent  *pComp, 
        CEvent         *pEvent, 
        FileType 	iType)
{
    time_t                  end_time = 0;
    bool                   isAllday = false;
    icalproperty          *pProp = 0;
    struct icaltimetype    e_time;
    string     	   endTimeZone;
    string     	   currentTimeZone;


    /* if no zone information is specified then 
     * import the event as if it were in local time zone 
     */
    CMulticalendar *mc = CMulticalendar::MCInstance();
    currentTimeZone = mc ->getSystemTimeZone();
    endTimeZone = currentTimeZone;
    parseTimeZone(endTimeZone);

    memset (&e_time, 0, sizeof (struct icaltimetype));
    /*
     * importing dtend 
     */
    pProp = icalcomponent_get_first_property(pComp, ICAL_DTEND_PROPERTY);

    if (pProp) {
        e_time = icalproperty_get_dtend(pProp);

        /* simple check to see if it is allday 
        */
        CAL_DEBUG_LOG("*********Date End from ICS *********** ");
        CAL_DEBUG_LOG(" Year is:    %d ", e_time.year);
        CAL_DEBUG_LOG(" Month is:   %d ", e_time.month);
        CAL_DEBUG_LOG(" Day is:     %d ", e_time.day);
        CAL_DEBUG_LOG(" Hour is:    %d ", e_time.hour);
        CAL_DEBUG_LOG(" Minute  is: %d ", e_time.minute);
        CAL_DEBUG_LOG(" Second is : %d ", e_time.second);
        CAL_DEBUG_LOG("**************************************** ");

        /* some Ical formats have date start, same as date end  
         * in such cases we will change the date end to format supported 
         * by Calendar application
         *
         * following 3 formats are supported which indicate that the event is 
         * allday
         *
         *  1. DTSTART:20081226T000000   DTEND:20081227T000000
         *  2. DTSTART:20081226T000000   DTEND:20081226T000000
         *  3. DTSTART:20081226T000000   DTEND:20081226T235959 
         *
         *  
         *  Export is always made in the following format 
         *  DTSTART:20081226T000000   DTEND:20081227T000000
         *
         * */
        if (((e_time.hour ==  0) && (e_time.minute ==  0) && (e_time.second ==  0) && (pEvent->getAllDay())) ||
       	    ((e_time.hour == 23) && (e_time.minute == 59) && (e_time.second ==  0) && (pEvent->getAllDay())) ||
       	    ((e_time.hour == 23) && (e_time.minute == 59) && (e_time.second == 59) && (pEvent->getAllDay())))

        {
        	CAL_DEBUG_LOG("Event is Allday event ");
        	isAllday = true;

        }
        icalparameter *pParam = 0;
        /* TZID will be specified only 
         * in ICAL format in VCALENDAR
         * format we have to use global timezone from 
         * vCalTimeZone  
         */ 

        pParam = icalproperty_get_first_parameter (
        		pProp, 
        		ICAL_TZID_PARAMETER);
        if ((pParam) &&
        		(iType == ICAL_TYPE))	{
        	const char *tzid = icalparameter_get_tzid(pParam);
        	endTimeZone = tzid;
        	CAL_DEBUG_LOG("date end is %s \n", ctime(&end_time));

        	unsigned int i = endTimeZone.find("-", 0);
        	if (i != string::npos) {
        		endTimeZone.replace(i, 1, "/");
        		CAL_DEBUG_LOG("ICAL Time zone is %s  " ,endTimeZone.c_str());
        	}

        }
        else if (iType == VCAL_TYPE )
        {
        	/* use the global vCalTimeZone to calculate the timediff *
        	*/
        	if (!vCalTimeZone.empty())
        		endTimeZone = vCalTimeZone;
        	CAL_DEBUG_LOG("VCal time zone is %s", endTimeZone.c_str());

        }

        else{
        	CAL_DEBUG_LOG("TZID not specified in ICAL event format  ");
        }
        end_time = icaltime_as_timet_with_zone(e_time,
        		icaltimezone_get_builtin_timezone(endTimeZone.c_str()));

        if (( end_time - pEvent->getDateStart() == 86400) && ( pEvent->getAllDay() == true))
        	end_time = end_time -1;
        pEvent->setDateEnd(end_time);
        pProp = NULL;
    }

    /* outlook sends allday  in format 1 of above so from date end we need to 
     * subtract 1 second */
    CAL_DEBUG_LOG ("date end is '%s'", ctime(&end_time));
    return isAllday;
}


void ICalConverter::importEventXProp(icalcomponent *pComp, CEvent *pEvent)
{
#if 0
    icalproperty *pProp = 0;
    icalparameter *pParam = 0;
    /*importing x-prop */
    pProp = icalcomponent_get_first_property(pComp, ICAL_X_PROPERTY);
    if (pProp) {
        CProperties *pPro = 0;
        vector <CProperties *> pProList;
        while (pProp != NULL) {
        pPro = getXProperty(pProp);
    if (pPro) {

    	PropType propVal;
    	propVal = pPro ->getPropValue();

    	pProList.push_back(pPro);
    	pParam =
    		icalproperty_get_first_parameter(pProp,
    				ICAL_ANY_PARAMETER);
    	if (pParam) {
    		CParameters *pPara = 0;
    		vector < CParameters * > vParamList;

    		while (pParam) {
    			pPara = getParameters(pParam);
    			if (pPara)
    				vParamList.push_back(pPara);
    			pParam =
    				icalproperty_get_next_parameter(pProp,
    						ICAL_ANY_PARAMETER);
    		}  
    		hashMap.insert(pair < string,
    				vector < CParameters * > >
    				(icalproperty_get_x_name(pProp),
    				 vParamList));
    	}
    }
            

        pProp =
            icalcomponent_get_next_property(pComp,
                    ICAL_X_PROPERTY);
        }
        pEvent->setXProperties(pProList);
    }
#endif
}
template<class T>
struct icaltimetype ICalConverter::importCreatedTime(icalcomponent *pComp, T *tComp, FileType iType, string strIcalComp)
{
    icalproperty *pProp = 0;
    time_t time1;
    struct icaltimetype c_time;
    memset (&c_time, 0, sizeof (struct icaltimetype));
    string szZone = tComp->getTzid();
    parseTimeZone(szZone);

    /*importing created time */
    if (iType == VCAL_TYPE) {
    VCalConverter *pVCal = new VCalConverter();
        c_time = pVCal->getCreatedTimeFromVcal(strIcalComp);
    delete pVCal;
    } else {
        pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_CREATED_PROPERTY);
        if (pProp) {
        c_time = icalproperty_get_created(pProp);
        }
    }
    if(c_time.is_utc) 
        szZone = "UTC";
    time1 = icaltime_as_timet_with_zone(c_time, 
            icaltimezone_get_builtin_timezone(szZone.c_str()));
    tComp->setCreatedTime(time1);
    pProp = NULL;
    return c_time;
}

template<class T>
void ICalConverter::importDateStamp(icalcomponent *pComp, T *tComp, struct icaltimetype c_time)
{
    icalproperty *pProp = 0;
    time_t time1;
    struct icaltimetype dtstamp_time;
    string szZone = tComp->getTzid();
    parseTimeZone(szZone);
    memset (&dtstamp_time, 0, sizeof (struct icaltimetype));
    /*importing date stamp */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_DTSTAMP_PROPERTY);
    if (pProp) {
        dtstamp_time = icalproperty_get_dtstamp(pProp);
    if(dtstamp_time.is_utc)
        szZone = "UTC";
        time1 = icaltime_as_timet_with_zone(c_time, 
            icaltimezone_get_builtin_timezone(szZone.c_str()));
        tComp->setDateStamp(time1);
        pProp = NULL;
    }
}

template<class T>
void ICalConverter::importOrganizer(icalcomponent *pComp, T *tComp)
{
    icalproperty *pProp = 0;
    /*importing organizer */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_ORGANIZER_PROPERTY);
    if (pProp) {
        COrganizer *pOrg = 0;
        pOrg = getOrganizerFromIcal(pProp);
        if (pOrg){
            tComp->setOrganizer(pOrg);
            delete pOrg;
            pOrg = 0;
        }
        pProp = NULL;
    }
}

template<class T>
void ICalConverter::importAttendees(icalcomponent *pComp, T *tComp, FileType iType)
{
    icalproperty *pProp = 0;
    /*importing Attendees */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_ATTENDEE_PROPERTY);
    if (pProp) {

        vector < CAttendee * >vAttenList;
        vAttenList.clear();
        CAttendee *pAtten = 0;
        while (pProp) {

        pAtten = getAttendeeFromIcal(pProp, iType);
        if (pAtten)
            vAttenList.push_back(pAtten);

        pProp = NULL;
        pProp =
            icalcomponent_get_next_property(pComp,
                            ICAL_ATTENDEE_PROPERTY);
        }
        if (vAttenList.size() != 0) {
            tComp->setAttendees(vAttenList);

            vector < CAttendee * >::iterator iter;
            for ( iter = vAttenList.begin(); iter != vAttenList.end(); iter++) {
                delete(*iter);
                (*iter) = 0;
            }
            vAttenList.clear();
        }
    }
}

void ICalConverter::importEventTransp(icalcomponent *pComp, CEvent *pEvent, FileType iType, string strIcalComp)
{
    icalproperty *pProp = 0;
    /*importing transparency */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_TRANSP_PROPERTY);
    if (pProp) {
        icalproperty_transp transp;
        string szTransp;
        if (iType == VCAL_TYPE) {
    VCalConverter *pVCal = new VCalConverter();
        szTransp = pVCal->getTranspFromVcal(strIcalComp);
    delete pVCal;
    }
        else {
        transp = icalproperty_get_transp(pProp);
        szTransp = getTranspFromIcal(transp);
        }
        pEvent->setTransparency(szTransp);
        pProp = NULL;
    }
}

template<class T>
vector < CParameters * > ICalConverter::importCategories(icalcomponent *pComp, T *tComp)
{
    icalproperty *pProp = 0;
    icalparameter *pParam = 0;
        vector < CParameters * >vCatParamList;
    /*importing categories */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_CATEGORIES_PROPERTY);
    if (pProp) {
        tComp->
        setCategories((char *) icalproperty_get_categories(pProp));
        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_LANGUAGE_PARAMETER);
        if (pParam) {
        CParameters *pLang = 0;
        pLang = getLanguageFromIcal(pParam);
        if (pLang)
            vCatParamList.push_back(pLang);
        }
        pProp = NULL;
    }
    return vCatParamList;
}

template<class T>
void ICalConverter::importSequence(icalcomponent *pComp, T *tComp)
{
    icalproperty *pProp = 0;
    /*importing sequence */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_SEQUENCE_PROPERTY);
    if (pProp) {
        tComp->setSequence(icalproperty_get_sequence(pProp));
        pProp = NULL;
    }
}

template<class T>
void ICalConverter::importPriority(icalcomponent *pComp, T *tComp)
{
    icalproperty *pProp = 0;
    /*importing priority */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_PRIORITY_PROPERTY);
    if (pProp) {
        tComp->setPriority(icalproperty_get_priority(pProp));
        pProp = NULL;
    }
}

template<class T>
vector <CParameters *> ICalConverter::importResources(icalcomponent *pComp, T *tComp)
{
    vector < CParameters * >vResParamList;
    icalproperty *pProp = 0;
    /*importing Resources */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_RESOURCES_PROPERTY);
    
    icalvalue *value = icalproperty_get_value (pProp);
    
    if (pProp && (icalvalue_is_valid (value))) {
        tComp->
        setResources((char *) icalproperty_get_resources(pProp));
        vResParamList = getLangAlterParam(pProp);
        pProp = NULL;
    }
    return vResParamList;
}

template<class T>
void ICalConverter::importUrl(icalcomponent *pComp, T *tComp)
{
    icalproperty *pProp = 0;
    /*importing URL */
    bool flag = ical_value_is_valid(pProp);
    pProp = icalcomponent_get_first_property(pComp, ICAL_URL_PROPERTY);
    if (pProp && flag) {
        tComp->setUrl((char *) icalproperty_get_url(pProp));
        pProp = NULL;
    }
}

void ICalConverter::importEventAttachements(icalcomponent *pComp, CEvent *pEvent)
{
#if 0
    icalproperty *pProp = 0;
    icalparameter *pParam = 0;
    /*importing Attach */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_ATTACH_PROPERTY);
    if (pProp) {
        vector < string > vAttachList;
        while (pProp) {

        icalattach *icalAtt = icalproperty_get_attach(pProp);
        string szAttach = icalattach_get_url(icalAtt);
        vector < CParameters * >vAttParamList;
        pParam =
            icalproperty_get_first_parameter(pProp,
                             ICAL_FMTTYPE_PARAMETER);
        if (pParam) {
            CParameters *pFm = 0;
            pFm = getFMTypeFromIcal(pParam);
            if (pFm)
            vAttParamList.push_back(pFm);
            if (vAttParamList.size() != 0)
            hashMap.insert(pair < string,
                       vector < CParameters * > >(ATTACH,
                                 vAttParamList));
        }
        if (!szAttach.empty())
            vAttachList.push_back(szAttach);
        pProp =
            icalcomponent_get_next_property(pComp,
                            ICAL_ATTACH_PROPERTY);
        }
        if (vAttachList.size() != 0)
        pEvent->setAttachments(vAttachList);
    }
#endif
}

template<class T>
void ICalConverter::importGeo(icalcomponent *pComp, T *tComp)
{
    icalproperty *pProp = 0;
    /*importing geo */
    pProp = icalcomponent_get_first_property(pComp, ICAL_GEO_PROPERTY);
    icalvalue* pVal = icalproperty_get_value(pProp);
    if(ICAL_GEO_VALUE != icalvalue_isa(pVal)){
	    return;
    }
    if (pProp) {
        string szGeo;
        string szLat;
        string szLon;
        icalgeotype geoStruct;

        geoStruct = icalproperty_get_geo(pProp);
        szLat = floatToString(geoStruct.lat);
        szLon = floatToString(geoStruct.lon);
        szGeo = szLat + ";" + szLon;
        tComp->setGeo((char *) szGeo.c_str());
        pProp = NULL;
    }
}

template<class T>
vector <CParameters *> ICalConverter::importContacts(icalcomponent *pComp, T *tComp)
{
    vector < CParameters * >vConParamList;
    icalproperty *pProp = 0;
    /*importing contacts */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_CONTACT_PROPERTY);
    if (pProp) {

        tComp->setContact(icalproperty_get_contact(pProp));

        vConParamList = getLangAlterParam(pProp);
        pProp = NULL;
    }
    return vConParamList;
}

template<class T>
vector<CParameters *> ICalConverter::importRelated(icalcomponent *pComp, T *tComp)
{
        vector < CParameters * >vRelParamList;
    icalproperty *pProp = 0;
    icalparameter *pParam = 0;
    /*importing related-to */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_RELATEDTO_PROPERTY);
    if (pProp && ical_value_is_valid (pProp)&& icalproperty_get_relatedto(pProp)) {
        tComp->setRelated((char *) icalproperty_get_relatedto(pProp));
        pParam =
        icalproperty_get_first_parameter(pProp,
                         ICAL_RELTYPE_PARAMETER);
        if (pParam) {
        vRelParamList = getRelationshipParamFromICal(pParam);
    }
        pProp = NULL;
    }
    return vRelParamList;
}

template<class T>
void ICalConverter::importClass(icalcomponent *pComp, T *tComp)
{
    icalproperty *pProp = 0;
//   bool flag =true;
    /*importing classification-class */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_CLASS_PROPERTY);
# if 0
    icalparameter* pParam = 0;
    pParam = icalproperty_get_first_parameter(pProp,ICAL_VALUE_PARAMETER);
    if (!icalproperty_kind_is_valid(ICAL_CLASS_PROPERTY)|| !ical_value_is_valid(pProp))
	    flag = false;
#endif 

    if (ical_value_is_valid(pProp) ) {
        icalproperty_class icalClass;
        string szClass;

        icalClass = icalproperty_get_class(pProp);
        szClass = getClasFromIcal(icalClass);
        CAL_DEBUG_LOG("class is %s\n", szClass.c_str());
        tComp->setClas(szClass);
    }
}

template<class T>
vector < CParameters * > ICalConverter::importLocation(icalcomponent *pComp, T *tComp)
{
    icalproperty *pProp = 0;
    icalparameter_encoding eValue = ICAL_ENCODING_NONE;
    vector < CParameters * >vLocParamList;
    /*importing location */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_LOCATION_PROPERTY);
    if (pProp) {
    
    /* decode the text filelds so that 
     * we might get the correct value 
     * there might be other encodings 
     */
        
        icalparameter *ep = icalproperty_get_first_parameter(pProp,
        	    ICAL_ENCODING_PARAMETER);
        eValue = ICAL_ENCODING_NONE;
        
        string szLocation = icalproperty_get_location(pProp);
        char * decodedLocation = 0;
        /* only base 64 and quoted printable algos are
         * supported */
        if (ep){
              eValue = icalparameter_get_encoding(ep);
              decodedLocation =  decodeParameterTextFields(szLocation,eValue);
              tComp->setLocation(decodedLocation);
              delete[] decodedLocation;
        }
        else 
            tComp->setLocation(szLocation);
        
    
        vLocParamList = getLangAlterParam(pProp);
        pProp = NULL;
    }
    return vLocParamList;
}

template<class T>
vector < CParameters * > ICalConverter::importDescription(icalcomponent *pComp, T *tComp)
{
    vector < CParameters * >vDesParamList;
    icalproperty *pProp = 0;
    icalparameter_encoding eValue = ICAL_ENCODING_NONE;
    /* importing description */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_DESCRIPTION_PROPERTY);
    if (pProp && ical_value_is_valid (pProp)) {
        /* decode the text filelds so that 
         * we might get the correct value */
        string encodedDescription = icalproperty_get_description(pProp);
        char * decodedDescription = 0;
        icalparameter *ep = icalproperty_get_first_parameter(pProp,
        		ICAL_ENCODING_PARAMETER);
                eValue = ICAL_ENCODING_NONE;
        if (ep){
        	eValue = icalparameter_get_encoding(ep);
        	decodedDescription =  decodeParameterTextFields(encodedDescription,eValue);
        	tComp->setDescription( decodedDescription);
        	delete[] decodedDescription;
        }
        else
        	tComp->setDescription( encodedDescription);
        vDesParamList = getLangAlterParam(pProp);
        pProp = NULL;
    }
    return vDesParamList;
}

template<class T>
vector < CParameters * > ICalConverter::importSummary(icalcomponent *pComp, T *tComp)
{
    vector < CParameters * >vSumParamList;
    icalproperty *pProp = 0;
    icalparameter_encoding eValue = ICAL_ENCODING_NONE;
    /*importing summary */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_SUMMARY_PROPERTY);
    if (pProp && icalproperty_get_summary(pProp)) {
        /* decode the text filelds so that 
         * we might get the correct value */
    string encodedSummary = icalproperty_get_summary(pProp);
    /* find if there is any encoding applied to 
     * file currently we support only 
     * QUOTED_PRINTABLE_ENCODING and base64
     */
    char* decodedSummary = 0;        
    icalparameter *ep = icalproperty_get_first_parameter(pProp,
        	ICAL_ENCODING_PARAMETER);
    eValue = ICAL_ENCODING_NONE;
    
    
    /* only these two encoding formats 
     * are supported */
    if (ep){
        eValue = icalparameter_get_encoding(ep);
        decodedSummary =  decodeParameterTextFields(encodedSummary,eValue);
        tComp->setSummary((char *)decodedSummary);
        delete[] decodedSummary;
    }
    else 
        tComp->setSummary(encodedSummary);
        vSumParamList = getLangAlterParam(pProp);
    pProp = NULL;
    }
    return vSumParamList;
}

template<class T>
vector < CParameters * > ICalConverter::importComments(icalcomponent *pComp, T *tComp)
{
    vector < CParameters * >vCommParamList;
    icalproperty *pProp = 0;
    /*importing comment */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_COMMENT_PROPERTY);
    /*
     * Fixes 39 instances of error @ 4610 in 100000-104999
     */
    if (pProp && icalproperty_get_comment(pProp)) {
        tComp->setComments(icalproperty_get_comment(pProp));
        vCommParamList = getLangAlterParam(pProp);
        pProp = NULL;
    }
    return vCommParamList;
}

void ICalConverter::importTodoDue(icalcomponent *pComp, CTodo *pTodo)
{
    icalproperty *pProp = 0;
    time_t due_timet = 0;
    struct icaltimetype d_time;
    string szZone = pTodo->getTzid();
    /**Fixes Security related crash
     */
    if(szZone.c_str())
    parseTimeZone(szZone);
    memset (&d_time, 0, sizeof (struct icaltimetype));
    /* task is like allday event so importing task as 
     * an allday event? */
    
    pProp = icalcomponent_get_first_property(pComp, ICAL_DUE_PROPERTY);
    icalparameter* pParam =0;
    pParam = icalproperty_get_first_parameter(pProp, ICAL_VALUE_PARAMETER);
    
    /*importing due*/
    if (ical_value_is_valid(pProp)) {
	    if ((pParam && icalparameter_isa(pParam)!= ICAL_NO_PARAMETER) ||(pParam == 0))
	    {
		    d_time = icalproperty_get_due(pProp);
		    due_timet = icaltime_as_timet_with_zone(d_time, 
				    d_time.zone);
	    }
    	/* We should use setDue but calendar uses setDateStart */
    	pTodo->setDateStart(due_timet);
    	CAL_DEBUG_LOG("Todo Due date is %s and time is %d", ctime(&due_timet),(int)due_timet);
    }
    else
    		CAL_DEBUG_LOG("Going to else condition Means invalid DUE  ");
    pProp = 0;
}

void ICalConverter::importTodoPercentCompleted(icalcomponent *pComp, CTodo *pTodo)
{
    icalproperty *pProp = 0;
    /*importing percent complete */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_PERCENTCOMPLETE_PROPERTY);
    if (pProp) {
        pTodo->
        setPercentComplete(icalproperty_get_percentcomplete
                   (pProp));
    }
    pProp = NULL;
}
void ICalConverter::importTodoCompleted(icalcomponent *pComp, CTodo *pTodo)
{
    time_t time1;
    icalproperty *pProp = 0;
    struct icaltimetype completed_time;
    string szZone = pTodo->getTzid();
    parseTimeZone(szZone);
    memset (&completed_time, 0, sizeof (struct icaltimetype));
    /*importing completed */
    pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_COMPLETED_PROPERTY);
    if (pProp) {
        completed_time = icalproperty_get_completed(pProp);
        time1 = icaltime_as_timet_with_zone(completed_time, 
        	icaltimezone_get_builtin_timezone(szZone.c_str()));
        pTodo->setCompleted(time1);
    }
}
void ICalConverter::importTodoAlarm(icalcomponent *pComp, CTodo *pTodo, icalcomponent *pAComp, string strIcalComp, FileType iType)
{
    //icalproperty *pProp = 0;

    /* HANDLING ALARM */
    pAComp =
        icalcomponent_get_first_component(pComp,
                          ICAL_VALARM_COMPONENT);
    CAlarm *alarm = NULL;
    if (pAComp) {
        alarm = getAlarm(strIcalComp, pAComp,pTodo);
    } else if (strIcalComp.find(A_ALARM, 0) != string::npos
           && iType == VCAL_TYPE) {
        alarm = getAlarm(strIcalComp, NULL,pTodo);
    } else {
    }
    if (alarm) {
        pTodo->setAlarm(alarm);
        delete alarm;
        alarm = 0;
    }
}
void 
ICalConverter::importEventAlarm (
        icalcomponent    *pComp, 
        CEvent           *pEvent, 
        icalcomponent    *pAComp, 
        string            strIcalComp, 
        FileType          iType)
{
    pAComp =
        icalcomponent_get_first_component(pComp,
                          ICAL_VALARM_COMPONENT);
    CAlarm *alarm = NULL;
    if (pAComp) {
        alarm = getAlarm(strIcalComp, pAComp,pEvent);
    } else if (strIcalComp.find(A_ALARM, 0) != string::npos
           && iType == VCAL_TYPE) {
        alarm = getAlarm(strIcalComp, NULL,pEvent);
    } else {
    }

    if (alarm) {
        pEvent->setAlarm(alarm);
        delete alarm;
        alarm = 0;
    }
}

template<class T>
void ICalConverter::importDateStart(icalcomponent *pComp, T *pEntry)
{
    icalproperty *pProp = 0;
    time_t time_start = 0;
    struct icaltimetype s_time;
    string szZone = pEntry->getTzid();
    parseTimeZone(szZone);
    /*importing dtstart */
    memset (&s_time, 0, sizeof (struct icaltimetype));
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_DTSTART_PROPERTY);
#if 0
    icalparameter *pParamDtStart =0;
    pParamDtStart = icalproperty_get_first_parameter(pProp, ICAL_VALUE_PARAMETER);
    if (!icalproperty_kind_is_valid(ICAL_DTSTART_PROPERTY) ||
		    icalparameter_isa(pParamDtStart)== ICAL_NO_PARAMETER)
	    return ;
#endif
    icalvalue *value = icalproperty_get_value (pProp);
    
    if (pProp && (icalvalue_is_valid (value))) {
        s_time = icalproperty_get_dtstart(pProp);
        time_start = icaltime_as_timet_with_zone(s_time,
    		    s_time.zone);
        if(pEntry->getType() == E_TODO) {
    	    pEntry->setDateStamp(time_start);	
        }
        else {
    	    pEntry->setDateStart(time_start);
        }
        pProp = NULL;
    }
        
}

void ICalConverter::importJournalAttach(icalcomponent *pComp, CJournal *pJournal)
{
#if 0
    icalproperty *pProp = 0;
    icalparameter *pParam = 0;
    /*importing Attach */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_ATTACH_PROPERTY);
    if (pProp) {
        vector < string > vAttachList;
        while (pProp) {

        icalattach *icalAtt = icalproperty_get_attach(pProp);
        string szAttach = icalattach_get_url(icalAtt);
        vector < CParameters * >vAttParamList;
        pParam =
            icalproperty_get_first_parameter(pProp,
                             ICAL_FMTTYPE_PARAMETER);
        if (pParam) {
            CParameters *pFm = 0;
            pFm = getFMTypeFromIcal(pParam);
            if (pFm)
            vAttParamList.push_back(pFm);
            if (vAttParamList.size() != 0)
            hashMap.insert(pair < string,
                       vector < CParameters * > >(ATTACH,
                                 vAttParamList));
        }
        if (!szAttach.empty())
            vAttachList.push_back(szAttach);
        pProp =
            icalcomponent_get_next_property(pComp,
                            ICAL_ATTACH_PROPERTY);
        }
        if (vAttachList.size() != 0)
        pJournal->setAttachments(vAttachList);
    }
#endif
}

void ICalConverter::importComponentStatus(icalcomponent *pComp, CComponent *pComponent, FileType iType, string strIcalComp)
{
    icalproperty *pProp = 0;
    /*importing status */
    pProp = icalcomponent_get_first_property(pComp, ICAL_STATUS_PROPERTY);
    if (pProp) {
    int status = 0;
    if (iType == VCAL_TYPE) {
    VCalConverter *pVCal = new VCalConverter();
        status = pVCal->getStatusFromVcal(strIcalComp);
    delete pVCal;
    }
    else
        status = getStatusFromIcal(icalproperty_get_status(pProp));
    pComponent->setStatus((statusParam) status);
    }
}
/**
 */
void ICalConverter::importComponentLastModified(icalcomponent *pComp, CComponent *pComponent)
{
    icalproperty *pProp = 0;
    time_t modified_time = 0;
    struct icaltimetype lastmod_time;
    string szZone = pComponent->getTzid();
    parseTimeZone(szZone);
    /*importing last modified */
    pProp =
    icalcomponent_get_first_property(pComp,
                     ICAL_LASTMODIFIED_PROPERTY);
    if (pProp) {
    lastmod_time = icalproperty_get_lastmodified(pProp);
    if(lastmod_time.is_utc)
        szZone = "UTC";
    modified_time = icaltime_as_timet_with_zone(lastmod_time,
            icaltimezone_get_builtin_timezone(szZone.c_str()));
    pComponent->setLastModified(modified_time);
    pProp = NULL;
    }
}
/**
 *importEventRecurrence 
 */
void ICalConverter::importEventRecurrence(icalcomponent *pComp, 
        			CEvent 		*pEvent, 
        			string 		 strIcalComp, 
        			FileType 	 iType, 
        			int 		&pErrorCode)
{
    CAL_DEBUG_LOG ("In importEventRecurrence ");	
    icalproperty *pProp = 0;
    time_t time1;
    struct icaltimetype recurId;
//  bool flag =true;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    /* HANDLING RRULE AND EXRULE */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_RRULE_PROPERTY);
    vector < CRecurrenceRule * >rrule_list;
    if (pProp && iType == ICAL_TYPE) {
	    /*Fixes related to 0-34999 series*/
	    icalvalue* pVal = icalproperty_get_value(pProp);
	    if(ICAL_RECUR_VALUE != icalvalue_isa(pVal)){
		    return;
	    }
	    rrule_list =
        getRecurrence(strIcalComp, pComp, ICAL_RRULE_PROPERTY,
                  iType);
    } else if (strIcalComp.find(RRULE, 0) != string::npos
           && iType == VCAL_TYPE) {
        rrule_list =
        getRecurrence(strIcalComp, NULL, ICAL_RRULE_PROPERTY,
                  iType);
    }
    else if(strIcalComp.find(RRULE, 0) != string::npos) {
        CAL_DEBUG_LOG("Invalid RRULE format");
        pErrorCode = CALENDAR_ICS_COMPONENT_INVALID;
        return;
    }
    /* our calendarui doesn't support multiple rules 
     * Rtype parameter is used by calendar ui
     */

    //pRecur->getUntil() 

    pProp = NULL;
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_EXRULE_PROPERTY);
    vector < CRecurrenceRule * >exrule_list;
    if (pProp && iType == ICAL_TYPE) {

        exrule_list =
        getRecurrence(strIcalComp, pComp, ICAL_EXRULE_PROPERTY,
                  iType);
    } else if (strIcalComp.find(EXRULE, 0) != string::npos
           && iType == VCAL_TYPE) {
        exrule_list =
        getRecurrence(strIcalComp, NULL, ICAL_EXRULE_PROPERTY,
                  iType);
    } else if (strIcalComp.find(EXRULE, 0) != string::npos) {
        CAL_DEBUG_LOG("Invalid EXRULE format");
        pErrorCode = CALENDAR_ICS_COMPONENT_INVALID;
        return;
    }
    pProp = NULL;

    if (exrule_list.size() != 0) {
        for (unsigned int l = 0; l < exrule_list.size(); l++) {
        rrule_list.push_back(exrule_list.at(l));
        }
    }

    CRecurrence *cRecur = NULL;
    if (rrule_list.size() != 0) {
        cRecur = new CRecurrence();
    ASSERTION(cRecur);
        cRecur->setRecurrenceRule(rrule_list);
    }
    /* HANDLING RDATE AND EXDATE */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_RDATE_PROPERTY);
#if 0
    icalparameter* pParamRD = 0;
    pParamRD = icalproperty_get_first_parameter(pProp,ICAL_VALUE_PARAMETER);
    if (!icalproperty_kind_is_valid(ICAL_RDATE_PROPERTY)||
    	    icalparameter_isa(pParamRD)== ICAL_NO_PARAMETER)
        flag = false;
#endif 
    vector < string > rdate_list;
    if (pProp && iType == ICAL_TYPE ) {
        rdate_list =
        getRecurrenceDates(strIcalComp, pComp,pEvent,
                   ICAL_RDATE_PROPERTY);
    } else if (strIcalComp.find(RDATE, 0) != string::npos 
            && iType == VCAL_TYPE ) {
        rdate_list =
        getRecurrenceDates(strIcalComp, NULL,pEvent, ICAL_RDATE_PROPERTY,VCAL_TYPE);
    } else if (strIcalComp.find(RDATE, 0) != string::npos) {
        CAL_DEBUG_LOG("Invalid RDATE format");
        pErrorCode = CALENDAR_ICS_COMPONENT_INVALID;
        return;
    }
    if (rdate_list.size() != 0) {
        if (!cRecur)
        cRecur = new CRecurrence();
    ASSERTION(cRecur);
        cRecur->setRDays(rdate_list);
    }
    pProp = NULL;
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_EXDATE_PROPERTY);
    vector < string > exdate_list;
# if 0 
    icalparameter* pParamED = 0;
    flag =true;
    pParamED = icalproperty_get_first_parameter(pProp,ICAL_VALUE_PARAMETER);
    if (!icalproperty_kind_is_valid(ICAL_EXDATE_PROPERTY)||
		    icalparameter_isa(pParamED)== ICAL_NO_PARAMETER)
	    flag = false;
    if (pProp && iType == ICAL_TYPE && flag) {
#endif 
    if (pProp && iType == ICAL_TYPE ) {
        exdate_list =
        getRecurrenceDates(strIcalComp, pComp,pEvent,
                   ICAL_EXDATE_PROPERTY);
    } else if (strIcalComp.find(EXDATE, 0) != string::npos 
            && iType == VCAL_TYPE) {
        exdate_list =
        getRecurrenceDates(strIcalComp, NULL,pEvent,
                   ICAL_EXDATE_PROPERTY,VCAL_TYPE);
    }
    else if (strIcalComp.find(EXDATE, 0) != string::npos) {
        CAL_DEBUG_LOG("Invalid EXDATE format");
        pErrorCode = CALENDAR_ICS_COMPONENT_INVALID;
        return;
    }
    pProp = NULL;
       
 
    if (exdate_list.size() != 0) {
        if (!cRecur)
        cRecur = new CRecurrence();
    ASSERTION(cRecur);
        cRecur->setEDays(exdate_list);
    }

    /*setRtype for rrulelist *
    E_NONE =0 ,
    E_DAILY,
    E_WEEKDAY,
    E_WEEKLY,
    E_MONTHLY,
    E_YEARLY,
    E_COMPLEX
        *****/

    if (cRecur) {
        if ((rrule_list.size() > 1) ||
          (rdate_list.size() > 0) ||
        (exdate_list.size() > 0))    //setting type as complex
        cRecur->setRtype(E_COMPLEX);
        else {
        if ((rrule_list[0]->getRrule().find(DAILY)) !=
            string::npos)
        cRecur->setRtype(E_DAILY);
        if ((rrule_list[0]->getRrule().find(WEEKLY)) !=
            string::npos)
            cRecur->setRtype(E_WEEKLY);

        if ((rrule_list[0]->getRrule().find(MONTHLY)) !=
            string::npos)
            cRecur->setRtype(E_MONTHLY);

        if ((rrule_list[0]->getRrule().find(YEARLY)) !=
            string::npos)
            cRecur->setRtype(E_YEARLY);

        if ((rrule_list[0]->getRrule().find(WEEKDAY)) !=
            string::npos)
            cRecur->setRtype(E_WEEKDAY);

    /* added additional check to see if the 
     * event is complex recurrence event  */
    
        string rrule = rrule_list[0]->getRrule();

    /* Finding complex recurrence pattern */
    CRecurrenceRule * pRrule= new  CRecurrenceRule(0,rrule);
    pRrule->rruleParser(rrule);
    if(pRrule->checkRuleIsComplex()) 
        cRecur->setRtype(E_COMPLEX);
    delete pRrule;
    pRrule = 0;
    
        unsigned int pos1;
        unsigned int pos2;
        if ((pos1 = (rrule.find(UNTIL_ICAL))) != string::npos) {
            rrule = rrule.substr(pos1 + 6);
            if ((pos2 = (rrule.find(' '))) != string::npos)
            rrule = rrule.substr(0, pos2);
            else if ((pos2 = (rrule.find(";"))) != string::npos)
            rrule = rrule.substr(0, pos2);
            struct icaltimetype until;
        CAL_DEBUG_LOG("Recurrence Rule is : ** %s **",rrule.c_str());
        until = icaltime_from_string(rrule.c_str());
        
        CAL_DEBUG_LOG("*********Until start from ICS *********** ");
        CAL_DEBUG_LOG(" Year is:     %d ", until.year);
        CAL_DEBUG_LOG(" Month is:     %d ", until.month);
        CAL_DEBUG_LOG(" Day is:     %d ", until.day);
        CAL_DEBUG_LOG(" Hour is:     %d ", until.hour);
        CAL_DEBUG_LOG(" Minute  is:    %d ", until.minute);
        CAL_DEBUG_LOG(" Second is :    %d ", until.second);
        CAL_DEBUG_LOG("**************************************** ");

        /*Calenar supports dates in range from 1970 
         * to 2037 only  then change the date to 
         * max supported date  31-12-2037
         * 
         */
        
        if(until.year > MAX_SUPPORTED_YEAR){
            until.year = MAX_SUPPORTED_YEAR;
            if(cRecur->getRtype() != E_YEARLY) {
                    until.month = 12;
                    until.day = 31;
                    until.hour = 0;
                    until.minute = 0;
                    until.second = 0;
            }
        }
        string szZone = pEvent->getTzid();
	    parseTimeZone(szZone);
        
        string szTempZone = szZone;
        if(until.is_utc == 1)
            szTempZone = "UTC";
        time_t until_t =icaltime_as_timet_with_zone(until, 
        	    icaltimezone_get_builtin_timezone(szTempZone.c_str()));

        //pEvent->setUntil(until_t);
        /* check if until has 6 T000000 if so replace 
         * add offset of 86399 sec to until field */
        unsigned int i = 0;
        i = rrule.find(SIX_O,0);
        
        if (i == string::npos || ((until_t + ONEDAY - 1) >= MAX_SUPPORTED_YEAR_IN_SEC)){
                 pEvent->setUntil(until_t);
        }
        else{ 
           
                 pEvent->setUntil(until_t + ONEDAY -1);
        }
        }


        }

        /* setting recurrence */
        /*importing recurid */
        pProp =
        icalcomponent_get_first_property(pComp,
                         ICAL_RECURRENCEID_PROPERTY);
        if (ical_value_is_valid(pProp)) {
        recurId = icalproperty_get_recurrenceid(pProp);
        time1 = getIntFromIcaltimetype(recurId);
        cRecur->setRecurId(time1);
        pProp = NULL;
        }
        pEvent->setRecurrence(cRecur);

        if (cRecur) {
        delete cRecur;
        cRecur = 0;
        }
    }
}

CComponent *ICalConverter::toLocal(string strIcalComp, FileType iType,
                   int &pErrorCode)
{
    icalcomponent *pComp = 0;
    icalcomponent *pAComp = 0;
    icalcomponent_kind kind;
    icalproperty *pProp = 0;
    icalparameter *pParam = 0;
    struct icaltimetype c_time;
    CComponent *pComponent = NULL;
    CEvent *pEvent = 0;
    CTodo *pTodo = 0;
    CJournal *pJournal = 0;
    CUtility *pUt = 0;
    pUt = CUtility::Instance();
    map < string, vector <CParameters *> >hashMap;
    /* clear the aten list */
    vector <char *> vAttachList;
    string temp;
    vAttachList.clear();

    if (strIcalComp.empty()) {
    pErrorCode = CALENDAR_INVALID_ARG_ERROR;
    return 0;
    }
       
   pComp = icalcomponent_new_from_string((char *) strIcalComp.c_str());

    if (!pComp) {
    pErrorCode = CALENDAR_ICAL_PARSE_ERROR;
    return 0;
    }
    kind = icalcomponent_isa(pComp);
    if (kind == ICAL_VEVENT_COMPONENT) {

    pEvent = new CEvent();
    ASSERTION(pEvent);
    pComponent = (CComponent *) pEvent;
    pEvent->setType(E_EVENT);
    vector < CAttendee * >pAttenList;

    /* importing datestart and date end */
     importEventDateStartAndDateEnd(pComp, pEvent, iType,pErrorCode);
     if(pErrorCode == CALENDAR_ICS_COMPONENT_INVALID)
     return 0;
    /*importing x-prop */
    //importEventXProp (pComp, pEvent);
    pProp = icalcomponent_get_first_property(pComp, ICAL_X_PROPERTY);
    if (pProp) {
        vector <CProperties *> pProList;
        CProperties *pPro = 0;
        while (pProp != NULL) {
    	    pPro = getXProperty(pProp);
    	    if (pPro) {

    		    PropType propVal;
    		    propVal = pPro->getPropValue();
    		    /* deletion of invalid Xproperties is handled inside the 
    		     * getXproperty
    		     */
    		    pProList.push_back(pPro);
    		    pParam =
    			    icalproperty_get_first_parameter(pProp,
    					    ICAL_ANY_PARAMETER);
    		    if (pParam) {
    			    CParameters *pPara = 0;
    			    vector < CParameters * > vParamList;

    			    while (pParam) {
    				    pPara = getParameters(pParam);
    				    if (pPara)
    					    vParamList.push_back(pPara);
    				    pParam =
    					    icalproperty_get_next_parameter(pProp,
    							    ICAL_ANY_PARAMETER);
    			    }  
    		    hashMap.insert(pair < string,
    					    vector < CParameters * > >
    					    (icalproperty_get_x_name(pProp),
    					     vParamList));
    		    }
    	    }


    	    pProp =
    		    icalcomponent_get_next_property(pComp,
    				    ICAL_X_PROPERTY);
        }
        pEvent->setXProperties(pProList);
        pUt->releasePropertiesVector(pProList);
    }
    /*importing created time */
    c_time = importCreatedTime (pComp, pEvent, iType, strIcalComp);    
    /*importing date stamp */
    importDateStamp (pComp, pEvent, c_time);
    /*importing organizer */
    importOrganizer (pComp, pEvent);
    /*importing Attendees */
    importAttendees  (pComp, pEvent, iType);
    /*importing transparency */
    importEventTransp    (pComp, pEvent, iType, strIcalComp);
    /*importing categories */
    vector<CParameters *> vCatParamList = importCategories (pComp, pEvent);
    if(vCatParamList.size()>0)
        hashMap.insert(pair < string,
                   vector < CParameters * > >(CATEGORIES,
                             vCatParamList));
    /*importing sequence */
    importSequence  (pComp, pEvent);
    /*importing priority */
    importPriority    (pComp, pEvent);
    /*importing Resources */
    vector<CParameters *> vResParamList = importResources (pComp, pEvent);
    if (vResParamList.size() != 0)
        hashMap.insert(pair < string,
            vector < CParameters * > >(RESOURCES,
            vResParamList));
    /*importing URL */
    importUrl    (pComp, pEvent);
    /*importing Attach */
    //importEventAttachements (pComp, pEvent);
    /*importing Attach */
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_ATTACH_PROPERTY);
    if (pProp) {
        vector < string > vAttachList;
        while (pProp) {

        icalattach *icalAtt = icalproperty_get_attach(pProp);
    string szAttach;
    if (icalAtt){
        char * pTemp =0 ;
        if (icalattach_get_is_url(icalAtt))
        	pTemp = (char*)icalattach_get_url(icalAtt);
        else
        	pTemp =(char*) icalattach_get_data(icalAtt);
        if(pTemp)
        {
        	szAttach = pTemp;
        }
    }    
    
        vector < CParameters * >vAttParamList;
        pParam =
            icalproperty_get_first_parameter(pProp,
                             ICAL_FMTTYPE_PARAMETER);
        if (pParam) {
            CParameters *pFm = 0;
            pFm = getFMTypeFromIcal(pParam);
            if (pFm)
            vAttParamList.push_back(pFm);
            if (vAttParamList.size() != 0)
            hashMap.insert(pair < string,
                       vector < CParameters * > >(ATTACH,
                                 vAttParamList));
        }
        if (!szAttach.empty())
            vAttachList.push_back(szAttach);
        pProp =
            icalcomponent_get_next_property(pComp,
                            ICAL_ATTACH_PROPERTY);
        }
        if (vAttachList.size() != 0)
        pEvent->setAttachments(vAttachList);
    }
    pProp = NULL;
    /*importing geo */
    importGeo    (pComp, pEvent);
    /*importing contacts */
    vector<CParameters *> vConParamList = importContacts (pComp, pEvent);
        if (vConParamList.size() != 0) {
        hashMap.insert(pair < string,
                   vector < CParameters * > >(CONTACT,
                   vConParamList));
        }
    /*importing related-to */
   vector<CParameters *> vRelParamList = importRelated (pComp, pEvent);;
        if (vRelParamList.size() != 0) {
        hashMap.insert(pair < string,
                   vector < CParameters * > >(RELATEDTO,
                   vRelParamList));
    }

    /*importing classification-class */
    importClass (pComp, pEvent);
/* For text fields like location 
 * description and summary we 
 * need to have a decoder function 
 */
    /*importing location */
    vector<CParameters *> vLocParamList = importLocation (pComp, pEvent); 
        if (vLocParamList.size() != 0)
        hashMap.insert(pair < string,
                   vector < CParameters * > >(LOCATION,
                   vLocParamList));
    /* importing description */
    vector<CParameters *> vDesParamList = importDescription (pComp, pEvent); 
        if (vDesParamList.size() != 0)
        hashMap.insert(pair < string,
            vector < CParameters * > >(DESCRIPTION,
             vDesParamList));
    /*importing summary */
    vector<CParameters *> vSumParamList = importSummary (pComp, pEvent); 
        if (vSumParamList.size() != 0)
        hashMap.insert(pair < string,
                   vector < CParameters * > >(SUMMARY,
                             vSumParamList));
    /*importing comment */
    vector<CParameters *> vCommParamList = importComments (pComp, pEvent); 
        if (vCommParamList.size() != 0)
        hashMap.insert(pair < string,
                   vector < CParameters * > >(COMMENT,
                             vCommParamList));
       
   /*importing recurrence*/
    importEventRecurrence (pComp, pEvent, strIcalComp, iType, pErrorCode);
    if(pErrorCode == CALENDAR_ICS_COMPONENT_INVALID)
     return 0;

    /* importing alarm */
    importEventAlarm (pComp, pEvent, pAComp, strIcalComp, iType);
    
    pEvent->setHashMap(hashMap);

    /* if the event is recursive then  register the alarm 
     * for trigger time > current system time  we need to do this only 
     * for task*/
     if (pEvent->getAlarm())
     {
        if (pEvent->updateAlarmTriggerTime(0))
        {
            time_t trigger = (pEvent->getAlarm())->getTrigger();
            CAL_DEBUG_LOG("Trigger time for '%s' is updated: %s", pEvent->getSummary().c_str(), ctime(&trigger));
        }
        else
        {
            CAL_DEBUG_LOG("No more instances so remove alarm");
            pEvent->removeAlarm();
        }
    }
	if ((pEvent->getRecurrence() != 0) && (pEvent->getAlarm() != 0))
		pEvent->setFlags(HAS_RECURRENCE_ALARM);
    	else if ((pEvent->getRecurrence() != 0) && (pEvent->getAlarm() == 0))
		pEvent->setFlags(HAS_RECURRENCE);
    	else if ((pEvent->getRecurrence() == 0) && (pEvent->getAlarm() != 0))
		pEvent->setFlags(HAS_ALARM);
    }
    
    else if (kind == ICAL_VTODO_COMPONENT) {
    	pTodo = new CTodo();
   	    ASSERTION(pTodo);
    	pComponent = (CComponent *) pTodo;
    	pTodo->setType(E_TODO);
    	vector < CAttendee * >pAttenList;
    	CMulticalendar* pMc = CMulticalendar::MCInstance();
    	string sZone =pMc->getSystemTimeZone();
    	/* tasks are always imported in the 
    	 * local time zone*/
    	pTodo -> setTzid(sZone);
       	
   	/*importing priority*/
    	importPriority     (pComp, pTodo);
    	/* importing startdate */
    	importDateStart(pComp,pTodo); 
    	/*importing due due is mapped to date start*/
    	importTodoDue    (pComp, pTodo);
    	/*importing categories */
    	vector<CParameters*> vCatParamList = importCategories (pComp, pTodo);
    	if(vCatParamList.size()>0) 
    		hashMap.insert(pair < string,
    			vector < CParameters * > >(CATEGORIES,
    			vCatParamList));
    	/*importing location */
    	vector<CParameters *> vLocParamList = importLocation (pComp, pTodo); 
    	if (vLocParamList.size() != 0)
    		hashMap.insert(pair < string,
    			vector < CParameters * > >(LOCATION,
    			vLocParamList));
    	/* importing description */
    	vector<CParameters *> vDesParamList = importDescription (pComp, pTodo); 
    	if (vDesParamList.size() != 0)
    		hashMap.insert(pair < string,
    			vector < CParameters * > >(DESCRIPTION,
    			vDesParamList));
    	/*importing summary */
    	vector<CParameters *> vSumParamList = importSummary (pComp, pTodo); 
    	if (vSumParamList.size() != 0)
    		hashMap.insert(pair < string,
    		vector < CParameters * > >(SUMMARY,
    						vSumParamList));
        /*importing x-prop */
    	pProp = icalcomponent_get_first_property(pComp, ICAL_X_PROPERTY);
    	if (pProp) {
	   CProperties *pPro = 0;
   	   vector <CProperties *> pProList;
    	   while (pProp != NULL) {
    	        pPro = getXProperty(pProp);
    		if (pPro) {
    		   pProList.push_back(pPro);
    		   pParam =
    		         icalproperty_get_first_parameter(pProp,
    				ICAL_X_PARAMETER);
    		   if (pParam) {
    			CParameters *pPara = 0;
    			vector < CParameters * >vParamList;
			while (pParam) {
			    pPara = getParameters(pParam);
			    if (pPara)
				vParamList.push_back(pPara);
			    pParam = icalproperty_get_next_parameter(pProp,
						ICAL_X_PARAMETER);
   			}
    			hashMap.insert(pair < string,
    				vector <CParameters *> >(icalproperty_get_x_name(pProp),
    				vParamList));
    		 }
 	     }

    	   pProp =
    	      icalcomponent_get_next_property(pComp,
    	        ICAL_X_PROPERTY);
    	}
    	pTodo->setXProperties(pProList);
    				pUt->releasePropertiesVector(pProList);
      }

   /*importing Attach */
    pProp =
       icalcomponent_get_first_property(pComp, ICAL_ATTACH_PROPERTY);
    if (pProp) {
      vector < string > vAttachList;
      while (pProp) {
	icalattach *icalAtt = icalproperty_get_attach(pProp);
	string szAttach;
	if (icalAtt && ical_value_is_valid (pProp))       
	szAttach = icalattach_get_url(icalAtt);
	vector < CParameters * >vAttParamList;
	pParam =
		icalproperty_get_first_parameter(pProp,
		ICAL_FMTTYPE_PARAMETER);
	if (pParam) {
            CParameters *pFm = 0;
	    pFm = getFMTypeFromIcal(pParam);
	    if (pFm)
		vAttParamList.push_back(pFm);
	    if (vAttParamList.size() != 0)
		hashMap.insert(pair < string,
	    vector < CParameters * > >(ATTACH,
				vAttParamList));
	}
       if (!szAttach.empty())
           vAttachList.push_back(szAttach);
      pProp =
	icalcomponent_get_next_property(pComp,
				ICAL_ATTACH_PROPERTY);
      }
      if (vAttachList.size() != 0)
	      pTodo->setAttachments(vAttachList);
    }
    /*importing contacts */
    vector<CParameters *> vConParamList = importContacts (pComp, pTodo); 
    if (vConParamList.size() != 0) 
	    hashMap.insert(pair < string,
			    vector < CParameters * > >(CONTACT,
				    vConParamList));

    /*importing created time */
    c_time = importCreatedTime (pComp, pTodo, iType, strIcalComp);    
    /*importing related-to */
    vector < CParameters * >vRelParamList = importRelated (pComp, pTodo);
    if(vRelParamList.size()>0)
	    hashMap.insert(pair < string,
		    vector < CParameters * > >(RELATEDTO,
		    vRelParamList));

    /*importing sequence */
    importSequence (pComp, pTodo); /*importing URL */
    importUrl (pComp, pTodo);
    /*importing geo */
    importGeo (pComp, pTodo);
    /*importing classification-class */
    importClass (pComp, pTodo);
    /*importing comment */
    vector < CParameters * >vCommParamList = importComments (pComp, pTodo);
    if (vCommParamList.size() != 0)
	hashMap.insert(pair < string,
		vector < CParameters * > >(COMMENT,
		vCommParamList));
   /*importing percent complete */
    importTodoPercentCompleted (pComp, pTodo);
    /*importing completed */
    importTodoCompleted (pComp, pTodo);

    /*importing Resources */
    vector < CParameters * > vResParamList = importResources (pComp, pTodo);
    if (vResParamList.size() != 0)
	    hashMap.insert(pair < string,
			    vector < CParameters * > >(RESOURCES,
				    vResParamList));
    /*importing organizer */
    importOrganizer (pComp, pTodo);
    /*importing Attendees */
    importAttendees (pComp, pTodo, iType);
    /* importing alarm */
    importTodoAlarm (pComp, pTodo, pAComp, strIcalComp, iType);

    pTodo->setHashMap(hashMap);
    /* this is a kind of hack for a task which doesn't 
     * have a start time / Due date add such task on current day 
     */
    if (pTodo->getDateStart() == -1 ) {
	    pTodo->setDateStart(time_get_time());
	    /* tasks date start is greater than  DUE 
	     * date then adjust it accordingly
	     */
	    if((pTodo->getDateStart() < pTodo->getDateStamp() ) && 
			    ( pTodo ->getDateStamp() != -1) ) 
		    pTodo->setDateStamp(pTodo->getDateStart());

    }
    } else if (kind == ICAL_VJOURNAL_COMPONENT) {
    pJournal = new CJournal();
    ASSERTION(pJournal);
    pComponent = (CComponent *) pJournal;
    pJournal->setType(E_JOURNAL);
    vector < CAttendee * >pAttenList;
    CMulticalendar* pMc = CMulticalendar::MCInstance();
    string sZone =pMc->getSystemTimeZone();
    /* tasks are always imported in the 
     * local time zone*/
    pJournal -> setTzid(sZone); 
    
    /*importing created time */
    c_time = importCreatedTime (pComp, pJournal, iType, strIcalComp);
    /*importing date stamp */
    importDateStamp (pComp, pJournal, c_time);
    /*importing categories */
    vector < CParameters * >vCatParamList = importCategories (pComp, pJournal);
    if(vCatParamList.size()>0)
    hashMap.insert(pair < string,
                   vector < CParameters * > >(CATEGORIES,
                   vCatParamList));
    /*importing dtstart */
    importDateStart (pComp, pJournal);
    /*importing related-to */
    vector < CParameters * >vRelParamList = importRelated (pComp, pJournal);
    hashMap.insert(pair < string,
                   vector < CParameters * > >(RELATEDTO,
                             vRelParamList));
    /*importing sequence */
    importSequence (pComp, pJournal);
    /*importing contacts */
    vector < CParameters * >vConParamList = importContacts (pComp, pJournal);
        hashMap.insert(pair < string,
                   vector < CParameters * > >(CONTACT,
                             vConParamList));
    /*importing classification-class */
    importClass (pComp, pJournal);
    /*importing comment */
    vector < CParameters * >vCommParamList = importComments (pComp, pJournal);
        hashMap.insert(pair < string,
                   vector < CParameters * > >(COMMENT,
                             vCommParamList));
    /*importing location */
    vector < CParameters * >vLocParamList = importLocation (pComp, pJournal);
        hashMap.insert(pair < string,
                   vector < CParameters * > >(LOCATION,
                             vLocParamList));
    /* importing description */
    vector < CParameters * >vDesParamList = importDescription (pComp, pJournal);
        if (vDesParamList.size() != 0)
        hashMap.insert(pair < string,
                   vector < CParameters * > >(DESCRIPTION,
                             vDesParamList));
    /*importing summary */
    vector < CParameters * >vSumParamList = importSummary (pComp, pJournal);
        if (vSumParamList.size() != 0)
        hashMap.insert(pair < string,
                   vector < CParameters * > >(SUMMARY,
                             vSumParamList));
    /*importing x-prop */
    pProp = icalcomponent_get_first_property(pComp, ICAL_X_PROPERTY);
    if (pProp) {
        CProperties *pPro = 0;
        vector < CProperties * >pProList;
        while (pProp != NULL) {
        pPro = getXProperty(pProp);
        if (pPro) {
            pProList.push_back(pPro);
            pParam =
            icalproperty_get_first_parameter(pProp,
                             ICAL_X_PARAMETER);
            if (pParam) {
            CParameters *pPara = 0;
            vector < CParameters * >vParamList;

            while (pParam) {
                pPara = getParameters(pParam);
                if (pPara)
                vParamList.push_back(pPara);

                pParam =
                icalproperty_get_next_parameter(pProp,
                                ICAL_X_PARAMETER);
            }
            hashMap.insert(pair < string,
                       vector <
                       CParameters *
                       > >(icalproperty_get_x_name(pProp),
                      vParamList));
            }
        }

        pProp =
            icalcomponent_get_next_property(pComp,
                            ICAL_X_PROPERTY);
        }
        pJournal->setXProperties(pProList);
    pUt->releasePropertiesVector(pProList);
    }
    
    /*importing URL */
    importUrl (pComp, pJournal);
    /*importing organizer */
    importOrganizer (pComp, pJournal);
    /*importing Attendees */
    importAttendees (pComp, pJournal, iType);
    /*importing Attach */
    //importJournalAttach (pComp, pJournal);
    pProp =
        icalcomponent_get_first_property(pComp, ICAL_ATTACH_PROPERTY);
    if (pProp) {
        vector < string > vAttachList;
        while (pProp) {

        icalattach *icalAtt = icalproperty_get_attach(pProp);
        string szAttach = icalattach_get_url(icalAtt);
        vector < CParameters * >vAttParamList;
        pParam =
            icalproperty_get_first_parameter(pProp,
                             ICAL_FMTTYPE_PARAMETER);
        if (pParam) {
            CParameters *pFm = 0;
            pFm = getFMTypeFromIcal(pParam);
            if (pFm)
            vAttParamList.push_back(pFm);
            if (vAttParamList.size() != 0)
            hashMap.insert(pair < string,
                       vector < CParameters * > >(ATTACH,
                                 vAttParamList));
        }
        if (!szAttach.empty())
            vAttachList.push_back(szAttach);
        pProp =
            icalcomponent_get_next_property(pComp,
                            ICAL_ATTACH_PROPERTY);
        }
        if (vAttachList.size() != 0)
        pJournal->setAttachments(vAttachList);
    }

    pJournal->setHashMap(hashMap);
    } else {
    pErrorCode = CALENDAR_INVALID_ICSFILE;
    icalcomponent_free(pComp);
    return NULL;
    }


    /*importing status */
    importComponentStatus (pComp, pComponent, iType, strIcalComp);
    /*importing last modified */
    importComponentLastModified (pComp, pComponent);
    icalcomponent_free(pComp);
    return pComponent;
}


/**
 * get_line
 * @param strIcal: Ical data in string format
 * @param start_loc: Staring position for search
 *
 * @return a string fromstart_loc to until newline char
 */
string ICalConverter::get_line(string strIcal, int start_loc)
{
    string strSub;
    string strLine;
    strSub = strIcal.substr(start_loc, strIcal.length());
    std::istringstream iss(strSub);
    getline(iss, strLine, '\n');
    return strLine;
}


/**
 * get_line_in_ical
 * @param strInVcal: string in Ical format
 * @param type, either RRULE, EXRULE, RDATE, EXDATE, AALARM
 *
 * This function is used to get substring from strInVcal which is matching with type (string)
 */
string ICalConverter::get_line_in_ical(string strInVcal, string type,
                       int index)
{
    CAL_DEBUG_LOG("strInVcal = %s\n ", strInVcal.c_str());
    string strSub;
    string::size_type iPos = 0;
    string::size_type startPos = iPos;
    int loop = 0;
    while (loop != index) {
    iPos = strInVcal.find(type, startPos);
    startPos = iPos + type.size();
    loop += 1;
    }
    if (iPos != string::npos) {
    strSub = get_line(strInVcal, iPos);
    }
    CAL_DEBUG_LOG("strSub = %s\n ", strSub.c_str());
    return strSub;
}

/**
 * function to get impending alarm 
 */
void ICalConverter::getImpendingAlarm (string szRrule,time_t & triggerTime, time_t dateStart, int alarmFlag)
{
  /* For a recursive event if the alarm time is
   * expired then we need to find the impending alarm time 
   * for next event
   */ 
   vector<time_t>     listtime ;
   vector<time_t>     instance_time ;
   vector<time_t>::iterator time_iter;
   int         	errVal;
   CMulticalendar *mc = CMulticalendar::MCInstance();
   icaltimezone* pTz= mc->getSystemTimeZoneAsIcalTimeZone();
   if ( triggerTime > time_get_time())
       return ;
   if(alarmFlag != E_AM_DAYBEFORE) {
           mc->getRecurrentTimes(szRrule,
        		   icaltime_from_timet_with_zone(triggerTime, ZERO,pTz),
                   0,
                   0,
        		   MAX_SUPPORTED_YEAR_IN_SEC,
        		   pTz,
                   listtime,
        		   errVal);
   }
   else {
        mc->getRecurrentTimes(szRrule,
        	   icaltime_from_timet_with_zone(dateStart, ZERO,pTz),
               0,
               0,
        	   MAX_SUPPORTED_YEAR_IN_SEC,
        	   pTz,
               instance_time,
        	   errVal);
       unsigned int titer = 0;
       for (titer = 0 ; titer < instance_time.size(); titer++) {
           int inst_time = instance_time[titer] - 86400;
           struct tm *tm_trigger = localtime (&triggerTime);
           inst_time += (3600 * tm_trigger->tm_hour ) +
        	   	(60 * tm_trigger->tm_min)  +
        		tm_trigger->tm_sec;
           listtime.push_back(inst_time);
       }
   }

    for (time_iter = listtime.begin(); time_iter != listtime.end(); time_iter++) {
        if (time_get_time() < (*time_iter)) {
        triggerTime = (time_t) (*time_iter);
        break;
        }
    }
}






string ICalConverter::toString()
{
       string res;

    /*
       sprintf(res,"icalcomponent : %p ", m_pCalcomp);

       return res;
     */
    return res;
}

string ICalConverter::getTimeInHHMM(int diffTime)
{
        CAL_DEBUG_LOG("**************************");
        CAL_DEBUG_LOG("Diff time is %d",diffTime );
        CAL_DEBUG_LOG("offset is %d",diffTime%60);
        CAL_DEBUG_LOG("**************************");
            int iSecOffset = 0;
        iSecOffset = diffTime%3600 ;
        iSecOffset = iSecOffset/60;
        diffTime = diffTime/3600;
        /* this will give the minute offset that has to be added 
         * to the TZ: Parameter */
        
        /* ensure that isec offset is always positive */
        if (iSecOffset < 0)
            iSecOffset = 0 - iSecOffset;
        
        char *hour= 0;
        if (diffTime >= 0)
            asprintf(&hour,"+%02d",diffTime);
        else
            asprintf(&hour,"-%02d",-(diffTime));
        char *hour_min = 0;
        if(iSecOffset)
            asprintf(&hour_min,"%s:%02d", hour, iSecOffset);
        else
            asprintf(&hour_min,"%s", hour);
            
        string sData;
        if (hour_min)
            sData = hour_min;
        free(hour);
        hour = 0;
        free(hour_min);
        hour_min = 0;
        return sData;
}

void ICalConverter::parseTimeZone(string &szZone)
{
    unsigned int pos = 0;
    if (szZone.empty())
	    return ;
    if((pos = szZone.find(":", 0)) != string::npos)
        szZone = szZone.substr(pos + 1);
}

void ICalConverter::setSyncing(bool sync)
{
    bSyncing = sync;
}

bool stringRemoveAll(string & toChange, const char * removeThis)
{
    bool              retval = false;
    size_t            pos = 0;

    size_t l = strlen(removeThis);

    CAL_DEBUG_LOG("%s", removeThis);

    while ((pos = toChange.find(removeThis, pos)) != string::npos)
    {
        retval = true;
        toChange.erase(pos, l);
    }

    CAL_DEBUG_LOG("returns %d", retval);
    return retval;
}

string guessLocationFromVTimezone(string szVTimezone)
{
    string location;

    icalcomponent * pTz = icalcomponent_new_from_string((char *)szVTimezone.c_str());

    if (pTz) {
        string szTzid;
        CAL_DEBUG_LOG ("VTimezone  entry\n-8<-\n%s\n-8<-",
                        szVTimezone.c_str());
        CAL_DEBUG_LOG ("VTimezone: %d TZID properties, %d xstandard, %d xdaylight",
                        icalcomponent_count_properties(pTz, ICAL_TZID_PROPERTY),
                        icalcomponent_count_components(pTz, ICAL_XSTANDARD_COMPONENT),
                        icalcomponent_count_components(pTz, ICAL_XDAYLIGHT_COMPONENT));

        // TZID
        icalproperty *prop_tzid = icalcomponent_get_first_property(pTz, ICAL_TZID_PROPERTY);

        if (prop_tzid != 0) {
            szTzid = icalproperty_get_tzid(prop_tzid);
            CAL_DEBUG_LOG("TZID: %s", szTzid.c_str());
        }

        // STANDARD
        icalcomponent *tz_std = icalcomponent_get_first_component(pTz, ICAL_XSTANDARD_COMPONENT);
        icalcomponent *tz_dlt = icalcomponent_get_first_component(pTz, ICAL_XDAYLIGHT_COMPONENT);

        icalproperty *std_offset_prop = icalcomponent_get_first_property(tz_std, ICAL_TZOFFSETTO_PROPERTY);
        icalproperty *dst_offset_prop = icalcomponent_get_first_property(tz_dlt, ICAL_TZOFFSETTO_PROPERTY);

        icalproperty *std_rrule_prop = icalcomponent_get_first_property(tz_std, ICAL_RRULE_PROPERTY);
        icalproperty *dst_rrule_prop = icalcomponent_get_first_property(tz_dlt, ICAL_RRULE_PROPERTY);

        time_t std_dtstart=0;
        time_t dst_dtstart=0;
        time_t std_offset=0;
        time_t dst_offset=0;
        string std_rrule;
        string dst_rrule;

        if (std_offset_prop != 0 &&
            std_rrule_prop != 0)
        {
            std_offset = icalproperty_get_tzoffsetto(std_offset_prop);
            icalrecurrencetype r(icalproperty_get_rrule(std_rrule_prop));
            const char * rrule = icalrecurrencetype_as_string(&r);

            if (rrule)
                std_rrule = rrule;

            icaltimetype dtstart(icalcomponent_get_dtstart(tz_std));

            struct tm t;
            memset(&t, 0, sizeof(t));
            t.tm_hour = dtstart.hour;
            t.tm_min = dtstart.minute;
            t.tm_sec = dtstart.second;
            t.tm_year = dtstart.year - 1980;
            t.tm_mon = dtstart.month-1;
            t.tm_mday = dtstart.day;

            std_dtstart  = timegm(&t);

            CAL_DEBUG_LOG("TZ STD: %02d:%02d:%02d, %02d.%02d.%d (%ld) offset=%d, rule=%s",
                            dtstart.hour,
                            dtstart.minute,
                            dtstart.second,
                            dtstart.day,
                            dtstart.month,
                            dtstart.year,
                            (long)std_dtstart,
                            (int)std_offset,
                            rrule);
        };

        if (dst_offset_prop != 0 &&
            dst_rrule_prop != 0)
        {
            dst_offset = icalproperty_get_tzoffsetto(dst_offset_prop);
            icalrecurrencetype r(icalproperty_get_rrule(dst_rrule_prop));
            const char * rrule = icalrecurrencetype_as_string(&r);
            if (rrule)
                dst_rrule = rrule;

            icaltimetype dtstart(icalcomponent_get_dtstart(tz_dlt));

            time_t dst_dtstart   = dtstart.hour * 3600 + dtstart.minute * 60 + dtstart.second  + dst_offset;

            CAL_DEBUG_LOG("TZ DLT: %02d:%02d:%02d (%ld), offset=%d, rule=%s",
                            dtstart.hour,
                            dtstart.minute,
                            dtstart.second,
                            (long)dst_dtstart,
                            (int)dst_offset,
                            rrule);
        };

        int errorCode;
        CTimezone *tz = CTimezone::guessTimeZone(std_dtstart,
                        dst_dtstart,
                        std_offset,
                        dst_offset,
                        std_rrule,
                        dst_rrule,
                        !dst_rrule.empty(),
                        errorCode);

        if (tz) {
            location = tz->getLocation();
            delete tz;
        } else {
            CAL_DEBUG_LOG("Faield to match. Lets try to guess");
        }

        if (location.empty())
        {
            if (dst_rrule.empty())
            {
                CAL_DEBUG_LOG("Check timezone for offset %ld w/o DST", std_offset);
                location  = CTimezone::getLocation(std_offset, false, 0, string());
            }
            else
            {
                CAL_DEBUG_LOG("Check timezone for offsets %ld/%ld", std_offset, dst_offset);
                location  = CTimezone::getLocation(std_offset, true, dst_offset, dst_rrule+"!"+std_rrule);
            }
        }

        icalcomponent_free(pTz);
    }
    else
    {
        CAL_ERROR_LOG ("Failed to parse timezone");
    }

    CAL_ERROR_LOG("returns '%s'", location.c_str());
    return location;
};
