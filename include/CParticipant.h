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

#ifndef __CPARTICIPANT__H_
#define __CPARTICIPANT__H_

/* Headers */
#include <string>
using namespace::std;
using std::string;

/**
 * CParticipant class
 * @brief This class has get/set API's for various participant parameters
 */
class CParticipant {

  public:

    /**
     * @param string Language parameter
     * @return bool indicates whether operation is successful or not 
     *
     * This function will be used to set the language parameter of participant
     */
    bool setLanguage(string szLanguage);

    /**
     * @param string SentBy parameter
     * @return bool indicates whether operation is successful or not 
     *
     * This function will be used to set the sentby parameter of participant
     */
    bool setSentBy(string szBy);

    /**
     * @param string common-name parameter
     * @return bool indicates whether operation is successful or not 
     *
     * This function will be used to set the common-name parameter of participant
     */
    bool setCommonName(string szCommonName);

    /**
     * @param string szxParameters parameter
     * @return bool indicates whether operation is successful or not 
     *
     * This function will be used to set the x-parameter of participant
     */
    bool setXparameters(string szXParameters);

    /**
     * @param string cal-Address parameter
     * @return bool indicates whether operation is successful or not 
     *
     * This function will be used to set the Cal-Address parameter of participant
     */
    bool setCalAddress(string szCalAddress);

    /**
     * @param string directory name parameter
     * @return bool indicates whether operation is successful or not 
     *
     * This function will be used to set the directory parameter of participant
     */
    bool setDirectoryParameter(string szDirParam);

    /**
     * overloaded assignment operator
     */
    const CParticipant & operator=(const CParticipant & right);

    /**
     * @return string directory name parameter
     * 
     * This function will be used to get the directory parameter of participant
     */
    string getDirectoryParameter();

    /**
     * @param none
     * @return string cal-Address parameter
     * 
     * This function will be used to get the Cal-Address parameter of participant
     */
    string getCalAddress();

    /**
     * @param none
     * @return string X-parameter
     * 
     * This function will be used to get the X-parameter of participant
     */
    string getXparameters();

    /**
     * @param none
     * @return string CommonName
     * 
     * This function will be used to get the Common-name of participant
     */
    string getCommonName();

    /**
     * @param none
     * @return string sentBy 
     * 
     * This function will be used to get the sentby of participant
     */
    string getSentBy();

    /**
     *  @param none
     *  @return string Language parameter
     * 
     * This function will be used to get the language parameter of participant
     */
    string getLanguage();
    /**
     * @param string szMailTo
     * @return none 
     * Function used to set mailto parameter
     */
    void setMailTo(string szMailTo);
    /**
     * @param none
     * @return string szMailTo
     * Function to retrieve MailTo from the class
     */
    string getMailTo();
    
    /**
     *Parameterized  constructor for CParticipant
     */
    CParticipant(string szSentBy, string szCommonName, string szDirectoryParameter,
                 string szLanguage, string szCalAddress, string szCXparameter,
                 string szMailTo);
    /**
     * default constructor for CParticipant
     */
    CParticipant();

    /**
     * copy Constructor
     */
    CParticipant(CParticipant & Cparef);

    /**
     * destructor for CParticipant
     */
    virtual ~ CParticipant();
    /**
     * toString
     */
    string toString();
	

  private:

    /* Parameters are defined as per RFC */

    string szSentBy; /*!< SentBy parameter */

    string szCommonName; /*!< Common name */

    string szDirectoryParameter; /*!< Directory parameter */

    string szLanguage; /*!< Language paramter */

    string szCalAddress; /*!< Address */

    string szCXparameter; /*!< CXParameter */

    string szMailTo ;/* Mailto parameter for Organizer and Attendee*/

};


#endif
