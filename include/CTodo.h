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

#ifndef __CTODO_H__
#define __CTODO_H__

/* Headers */
#include "CComponentDetails.h"

class CProperties;


/**
 * CTodo class
 * @brief This class contains the details of Todo component.
 * Inherited from CComponentDetails
 */
class CTodo: public CComponentDetails
{

public:
    /**
     * @param string szGeo  consisting folat variables (Latitude and Longitude)
     * @return bool
     *
     * This function will be used to set geo for a todo entry
     * The geo is combination of latitude and
     * longitude.ex: 120.98;45.64
     * returns true when geo is set correctly.
     * returns false when geo is empty.
     */
    bool setGeo(string szGeo);

    /**
     * @param none
     * @return string geo value in form of string (Latitude;longitude)
     *
     * This function will be used to extract geo for a todo entry
     * The geo is combination of latitude and
     * longitude.ex: 120.98;45.64
     * returns geo string

     * see setGeo().
     */
    string getGeo();

    /**
     * @param none
     * @return due time in int
     *
     * This function will be used to due time for a todo
     * returns due time

     * see setDue().
     */
    int getDue();

    /**
     * @param int due time
     * @return bool
     *
     * This function will be used to set due date for a todo entry
     * returns true when due date is set correctly.
     * returns false when due date is empty.

     * see getDue().
     */
    bool setDue(int iDue);

    /**
     * @param none
     * @return completed  in int
     *
     * This function will be used to completed time for a todo
     * returns completed time
     * see setCompleted().
    */
    int getCompleted();

    /**
     * @param int completed time
     * @return bool
     *
     * This function will be used to set completed date for a todo entry
     *
     * returns true when  date is set correctly.
     * returns false when  date is empty.
     *
     * see getCompleted().
     */
    bool setCompleted(int iCompleted);

    /**
     * @param integer  percentage of todo completed
     * @return bool
     * This function will be used to set percentage of todo completed for  a todo entry
     * returns true when percent  is set correctly.

     * see getPercentComplete().
     */
    bool setPercentComplete(int iPercentComplete);

    /**
     * @param none
     * @return int percentage of todo completed.
     * This function will be used to extract percent of todo completed for a todo entry
     * which has been previously set using setPercentComplete().
     */
    int getPercentComplete();


    /**
     * @param none
     * @return int priority of todo .
     * This function will be used to extract priority of todo
     * which has been previously set using setPriority().
     */
    int getPriority();

    /**
     * @param integer pripority
     * @return bool
     *
     * This function will be used to set priority of todo
     * returns true when priority  is set correctly.
     */
    bool setPriority(int iPriority);

    /**
     * copy Constructor
     */
    CTodo(CTodo &ref);

    /**
     * overloaded assignment operator
     */
    CTodo &operator=(CTodo &right);

    /* Constructors and paramterized constructors */
    CTodo();
    /* overloaded constructor */
    CTodo(string szSummary, int iDateStart, int iStatus);
    /* Destructors */
    ~CTodo();
    /**
     * toString
     */
    string toString();


private:

    /* Member variables are defined as per ical rfc */

    string szGeo; /*!< Geo parameter */

    int iDue; /*!< Todo due */

    int iCompleted; /*!< Todo completed */

    int iPercentComplete; /*!< Todo percent complete */

    int iPriority; /*!< Todo priority */
};

#endif
