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

#ifndef __CSMARTCALENDAR_H__
#define __CSMARTCALENDAR_H__

/* Headers */
#include <string>
#include <vector>
#include "CComponent.h"

using namespace std;
using std::string;
using std::vector;

/**
 * CBdayEvent class
 * @brief This class contain's the API's to get/set the birthday related parameters
 * Inherited from CComponent class.
 */

class CBdayEvent: public CComponent
{

public:

    /* Constructor */
    CBdayEvent();

    /* Parameterized constructor */
    CBdayEvent(string szUId, string szname, int idate);

    /* Destructor */
    ~CBdayEvent();

    /**
     * setEbookUid
     * Function to set the Ebook uid fetched from AB
     *
     * @param string Ebook uid
     *
     * @return TRUE/FALSE
     */
    bool setEBookUid(string szEbookUid);

    /**
     * getEbookUid
     * Function to get the Ebook UID
     *
     * @param none
     *
     * @return string Ebook uid
     */
    string getEBookUid();

    /**
     * setFirstName
     * Function used to set the first name of birthday contact
     *
     * @param string First name
     *
     * @return TRUE/FALSE
     */
    bool setFirstName(string szName);

    /**
     * getFirstName
     * Function used to get the first name
     *
     * @param none
     *
     * @return string First name
     */
    string getFirstName();

    /**
     * setBirthDate
     * Function used to set the birthdate.
     *
     * @param int Birthdate
     *
     * @return TRUE/FALSE
     */
    bool setBirthDate(int idate);

    /**
     * getBirthDate
     * Function used to get the birthdate
     *
     * @param none
     *
     * @return int Birthdate
     */
    int getBirthDate();

    /**
     * toString
     */
    string toString();


};

#endif
