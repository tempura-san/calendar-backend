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

#include <iostream>
#include "CAttendee.h"
#include "CalendarLog.h"
#include "Common.h"
/**
 * copy Constructor
 */
CAttendee::CAttendee(CAttendee &ref): CParticipant(ref)
{
    szMember = ref.szMember;
    iParticipantRole = ref.iParticipantRole;
    iParticipantStatus = ref.iParticipantStatus;
    bRsvp = ref.bRsvp;
    szDelegatees = ref.szDelegatees;
    szDelegators = ref.szDelegators;
    iCalendarUserType = ref.iCalendarUserType;
}

/**
 * overloaded assignment operator
 */
const CAttendee &CAttendee::operator=(const CAttendee &right)
{
    if(&right != this) {       //check for self assignment
        CParticipant::operator=(right);
        szMember = right.szMember;
        iParticipantRole = right.iParticipantRole;
        iParticipantStatus = right.iParticipantStatus;
        bRsvp = right.bRsvp;
        szDelegatees = right.szDelegatees;
        szDelegators = right.szDelegators;
        iCalendarUserType = right.iCalendarUserType;
    }

    return *this;
}


/**
 * default constructor
 *
 */
CAttendee::CAttendee(): CParticipant(), iParticipantRole(ROLE_NONE),
    iParticipantStatus(STATUS_NONE), bRsvp(false),
    iCalendarUserType(INDIVIDUAL)
{
}

/**
 * default destructor
 */
CAttendee::~CAttendee()
{
}


/**
 * setCalendarUserType
 *
 * @param icuType  type of calendar user is an enumerated type
 * @return bool
 *
 * This function will be used to set type of calendar user
 * the calendarUserType can be "INDIVIDUAL"       ; An individual
   / "GROUP"               ; A group of individuals
   / "RESOURCE"            ; A physical resource
   / "ROOM"                ; A room resource
   / "UNKNOWN"             ; Otherwise not known
   / x-name                ; Experimental type
   / iana-token)           ; Other IANA registered
 */
bool CAttendee::setCalendarUserType(CalendarUserType iCUType)
{
    this->iCalendarUserType = iCUType;
    return true;
}



/**
 * getCalendarUserType
 *
 * @param none
 * @return int type of calendar user
 *
 * This function will be used to get the type of calendar user
 * which has been previously set using setcalendarUserType().
*/
CalendarUserType CAttendee::getCalendarUserType()
{
    return iCalendarUserType;
}




/**
 * setMember
 *
 * @param string member name
 * @return bool
 *
 * This function will be used to set member name.
 * Member indicates the groups that attendee belongs to.
 * returns true when member name is set correctly.
 */
bool CAttendee::setMember(string szMember)
{
    this->szMember = szMember;
    return true;
}



/**
 * getMember
 *
 * @param none
 * @return string name of the member
 *
 * This function will be used to get the member
 * which has been previously set using setcalendarUserType().
 * member indicates the groups that attendee belongs to.
 */
string CAttendee::getMember()
{
    return szMember;
}




/**
 * setRole
 *
 * @param int iRole
 * @return bool
 *
 * This function will be used to set role that the attendee
 * will have in the calendar component.
 * returns true when  role is set correctly.
 */
bool CAttendee::setRole(ParticipantRole iRole)
{
    this->iParticipantRole = iRole;
    return true;
}




/**
 * getRole
 *
 * @param none
 * @return string role of attendee.
 *
 * This function will be used to get the role
 * which has been previously set using setRole().
 */
ParticipantRole CAttendee::getRole()
{
    return this->iParticipantRole;
}



/**
 * setParticipationStatus
 *
 * @param string status of the attendee's participation
 * @return bool
 *
 * This function will be used to set status of the
 * attendee's participation.
 * returns true when is status is  set correctly.
 */
bool CAttendee::setParticipationStatus(ParticipantStatus iStatus)
{
    this->iParticipantStatus = iStatus;
    return true;
}


/**
 * getParticipationStatus
 *
 * @param none
 * @return string status of the attendee's participation
 *
 * This function will be used to get status
 * of the attendee's participation
 * which has been previously set using setparticipationStatus().
 */
ParticipantStatus CAttendee::getParticipationStatus()
{
    return iParticipantStatus;
}




/**
 * setRSVP
 *
 * @param string indicating whether the favour of a reply is requested
 * @return bool rsvp
 *
 * This function will be used to set RSVP1
 * which indicates whether the favour of a reply is requested or not.
 * returns true when RSVP is  set correctly.
 */
bool CAttendee::setRSVP(bool brsvp)
{
    bRsvp = brsvp;
    return true;
}


/**
 * getRSVP
 *
 * @param none
 * @return bool  RSVP
 *
 * This function will be used to get RSVP
 * which has been previously set using setRSVP().
 */
bool CAttendee::getRSVP()
{
    return bRsvp;
}


/**
 * setDelagatees
 *
 * @param string list of Delegatees
 * @return bool
 *
 * This function will be used to set DELEGATED-TO
 * which indicates the calendar users that the orignal
 * request was delegated to.
 * returns true when Delegated to is  set correctly.
 */
bool CAttendee::setDelegatees(string szDelegatees)
{
    this->szDelegatees = szDelegatees;
    return true;
}

/**
 * getDelelegatees
 *
 * @param none
 * @return string users
 *
 * This function will be used to get calendar users
 * that the original request was delegated to
 * which has been previously set using setdelegatees().
 */
string CAttendee::getDelegatees()
{
    return szDelegatees;
}




/**
 * setDelegator
 *
 * @param string calendar user
 * @return bool
 *
 * This function will be used to set DELEGATED-FROM
 * which indicates whom the request was delegated from
 * returns true when Delegated from is  set correctly.
 */
bool CAttendee::setDelegator(string szDelegators)
{
    this->szDelegators = szDelegators;
    return true;
}



/**
 * getDelegator
 *
 * @param none
 * @return string user
 *
 * This function will be used to get DELEGATED-FROM
 * which indicates whom the request was delegated from
 * which has been previously set using setdelegators().
 */
string CAttendee::getDelegator()
{
    return szDelegators;
}


/* to string function to
    print the attendee propoerties
*/
string CAttendee::toString()
{
    string szRet;
    string szTemp;
    std::stringstream ss;

    ss << "SentBy=";

    if(getSentBy().c_str()) {
        szTemp = getSentBy().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",CommonName=";

    if(getCommonName().c_str()) {
        szTemp = getCommonName().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",Directory=";

    if(getDirectoryParameter().c_str()) {
        szTemp = getDirectoryParameter().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",Lang=";

    if(getLanguage().c_str()) {
        szTemp = getLanguage().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",CalAddr=";

    if(getCalAddress().c_str()) {
        szTemp = getCalAddress().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",CXparam=";

    if(getXparameters().c_str()) {
        szTemp = getXparameters().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",MailTo=";

    if(getMailTo().c_str()) {
        szTemp = getMailTo().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }


    ss << ",Member";

    if(szMember.c_str()) {
        szTemp = szMember.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",Delegatee";

    if(szDelegatees.c_str()) {
        szTemp = szDelegatees.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",Delagator";

    if(szDelegators.c_str()) {
        szTemp = szDelegators.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",Role=";
    ss << getRole();
    ss << ",Status=";
    ss << getParticipationStatus();
    ss << ",RSVP=";
    ss << getRSVP();
    ss << ",CalUserType=,";
    ss << getCalendarUserType();
    szRet.append(ss.str());

    return szRet;
}

