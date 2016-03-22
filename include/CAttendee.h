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

#ifndef __CALATTENDEE__H_
#define __CALATTENDEE__H_

/**
 *  Headers */
#include "CParticipant.h"
#include <string>
using namespace::std;
using std::string;

/**
 *  Enums defined as per RFC 2445 (ical rfc) for ParticipantRole */
enum ParticipantRole {
    CHAIR = 0,
    REQ_PARTICIPANT,
    OPT_PARTICIPANT,
    NON_PARTICIPANT,
    ATTENDEE_ROLE,
    ORGANIZER_ROLE,
    DELEGATE_ROLE,
    OWNER_ROLE,
    OTHER_ROLE,
    ROLE_NONE
};
/**
 * supported values for Participation status*/
typedef enum {
    NEEDS_ACTION,
    ACCEPTED,
    DECLINED,
    TENTATIVE,
    DELEGATED,
    COMPLETED,
    IN_PROCESS,
    OTHER_STATUS,
    STATUS_NONE
} ParticipantStatus;
/**
 * enumeration for calendar user type*/
typedef enum {

    INDIVIDUAL,
    GROUP,
    RESOURCE,
    ROOM,
    UNKNOWN_USER,
    USER_NONE
} CalendarUserType;

/**
 * CAttendee
 * @brief This class contains the API's to store the attendee details in database
 */

class CAttendee: public CParticipant
{

public:

    /**
     * copy Constructor
     */
    CAttendee(CAttendee &ref);

    /**
     * CAttendee
     * default constructor
     * @param none
     * @return none
     */
    CAttendee();

    /**
     * ~CAttendee
     * default destructor
     * @param none
     * @return none
     */
    ~CAttendee();

    /*
     * overloaded assignment operator
     */
    const CAttendee &operator=(const CAttendee &right);

    /**
     * @param CalendarUserType  type of calendar user
     * @return bool
     *
     * This function will be used to set type of calendar user
     * the calendarUserType can be "INDIVIDUAL"       ; An individual
     * "GROUP"               ; A group of individuals
     * "RESOURCE"            ; A physical resource
     * "ROOM"                ; A room resource
     * "UNKNOWN"             ; Otherwise not known
     * x-name                ; Experimental type
     * iana-token)           ; Other IANA registered
     * Default is INDIVIDUAL
     */
    bool setCalendarUserType(CalendarUserType iCalendarUserType);

    /**
     * @param none
     * @return CalendarUserType type of calendar user
     *
     * This function will be used to get the type of calendar user
     * which has been previously set using setCalendarUserType().
     */
    CalendarUserType getCalendarUserType();

    /**
     * @param string szMember member name
     * @return bool
     *
     * This function will be used to set member name.
     * Member indicates the groups that attendee belongs to.
     * returns true when member name is set correctly.
     */
    bool setMember(string szMember);

    /**
     * @param none
     * @return string name of the member
     *
     * This function will be used to get the member
     * which has been previously set using setCalendarUserType().
     * member indicates the groups that attendee belongs to.
     */
    string getMember();

    /**
     * @param ParticipantRole iParticipantRole role of participant
     * @return bool
     *
     * This function will be used to set role that the attendee
     * will have in the calendar component.
     * returns true when  role is set correctly.
     */
    bool setRole(ParticipantRole iParticipantRole);

    /**
     * @param none
     * @return string role of attendee.
     *
     * This function will be used to get the role
     * which has been previously set using setRole().
     */
    ParticipantRole getRole();

    /**
     * @param ParticipantStatus iParticipantStatus status of the attendee's participation
     * @return bool
     *
     * This function will be used to set status of the
     * attendee's participation.
     * returns true when is status is  set correctly.
     */
    bool setParticipationStatus(ParticipantStatus iParticipantStatus);

    /**
     * @param none
     * @return ParticipantStatus status of the attendee's participation
     *
     * This function will be used to get status
     * of the attendee's participation
     * which has been previously set using setParticipationStatus().
     */
    ParticipantStatus getParticipationStatus();

    /**
     * @param bool brsvp indicating whether the favour of a reply is requested
     * @return bool
     *
     * This function will be used to set RSVP
     * which indicates whether the favour of a reply is requested or not.
     * returns true when RSVP is  set correctly.
     */
    bool setRSVP(bool bRsvp);

    /**
     * @param none
     * @return bool RSVP value
     *
     * This function will be used to get RSVP
     * which has been previously set using setRSVP().
     */
    bool getRSVP();

    /**
     * @param string calendar users
     * @return bool
     *
     * This function will be used to set DELEGATED-TO
     * which indicates the calendar users that the orignal
     * request was delegated to.
     * returns true when Delegated to is  set correctly.
     */
    bool setDelegatees(string szDelegatees);

    /**
     * @param none
     * @return string users
     *
     * This function will be used to get calendar users
     * that the original request was delegated to
     * which has been previously set using setDelegatees().
     */
    string getDelegatees();

    /**
     * @param string calendar user
     * @return bool
     *
     * This function will be used to set DELEGATED-FROM
     * which indicates whom the request was delegated from
     * returns true when Delegated from is  set correctly.
     */
    bool setDelegator(string szDelegator);

    /**
     * @param none
     * @return string user
     *
     * This function will be used to get DELEGATED-FROM
     * which indicates whom the request was delegated from
     * which has been previously set using setDelegators().
     */
    string getDelegator();
    /**
    * toString
     */
    string toString();



private:

    /* Parameters are defined as per ICAL rfc */

    string szMember; /*!< Attendee name */

    ParticipantRole iParticipantRole; /*!< Attendee role */

    ParticipantStatus iParticipantStatus; /*!< Attendee status */

    bool bRsvp;    /*!< RSVP parameter */

    string szDelegatees; /*!< Delegatees */

    string szDelegators; /*!< Delegators */

    CalendarUserType iCalendarUserType; /*!< User type */


};

#endif
