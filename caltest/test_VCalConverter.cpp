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

#include "test_VCalConverter.h"


CPPUNIT_TEST_SUITE_REGISTRATION(VCalConverter_TS1);

using namespace std;

void VCalConverter_TS1::setUp()
{
    vCalConv = new VCalConverter();
}

void VCalConverter_TS1::tearDown()
{
    vCalConv = NULL;
}

void VCalConverter_TS1 ::test_vCalToICalRule1()
{
    string vcal = "W MO TU WE TH FR SA SU 20090807T141500Z";
    string ical = "FREQ=WEEKLY;UNTIL=20090807T141500Z;WKST=SU;BYDAY=MO,TU,WE,TH,FR,SA,SU";
    string icalrule = vCalConv->vCalToICalRule(vcal);
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (ical == icalrule));
}
void VCalConverter_TS1 ::test_vCalToICalRule2()
{
    string vcal = "YM1 6 7 8 #10";
    string ical = "FREQ=YEARLY;INTERVAL=1;COUNT=10;BYMONTH=6,7,8";
    string icalrule = vCalConv->vCalToICalRule(vcal);
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (ical == icalrule));
}
void VCalConverter_TS1 ::test_vCalToICalRule3()
{
    string vcal = "MP1 1+ 3+ TU #0";
    string ical = "FREQ=MONTHLY;INTERVAL=1;WKST=SU;BYDAY=1TU,3TU";
    string icalrule = vCalConv->vCalToICalRule(vcal);
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (ical == icalrule));
}
void VCalConverter_TS1 ::test_vCalToICalRule4()
{
    string vcal = "MD1 2 4 5 #10";
    string ical = "FREQ=MONTHLY;INTERVAL=1;COUNT=10;BYMONTHDAY=2,4,5";
    string icalrule = vCalConv->vCalToICalRule(vcal);
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (ical == icalrule));
}

void VCalConverter_TS1 ::test_iCalToVCalRule1()
{
    string ical = "FREQ=WEEKLY;INTERVAL=1;BYDAY=MO,TU,WE,TH,FR,SA,SU;UNTIL=20090807T141500Z";
    string vcalrule = vCalConv->iCalToVCalRule(ical, time(NULL));
    string vcal = "W1 MO TU WE TH FR SA SU 20090807T141500Z";
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (vcal == vcalrule));
}

void VCalConverter_TS1 ::test_iCalToVCalRule2()
{
    string ical = "FREQ=MONTHLY;INTERVAL=1;COUNT=5;WKST=SU;BYDAY=1TU";
    string vcalrule = vCalConv->iCalToVCalRule(ical, time(NULL));
    string vcal = "MD1 1+ TU #5";
    /*Assertion is commented as the string depends on the day of the month*/
    //CPPUNIT_ASSERT_MESSAGE("Strings should be equal",(vcal==vcalrule));
}
void VCalConverter_TS1 ::test_iCalToVCalRule3()
{
    string ical = "FREQ=YEARLY;INTERVAL=1;COUNT=10;BYMONTH=6,7,8";
    string vcalrule = vCalConv->iCalToVCalRule(ical, time(NULL));
    string vcal = "YM1 6 7 8 #10";
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (vcal == vcalrule));
}
void VCalConverter_TS1 ::test_iCalToVCalRule4()
{
    string ical = "FREQ=DAILY;INTERVAL=1;COUNT=10";
    string vcalrule = vCalConv->iCalToVCalRule(ical, time(NULL));
    string vcal = "D1 #10";
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (vcal == vcalrule));
}
void VCalConverter_TS1 ::test_iCalToVCalRule5()
{
    string ical = "FREQ=MONTHLY;BYMONTHDAY=2;COUNT=10";
    string vcalrule = vCalConv->iCalToVCalRule(ical, time(NULL));
    string vcal = "MD1 2+ #10";
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (vcal == vcalrule));
}
void VCalConverter_TS1 ::test_vCalToICalDate()
{
    string vcaldate = "20070101T010203Z";
    string icaldate = vCalConv->vCalToICalDate(icaldate);
    CPPUNIT_ASSERT_MESSAGE("Error: should return same date", (icaldate.empty() == 0));

}

void VCalConverter_TS1 ::test_iCalToVCalDate()
{
    string icaldate = "BEGIN:VTIMEZONE\nTZID:New York\nBEGIN:STANDARD\nTZOFFSETFROM:-0400\nTZOFFSETTO:-0500\nTZNAME:EST\nDTSTART:19701025T020000\nRRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=-1SU;BYMONTH=10\nEND:STANDARD\nBEGIN:DAYLIGHT\nTZOFFSETFROM:-0500\nTZOFFSETTO:-0400\nTZNAME:EDT\nDTSTART:19700405T020000\nRRULE:FREQ=YEARLY;INTERVAL=1;BYDAY=1SU;BYMONTH=4\nEND:DAYLIGHT\nEND:VTIMEZONE";
    string vcaldate = vCalConv->iCalToVCalDate(icaldate);

    icaldate = "RDATE:19971026T020000";
    vcaldate = vCalConv->iCalToVCalDate(icaldate);
    //CPPUNIT_ASSERT_MESSAGE("Error: iCalToVCalDate", string("19971026T020000") == vcaldate);

    icaldate = "RDATE;TZID=US-EASTERN:19970714";
    vcaldate = vCalConv->iCalToVCalDate(icaldate);
    //CPPUNIT_ASSERT_MESSAGE("Error: iCalToVCalDate", string("19970714") == vcaldate);

    icaldate = "RDATE;VALUE=PERIOD:19960403T020000Z:19960413T020000Z";
    vcaldate = vCalConv->iCalToVCalDate(icaldate);

    icaldate = "RDATE;VALUE=PERIOD:19960403T020000Z/19960413T020000Z";
    vcaldate = vCalConv->iCalToVCalDate(icaldate);
}

void VCalConverter_TS1 ::test_vCalToICalAlarm()
{
    string vcalalarm = "AALARM;TYPE=WAVE;VALUE=URL:20020830T113000Z;PT10M;3;file:///ping.wav";
    string icalalarm = vCalConv->vCalToICalAlarm(vcalalarm);

    string ialarm = "BEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20020830T113000Z\nDURATION:PT10M\nREPEAT:3\nATTACH;FMTTYPE=audio/basic:file:///ping.wav\nACTION:AUDIO\nEND:VALARM\n";
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (icalalarm.compare(ialarm.c_str()) == 0));
}

void VCalConverter_TS1 ::test_iCalToVCalAlarm()
{
    string icalalarm = "BEGIN:VALARM\nTRIGGER;VALUE=DATE-TIME:20080807T152000Z\nDURATION:PT1M\nREPEAT:1\nACTION:AUDIO\nEND:VALARM";
    string vcalalarm = vCalConv->iCalToVCalAlarm(icalalarm);
    string vcala = "AALARM:20080807T152000Z;PT1M;1";
    CPPUNIT_ASSERT_MESSAGE("Strings should be equal", (vcala.compare(vcalalarm.c_str()) == 0));
}

void VCalConverter_TS1 ::test_getTokens()
{
    vector<string> token = vCalConv->getTokens("abc;def;ghi;wert", ";");
    CPPUNIT_ASSERT_MESSAGE("Error Number of tokens", token.size() == 4);
}

void VCalConverter_TS1::test_checkContentsTypeIcal()
{
    string szInput = "BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;MEMBER=\"MAILTO:DEV-GROUP@host2.com\":MAILTO:joecool@host2.com\nATTENDEE;DELEGATED-FROM=\"MAILTO:immud@host3.com\":MAILTO:ildoit@host1.com\nEND:VEVENT";
    CPPUNIT_ASSERT_MESSAGE("error in checking icaltype", ICAL_TYPE == vCalConv->checkContentsType(szInput));
}
void VCalConverter_TS1::test_checkContentsTypeVcal()
{
    string szInput = "BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=ATTENDEE:MAILTO:joecool@host2.com\nATTENDEE;ROLE=ORGANIZER:MAILTO:ildoit@host1.com\nRRULE:D1 #10\nEND:VEVENT";
    CPPUNIT_ASSERT_MESSAGE("error in checking icaltype", vCalConv->checkContentsType(szInput) == VCAL_TYPE);
    szInput = "BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=ATTENDEE:MAILTO:joecool@host2.com\nATTENDEE;ROLE=ORGANIZER:MAILTO:ildoit@host1.com\nRRULE:W #10\nEND:VEVENT";
    CPPUNIT_ASSERT_MESSAGE("error in checking icaltype", vCalConv->checkContentsType(szInput) == VCAL_TYPE);
    szInput = "BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=ATTENDEE:MAILTO:joecool@host2.com\nATTENDEE;ROLE=ORGANIZER:MAILTO:ildoit@host1.com\nRRULE:D #10\nEND:VEVENT";
    CPPUNIT_ASSERT_MESSAGE("error in checking icaltype", VCAL_TYPE == vCalConv->checkContentsType(szInput));
    szInput = "BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=ATTENDEE:MAILTO:joecool@host2.com\nATTENDEE;ROLE=ORGANIZER:MAILTO:ildoit@host1.com\nRRULE:M #10\nEND:VEVENT";
    CPPUNIT_ASSERT_MESSAGE("error in checking icaltype", VCAL_TYPE == vCalConv->checkContentsType(szInput));
    szInput = "BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=ATTENDEE:MAILTO:joecool@host2.com\nATTENDEE;ROLE=ORGANIZER:MAILTO:ildoit@host1.com\nRRULE:P #10\nEND:VEVENT";
    CPPUNIT_ASSERT_MESSAGE("error in checking icaltype", VCAL_TYPE == vCalConv->checkContentsType(szInput));
    szInput = "BEGIN:VEVENT\nDTSTART:20080618T114105Z\nDTEND:20080618T114105Z\nSUMMARY:Valid Case setting proper data values to pEvent\nLOCATION:Bangalore\nATTENDEE;ROLE=ATTENDEE:MAILTO:joecool@host2.com\nATTENDEE;ROLE=ORGANIZER:MAILTO:ildoit@host1.com\nRRULE:Y #10\nEND:VEVENT";
    CPPUNIT_ASSERT_MESSAGE("error in checking icaltype", VCAL_TYPE == vCalConv->checkContentsType(szInput));
}

void VCalConverter_TS1 :: test_VcalRSVP()
{
    icalparameter *param = NULL;
    param = vCalConv->getVcalRSVP(0);
    CPPUNIT_ASSERT_MESSAGE("Error getVcalRSVP", param != NULL);
    param = NULL;
    param = vCalConv->getVcalRSVP(1);
    CPPUNIT_ASSERT_MESSAGE("Error getVcalRSVP", param != NULL);
    int ret = vCalConv->getRSVPFromVcal("");
    CPPUNIT_ASSERT_MESSAGE("Error getRSVP", ret == 0);
    ret = vCalConv->getRSVPFromVcal("RSVP=YES");
    CPPUNIT_ASSERT_MESSAGE("Error getRSVP", ret == 1);
    ret = vCalConv->getRSVPFromVcal("RSVP=NO");
    CPPUNIT_ASSERT_MESSAGE("Error getRSVP", ret == 0);
}

void VCalConverter_TS1 :: test_getStatusFromVcal()
{
    int ret = -1;
    ret = vCalConv->getStatusFromVcal("a status");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == NEEDSACTION_STATUS));
    ret = vCalConv->getStatusFromVcal("status is STATUS:SENT");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == SENT_STATUS));
    ret = vCalConv->getStatusFromVcal("status is STATUS:DECLINED");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == DECLINED_STATUS));
    ret = vCalConv->getStatusFromVcal("status is STATUS:DELEGATED");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == DELEGATED_STATUS));
    ret = vCalConv->getStatusFromVcal("status is STATUS:ACCEPTED");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == ACCEPTED_STATUS));
    ret = vCalConv->getStatusFromVcal("status is STATUS:NEEDS ACTION");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == NEEDSACTION_STATUS));
    ret = vCalConv->getStatusFromVcal("status is STATUS:CONFIRMED");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == CONFIRMED_STATUS));
    ret = vCalConv->getStatusFromVcal("status is STATUS:TENTATIVE");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == TENTATIVE_STATUS));
    ret = vCalConv->getStatusFromVcal("status is STATUS:COMPLETED");
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (ret == COMPLETED_STATUS));
}

void VCalConverter_TS1 :: test_getVcalStatus()
{
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal",
                           (STATUS_TENTATIVE == vCalConv->getVcalStatus(TENTATIVE_STATUS)));
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (
                               STATUS_CONFIRMED == vCalConv->getVcalStatus(CONFIRMED_STATUS)));
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal",
                           (STATUS_COMPLETED == vCalConv->getVcalStatus(COMPLETED_STATUS)));
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal", (
                               STATUS_NEEDS_ACTION == vCalConv->getVcalStatus(NEEDSACTION_STATUS)));
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal",
                           (STATUS_ACCEPTED == vCalConv->getVcalStatus(ACCEPTED_STATUS)));
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal",
                           (STATUS_DELEGATED == vCalConv->getVcalStatus(DELEGATED_STATUS)));
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal",
                           (STATUS_DECLINED == vCalConv->getVcalStatus(DECLINED_STATUS)));
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal",
                           (STATUS_SENT == vCalConv->getVcalStatus(SENT_STATUS)));
    CPPUNIT_ASSERT_MESSAGE("Error getStatusFromVcal",
                           (STATUS_NEEDS_ACTION == vCalConv->getVcalStatus((statusParam)100)));
}


void VCalConverter_TS1 :: test_getTranspFromVcal()
{
    CPPUNIT_ASSERT_MESSAGE("Error: getTranspFromVcal",
                           strcmp(vCalConv->getTranspFromVcal("TRANSP:0").c_str(), "OPAQUE") == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: getTranspFromVcal",
                           strcmp(vCalConv->getTranspFromVcal("TRANSP:1").c_str(), "TRANSPARENT") == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: getTranspFromVcal",
                           strcmp(vCalConv->getTranspFromVcal("TRANSP:>1").c_str(), "TRANSPARENT") == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: getTranspFromVcal",
                           strcmp(vCalConv->getTranspFromVcal("TRANSP").c_str(), "OPAQUE") == 0);

}

void VCalConverter_TS1 :: test_getVcalPartRole()
{
    CPPUNIT_ASSERT_MESSAGE("Error: getVcalPartRole",
                           strcmp(vCalConv->getVcalPartRole(4).c_str(), "ROLE=ATTENDEE") == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: getVcalPartRole",
                           strcmp(vCalConv->getVcalPartRole(5).c_str(), "ROLE=ORGANIZER") == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: getVcalPartRole",
                           strcmp(vCalConv->getVcalPartRole(7).c_str(), "ROLE=OWNER") == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: getVcalPartRole",
                           strcmp(vCalConv->getVcalPartRole(6).c_str(), "ROLE=DELEGATE") == 0);
    CPPUNIT_ASSERT_MESSAGE("Error: getVcalPartRole",
                           strcmp(vCalConv->getVcalPartRole(10).c_str(), "ROLE=ATTENDEE") == 0);
}
