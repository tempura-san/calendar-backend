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

#include "VCalConverter.h"
#include <sstream>
#include "CAlarm.h"
#include "Common.h"

/**
 * consttructor 
 */

VCalConverter::VCalConverter()
{

}

/**
 * destructor 
 */
VCalConverter::~VCalConverter()
{

}

FileType VCalConverter::checkContentsType(string szContents)
{
    unsigned int loc = 0;
    /*checking for DCREATED */
    if(( szContents.find(DCREATED, 0) != string::npos)||
       (szContents.find(RSVP_YES, 0) != string::npos) ||
       (szContents.find(RSVP_NO, 0)  != string::npos) || 
       (szContents.find(PARTSTAT_SENT, 0) != string::npos) ||
       (szContents.find(PARTSTAT_CONFIRMED, 0)  != string::npos) ||
       (szContents.find(STATUS_SENT, 0) != string::npos) ||
       (szContents.find(STATUS_DECLINED, 0) != string::npos) ||
       (szContents.find(STATUS_DELEGATED, 0) != string::npos) ||
       (szContents.find(STATUS_ACCEPTED, 0) != string::npos) ||
       (szContents.find(EXPECT, 0) != string::npos) ||
       (szContents.find(TRANSP_ZERO, 0) != string::npos) ||
       (szContents.find(TRANSP_ONE, 0) != string::npos) ||
       (szContents.find(A_ALARM, 0) != string::npos) ||
       (szContents.find(PALARM, 0) != string::npos) ||
       (szContents.find(DALARM, 0) != string::npos) ||
       (szContents.find("TZ:", 0) != string::npos) ||
       (szContents.find(TRANSP_GT_ONE, 0) != string::npos) )
	    return VCAL_TYPE;
    
       
    /* FIXME:
     * a) will the presence of whitespaces in the content cause
     * the parse/searching to fail
     * b) We can have one string variable for each of D,M,Y,W and combine
     * them into one if + || statement to make the below code concise.
     */
    /*checking for recurrence */
    string rrule = RRULE_COL;
    rrule = rrule + D;
    
    loc = szContents.find(rrule, 0);
    if (loc != string::npos)
    return VCAL_TYPE;
    
    rrule = RRULE_COL;
    rrule = rrule + Y;
    loc = szContents.find(rrule, 0);
    if (loc != string::npos)
    return VCAL_TYPE;
    
    rrule = RRULE_COL;
    rrule = rrule + M;
    loc = szContents.find(rrule, 0);
    if (loc != string::npos)
    return VCAL_TYPE;
    
    rrule = RRULE_COL;
    rrule = rrule + W;
    loc = szContents.find(rrule, 0);
    if (loc != string::npos)
    return VCAL_TYPE;
    
    rrule = RRULE_COL;
    rrule = rrule + P;
    loc = szContents.find(rrule, 0);
    if (loc != string::npos)
    return VCAL_TYPE;
    

    return ICAL_TYPE;
}

string VCalConverter::getTranspFromVcal(string szContents)
{
    unsigned int loc;

    /* FIXME: We should write a parser that uses ':' as the delimiter
     * instead of doing string.find(). This will not only make the
     * code concise but also improve performance.
     */
    loc = szContents.find(TRANSP_ZERO, 0);
    if (loc != string::npos)
    return OPAQUE;

    loc = szContents.find(TRANSP_ONE, 0);
    if (loc != string::npos)
    return TRANSPERANCY;

    loc = szContents.find(TRANSP_GT_ONE, 0);
    if (loc != string::npos)
    return TRANSPERANCY;

    return OPAQUE;

}

icaltimetype VCalConverter::getCreatedTimeFromVcal(string szContents)
{

    unsigned int loc;
    std::istringstream iss(szContents);
    string szLine;
    string time;
    icaltimetype iCTime;
    memset (&iCTime, 0, sizeof (struct icaltimetype));

    while (getline(iss, szLine)) {
	loc = szLine.find(DCREATED, 0);
	if (loc != string::npos) {
	    time = szLine.substr(loc + strlen(DCREATED), szLine.length());
	    CAL_DEBUG_LOG("The time is %s\n", time.c_str());
	    iCTime = icaltime_from_string(time.c_str());
	    break;
	}
    }
    return iCTime;
}


int VCalConverter::getStatusFromVcal(string szContents)
{
    unsigned int loc;

    /* FIXME: Use ':' based parser */
    loc = szContents.find(STATUS_SENT, 0);
    if (loc != string::npos)
	return SENT_STATUS;

    loc = szContents.find(STATUS_DECLINED, 0);
    if (loc != string::npos)
	return DECLINED_STATUS;

    loc = szContents.find(STATUS_DELEGATED, 0);
    if (loc != string::npos)
	return DELEGATED_STATUS;

    loc = szContents.find(STATUS_ACCEPTED, 0);
    if (loc != string::npos)
	return ACCEPTED_STATUS;

    loc = szContents.find(STATUS_NEEDS_ACTION, 0);
    if (loc != string::npos)
	return NEEDSACTION_STATUS;

    loc = szContents.find(STATUS_CONFIRMED, 0);
    if (loc != string::npos)
	return CONFIRMED_STATUS;

    loc = szContents.find(STATUS_TENTATIVE, 0);
    if (loc != string::npos)
	return TENTATIVE_STATUS;

    loc = szContents.find(STATUS_COMPLETED, 0);
    if (loc != string::npos)
	return COMPLETED_STATUS;

    return NEEDSACTION_STATUS;
}

int VCalConverter::getParticipationRoleFromVcal(string szContents)
{
    unsigned int loc;

    /* FIXME: Use ':' based parser */
    loc = szContents.find(ROLE_ATTENDEE, 0);
    if (loc != string::npos)
    return ATTENDEE_ROLE;

    loc = szContents.find(ROLE_ORGANIZER, 0);
    if (loc != string::npos)
    return ORGANIZER_ROLE;

    loc = szContents.find(ROLE_OWNER, 0);
    if (loc != string::npos)
    return OWNER_ROLE;

    loc = szContents.find(ROLE_DELEGATE, 0);
    if (loc != string::npos)
    return DELEGATE_ROLE;

    return ATTENDEE_ROLE;
}

string VCalConverter::getVcalPartRole(int iRole)
{
    string  szVcalRole(ROLE_ATTENDEE);
    
    switch (iRole) {
        case ATTENDEE_ROLE:
            szVcalRole = ROLE_ATTENDEE;
	    break;
        case ORGANIZER_ROLE:
            szVcalRole = ROLE_ORGANIZER;
	    break;
        case OWNER_ROLE:
            szVcalRole = ROLE_OWNER;
	    break;
        case DELEGATE_ROLE:
            szVcalRole = ROLE_DELEGATE;
	    break;
        default:
	    break;
    }
    return szVcalRole;
}


string VCalConverter::getVcalStatus(statusParam iStatus)
{
    string szStatus;

    switch (iStatus) {
        case TENTATIVE_STATUS:
            szStatus = STATUS_TENTATIVE;
	    break;
        case CONFIRMED_STATUS:
            szStatus = STATUS_CONFIRMED;
	    break;
        case COMPLETED_STATUS:
            szStatus = STATUS_COMPLETED;
	    break;
        case NEEDSACTION_STATUS:
            szStatus =  STATUS_NEEDS_ACTION;
	    break;
        case ACCEPTED_STATUS:
            szStatus =  STATUS_ACCEPTED;
	    break;
        case DELEGATED_STATUS:
            szStatus = STATUS_DELEGATED;
	    break;
        case DECLINED_STATUS:
            szStatus = STATUS_DECLINED;
	    break;
        case SENT_STATUS:
            szStatus = STATUS_SENT;
	    break;
        default:
            szStatus =  STATUS_NEEDS_ACTION;
	    break;
    }
    return szStatus;
}

icalproperty *VCalConverter::getVcalTransp(string szTransp)
{
    icalproperty *pProp = 0;

    if (szTransp == TRANSPERANCY)
	pProp = icalproperty_new_from_string(TRANSP_ONE);

    if (szTransp == OPAQUE)
	pProp = icalproperty_new_from_string(TRANSP_ZERO);

    return pProp;
}

icalparameter *VCalConverter::getVcalRSVP(bool fRSVP)
{
    icalparameter *pParam = 0;

    if (fRSVP)
	pParam = icalparameter_new_from_string(RSVP_YES);
    else
	pParam = icalparameter_new_from_string(RSVP_NO);

    return pParam;
}

bool VCalConverter::getRSVPFromVcal(string szContents)
{
    unsigned int loc;

    /* FIXME: Use ':' based parser */
    loc = szContents.find(RSVP_YES, 0);
    if (loc != string::npos)
    return 1;

    loc = szContents.find(RSVP_NO, 0);
    if (loc != string::npos)
    return 0;
    return 0;

}

string VCalConverter::getVcalCreatedTime(string szTime, string szContents)
{
    unsigned int loc;
    string szBefore;
    string szAfter;
    string szRet;
    
    /* FIXME: If 'END:V' is correct, the define a constant for it */
    loc = szContents.find("END:V", 0);
    if (loc != string::npos) {
	szBefore = szContents.substr(0, loc);
	szAfter = szContents.substr(loc, szContents.length());
	szRet = szBefore + NEWLINE + DCREATED + szTime + NEWLINE + szAfter;
    }
    return szRet;
}


/**
 * Tokenize
 * @param string to be tokenize
 * @param list of tokens
 * @param delimiters
 * @return None
 *
 * This function is used to get tokens from given string
 */
void VCalConverter::Tokenize (const string & str,
				vector <string> &tokens,
				string delimiters)
{
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos) {
	tokens.push_back(str.substr(lastPos, pos - lastPos));
	lastPos = str.find_first_not_of(delimiters, pos);
	pos = str.find_first_of(delimiters, lastPos);
    }
}

string::size_type getNextToken (const string parsedString,
				const string delimiters,
				string::size_type position,
				string &token,
				string &delimiter)
{
	string::size_type next_pos;

	/* FIXME: we should define an empty string and use it here */
	token = "";
	next_pos = parsedString.find_first_of (delimiters, position);

	if (next_pos != string::npos) {
	    token = parsedString.substr (position, next_pos - position);
	    delimiter = parsedString.substr (next_pos, 1);
	    return next_pos + 1;
	} 
	token = parsedString.substr (position);
	delimiter = "";

	return string::npos;
}


/**
 * getTokens
 * @param string
 * @param delimiters
 * @return list of tokens
 *
 * This function is public function used to get
 * tokens for given string
 */
vector <string> VCalConverter::getTokens(string str, string delimiter)
{
    vector <string> tokens;
    Tokenize(str, tokens, delimiter);
    return tokens;
}

/**
 * initRecurrence
 * @param recurrence item
 * @return None
 *
 * It initializes the recurrence item
 */
void VCalConverter::initRecurrenceItem(RecurrenceItem * item)
{
    item->freq = RecursSecondly;
    item->interval = 0;
    item->modifier = 0;
    item->duration = 0;
}

/**
 * getDay
 * @param vcal string
 * @param opFlag, indicates 1+ / 1-
 * @return day value
 *
 * This function is used to get the day
 */

int VCalConverter::getDay(string vcal, bool opFlag)
{
    unsigned int found = -1;
    int day = 0;
   
    /* FIXME: Use getTokens() and conditional operator */ 
    if ((found = vcal.find(MON_DAY)) != string::npos) {
	day |= Monday;
	if (opFlag == true)
	    return day;
    }
    if ((found = vcal.find(TUES_DAY)) != string::npos) {
	day |= Tuesday;
	if (opFlag == true)
	    return day;
    }
    if ((found = vcal.find(WEDNES_DAY)) != string::npos) {
	day |= Wednesday;
	if (opFlag == true)
	    return day;
    }
    if ((found = vcal.find(THURS_DAY)) != string::npos) {
	day |= Thursday;
	if (opFlag == true)
	    return day;
    }
    if ((found = vcal.find(FRI_DAY)) != string::npos) {
	day |= Friday;
	if (opFlag == true)
	    return day;
    }
    if ((found = vcal.find(SATUR_DAY)) != string::npos) {
	day |= Saturday;
	if (opFlag == true)
	    return day;
    }
    if ((found = vcal.find(SUN_DAY)) != string::npos) {
	day |= Sunday;
	if (opFlag == true)
	    return day;
    }
    return day;
}

/**
 * getByMonthFromVcal
 * @param vcal string
 * @return BYMONTH ical modifier
 *
 * This function is used to get BYMONTH modifier for recurrence
 */
string VCalConverter::getByMonthFromVcal(string vcal)
{
    string modifier = string();
    vector <string> tokens;
    Tokenize(vcal, tokens);
    bool start = true;
    unsigned int index = 0;
    unsigned int found = -1;
   
    /* FIXME: following if combinations have to be re-written */ 
    index = tokens.size() - 1;
    for (unsigned int i = 1; i < index; i++) {
	if (start != true)
	    modifier.append(COMMA);
	if ((found = ((tokens.at(i)).find(PLUS))) != string::npos ||
	    ((found = ((tokens.at(i)).find_first_of(HYPHEN, 0))) !=
	     string::npos)) {
	    if ((tokens.at(i)).find(PLUS) == string::npos) {
	    modifier.append((tokens.at(i)).substr(index, 1));
	    }
	}
	modifier.append((tokens.at(i)).substr(0, index));
	start = false;
    }
    return modifier;
}

/**
 * getByDayFromVcal
 * @param vcal string
 * @return BYDAY modifier
 *
 * This function is used to get BYDAY modifier for recurrence
 */
string VCalConverter::getByDayFromVcal(string vcal)
{
    string modifier = string();
    unsigned int found = -1;
    unsigned int day = 0;
    bool flag = false;
    bool start = true;

    /* FIXME: Use switch case instead of if (inner) */
    /* FIXME: Make MON_DAY to MONDAY, etc */
    
//if (!((found = vcal.find(string(PLUS))) || (found = vcal.find(string(HYPHEN))))) {

    
    if (!(((found = vcal.find(string(PLUS))) != string::npos)
      || ((found = vcal.find(string(HYPHEN))) != string::npos))) {
	day = getDay(vcal, flag);
	if (day & Monday) {
	    modifier.append(MON_DAY);
	    start = false;
	}
	if (day & Tuesday) {
	    if (start != true)
	    modifier.append(COMMA);
	    modifier.append(TUES_DAY);
	    start = false;
	}
	if (day & Wednesday) {
	    if (start != true)
	    modifier.append(COMMA);
	    modifier.append(WEDNES_DAY);
	    start = false;
	}
	if (day & Thursday) {
	    if (start != true)
	    modifier.append(COMMA);
	    modifier.append(THURS_DAY);
	    start = false;
	}
	if (day & Friday) {
	    if (start != true)
	    modifier.append(COMMA);
	    modifier.append(FRI_DAY);
	    start = false;
	}
	if (day & Saturday) {
	    if (start != true)
	    modifier.append(COMMA);
	    modifier.append(SATUR_DAY);
	    start = false;
	}
	if (day & Sunday) {
	    if (start != true)
	    modifier.append(COMMA);
	    modifier.append(SUN_DAY);
	    start = false;
	}
    } else
	//    if (!(found = vcal.find(string(PLUS))) || (found = vcal.find(string(HYPHEN)))) {
	    if (!((found = vcal.find(string(PLUS))) == string::npos) ||
	       ((found = vcal.find(string(HYPHEN))) == string::npos)) {

		vector < string > tokens;
		Tokenize(vcal, tokens);

		/* FIXME: the following if conditions have to be broken down */
		unsigned int k = 1;
		found = -1;
		for (int i = 1; (unsigned) i < tokens.size() - 1; i++) {
		    if ((found = ((tokens.at(i)).find(PLUS))) != string::npos ||
			((found = ((tokens.at(i)).find_first_of(HYPHEN, 0))) !=
			 string::npos)) {
			found = -1;
			if (start != true)
			    modifier.append(COMMA);
			if ((tokens.at(i)).find(PLUS) == string::npos)
			    modifier.append((tokens.at(i)).substr(1, 1));
			modifier.append((tokens.at(i)).substr(0, 1));
			if (((((tokens.at(i + 1)).find(PLUS))) == string::npos) &&
			    ((((tokens.at(i + 1)).find(HYPHEN))) == string::npos))
			    modifier.append(tokens.at(++i));
			else {
			    string weekday = string();
			    for (k = i + 1; k < tokens.size() - 1; k++) {
				if (((((tokens.at(k)).find(PLUS))) == string::npos)
				    && ((((tokens.at(k)).find(HYPHEN))) ==
				    string::npos)) {
				    weekday = tokens.at(k);
				    break;
				}
			    }
			    modifier.append(weekday);
			}
			start = false;

		    } else
			if ((found = ((tokens.at(i)).find_first_of(HASH, 0))) ==
			   string::npos) {

			    if (start != true)
				modifier.append(COMMA);
			    modifier.append((tokens.at(i)));
			    start = false;
			}
		}

	    } else { /*FIXME: remove this empty else */
	    }
    
    return modifier;
}

/**
 * getDurationFromVcal
 * @param vcal string
 * @return duration in string format
 *
 * This function is used to get duration for recurrence
 */
string VCalConverter::getDurationFromVcal(string vcal)
{
    vector <string> tokens;
    string          duration;
    string          count;

    CAL_DEBUG_LOG ("*** vcal '%s'", vcal.c_str());

    Tokenize(vcal, tokens);
    if (tokens.size() < 1)
        return duration;

    unsigned int index = tokens.size() - 1;

    CAL_DEBUG_LOG("tokens at index is %s", tokens.at(index).c_str());
    if ((tokens.at(index)).find(HASH) != string::npos ||
	(tokens.at(index)).find(" ") != string::npos) {
	    count = 
		(tokens.at(index)).substr(1, (tokens.at(index)).size() - 1);
	    if (atoi((char *) count.c_str()) != 0) {
		duration.append(COUNT_VCAL);
		duration.append(count);
	    }
    } else {
        duration.append(UNTIL_VCAL);
        duration.append(tokens.at(index));
    }

    CAL_DEBUG_LOG("DURATION returned = %s\n", duration.c_str());
    return duration;
}

/**
 * vCalToICal
 * @param vcal string
 * @return ical string
 *
 * This function is used to convert vcal recurrence format
 * to ical recurence format
 */
string VCalConverter::vCalToICalRule(string vcal)
{
    RecurrenceItem *item = new RecurrenceItem;
    char *token = new char[vcal.size() + 1];
    int index = 0;
    RecurrenceType recType = RecursSecondly;

    memset(item,0,sizeof(RecurrenceItem));
    memset(token,0,vcal.size()+1);
    initRecurrenceItem(item);
    ASSERTION(token);
    ASSERTION(item);
    
    sprintf(token, "%s", vcal.c_str());
    CAL_DEBUG_LOG("token is %s\n", token);

    /* getting freq of recurrence */
    /* FIXME: Replace the if-else with switch-case. use substr() */
    if (token[0] == 'D') {
	recType = RecursDaily;
	index++;
    } else if (token[0] == 'W') {
	recType = RecursWeekly;
	index++;
    } else if (token[0] == 'M' && token[1] == 'D') {
	recType = RecursMonthly;
	index += 2;
    } else if (token[0] == 'M' && token[1] == 'P') {
	recType = RecursMonthNth;
	index += 2;
    } else if (token[0] == 'Y' && token[1] == 'M') {
	recType = RecursYearly;
	index += 2;
    }
    item->freq = recType;
    delete[]token;
    /* getting interval of recurrence rule */
    CAL_DEBUG_LOG("vcal is %s\n", vcal.c_str());

    int interval_pos = vcal.find(" ", index);
    CAL_DEBUG_LOG("pos is %d\n", interval_pos);

    if (interval_pos - index != 0) {
	string sInterval = vcal.substr(index, interval_pos - index);
	item->interval =
	    new char[(sInterval.size())+1];
	ASSERTION ( item->interval);
	memset(item->interval,0,(sInterval.size())+1);
	/* Make the default interval to 1 in case interval value is  
	 * out of range (0 to SHRT_MAX) */
	if (atoi(sInterval.c_str()) <= 0 ||
			atoi(sInterval.c_str()) > SHRT_MAX )
		sprintf(item->interval, "%s","1");
	else
		sprintf(item->interval, "%s",
				(char *) (sInterval. c_str()));
	CAL_DEBUG_LOG("item =  %s\n", item->interval);
    }
    /* getting count/until */
    string duration = getDurationFromVcal(vcal);
    if (duration.empty() == 0) {
	item->duration = new char[duration.size()+1];
	ASSERTION ( item->duration);
	sprintf(item->duration, "%s", (char *) duration.c_str());
    }

    /* FIXME: following can be moved to another function */
    string ical;
    string modifier;
    switch (item->freq) {
        case RecursDaily:        /* D */
            ical.append(FREQ_DAILY);
            if (item->interval) {
                ical.append(";INTERVAL="); /* FIXME: Remove hardcoding */
                ical.append(string(item->interval));
            }
            if (item->duration)
                ical.append(string(item->duration));
            break;
        case RecursWeekly:        /* W */
            /* getting modifier of recurrence rule */
            modifier = getByDayFromVcal(vcal);
            if (modifier.length() != 0) {
                item->modifier = new char[modifier.length() + 1];
    		ASSERTION ( item->modifier);
		memset(item->modifier,0, modifier.length() + 1);
                sprintf(item->modifier, "%s", (char *) modifier.c_str());
            }
            ical.append(FREQ_WEEKLY);
            if (item->interval) {
                ical.append(INTERVAL_VCAL);
                ical.append(string(item->interval));
            }
            if (item->duration)
                ical.append(string(item->duration));
            if (item->modifier) {
                ical.append(WKSTRT_SU_VCAL);
                ical.append(BY_DAY_VCAL);
                ical.append(string(item->modifier));
            }
            break;
        case RecursMonthNth:    /* MP */
            /* getting modifier of recurrence rule */
            modifier = getByDayFromVcal(vcal);
            if (modifier.empty() == 0) {
                item->modifier = new char[modifier.size() + 1];
    		ASSERTION ( item->modifier);
		memset(item->modifier,0,modifier.size() + 1);
                sprintf(item->modifier, "%s", (char *) modifier.c_str());
            }
            ical.append(FREQ_MONTHLY);
            if (item->interval) {
                ical.append(INTERVAL_VCAL);
                ical.append(string(item->interval));
            }
            if (item->duration)
                ical.append(string(item->duration));
            if (item->modifier) {
                ical.append(WKSTRT_SU_VCAL);
                ical.append(BY_DAY_VCAL);
                ical.append(string(item->modifier));
            }
            break;
        case RecursMonthly:    /* MD */
            ical.append(FREQ_MONTHLY);
            if (item->interval) {
                ical.append(INTERVAL_VCAL);
                ical.append(string(item->interval));
            }
            if (item->duration)
                ical.append(string(item->duration));
            modifier = getByMonthFromVcal(vcal);
            if (modifier.empty() == 0) {
                ical.append(BY_MONTHDAY_VCAL);
                ical.append(modifier);
            }
            break;
        case RecursYearly:        /* YM */
            ical.append(FREQ_YEARLY);
            if (item->interval) {
                ical.append(INTERVAL_VCAL);
                ical.append(string(item->interval));
            }
            if (item->duration)
                ical.append(string(item->duration));
            modifier = getByMonthFromVcal(vcal);
            if (modifier.empty() == 0) {
                ical.append(BY_MONTH_VCAL);
                ical.append(modifier);
            }
            break;
        default:
            break;
    }
   /* freeing the recurrence item object i
    * and its contents */
    if (item->interval)
	    delete[] (item->interval);
    if (item->modifier)
	    delete[] (item->modifier);
    if (item->duration)
	    delete[] (item->duration);
    
    /* finally deleting item */
    delete item;
    
    CAL_DEBUG_LOG("string in Ical format = %s", ical.c_str());
    return ical;
}

/* FIXME: Following function is used only at one place in the
 * code and the functionality can be achieved using conditional
 * operator (and this function can be removed)
 */    
static const char * _getDayNamefromNumber (int dow)
{
    
    const char *names[] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA" };

    if (dow < 0 || dow > 6)
	    return "";
    
    return names[dow];
}

/**
 * iCalToVCal
 *
 * vcal:RRULE:FREQ=MONTHLY;INTERVAL=10;BYMONTHDAY=6,7,8;COUNT=10
 * ical:MD10 6 7 8 #10
 * RRULE FREQ MONTHLY INTERVAL 10 BYMONTHDAY 6 7 8 COUNT 10
 */
string VCalConverter::iCalToVCalRule (string ical, time_t startTime)
{
    string vcal ;
    struct tm *startTimeTm;
    char *retChar = 0;
    int               state = 0;
    string::size_type parsingAt = 0;
    string            token;
    string            delimiter;
    /* FIXME: replace harcoding with const */
    string            delimiters("=;, ");
    char     frequency = '\0';
    int      countValue = -1;
    int      intervalValue = 1;
    string   untilValue="#0";
    string   byMonthDayValue;
    string   byDayValue;
    string   byMonthValue;

    CAL_DEBUG_LOG ("*** incoming    = '%s'", ical.c_str());
    do {
        parsingAt = getNextToken (ical, delimiters, parsingAt, 
			token, delimiter);
        CAL_DEBUG_LOG ("*******************************");
        //CAL_DEBUG_LOG ("*** parsingAt    = %d", parsingAt);
        CAL_DEBUG_LOG ("*** token        = '%s'", token.c_str());
        //CAL_DEBUG_LOG ("*** delimiter    = '%s'", delimiter.c_str());
        CAL_DEBUG_LOG ("*** state        = %d", state);
	switch (state) {
	    case 0:
		/*
		 * The initial state to read some field.
		 */
		if (strcasecmp(token.c_str(), FREQUENCY) == 0)
		    state = 2;
		if (strcasecmp(token.c_str(), COUNT) == 0)
		    state = 3;
		if (strcasecmp(token.c_str(), BY_MONTHDAY) == 0)
		    state = 4;
		if (strcasecmp(token.c_str(), UNTIL) == 0)
		    state = 5;
		if (strcasecmp(token.c_str(), BY_DAY) == 0)
		    state = 6;
		if (strcasecmp(token.c_str(), BY_MONTH) == 0)
		    state = 7;
		if (strcasecmp(token.c_str(), INTERVAL) == 0)
		    state = 1;
		else {
		    CAL_DEBUG_LOG ("*** Unknown token: '%s'", token.c_str());
		}
		break;
	    
	    case 1:
		/*
		 * Reading the 'INTERVAL' value.
		 */
		intervalValue = atoi (token.c_str());
		state = 0;
		break;

	    case 2:
		/*
		 * The 'FREQ' token just came, we read the frequency.
		 */
	        if (strcasecmp(token.c_str(), DAILY) == 0)
		    frequency = 'D';
                else if (strcasecmp(token.c_str(), WEEKLY) == 0)
                    frequency = 'W';
                else if (strcasecmp(token.c_str(), MONTHLY) == 0)
                    frequency = 'M';
                else if (strcasecmp(token.c_str(), YEARLY) == 0)
                    frequency = 'Y';
		else {
		    CAL_DEBUG_LOG ("Unknown frequency: '%s'", token.c_str());
		}

		CAL_DEBUG_LOG ("*** frequency = '%c'", frequency);
		state = 0;
		break;


	    case 3:
	        /*
		 * Reading the 'COUNT' value.
		 */
		countValue = atoi (token.c_str());
		CAL_DEBUG_LOG ("*** countValue = %d", countValue);
		state = 0;
		break;

	    case 4:
		/*
		 * Reading the 'BYMONTHDAY' value. It is a list, so we
		 * have to stay in this state until the ';' delimiter 
		 * found.
		 */
		if (!byMonthDayValue.empty())
		    byMonthDayValue += " ";

		byMonthDayValue += token + "+";
		CAL_DEBUG_LOG ("*** byMonthDayValue = '%s'", byMonthDayValue.c_str());
		if (delimiter == ";")
		  state = 0;
		break;

	    case 5:
		/*
		 * Reading the 'UNTIL' value.
		 */
		untilValue = token;
		CAL_DEBUG_LOG ("*** untilValue = '%s'", untilValue.c_str());
		state = 0;
		break;

	    case 6:
		/*
		 * Reading the byday value. We have to stay in this state
		 * until the ';' is reached.
		 */
		if (!byDayValue.empty())
		  byDayValue += " ";

		byDayValue += token;
		if (token.c_str()[0] >= '0' && token.c_str()[0] <= '9') 
		  byDayValue += "+";
		
		CAL_DEBUG_LOG ("*** byDayValue = '%s'", byDayValue.c_str());
		if (delimiter == ";")
		  state = 0;
		break;
		
	    case 7:
		/*
		 * Reading the 'BYMONTH' value. We have to stay in this state
		 * until the ';' is reached.
		 */
		if (!byMonthValue.empty())
		  byMonthValue += " ";

		byMonthValue += token;
		if (delimiter == ";")
		  state = 0;
		break;


	} /* switch */
    } while (parsingAt != string::npos);


//    startTimeTm = gmtime (&startTime);
  /* we should be using the local time of the event instead of the 
   * GM time to see that the event has proper recurrence pattern 
   * when exported */
    startTimeTm =  localtime(&startTime);
   // time_get_local_ex(startTime , startTimeTm);
    
    CAL_DEBUG_LOG ("*** frequency  = '%c'", frequency);
    /* FIXME: move the following logic to a separate function */
    /* FIXME: remove hardcoding of the days, months, years, ...*/
    switch (frequency) {
	    case 'D':
		    if (countValue != -1)
		        asprintf (&retChar, "D%d #%d",
				    intervalValue,
				    countValue);
		    else 
		        asprintf (&retChar, "D%d %s",
				    intervalValue,
				    untilValue.c_str());
		    
		    vcal = retChar;
		    /* FIXME: why do we need 'goto finalize' here? use break instead */
		    goto finalize;

	    case 'M':
		    if (byMonthDayValue.empty()) {
			asprintf (&retChar, "%d+", startTimeTm->tm_mday);
			if(retChar) {
		        	byMonthDayValue = retChar;
				free(retChar);
				retChar = 0;
			}
			else {
			// low memory case
				CAL_DEBUG_LOG(" low memory");
				return vcal;
			}
		    }

		    if (countValue != -1)
		        asprintf (&retChar, "MD%d %s #%d",
				    intervalValue,
				    byMonthDayValue.c_str(),
				    countValue);
		    else 
		        asprintf (&retChar, "MD%d %s %s",
				    intervalValue,
				    byMonthDayValue.c_str(),
				    untilValue.c_str());

		    vcal = retChar;
		    goto finalize;

	    case 'W':
		    if (byDayValue.empty())
		        byDayValue = _getDayNamefromNumber(startTimeTm->tm_wday);

		    if (countValue != -1)
		        asprintf (&retChar, "W%d %s #%d",
				    intervalValue,
				    byDayValue.c_str(),
				    countValue);
		    else
		        asprintf (&retChar, "W%d %s %s",
				    intervalValue,
				    byDayValue.c_str(),
				    untilValue.c_str());

		    vcal = retChar;
		    goto finalize;

	    case 'Y':
		    if (byMonthValue.empty()) {
			asprintf (&retChar, "%d", startTimeTm->tm_mon + 1);
			if(retChar) {
				byMonthValue = retChar;
				free(retChar);
				retChar = 0;
			}
			else {
			// low memory case
				CAL_DEBUG_LOG(" low memory");
				return vcal;
			}
		    }

		    if (countValue != -1)
		        asprintf (&retChar, "YM%d %s #%d",
				    intervalValue,
				    byMonthValue.c_str(),
				    countValue);
		    else
		        asprintf (&retChar, "YM%d %s %s",
				    intervalValue,
				    byMonthValue.c_str(),
				    untilValue.c_str());

		    vcal = retChar;
		    goto finalize;
    }


finalize:
    if(retChar) {
    free(retChar);
    retChar  = 0;
    }
    CAL_DEBUG_LOG ("*** outgoing    = '%s'", vcal.c_str());
    return vcal;

}


/**
 * getVcalDate
 * @param tokens in ical
 * @param pos of date
 * @param type of date (i.e RDATE/EXDATE)
 * @return date in vcal format
 *
 * This function is used to getDATE in Vcal format
 */
string VCalConverter::getVCalDate(vector <string> tokens,
				    int index,
				    int type)
{
    bool start = true;
    int i = 0;
    string str = string();

    /* FIXME: replace hardcoded 24*60*60 with a const */
    if (type == 1) {
	int diff = 0;
	for (i = index; i < (int) tokens.size(); i += 2) {
	    diff += (tzStringTogmtime(tokens.at(i + 1)) -
		     tzStringTogmtime(tokens.at(i))) / (24 * 60 * 60);
	}
	if (diff <= 10 && diff != 0) {
	    for (i = index; i < (int) tokens.size(); i += 2) {
		int count = (tzStringTogmtime(tokens.at(i + 1)) -
			 tzStringTogmtime(tokens.at(i))) / (24 * 60 * 60);
		int date = tzStringTogmtime(tokens.at(i));
		int level = 1;
		while (count--) {
		    if (!start)
		    str.append(SEMI_COLON);
		    str.append(gmttimeToTZString
			   ((date + (24 * 60 * 60) * (level)), false));
		    level += 1;
		    start = false;
		}
	    }
	} else {
	    str.clear();
	}
    } else {
	if(type == 3)
	    index -= 1;
	for (i = index; i < (int) tokens.size(); i++) {
	    if (!start)
		str.append(SEMI_COLON);
	    str.append(tokens.at(i));
	    if (type == 2)
		str.append(TIMEZONE);
	    start = false;
	}
    }
    return str;
}

/**
 * getNoDelimiterString
 *
 * This function is used to get string with no delimiters
 */
string VCalConverter::getNoDelimiterString(string str, string delimiters,
                       string repStr)
{
    unsigned int found;
    found = str.find_first_of(delimiters);
    while (found != string::npos) {
	str[found] = repStr.c_str()[0];
	found = str.find_first_of(delimiters, found + 1);
    }
    return str;
}

/**
 * iCalToVcalDate
 * @param iCal Date
 * @return Vcal Date
 *
 * This function is used to convert ical date to vcal date
 */
string VCalConverter::iCalToVCalDate(string iCalDate)
{
    string vCalDate = string();
    string date = string();
    iCalDate = getNoDelimiterString(iCalDate, DATE_DELIMITER, " ");
    vector <string> tokens;
    Tokenize(iCalDate, tokens);

    if (tokens.size() > 2 && tokens.at(1).compare(TZID) == 0) {
    date.append(getVCalDate(tokens, 3, 0));
    } else if (tokens.size() > 3 && tokens.at(2).compare(PERIOD) == 0) {
    date.append(getVCalDate(tokens, 3, 1));
    } else if (tokens.size() > 3 && tokens.at(2).compare(DATE) == 0) {
    date.append(getVCalDate(tokens, 3, 2));
    } else {
    date.append(getVCalDate(tokens, 1, 3));
    }
    if (date.empty() == 0) {
    vCalDate.append(date);
    } else {
    vCalDate.clear();
    }
    return vCalDate;

}

/* FIXME: remove this function. its not used */
/**
 * vCalToICalDate
 */
string VCalConverter::vCalToICalDate(string vCalDate)
{
    return vCalDate;
}

/**
 * getAudioAlarm
 */
string VCalConverter::getAudioAlarm(string alarm)
{
    string strAlarm ;
    strAlarm.append(string(BEGIN_ALARM)).append(string("\n"));

    /* VCalendar Alarm format coming from the mobile is 
     * AALARM;TYPE=X-EPOCSOUND:20080509T184500;;0;z:\system\SystemSounds\alarm.wav
     *
     * we support only audio alarm 
     * AALARM ; TYPE=AUDIO: alarm trigger time; Snooze duration;Repeats;location of audio file 
     * AALARM;TYPE=WAVE;VALUE=CONTENT-ID:20090108T101000Z;PT7M; 
     */

    int initial = 0;
    int final = 0;
    initial = alarm.find(":");
    final = alarm.find(";", initial + 1);


    string trigger = alarm.substr(initial + 1, final - initial - 1);
    /* pC suite launch application  is sending AAlarm with 
     * 000000T000000Z in trigger portion , 
     * In such cases we need to ignore adding alarm 
     */
    string sTemp (SIX_T_SIX);
    if (trigger.find(sTemp.c_str(),0)!= string::npos)
	    return strAlarm;

    strAlarm = strAlarm + TRIGGER_VALUE_DATE_TIME + trigger +  "\n";

    /* FIXME: code is repeated. we can have a separate function for this */
    if ( final+ 1 < (int)alarm.size()){
	initial = final;
	final = alarm.find(";", initial + 1);
	string duration = alarm.substr(initial + 1, final - initial - 1);

	if (!duration.empty()) {
	    CAL_DEBUG_LOG("Duration is present \n");
	    strAlarm = strAlarm + DURATION_COL + duration + "\n";
	}
    }
    
    if ( final+ 1 < (int)alarm.size()){
	initial = final;
	final = alarm.find(";", initial + 1);
	string Repeats = alarm.substr(initial + 1, final - initial - 1);

	if (!Repeats.empty()) {
	    CAL_DEBUG_LOG("Repeats is present \n");
	    strAlarm = strAlarm + REPEAT_COL + Repeats + "\n";
	}
    }

    /* next field is path in which audio file is located */
    if ( final+ 1 < (int)alarm.size()){
	string path = alarm.substr(final + 1);
	if (!path.empty()) {
	    CAL_DEBUG_LOG("valid path exist\n");
	    strAlarm = strAlarm + ATTACH_ALARM + path + "\n";
	}
    }
    /* appending the common parameters */
    strAlarm.append(string(ACTION_AUDIO)).append(string("\n"));
    strAlarm.append(string(END_ALARM)).append(string("\n"));
    
    return strAlarm;
}

string VCalConverter::vCalToICalAlarm(string vCalAlarm)
{
    string iCalAlarm;

    if (vCalAlarm.find(A_ALARM) != string::npos) 
	iCalAlarm = getAudioAlarm(vCalAlarm);
    return iCalAlarm;
}

string VCalConverter::iCalToVCalAlarm(string iCalAlarm)
{
     /* we support only audio alarm 
     * AALARM ; TYPE=AUDIO: alarm trigger time; Snooze duration;
     * Repeats;location of audio file 
     * AALARM;TYPE=WAVE;VALUE=CONTENT-ID:20090108T101000Z;PT7M; 
     */
    string 		vCalAlarm(ALARM_TYPE_WAVE_VALUE);
    int 		sPos = string::npos;
    string 		szTemp;
    string 		szLine;
    std::istringstream  streamDescriptor(iCalAlarm);

    /* getting line by line and adding \n
     * in the end*/

    CAL_DEBUG_LOG(" ****** ICAL Alarm component is :  %s\n ", iCalAlarm.c_str());
    while (getline(streamDescriptor, szLine)) {
//         CAL_DEBUG_LOG("Line1: %s", szLine.c_str());

        // trim extra \r
        size_t pos = 0;
        while ((pos = szLine.find('\r', pos)) != std::string::npos)
        {
//             CAL_DEBUG_LOG("Remove \\r character at position %ld", (long)pos);
            szLine.erase(pos);
        }
//         CAL_DEBUG_LOG("Line2: %s", szLine.c_str());

        if ((sPos = szLine.find(TRIGGER_VALUE_DATE_TIME, 0)) != (int)string::npos) {
            szTemp = szLine.substr((sPos+ strlen(TRIGGER_VALUE_DATE_TIME)));
        }
        else if ((sPos = szLine.find(DURATION_COL, 0)) != (int)string::npos ) {
            szTemp+= SEMI_COLON;
            szTemp += szLine.substr((sPos + strlen(DURATION_COL)));
        } else if ((sPos = szLine.find(REPEAT_COL, 0)) != (int)string::npos){
            szTemp+= SEMI_COLON;
            szTemp += szLine.substr((sPos + strlen(REPEAT_COL)));
        } else
            CAL_DEBUG_LOG(" no  need to add this parameter alarm ");

        if (sPos != (int)string::npos)	
            vCalAlarm +=  szTemp;
        sPos = string::npos;
        szTemp.clear();
    } 
    CAL_DEBUG_LOG("Before returning:\n %s",vCalAlarm.c_str() );

    /* FIXME: remove the following commented code */
/******************* above piece of code is better organized ******************
       
    
    if (iCalAlarm.empty() == 0) {
	CAL_DEBUG_LOG(" ****** ICAL Alarm component is : **********\n  %s\n ", iCalAlarm.c_str());
	CAL_DEBUG_LOG("**********************************************" );
    if ((sPos = iCalAlarm.find(TRIGGER_VALUE_DATE_TIME, 0)) != string::npos) {
        if ((ePos = iCalAlarm.find("\n", sPos)) != string::npos) {
        length = sPos + string(TRIGGER_VALUE_DATE_TIME).size();
	CAL_DEBUG_LOG(" Trigger , length:  %d , endpos: %d ,spos: %d", length, ePos, sPos );
        vCalAlarm += iCalAlarm.substr(length, ePos - length);
        sPos = -1;
        ePos = -1;
        length = -1;
        }
    }
	CAL_DEBUG_LOG("Trigger Parsed :   -- %s",vCalAlarm.c_str() );
     
    if ((sPos = iCalAlarm.find(DURATION, 0)) != string::npos) {
        if ((ePos = iCalAlarm.find("\n", sPos)) != string::npos) {
        length = sPos + string(DURATION_COL).size();
	CAL_DEBUG_LOG(" Duration , length:  %d , endpos: %d ,spos: %d", length, ePos, sPos );
        vCalAlarm +=
             SEMI_COLON + iCalAlarm.substr(length,
                                  ePos - length ) ;
        sPos = -1;
        ePos = -1;
        length = -1;
        }
    }
	CAL_DEBUG_LOG(" Duration Parsed :  -- %s",vCalAlarm.c_str() );
    if ((sPos = iCalAlarm.find(REPEAT, 0)) != string::npos) {
        if ((ePos = iCalAlarm.find("\n", sPos)) != string::npos) {
        length = sPos + string(REPEAT_COL).size();
	CAL_DEBUG_LOG(" Repeat  , length:  %d , endpos: %d ,spos: %d", length, ePos, sPos );
        vCalAlarm +=
             SEMI_COLON + iCalAlarm.substr(length,
                                  ePos -
                                  length);
        sPos = -1;
        ePos = -1;
        length = -1;
        }
    }
    }
    ***************************************************************************************/
    return vCalAlarm;
}

/**
 *tzStringTogmtime
 * @param strIcalComp: Time in TZ string format
 * @return time in seconds since 1970 in time_t format
 * 
 * This function converts TZ string to time_t seconds
 **/
time_t VCalConverter::tzStringTogmtime(string strIcalComp)
{
    char inputTime[20];
    /* FIXME: should do memset for allocated memory; remove hardcoding */
    struct tm time ;
    memset (&time, 0, sizeof (struct tm));

    /* FIXME: Hardcoding should be removed */
    if (strIcalComp.size() < 8) {
	CAL_DEBUG_LOG( "Not in TZ Time String format");
	return -1; /* FIXME: incorrect return value */
    }
    sprintf(inputTime, strIcalComp.c_str());
    sscanf(inputTime, "%4d%2d%2d", &time.tm_year, &time.tm_mon,
       &time.tm_mday);

    if (strIcalComp.size() > 9 && strIcalComp.size() < 17) {
	sscanf(&inputTime[9], "%2d%2d%2d", &time.tm_hour, &time.tm_min,
           &time.tm_sec);
    }
    else {
	time.tm_hour = 0;
	time.tm_min = 0;
	time.tm_sec = 0;
    }

    time.tm_year = time.tm_year - NINETEEN_HUNDRED;
    time.tm_mon = time.tm_mon - ONE;
    return time_mktime(&time, NULL);
}

string VCalConverter::gmttimeToTZString(time_t tTime, bool onlyDate)
{
    struct tm tim;
    string strTZdate;
    char date[20]; /* FIXME: should do memset for allocated memory;
   			remove hardcoding */

    /* FIXME: Hardcoding should be removed */
    //tim = gmtime(&tTime);
    time_get_utc(&tim);
    sprintf(date, "%d%02d%02d", tim.tm_year + NINETEEN_HUNDRED,
        tim.tm_mon + 1, tim.tm_mday);
    if (!onlyDate) {
    sprintf(&date[8], "T%02d%02d%02dZ", tim.tm_hour, tim.tm_min,
        tim.tm_sec);
    }

    strTZdate = date;

    return strTZdate;
}
