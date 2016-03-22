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

#ifndef __RECURRENCE_H__
#define __RECURRENCE_H__

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "CRecurrenceRule.h"

/** @file CRecurrence.h */

/**
 * @brief Enumeration of possible types of recurrences.
 */
typedef enum {
    E_NONE = 0,//!< No recurrence.
    E_DAILY,   //!< Daily recurrence.
    E_WEEKDAY, //!< Recurrence by weekday.
    E_WEEKLY,  //!< Weekly recurrence.
    E_MONTHLY, //!< Monthly recurrence.
    E_YEARLY,  //!< Yearly recurrence.
    E_COMPLEX  //!< Complex recurrence (e.g. defined by rules, exceptions,...).
} RecurrenceType_t;

/**
 * @brief A class to encapsulate the recurrence information.
 *
 * Recurrences may consist of:
 * @li Recurrence rules (encapsulated in CRecurrenceRule objects)
 * @li Recurrence days
 * @li Exception rules (encapsulated in CRecurrenceRule objects)
 * @li Exception days
 *
 * The instances of a recurrence can be obtained using the generateInstanceTimes()
 * function.
 *
 * Note that no checks of validity are done when adding rules and/or days. It is
 * up to the caller to ensure that the passed information is valid.
 *
 * The toString() function can be used to get a overview of the objects content.
 * The resulting string is intended only for debugging purposes.
 */
class CRecurrence
{

public:

    /**
     * @brief Append the given rules to the current set of recurrence rule objects.
     * @param VRRuleList A non-empty vector of pointers to CRecurrenceRule objects.
     * @return \c true if rules were appended, \c false otherwise.
     * @attention Despite its naming, existing rules are not cleared.
     */
    bool setRecurrenceRule(vector<CRecurrenceRule *> VRRuleList);

    /**
     * @brief Return the current set of recurrence rule objects.
     * @return A vector of pointers to CRecurrenceRule objects.
     */
    vector<CRecurrenceRule *> getRecurrenceRule();

    /**
     * @brief Return the contained exception rules.
     * @return A vector of contained exception rules as strings. An empty string
     * is returned, if no exception rules are found.
     */
    vector<string> getErule();

    /**
     * @brief Return the contained recurrence rules.
     * @return A vector of contained recurrence rules as strings. An empty
     * string is returned, if no recurrence rules are found.
     */
    vector<string> getRrule();

    /**
     * @brief Copy constructor for the CRecurrence class.
     * @param ref The object to be copied.
     */
    CRecurrence(CRecurrence &ref);

    /**
     * @brief Append a set of recurrence rules.
     * @param vRRuleList A non-empty vector of recurrence rule strings.
     * @return \c false if the passed vector was empty, \c true otherwise.
     * @attention Despite its naming, existing rules are not cleared. The caller
     * has to ensure that the list of rules is valid, as no checks are
     * performed on adding the rules.
     */
    bool setRrule(vector<string> vRRuleList);

    /**
     * @brief Append a set exception rules.
     * @param vERuleList A non-empty vector of recurrence rule strings.
     * @return \c false if the passed vector was empty, \c true otherwise.
     * @attention Despite its naming, existing rules are not cleared. The caller
     * has to ensure that the list of rules is valid, as no checks are
     * performed on adding the rules.
     */
    bool setErule(vector<string> vERuleList);

    /**
     * @brief Set the recurring dates for this recurrence object.
     * @param vRecRuleList A non-empty vector of calendar recurrence dates as
     * strings.
     * @return \c false if the passed vector was empty, \c true otherwise.
     * @attention The caller has to ensure that the list of dates is valid, as
     * no checks are performed on adding the dates.
     */
    bool setRDays(vector<string> vRecRuleList);

    /**
     * @brief Get the recurring dates for this recurrence object.
     * @return A vector of calendar recurrence dates as strings.
     */
    vector<string> getRDays();

    /**
     * @brief Set the exception days for this recurrence object.
     * @param vExceptionDateList A non-empty vector of calendar exception dates
     * as strings.
     * @return \c false if the passed vector was empty, \c true otherwise.
     * @attention The caller has to ensure that the list of dates is valid, as
     * no checks are performed on adding the dates.
     */
    bool setEDays(vector<string> vExceptionDateList);

    /**
     * @brief Get the exception dates for this recurrence object.
     * @return A vector of calendar recurrence dates as strings.
     */
    vector<string> getEDays();

    /**
     * @brief Set the ID of the recurrence object.
     * @param iRId A non negative ID to set.
     * @return \c true if the passed ID was valid, \c false otherwise.
     */
    bool setRecurId(int iRId);

    /**
     * @brief Return the ID of the recurrence object.
     * @return The recurrence ID.
     */
    int getRecurId();

    /**
     * @brief Set the type of recurrence.
     * @param iRType The type of recurrence from the enumeration
     * RecurrenceType_t.
     * @return \c true if the passed type was valid and set, \c false otherwise.
     * @note For external events/todos the value should be set to \c E_NONE (0).
     * @remarks The interface should use the enumeration instead (not \c int).
     * @todo The type of recurrence could be determined automatically when
     * (exception) rules and (exception) dates are added, making this function
     * obsolete as it might set an inconsistent recurrence type.
     */
    bool setRtype(int iRType);

    /**
     * @brief Get the type of recurrence.
     * @return The type of recurrence from the enumeration RecurrenceType_t.
     * @remarks The interface should use the enumeration instead (not \c int).
     */
    int getRtype();

    /**
     * @brief An overloaded constructor for the CRecurrence class.
     * @param vRRuleList A vector of recurrence rules to set.
     * @param iRId The non-negative recurrence ID to set.
     * @attention The caller has to ensure that the list of rules is valid, as
     * no checks are performed on adding the rules.
     */
    CRecurrence(vector<string> vRRuleList, int iRId);

    /**
     * @brief Generate a vector of instance times when the recurrence is
     * happening.
     * @param iViewBegin The start time of the view to generate.
     * @param iViewEnd The end time of the view to generate. If this parameter
     * is set to 0, the recurrence will be expanded to one recurrence after
     * iViewBegin.
     * @param iDateStart The start date of the recurrence.
     * @param iEventDuration The duration of the original event (as difference
     * between date start and date end of the orginal instance).
     * @param is_day UNUSED.
     * @param sTzid The time zone of the event as string.
     * @return The instance times as a vector of \c time_t entries. An empty
     * list if no entries are found in the specified range.
     * @deprecated Use generateInstanceTimes() with \c instance_times as
     * reference argument instead.
     */
    vector<time_t> generateInstanceTimes(time_t iViewBegin, time_t iViewEnd,
                                         time_t iDateStart, int iEventDuration, int is_day, string sTzid);

    /**
     * @brief Generate a vector of instance times when the recurrence is
     * happening.
     * @param iViewBegin The start time of the view to generate.
     * @param iViewEnd The end time of the view to generate. If this parameter
     * is set to 0, the recurrence will be expanded to one recurrence after
     * iViewBegin.
     * @param iDateStart The start date of the recurrence.
     * @param iEventDuration The duration of the original event (as difference
     * between date start and date end of the original instance).
     * @param is_day UNUSED.
     * @param sTzid The time zone of the event as string.
     * @param instance_times The instance times as a vector of \c time_t
     * entries. An empty list if no entries are found in the specified range.
     * @param sort_times Sort the resulting vector (\c true by default).
     */
    void generateInstanceTimes(time_t iViewBegin, time_t iViewEnd,
                               time_t iDateStart, int iEventDuration, int is_day, string sTzid,
                               vector<time_t> &instance_times, bool sort_times = true);

    /**
     * @brief Check if any instance of this event occurs in the given interval.
     * @param iViewBegin lower range of current view
     * @param iViewEnd higher range of current view
     * @param iDateStart The start date of the recurrence.
     * @param iEventDuration The duration of the original event (as difference
     * between date start and date end of the original instance).
     * @param is_day UNUSED.
     * @param sTzid The time zone of the event as string.
     * @return \c true if at lease one instance found in the specified range.
     * @note The function will call generateInstanceTimes() to determine if an
     * instance of an event lies in the specified range.
     */
    bool checkInstances(time_t iViewBegin, time_t iViewEnd, time_t iDateStart,
                        int iEventDuration, int is_day, string sTzid);

    /**
     * @brief The default constructor for CRecurrence class.
     */
    CRecurrence();

    /**
     * @brief Overloaded assignment operator for the CRecurrence class.
     * @param right The right hand operand for the assignment.
     */
    CRecurrence &operator=(const CRecurrence &right);

    /**
     * @brief The destructor of the CRecurrence class.
     * @note The destructor will also release the recurrence rule objects.
     */
    ~CRecurrence();

    /**
     * @brief Return the string representation of the CRecurrence objects.
     * @return A string representing the CRecurrence object.
     */
    string toString();

private:

    vector<CRecurrenceRule *> vRecrRuleList; /*!< Vector of recurrence rules */
    vector<string> vExceptionDateList; /*!< Exception date list */
    vector<string> vRecRuleList; /*!< Recurrence rule list */
    RecurrenceType_t iRType; /*!< Recurrence type */
    int iRecurId; /*!< Recurrence ID */
};

#endif
