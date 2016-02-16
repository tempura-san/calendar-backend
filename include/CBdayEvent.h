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

#ifndef __CBDAYEVENT_H__
#define __CBDAYEVENT_H__

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "CComponent.h"

/**
 * @brief A class to hold information about birthdays.
 *
 * This class contain's the API's to get/set the birthday related parameters.
 *
 * @note The class expects and returns dates as \c int instead of \c time_t.
 */
class CBdayEvent: public CComponent {
public:

    /**
     * @brief The default constructor.
     */
    CBdayEvent();

    /**
     * @brief The parameterised constructor.
     *
     * @param szUId The UID of the birthday event.
     * @param szname The person's name.
     * @param idate The date of the birthday.
     */
    CBdayEvent(string szUId, string szname, int idate);

    /**
     * @brief The default destructor.
     */
    ~CBdayEvent();

    /**
     * @brief Set the UID for this event.
     *
     * @param szEbookUid The UID to set.
     * @return \c true if the UID passed was valid, \c false otherwise.
     */
    bool setEBookUid(string szEbookUid);

    /**
     * @brief Get the UID for this event.
     *
     * @return The UID of this event.
     */
    string getEBookUid();

    /**
     * @brief Set the name for this event.
     *
     * @param szName The name to set.
     * @return \c true if the name passed was valid, \c false otherwise.
     */
    bool setFirstName(string szName);

    /**
     * @brief Get the name for this event.
     *
     * @return The name of this event.
     */
    string getFirstName();

    /**
     * @brief Set the date for this birthday event.
     *
     * @param idate The date to set.
     * @return \c true if the date passed was valid, \c false otherwise.
     */
    bool setBirthDate(int idate);

    /**
     * @brief Get the date of this birthday event.
     *
     * @return The date of this event.
     */
    int getBirthDate();

    /**
     * @brief Return a textual representation of this event.
     *
     * @return A \c string object describing this event.
     */
    string toString();
};

#endif
