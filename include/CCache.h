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

#ifndef __CCACHE_H__
#define __CCACHE_H__

#include <string>
#include <vector>

using namespace std;
using std::vector;
class CCache
{

public:

    /**
     * CCache()
     * This is the defult constructor for CComponent class  */
    CCache();


    /**
     * overloaded Constructor
     */
    CCache(time_t iStartDate, vector <string> vList);


    /**
     * Destructor for CCache class.
     */
    virtual ~CCache();

    /**
     * @param None
     * @return: vector containing Id strings
     *
     * This function gives the list of Event/ToDo Ids
     * pertaining to a certain date
     */
    vector <string> getCacheIds();

    /**
     * @param None
     * @return: time_t of start date
     *
     * This function gives the start date of Event/ToDo
     */
    time_t getCacheStartDate();


    /**
     * @param time_t is the date string
     * @return: None
     *
     * This function is used to set start date
     */
    void setCacheStartDate(time_t startDate);

    /**
     * @param vector of strings containing the Ids
     * return none
     *
     * This function is used to set Ids
     */
    void setCacheIds(vector <string> vList);

    /**
     * function used to get the Ids in
     * comma seperated string
     */
    string getCommaSeperatedIdString();

private:

    time_t iStartDate;
    vector <string> vListId;

};

#endif
