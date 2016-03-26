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
 * This library is distributed in the hope that it is useful, but
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

#ifndef __CPARTICIPANT__H_
#define __CPARTICIPANT__H_

#include <string>
using std::string;

/**
* @brief This class encapsulates PARTICIPANT values as defined per RFC5545.
* @see http://tools.ietf.org/html/rfc5545
* @todo The setters return always \c true regardless of the input. This could be improved
*       by checking the input for validity according to the RFC.
 */
class CParticipant
{

public:

    /**
     * @brief This function is used to set the LANGUAGE value of PARTICIPANT.
     * @param szLanguage The LANGUAGE value to set.
     * @return \c true if the passed value was valid or empty, \c false otherwise.
     */
    bool setLanguage(string szLanguage);

    /**
     * @brief This function is used to set the SENT-BY value of PARTICIPANT.
     * @param szBy The SENT-BY value to set.
     * @return \c true if the passed value was valid or empty, \c false otherwise.
     */
    bool setSentBy(string szBy);

    /**
     * @brief This function is used to set the CN value of PARTICIPANT.
     * @param szCommonName The CN value to set.
     * @return \c true if the passed value was valid or empty, \c false otherwise.
     */
    bool setCommonName(string szCommonName);

    /**
     * @brief This function is used to set the X-PARAMs of PARTICIPANT.
     * @param szXParameters The X-PARAMs value to set.
     * @return \c true if the passed value was valid or empty, \c false otherwise.
     */
    bool setXparameters(string szXParameters);

    /**
     * @brief This function is used to set the CAL-ADDRESS value of PARTICIPANT.
     * @param szCalAddress The CAL-ADDRESS value to set.
     * @return \c true if the passed value was valid or empty, \c false otherwise.
     */
    bool setCalAddress(string szCalAddress);

    /**
     * @brief This function is used to set the DIR value of PARTICIPANT.
     * @param szDirParam The DIR name value to set.
     * @return \c true if the passed value was valid or empty, \c false otherwise.
     */
    bool setDirectoryParameter(string szDirParam);

    /**
     * @brief This function is used to get the DIR value of PARTICIPANT.
     * @return The DIR value.
     */
    string getDirectoryParameter();

    /**
     * @brief This function is used to get the CAL-ADDRESS value of PARTICIPANT.
     * @return The CAL-ADDRESS value.
     */
    string getCalAddress();

    /**
     * @brief This function is used to get the X-parameters of PARTICIPANT.
     * @return The X-parameters value.
     */
    string getXparameters();

    /**
     * @brief This function is used to get the CN value of PARTICIPANT.
     * @return The CN value.
     */
    string getCommonName();

    /**
     * @brief This function is used to get the SENT-BY value of PARTICIPANT.
     * @return The SENT-BY value.
     */
    string getSentBy();

    /**
     * @brief This function is used to get the LANGUAGE value of PARTICIPANT.
     * @return The LANGUAGE value.
     */
    string getLanguage();

    /**
     * @brief This function is used to set the MAIL-TO value of PARTICIPANT.
     * @param szMailTo The MAIL-TO value to set.
     */
    void setMailTo(string szMailTo);

    /**
     * @brief This function is used to get the MAIL-TO value of PARTICIPANT.
     * @return The MAIL-TO value.
     */
    string getMailTo();

    /**
     * @brief Parameterised constructor for CParticipant.
     * @param szSentBy The SENT-BY value to set.
     * @param szCommonName The CN value to set.
     * @param szDirectoryParameter The DIR value to set.
     * @param szLanguage The LANGUAGE value to set.
     * @param szCalAddress The CAL-ADDRESS value to set.
     * @param szCXparameter The X-PARAMs value to set.
     * @param szMailTo The MAIL-TO value to set.
     */
    CParticipant(string szSentBy, string szCommonName, string szDirectoryParameter,
                 string szLanguage, string szCalAddress, string szCXparameter,
                 string szMailTo);

    /**
     * @brief Default constructor for CParticipant.
     */
    CParticipant();

    /**
     * @brief Copy Constructor for CParticipant.
     * @param Cparef The source instance to copy.
     */
    CParticipant(CParticipant &Cparef);

    /**
     * @brief The overloaded assignment operator.
     * @param right The right-hand instance to assign.
     */
    const CParticipant &operator=(const CParticipant &right);

    /**
     * @brief The destructor for CParticipant.
     */
    virtual ~ CParticipant();

    /**
     * @brief A function to convert the object to its string representation.
     * @return The string representation of the object.
     */
    string toString();


private:

    string szSentBy; /*!< The SENT-BY value. */
    string szCommonName; /*!< The CN value. */
    string szDirectoryParameter; /*!< The DIR value. */
    string szLanguage; /*!< The LANGUAGE value. */
    string szCalAddress; /*!< The CAL-ADDRESS value. */
    string szCXparameter; /*!< The X-PARAM value. */
    string szMailTo ; /*!< The MAIL-TO value. */
};

#endif
