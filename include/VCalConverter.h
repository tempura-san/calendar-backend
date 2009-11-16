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

#ifndef __VCALCONVERTER_H__
#define __VCALCONVERTER_H__

#include "Common.h"
#include "ICalConverter.h"
#include <vector>
#include <string>
using namespace std;
extern "C" {
#include <libical/ical.h>
#include <libical/icalcomponent.h>
#include <libical/icalenums.h>
#include "stdlib.h"
#include "stdio.h"
}

using std::string;
/**
 * VCalConverter
 * @brief converts local database format to vcal data and vice versa
 */

class VCalConverter
{
    public:
        /**
         * constructor
         */
        VCalConverter();
        /**
         * destructor
         */
        ~VCalConverter();

        /**
         * @param szContents string
         * @return FileType (ICAL/VCAL)
         *
         * This function returns the format of the given contents as ICAL/VCAL.
         */
        FileType checkContentsType(string szContents);
        
        /**
         * @param vcal string
         * @return string
         *
         * This function is used to convert vcal recurrence format
         * to ical recurence format
         */
        string vCalToICalRule(string vcal);
        
        /**
         * @param ical string
         * @param string
         *
         * This function is used to convert ical recurrence rule to
         * vcal recurrence rule format
         */
        string iCalToVCalRule(string ical, time_t startTime);

        /**
         * @param vcal string
         * @return string
         *
         * This function is used to convert vcal recurrence date format
         * to ical recurence date format
         */
        string vCalToICalDate(string vcal);
        
        /**
         * @param ical string
         * @return string
         *
         * This function is used to convert ical recurrence date to
         * vcal recurrence date format
         */
        string iCalToVCalDate(string ical);
        /**
         * @param vcalAlarm string
         * @return string
         *
         * This function is used to convert vcal alarm to
         * ical alarm format
         */
        string vCalToICalAlarm(string vCalAlarm);
        /**
         * @param icalAlarm string
         * @return string
         *
         * This function is used to convert ical alarm to
         * vcal alarm format
         */
        string iCalToVCalAlarm(string iCalAlarm);
        /**
         * @param str string
         * @param delimiter string
         * @return vector<string>
         *
         */
        vector<string> getTokens(string str, string delimiter);
        
        /**
         * @param szContents string
         * @return string
         *
         * This function returns the mapping for transparency from VCAL format to local 
         * calendar database format.
         */
        string getTranspFromVcal(string szContents);
        
        /**
         * @param szContents string
         * @return icaltimetype
         *
         * This function returns the mapping for created time from VCAL format to local 
         * calendar database format.
         */
        icaltimetype getCreatedTimeFromVcal(string szContents);
        
        /**
         * @param szContents string
         * @return int
         *
         * This function returns the mapping for status from VCAL format to local 
         * calendar database format.
         */
        int getStatusFromVcal(string szContents);
        /**
         * @param szTransp string 
         * @return icalproperty object
         *
         * This function returns the mapping for Transparency from local databse format to 
         * VCAL format.
         */
        icalproperty* getVcalTransp(string szTransp);
        /**
         * @param iStatus statusParam
         * @return string
         *
         * This function returns the mapping for statusParam from local databse format to 
         * VCAL format.
         */
        string getVcalStatus(statusParam iStatus);
        /**
         * @param szAtten string
         * @return int
         *
         * This function returns the mapping for ParticipationRole from VCAL format to local 
         * calendar database format.
         */
        int getParticipationRoleFromVcal(string szAtten);
        /**
         * @param iRole int
         * @return string
         *
         * This function returns the mapping for ParticipationRole from local databse format to 
         * VCAL format.
         */
        string getVcalPartRole(int iRole);
        /**
         * @param szTime string
         * @param szContents string
         * @return string
         *
         * This function returns the mapping for created time from local databse format to 
         * VCAL format.
         */
        string getVcalCreatedTime(string szTime,string szContents);
        /**
         * @param fRSVP bool
         * @return icalparameter
         *
         * This function returns the mapping for RSVP from local databse format to 
         * VCAL format.
         */
        icalparameter* getVcalRSVP(bool fRSVP);
        /**
         * @param szContents string
         * @return int
         *
         * This function returns the mapping for RSVP from VCAL format to local 
         * calendar database format.
         */
        bool getRSVPFromVcal(string szContents);
        
    private:
	/**
	 * Tokenize
	 * @param string to be tokenize
	 * @param list of tokens
	 * @param delimiters
	 * @return None
	 *
	 * This function is used to get tokens from given string
	 */
	void Tokenize(const string & str, 
			vector <string> &tokens,
		         string delimiters = " ");
	/**
	 * @param str string
         * @param delimiters string
         * @param repStr string
         * @return string
         * 
         * This function returns a string by removing the given delimiters in the given string.
         */
        string getNoDelimiterString(string str, string delimiters, string repStr);
            
        /**
         * @param item RecurrenceItem object
         * @return void
         *
         */
        void initRecurrenceItem(RecurrenceItem* item);
        
        /**
         * @param vcal string
         * @param opFlag bool
         * @return int
         * 
         * This function returns the Day from the given vcal string
         */
        int getDay(string vcal, bool opFlag);
    
        /**
         * @param vcal string
         * @return string 
         * 
         * This function returns ByMonth from the given vcal string.
         */
        string getByMonthFromVcal(string vcal);
        
        /**
         * @param vcal string
         * @return string 
         * 
         * This function returns ByDay from the given vcal string.
         */
        string getByDayFromVcal(string vcal);
        
        /**
         * @param vcal string
         * @return string 
         * 
         * This function returns Duration from the given vcal string.
         */
        string getDurationFromVcal(string vcal);
        
        /**
         * @param tokens vector<string>
         * @param index int
         * @return string
         *
         * This function returns the frequency from the given vector of string.
         */
        string getFrequency(vector<string> tokens, int index);
        
        /**
         * @param tokens vector<string>
         * @param index int
         * @return string
         *
         * This function returns the interval from the given vector of string.
         */
        string getInterval(vector<string> tokens, int index);
        
        /**
         * @param tokens vector<string>
         * @param index int
         * @return string
         *
         * This function returns the count from the given vector of string.
         */
        string getCount(vector<string> tokens, int index);
        
        /**
         * @param tokens vector<string>
         * @param index int
         * @return string
         *
         * This function returns the until from the given vector of string.
         */
        string getUntil(vector<string> tokens, int index);
        
        /**
         * @param tokens vector<string>
         * @param index int
         * @return string
         *
         * This function returns the ByDay from the given vector of string.
         */
        string getByDayFromIcal(vector<string> tokens, int index);
        
        /**
         * @param tokens vector<string>
         * @param index int
         * @return string
         *
         * This function returns the ByMonth from the given vector of string.
         */
        string getByMonthFromIcal(vector<string> tokens, int index);

        /**
         * @param strIcalComp string
         * @return time_t
         *
         * This function returns gmt time from the given string.
         */
        time_t tzStringTogmtime(string strIcalComp);
        
        /**
         * @param tTime time_t
         * @param onlyDate bool
         * @return string
         *
         * This function returns ignt time in string format.
         */
        string gmttimeToTZString(time_t tTime, bool onlyDate);
        
        /**
         * @param tokens vector<string>
         * @param index int
         * @return string
         *
         * This function returns the Vcal Date from the given vector of string.
         */
        string getVCalDate(vector<string> tokens, int index, int type);

        /**
         * @param alarm string
         * @return string
         *
         * This function returns Audio Alarm from the given string.
         */
        string getAudioAlarm(string alarm);
};
#endif
