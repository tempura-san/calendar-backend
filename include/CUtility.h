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

#ifndef __CUTILITY_H__
#define __CUTILITY_H__

#include <string>
#include <vector>

using std::string;
using std::vector;

extern "C" {
#include <libical/ical.h>
}

/**
 * @brief A class to contain some utility functions.
 *
 * @note This class is implemented as a Singleton. This is not needed, as there
 * is no internal state of the class. In order to not break the API, the Singleton
 * mechanism is retained but deprecated. Either all functions could be made
 * \c static, or \c CUtility could be turned into a namespace instead.
 */
class CUtility
{
public:
    /**
     * @brief Extracts the date from a given date/time value.
     *
     * @param startDate The \c time_t information to get the date part from.
     * @return The date part of the date/time value or 0 if the value could
     * not be converted.
     */
    time_t getDateFromTime(time_t startDate);

    /**
     * @brief Parse a list of IDs and make them a \c vector.
     *
     * @param szIds A comma (,) separated list of IDs to parse.
     * @return A list of IDs as \c vector.
     * @note Empty entries will be dropped and are not part of the return value.
     */
    vector<string> parseIds(string szIds);

    /**
     * @brief Change a single occurrence of a string in a string.
     *
     * @param toChange The \c string to modify.
     * @param changeThis The text to replace.
     * @param toThis The text which will be inserted instead.
     * @return \c true if the string was successfully found and replaced,
     * \c false otherwise.
     */
    bool stringReplaceOne(string &toChange, const char *changeThis, const char *toThis);

    /**
     * @brief Convert a \c string to its Quoted-Printable representation.
     *
     * @param szInput The input string to convert.
     * @return The resulting Quoted-Printable string.
     * @note This function is mostly used for vCal/iCal exports, so COMMA,
     * COLON and SEMICOLON will also be escaped always.
     * @note The result of the function is to be embedded as property, so the
     * first line will likely be prepended with a string of unknown length. In
     * order to not exceed the line length limit, the encoded string will be
     * unconditionally prefixed with a soft line break.
     * @note Also TAB and SPACE are encoded even if it is optional. The reason
     * is to skip the special end of line handling for them.
     * @see RFC1521(obsolete) and RFC2045(MIME) for details on Quoted-Printable.
     */
    string encodeQuotedPrintable(string szInput);

    /**
     * @brief Template function to cleanup a \c vector of pointers.
     *
     * @param vec The \c vector of pointers to cleanup.
     * @note For each element \c delete is called on it and the element pointer
     * is set to zero before the \c vector is cleaned.
     * @note There is no check for NULL items before calling \c delete.
     */
    template <class T> void releaseVector(vector <T *> vec);

    /**
     * @brief Get a \c vector of times based on the given time span and recurrence rule.
     *
     * @param szRRule The recurrence rule to use.
     * @param iDtStart The start date of the date range to be evaluated.
     * @param iViewEnd The time offset to the end of the date range to be evaluated.
     * @param pTz The time zone of the evaluation.
     * @param pErrorCode Unused.
     * @note If \c iViewEnd is passed as 0, all possible recurrences until the end
     * of the \c time_t range are returned.
     * @return A \c vector of \c time_t entries, representing all recurrences for
     * the requested interval.
     */
    vector<time_t> getRecurrentTimes(string szRRule,
                                     icaltimetype iDtStart, time_t iViewEnd, icaltimezone *pTz,
                                     int &pErrorCode);

    /**
     * @brief Get the textual representation of the system's time zone.
     *
     * @return The system time zone as string or an empty string if the time
     * zone could not be evaluated..
     */
    string getSystemTimeZone(void);

    /**
     * @brief Get the textual representation of the system's time zone as Ical (as in libical) time zone.
     *
     * @return A pointer to the Ical time zone information or a NULL pointer in
     * case the time zone could not be evaluated.
     */
    icaltimezone *getSystemTimeZoneAsIcalTimeZone(void);

    /**
     * @brief Implements the instance retrieval mechanism of the Singleton pattern.
     *
     * @return A pointer to the Singleton \c CUtility class.
     */
    static CUtility *Instance(void);

private:
    /**
     * @brief The constructor of the class.
     * @note The constructor is made private to disallow invocation outside of
     * the \c Instance() function.
     */
    CUtility(void);
    /**
     * The destructor of the class.
     */
    virtual ~CUtility(void);
    /**
     * Holds the pointer to the (single) class instance.
     */
    static CUtility *pUt;
};

// implementation of template function (must be present in header file)
template <class T> void CUtility::releaseVector(vector <T *> vec)
{
    class vector <T *>::iterator it;

    for(it = vec.begin(); it != vec.end(); ++it) {
        delete(*it);
        (*it) = 0;
    }

    vec.clear();
}

#endif
