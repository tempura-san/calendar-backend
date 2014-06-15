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

#ifndef __ICALCONVERTER_H__
#define __ICALCONVERTER_H__
using namespace std;
extern "C" {
#include <libical/ical.h>
#include <libical/icalcomponent.h>
#include <libical/icalenums.h>
#include "stdlib.h"
#include "stdio.h"
}
#include "Common.h"
#include "CParameters.h"
typedef enum {
    ICAL_TYPE = 0,
    VCAL_TYPE,
    UNKNOWN_TYPE
}FileType;
/**
 * ICalConverter
 * @brief converts local database format to ical data and vice versa
 */
class ICalConverter {

      public:
       /**
        * @param pEntry CComponent object
        * @param iType FileType ( ICAL/VCAL/UNKNOWN )  
        * @param pErrorCode Errorcode for the function
        * @return string ical format string 
        *
        * This function takes CComponent object as parameter
        * converts this into ical/vcal format and
        * returns vcalendar string
        */
       string localToIcalVcal(CComponent *pEntry,FileType iType,int &pErrorCode);

       /**
        * @param bool indication whethere syncing 
        * is turned on or off
        * @return void
        */
       void setSyncing(bool sync);
       /** 
        * Function to calculate effective DST offset */
       
       void findEffectiveDSToffset(string szLine,string &szDaylight);
       /**
        * @param strIcalComp ical string to be converted
        * @param iType FileType ( ICAL/VCAL/UNKNOWN )
        * @param pErrorCode Errorcode for the function
        * @return CComponent object pointer
        *
        * This function takes string as parameter
        * converts the ical/vcal formatted string into local calendar database entry
        * returns the CComponent object
        */
       CComponent *toLocal(string strIcalComp,FileType iType,int &pErrorCode);
       /**
        * constructor
        */
       ICalConverter();
       /**
        * destructor
        */
       ~ICalConverter();

       /**
        * @param szContents
        * @return string
        *
        * This function appends ctrl+M to each line as required for pc suite sync
        */
       string appendControlM(string szContents);

       /**
        * @param string
        * 
        * This function prints all the member variable values.
        */
       string toString();
       /**
        * @param strIcalComp ical string to be converted
        * @param iType FileType ( ICAL/VCAL/UNKNOWN )
        * @param pErrorCode Errorcode for the function
        * @return vector <CComponent*> 
        *
        * This function takes string as parameter
        * converts the ical/vcal formatted string into local calendar database entries
        * and returns them as vector of CComponent object
        */
       vector < CComponent* > icalVcalToLocal(string strIcalComp,FileType iType,int &pErrorCode);
       /**
        * @param szContents string
        * @return string
        * 
        * This function is used to strip the BEGIN and END of VCALENDAR component 
        * in the given contents.This is used by icalVcalToLocal function.
        */
       string strip(string szContents);
       /**
        * @param szContents string
        * @param szBegin begin string of icalcomponent
        * @param szEnd end string of icalcomponent
        * @param iCount integer reference Count number of instance of 
        *        entry(event/todo/journal) in ics file
        * @return bool
        *
        * This function is used to validate the string passed to icalVcalToLocal function
        * by counting the number of BEGIN and END tags of the components.
        * This is used by icalVcalToLocal function.
        */
       bool checkCount(string szContents,string szBegin,string szEnd,unsigned int& iCount);
       /**
        * @param szContents string
        * @return string
        *
        * This function is used to validate the string passed to icalVcalToLocal function
        * by removing trailing spaces and ctrl+M in the contents.This is
        * used by icalVcalToLocal function.
        */
       string validateContents(string szContents);
       /**
        * @param ical_time icaltimetype structure
        * @return integer 
        * 
        * This function returns an int value for the given icaltimetype
        * structure. 
        */    
       int getIntFromIcaltimetype(struct icaltimetype ical_time);
       /**
        * @param string Value of property
        * @param decoding type - 0 for quoted printable
        * Function to decode the text fields in ICS 
        */
       char* decodeParameterTextFields(string encodedString, 
                icalparameter_encoding  encodingType);

       /* gives the current time zone
        * Has to be freed by the API user  
        */
       char* getCurrentTimeZone();
       /**
        * Function to parse the VCAL timezone field 
        */
       void fillvCalTimeZone(string &szContents);
       string exportTimeZone(time_t dtstart,string szZone,FileType iType);
       string calculateMonthdayFromRule(time_t dtstart,struct icaltimetype &dtstartStd,string ruleStd,string szZone);
      private:
      /**
       * Find the daylight start time from daylight string 
       * from daylight start time we figure out the 
       * Repeat pattern for daylight   
       * 
       **/
      void findZoneStartTime(string &szRepeatPattern, int correction);
       /**
        * Function to find the localtion from time offset of GMT 
        * and daylight information 
        * we find the nearest possible match for city name from the 
        * timezone configuration table stored in the DB 
        *  
        */
       void findLocationFromOffset(string szDaylight);
      /**
       * Private function to limit the Date Range 
       * for event coming from sync 
       */
       void limitDateRange(struct icaltimetype &s_time,bool isExport);

       /**
        * Add encoded content
        */
       void replaceWithEncodedString(string &szIcalComp,
                          CComponentDetails *pEntry);
       /**
        * Function to convert attach  
        */
       void convertAttach(string &szAttach);
       /**
        * function to get impending alarm 
        */
       void getImpendingAlarm (string szRrule,time_t & triggerTime, time_t dateStart, int alarmFlag);
       /**
        * @param  string the input string value
        * @return  float 
        *
        * This function is used to convert string to float.
        */
       float stringToFloat(string szValue);
       /**
        * @param strIcalComp string
        * @param strVCalAlarmDate string
        * @param int Type of Component
        * @return string
        *
        * This function converts alarm date to string format.
        */
       string addAlarmDateToString(string strIcalComp, string strVCalAlarmDate,
     			  entryType iType);
       /**
        * @param strIcalComp string
        * @param rrules vector <string>
        * @param flag int
        * @return string
        *
        * Utility function used for parsing recurrence rules
        */
       string addRuleToString(string strIcalComp, vector < string > rrules,
     			  int flag);
       /**
        * getIcalAlarm()
        * @param cAlarm CAlarm object
        * @param sZone timezone of the event.
        * @param bool allday/task -allday /task.
        * @return string
        *
        * Function to get the Alarm string from Alarm component
        */
       string getIcalAlarm(CAlarm * cAlarm,string sZone,bool isAllday = false);
       /**
        * @param  szSpaceString 
        * @return NONE 
        * 
        * Function to strip all the white spaces 
        */
       void deleteWhiteSpaces(string &szSpaceString );
       /**
        * @param strIcalComp string 
        * @param pComp icalcomponent object
        * @param kind icalproperty_kind
        * @param iType FileType (ICAL/VCAL/UNKNOWN)
        * @return vector <CRecurrenceRule*>
        *
        * This function is used to get vector of recurrencerule  objects from 
        * the given strIcalComp.
        */
       vector<CRecurrenceRule*> getRecurrence(string strIcalComp, icalcomponent* pComp, icalproperty_kind kind,FileType iFType);

       /** 
        * @param strIcalComp string
        * @param index position of the rule
        * @param strRuleType rule type (RRULE/EXRULE)
        * @param iType FileType (ICAL/VCAL/UNKNOWN)
        * @return CRecurrenceRule object
        *
        * This function is used to create recurrence rule from the 
        * given strIcalComp.
        */
       CRecurrenceRule* getRecurrenceRule(string strIcalComp, int index, string strRuleType,FileType iFType);

       /**
        * @param strIcalComp string
        * @param pComp icalcomponent object
        * @param CEvent* Pointer to Event.
        * @param kind icalproperty_kind kind of property (ICAL_RDATE_PROPERTY/ICAL_EXDATEPROPERTY)
        * @return vector <string> list of dates
        *
        * This function is used to get list of dates (RDATE/EXDATE)
        * from given strIcalComp.
        */
       vector<string> getRecurrenceDates(string strIcalComp, icalcomponent *pComp,
     	 CEvent* pEvent, icalproperty_kind kind,int type=ICAL_TYPE);



       /**
        * @param strIcalComp string 
        * @param pComp icalcomponent object 
        * @param Entry pointer can be event or task
        * @param diffTime difference in times 
        * @return CAlarm object
        *
        * This function is used to get the CAlarm object from 
        * given strIcalComp.
        */
       template<class T> CAlarm* getAlarm(string strIcalComp, icalcomponent  *pComp,
     			  T* pEntry);


       /**
        * @param time_ical icaltimetype structure
        * @return tm structure
        *
        * This function takes icaltimetype structure and coverts it into tm structure.
        */
       struct tm initTime(icaltimetype time_ical);

       /**
        * @param strIcal Ical data in string format
        * @param start_loc Staring position for search
        * @return  string 
        *
        * This function returns string from given start_loc 
        * to a newline char in the given contents.
        */
       string get_line(string strIcal, int start_loc);

       /**
        * @param strInVcal string 
        * @param strType string 
        * @param index int
        * @return string
        *
        */
       string get_line_in_ical(string strInVcal, string strType, int index);

       /**
        * @param iTransp icalproperty_transp
        * @return string 
        *
        * This function returns the mapping for transparency from ICAL format to local 
        * calendar database format.
        */
       string getTranspFromIcal(icalproperty_transp iTransp);

       /**
        * @param pProp icalproperty object
        * @return COrganizer object
        *
        * This function converts the ICAL format  organizer to COrganizer object.
        */    
       COrganizer* getOrganizerFromIcal(icalproperty *pProp);

       /**
        * @param pProp icalproperty object
        * @param FileType iType (ICAL/VCAL/UNKNOWN)
        * @return CAttendee object
        *          
        * This function converts the ICAL format attendee to CAttendee object.
        */    
       CAttendee* getAttendeeFromIcal(icalproperty *pProp,FileType iType);

       /**
        * @param icalRole icalparameter_role 
        * @return int
        *
        * This function returns the mapping for Participation Role from ICAL format to local 
        * calendar database format.
        */    
       int getParticipationRoleFromIcal(icalparameter_role icalRole);

       /**
        * @param icalPartStat icalparameter_partstat
        * @return int
        *
        * This function returns the mapping for Participation Status from ICAL format to local 
        * calendar database format.
        */    
       int getParticipationStatusFromIcal(icalparameter_partstat icalPartStat);

       /**
        * @param icalRSVP icalparameter_rsvp 
        * @return bool
        *
        * This function returns the mapping for RSVP from ICAL format to local 
        * calendar database format.
        */    
       bool getRSVPFromIcal(icalparameter_rsvp icalRSVP);

       /**
        * @param icalCUType icalparameter_cutype 
        * @return int
        *
        * This function returns the mapping for CUType from ICAL format to local 
        * calendar database format.
        */    
       int getCUTypeFromIcal(icalparameter_cutype icalCUType);

       /**
        * @param iClass icalproperty_class 
        * @return string
        *
        * This function returns the mapping for Class from ICAL format to local 
        * calendar database format.
        */    
       string getClasFromIcal(icalproperty_class iClass);

       /**
        * @param pOrg COrganizer object
        * @return icalproperty object
        *
        * This function returns the ical organizer property from the given COrganizer object.
        */
       icalproperty* getIcalOrganizer(COrganizer* pOrg);

       /**
        * @param pProp icalproperty object
        * @return string
        *
        * This function returns contact from ICAL format to local database format.
        */
       string getContactFromIcal(icalproperty* pProp);

       /**
        * @param RSVP bool
        * @return icalparameter_rsvp
        *
        * This function returns the mapping for RSVP from local databse format to 
        * ICAL format.
        */
       icalparameter_rsvp getIcalRSVP(bool fRSVP);

       /**
        * @param iPartStat ParticipantStatus
        * @return icalparameter_partstat
        * 
        * This function returns the mapping for ParticipantStatus from local databse format to 
        * ICAL format.
        */
       icalparameter_partstat getIcalPartStatus(ParticipantStatus iPartStat);

       /**
        * @param iRole ParticipantRole
        * @return icalparameter_role
        * 
        * This function returns the mapping for ParticipantRole from local databse format to 
        * ICAL format.
        */
       icalparameter_role getIcalPartRole(ParticipantRole iRole);

       /**
        * @param iCUType CalendarUserType
        * @return icalparameter_cutype
        * 
        * This function returns the mapping for CalendarUserType from local databse format to 
        * ICAL format.
        */
       icalparameter_cutype getIcalCalendarUserType(CalendarUserType iCUType);

       /**
        * @param pAtten CAttendee object
        * @return icalproperty object
        *
        * This function returns the ical attendee property from the given CAttendee object.
        */
       icalproperty* getIcalAttendee_ical(CAttendee* pAtten);
       
       icalproperty* getIcalAttendee_vcal(CAttendee* pAtten);

       /**
        * @param pProp icalproperty object
        * @return CProperties object
        *
        * This function returns CProperties object for fiven ICAL/VCAL X property.
        */ 
       CProperties* getXProperty(icalproperty *pProp);

       /**
        * @param pParam icalparameter object
        * @return CParameters object
        *
        * This function returns CParameters object for fiven ical parameter.
        */ 
       CParameters* getParameters(icalparameter *pParam);

       /**
        * @param pParam icalparameter object
        * @return vector <CParameters*> 
        *
        * This function returns Relationship param  from ICAL format to local database format.
        */
       vector <CParameters*> getRelationshipParamFromICal(icalparameter* pParam);

       /**
        * @param icalStatus icalproperty_status
        * @return int
        *
        * This function returns Status from ICAL format to local database format.
        */
       int getStatusFromIcal(icalproperty_status icalStatus);

       /**
        * @param pParam icalparameter object
        * @return CParameters object
        *
        * This function returns language param from ICAL format to local database format.
        */
       CParameters* getLanguageFromIcal(icalparameter* pParam);

       /**
        * @param pParam icalparameter object
        * @return CParameters object
        *
        * This function returns altrep param from ICAL format to local database format.
        */
       CParameters* getAlterFromIcal(icalparameter* pParam);

       /**
        * @param pProp icalproperty object
        * @return vector <CParameters> object
        *
        * This function returns language and altrep param from ICAL format to local database format.
        */
       vector < CParameters* > getLangAlterParam(icalproperty* pProp);

       /**
        * @param iStatus statusparam
        * @return int
        * 
        * This function returns the mapping for status from local databse format to 
        * ICAL format.
        */
       int getIcalStatus(statusParam iStatus);
       /**
        * @param iRel RelationshipType
        * @return icalparameter_reltype
        * 
        * This function returns the mapping for RelationshipType from local databse format to 
        * ICAL format.
        */
       icalparameter_reltype getIcalRelType(RelationshipType iRel);
       /**
        * @param pParam icalparameter object
        * @return CParameters object
        *
        * This function returns FMType param from ICAL format to local database format.
        */
       CParameters* getFMTypeFromIcal(icalparameter* pParam);
       /**
        * @return icalparameter object
        *
        * This function returns the local timezone.
        */
       icalparameter* getTZIDParam();
       
       string getTimeInHHMM(int offset);


       time_t getTimeDiff(time_t diffTime );
       void exportEventProperties (icalcomponent *pEntComp, CEvent *pEvent, FileType iType);
       void exportTodoProperties (icalcomponent *pEntComp, CTodo *pTodo, FileType iType);
       void exportJournalProperties (icalcomponent *pEntComp, CJournal *pJournal, FileType iType);

       template<class T> void exportCreatedTimeFromLocal(icalcomponent *pEntComp, 
     			  T *pComp, FileType iType);
       template<class T> void exportDueFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportDateStampFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportSummaryFromLocal(icalcomponent *pEntcomp, T *pComp, FileType iType);
       template<class T> void exportLocationFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportDescriptionFromLocal(icalcomponent *pEntcomp,  T *pComp, FileType iType);
       template<class T> void exportUidFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportDateStartFromLocal(icalcomponent *pEntcomp, T *pComp,FileType iType);
       template<class T> void exportAttendeesFromLocal(icalcomponent *pEntcomp, T *pComp, FileType iType);
       template<class T> void exportAttachmentsFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportPriorityFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportSequenceFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportCommentsFromLocal(icalcomponent *pEntcomp, T *pComp, FileType iType);
       template<class T> void exportUrlFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportCategoriesFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportGeoFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportClassFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportRelatedFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportOrganizerFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportXPropFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportResourcesFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> void exportContactsFromLocal(icalcomponent *pEntcomp, T *pComp);
       template<class T> string exportAlarmFromLocal(T *pComp, FileType iType);
       void exportEventDateEndFromLocal(icalcomponent *pEntcomp, CEvent *pEvent,FileType iType);
       void exportEventTranspFromLocal(icalcomponent *pEntcomp, CEvent *pEvent, FileType iType);
       void exportTodoPercentCompleteFromLocal(icalcomponent *pEntcomp, CTodo *pTodo);
       void exportTodoCompletedFromLocal(icalcomponent *pEntcomp, CTodo *pTodo);

       void exportEntryStatusFromLocal(icalcomponent *pEntcomp, CComponent *pEntry,
     			  FileType iType);
       void exportEntryLastModifiedFromLocal(icalcomponent *pEntcomp, CComponent *pEntry);
       template<class T> struct icaltimetype importCreatedTime(icalcomponent *pComp,
     			  T *tComp, FileType iType, string strIcalComp);
       template<class T> void importDateStamp(icalcomponent *pComp, T *tComp, struct icaltimetype c_time);
       template<class T> void importOrganizer(icalcomponent *pComp, T *tComp);
       template<class T> void importAttendees(icalcomponent *pComp, T *tComp, FileType iType);
       template<class T> void importSequence(icalcomponent *pComp, T *tComp);
       template<class T> void importPriority(icalcomponent *pComp, T *tComp);
       template<class T> void importUrl(icalcomponent *pComp, T *tComp);
       template<class T> void importGeo(icalcomponent *pComp, T *tComp);
       template<class T> void importClass(icalcomponent *pComp, T *tComp);
       template<class T> vector <CParameters *> importResources(icalcomponent *pComp, T *tComp);
       template<class T> vector <CParameters *> importCategories(icalcomponent *pComp, T *tComp);
       template<class T> vector <CParameters *> importContacts(icalcomponent *pComp, T *tComp);
       template<class T> vector <CParameters *> importRelated(icalcomponent *pComp, T *tComp);
       template<class T> vector <CParameters *> importLocation(icalcomponent *pComp, T *tComp);
       template<class T> vector <CParameters *> importDescription(icalcomponent *pComp, T *tComp);
       template<class T> vector <CParameters *> importSummary(icalcomponent *pComp, T *tComp);
       template<class T> vector <CParameters *> importComments(icalcomponent *pComp, T *tComp);
       template<class T> void importDateStart(icalcomponent *pComp, T *tComp);
       bool importEventDateStartAndDateEnd(icalcomponent *pComp, CEvent *pEvent,FileType iType,int& pErrorCode);
       bool importEventDateEnd(icalcomponent *pComp, CEvent *pEvent,FileType iType);
       void importEventXProp(icalcomponent *pComp, CEvent *pEvent);
       void importEventTransp(icalcomponent *pComp, CEvent *pEvent, FileType iType, string strIcalComp);
       void importEventAttachements(icalcomponent *pComp, CEvent *pEvent);
       void importEventAlarm(icalcomponent *pComp, CEvent *pEvent, icalcomponent *pAComp, 
     			  string strIcalComp, FileType iType);
       void importEventRecurrence(icalcomponent *pComp, CEvent *pEvent, string strIcalComp, FileType iType, int &pErrorCode);

       void importTodoDue(icalcomponent *pComp, CTodo *pTodo);
       void importTodoPercentCompleted(icalcomponent *pComp, CTodo *pTodo);
       void importTodoCompleted(icalcomponent *pComp, CTodo *pTodo);
       void importTodoAlarm(icalcomponent *pComp, CTodo *pTodo, icalcomponent *pAComp, 
     			  string strIcalComp, FileType iType);
       void importJournalAttach(icalcomponent *pComp, CJournal *pJournal);

       void importComponentStatus(icalcomponent *pComp, CComponent *pComponent, FileType iType, string strIcalComp);
       void importComponentLastModified(icalcomponent *pComp, CComponent *pComponent);

       void parseTimeZone(string &szZone);
      private:
       bool bSyncing;
};
#endif
