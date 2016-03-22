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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "CEvent.h"
#include "CComponent.h"
#include "CComponentDetails.h"
#include "Common.h"
#include "ICalConverter.h"


class ICalConverter_TS1 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ICalConverter_TS1);
    CPPUNIT_TEST(test_localToIcalVcal_ValidEvent);
    CPPUNIT_TEST(test_localToIcalVcal_InvalidEvent1);
    CPPUNIT_TEST(test_localToIcalVcal_ValidToDo);
    CPPUNIT_TEST(test_localToIcalVcal_InValidToDo2);
    CPPUNIT_TEST(test_localToIcalVcal_ValidJournal);
    CPPUNIT_TEST(test_icalVcalToLocal_EventWithAttendees);// A event with more than 1 attendee
    CPPUNIT_TEST(test_icalVcalToLocal_ValidCompleteEvent11);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidCompleteEvent12);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidCompleteEvent13);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidCompleteEvent_VCAL);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidEvent_Component);// all component parameters
    CPPUNIT_TEST(test_icalVcalToLocal_ValidEvent_ComponentDetails);// all componentdetails parameters
    CPPUNIT_TEST(test_icalVcalToLocal_EventWithOrg);// a event with organizer details
    CPPUNIT_TEST(test_icalVcalToLocal_InvalidEvent2); // Special characters in txt
    CPPUNIT_TEST(test_icalVcalToLocal_ValidToDo); // All entries are valid
    CPPUNIT_TEST(test_icalVcalToLocal_ValidToDo1);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidToDo2);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidToDo3);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidToDo4);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidToDo5);
    CPPUNIT_TEST(test_icalVcalToLocal_InValidToDo1); // Special characters in summary
    CPPUNIT_TEST(test_icalVcalToLocal_InValidToDo11);
    CPPUNIT_TEST(test_icalVcalToLocal_InValidToDo12);
    CPPUNIT_TEST(test_icalVcalToLocal_ValidJournal); // All entries are valid
    CPPUNIT_TEST(test_icalVcalToLocal_InValidJournal1); // Special characters in summary
    CPPUNIT_TEST(test_icalVcalToLocal_InValidFormat);
    CPPUNIT_TEST(test_icalVcalToLocal_InValidFormat2);
    CPPUNIT_TEST(test_icalVcalToLocal_InValidFormat3);
    CPPUNIT_TEST(test_icalVcalToLocal_EventWithVcalData);
    CPPUNIT_TEST(test_icalVcalToLocal_JournalWithVcalData);
    CPPUNIT_TEST(test_icalVcalToLocal_TodoWithVcalData);
    CPPUNIT_TEST(test_localToIcalVcal_ValidEventWithVcalData);
    CPPUNIT_TEST(test_icalVcalToLocal_empty);//with empty data
    CPPUNIT_TEST(test_icalVcalToLocal_ivnalidates);//invalid date formats
    CPPUNIT_TEST(test_getCurrentTimeZone);
    CPPUNIT_TEST(test_icalVcalToLocal_aalarm);
    CPPUNIT_TEST_SUITE_END();
public:
    ICalConverter *iCalConv;
    void setUp();
    void tearDown();
    void test_localToIcalVcal_ValidEvent();//All entries are valid
    void test_localToIcalVcal_InvalidEvent1(); // null pointer
    void test_localToIcalVcal_ValidToDo(); // All entries are valid
    void test_localToIcalVcal_InValidToDo2(); // Negative integers in setDate start
    void test_localToIcalVcal_ValidJournal(); // All entries are valid

    void test_icalVcalToLocal_ValidEvent_Component();//all component parameters
    void test_icalVcalToLocal_EventWithAttendees(); // A event with more than 1 attendee
    void test_icalVcalToLocal_ValidEvent_ComponentDetails();//all componentdetails parameters
    void test_icalVcalToLocal_EventWithOrg();//a event with organizer details
    void test_icalVcalToLocal_InvalidEvent2(); // Special characters in txt

    void test_icalVcalToLocal_ValidToDo(); // All entries are valid
    void test_icalVcalToLocal_InValidToDo1(); // Special characters in summary

    void test_icalVcalToLocal_ValidJournal(); // All entries are valid
    void test_icalVcalToLocal_InValidJournal1(); // Special characters in summary

    void test_icalVcalToLocal_InValidFormat(); // No format specified journal,todo etc
    void test_icalVcalToLocal_InValidFormat2(); // No format specified journal,todo etc
    void test_icalVcalToLocal_InValidFormat3(); // No format specified journal,todo etc
    void test_icalVcalToLocal_WithVcalData(); // No format specified journal,todo etc

    void test_localToIcalVcal_ValidEventWithVcalData();  //All entries are valid
    void test_icalVcalToLocal_JournalWithVcalData();  //All entries are valid
    void test_icalVcalToLocal_TodoWithVcalData();  //All entries are valid
    void test_icalVcalToLocal_EventWithVcalData();  //All entries are valid

    void test_icalVcalToLocal_ValidCompleteEvent_VCAL();
    void test_icalVcalToLocal_ValidToDo1();
    void test_icalVcalToLocal_ValidToDo2();
    void test_icalVcalToLocal_ValidToDo3();
    void test_icalVcalToLocal_ValidToDo4();
    void test_icalVcalToLocal_ValidToDo5();
    void test_icalVcalToLocal_ValidCompleteEvent11();
    void test_icalVcalToLocal_ValidCompleteEvent12();
    void test_icalVcalToLocal_ValidCompleteEvent13();
    void test_icalVcalToLocal_InValidToDo11();
    void test_icalVcalToLocal_InValidToDo12();
    void test_icalVcalToLocal_ivnalidates();
    void test_icalVcalToLocal_aalarm();
    void test_icalVcalToLocal_empty();
    void test_getCurrentTimeZone();
};


class ICalConverter_TS2 : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ICalConverter_TS2);
    CPPUNIT_TEST(test_validICalRRule);
    CPPUNIT_TEST(test_validVCalRRule);
    CPPUNIT_TEST(test_validICalRRule_monthly);
    CPPUNIT_TEST(test_validICalRRule_complex);
    CPPUNIT_TEST(test_validICalAlarm_durations_5min);
    CPPUNIT_TEST(test_validICalAlarm_durations_0min);
    CPPUNIT_TEST(test_validICalAlarm_durations_15min);
    CPPUNIT_TEST(test_validICalAlarm_durations_30min);
    CPPUNIT_TEST(test_validICalAlarm_durations_3hr);
    CPPUNIT_TEST(test_validICalRrule_exceed_datelimit);
    CPPUNIT_TEST_SUITE_END();
public:
    ICalConverter *iCalConv;
    void setUp();
    void tearDown();
    void test_validICalRRule();
    void test_validVCalRRule();
    void test_validICalRRule_monthly();
    void test_validICalRRule_complex();
    void test_validICalAlarm_durations_5min();
    void test_validICalAlarm_durations_15min();
    void test_validICalAlarm_durations_0min();
    void test_validICalAlarm_durations_30min();
    void test_validICalAlarm_durations_3hr();
    void test_validICalRrule_exceed_datelimit();

};
