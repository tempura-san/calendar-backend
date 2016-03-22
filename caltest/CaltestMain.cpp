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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <cppunit/config/SourcePrefix.h>

#include <iostream>
#include <string>

#include "test_CEvent.h"
#include "test_CJournal.h"
#include "test_CParticipant.h"
#include "test_COrganizer.h"
#include "test_CComponent.h"
#include "test_ComponentDetails.h"
#include "test_CAttendee.h"
#include "test_CTodo.h"
#include "test_CCalendardb.h"
#include "test_MultiCalendar.h"
#include "test_CAlarm.h"
#include "test_CRecurrence.h"
#include "test_CRecurrenceRule.h"
#include "test_CalendarAlarms.h"
#include "test_ICalConverter.h"
#include "test_VCalConverter.h"
#include "test_CCalendar.h"
#include "test_CBday.h"
#include "test_CProperties.h"
#include "test_CParameters.h"
#include "test_CTimezone.h"
#include "test_CUtility.h"


using namespace std;

int main(int ac, char **av)
{
    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);

    // Add a listener that print dots as test run.
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener(&progress);


    CppUnit::TextUi::TestRunner runner;
    // Add the top runner.to the test runner

    runner.addTest(CUtility_TS1::suite());
    runner.addTest(CMulticalendar_TS3::suite());
    runner.addTest(CCalendar_TS3::suite());
    runner.addTest(CMulticalendar_TS1::suite());
    runner.addTest(CCalendar_TS1::suite());
    runner.addTest(CEvent_TS1::suite());
    runner.addTest(CParticipant_TS1::suite());
    runner.addTest(new CppUnit::TestCaller<ValidCParticipantConstructorTestCase>(
                       "test_validCParticipantConstructor",
                       &ValidCParticipantConstructorTestCase::test_validCParticipantConstructor));
    runner.addTest(CJournal_TS1::suite());
    runner.addTest(new CppUnit::TestCaller<ValidCJournalConstructorTestCase>(
                       "test_validCJournalConstructor",
                       &ValidCJournalConstructorTestCase::test_validCJournalConstructor));
    runner.addTest(COrganizer_TS1::suite());
    runner.addTest(CComponent_TS1::suite());

    runner.addTest(ComponentDetails_TS1::suite());
    runner.addTest(CAttendee_TS1::suite());
    runner.addTest(new CppUnit::TestCaller<ValidCAttendeeConstructorTestCase>(
                       "test_validCAttendeeConstructor",
                       &ValidCAttendeeConstructorTestCase::test_validCAttendeeConstructor));
    runner.addTest(CTodo_TS1::suite());
    runner.addTest(new CppUnit::TestCaller<ValidCTodoConstructorTestCase>(
                       "test_validCTodoConstructor",
                       &ValidCTodoConstructorTestCase::test_validCTodoConstructor));

    runner.addTest(CAlarm_TS1::suite());
    runner.addTest(CRecurrence_TS1::suite());
    runner.addTest(CalendarAlarms_TS1::suite());
    runner.addTest(ICalConverter_TS1::suite());
    runner.addTest(VCalConverter_TS1::suite());
    runner.addTest(CBday_TS1::suite());
    runner.addTest(CRecurrenceRule_TS1::suite());
    runner.addTest(CProperties_TS1::suite());
    runner.addTest(CParameters_TS1::suite());

    runner.addTest(ICalConverter_TS2::suite());

    runner.addTest(CBday_TS2::suite());
    runner.addTest(CCalendar_TS2::suite());
    runner.addTest(CAlarm_TS2::suite());
    runner.addTest(CRecurrence_TS2::suite());
    runner.addTest(CEvent_TS2::suite());
    runner.addTest(CTodo_TS2::suite());
    runner.addTest(CJournal_TS2::suite());
    runner.addTest(ComponentDetails_TS2::suite());
    runner.addTest(COrganizer_TS2::suite());
    runner.addTest(CComponent_TS2::suite());
    runner.addTest(CAttendee_TS2::suite());
    runner.addTest(CRecurrenceRule_TS2::suite());
    runner.addTest(CAttendee_TS2::suite());
    runner.addTest(CParticipant_TS2::suite());
    runner.addTest(CParameters_TS2::suite());
    runner.addTest(CProperties_TS2::suite());
    runner.addTest(CCalendarDB_TS1::suite());
    runner.addTest(CTimezone_TS1::suite());
    runner.run(controller);
    // Print test in a compiler compatible format.
    std::ofstream file("Caltest.xml");
    CPPUNIT_NS::XmlOutputter xml(&result, file);
    xml.setStyleSheet("report.xsl");
    xml.write();
    file.close();
    return result.wasSuccessful() ? 0 : 1;

}

