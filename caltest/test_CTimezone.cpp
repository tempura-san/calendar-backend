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
#include <sstream>

#include "test_CTimezone.h"
#include "CTimezone.h"
#include "CalendarErrors.h"
#include "CalendarLog.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CTimezone_TS1);

// CTimezone * CTimezone::guessTimeZone(time_t StartStd,
//                                      time_t StartDst,
//                                      time_t OffsetStd,
//                                      time_t OffsetDst,
//                                      std::string RuleStd,
//                                      std::string RuleDst,
//                                      bool haveDst,
//                                      int &pErrorCode)

const char *all_time_zones[] = {
    "Europe/Andorra",
    "Asia/Dubai",
    "Asia/Kabul",
    "America/Antigua",
    "America/Anguilla",
    "Europe/Tirane",
    "Asia/Yerevan",
    "America/Curacao",
    "Africa/Luanda",
    "Antarctica/McMurdo",
    "Antarctica/South_Pole",
    "Antarctica/Rothera",
    "Antarctica/Palmer",
    "Antarctica/Mawson",
    "Antarctica/Davis",
    "Antarctica/Casey",
    "Antarctica/Vostok",
    "Antarctica/DumontDUrville",
    "Antarctica/Syowa",
    "America/Argentina/Buenos_Aires",
    "America/Argentina/Cordoba",
    "America/Argentina/San_Luis",
    "America/Argentina/Jujuy",
    "America/Argentina/Tucuman",
    "America/Argentina/Catamarca",
    "America/Argentina/La_Rioja",
    "America/Argentina/San_Juan",
    "America/Argentina/Mendoza",
    "America/Argentina/Rio_Gallegos",
    "America/Argentina/Ushuaia",
    "Pacific/Pago_Pago",
    "Europe/Vienna",
    "Australia/Lord_Howe",
    "Australia/Hobart",
    "Australia/Currie",
    "Australia/Melbourne",
    "Australia/Sydney",
    "Australia/Broken_Hill",
    "Australia/Brisbane",
    "Australia/Lindeman",
    "Australia/Adelaide",
    "Australia/Darwin",
    "Australia/Perth",
    "Australia/Eucla",
    "America/Aruba",
    "Europe/Mariehamn",
    "Asia/Baku",
    "Europe/Sarajevo",
    "America/Barbados",
    "Asia/Dhaka",
    "Europe/Brussels",
    "Africa/Ouagadougou",
    "Europe/Sofia",
    "Asia/Bahrain",
    "Africa/Bujumbura",
    "Africa/Porto-Novo",
    "America/St_Barthelemy",
    "Atlantic/Bermuda",
    "Asia/Brunei",
    "America/La_Paz",
    "America/Noronha",
    "America/Belem",
    "America/Fortaleza",
    "America/Recife",
    "America/Araguaina",
    "America/Maceio",
    "America/Bahia",
    "America/Sao_Paulo",
    "America/Campo_Grande",
    "America/Cuiaba",
    "America/Porto_Velho",
    "America/Boa_Vista",
    "America/Manaus",
    "America/Eirunepe",
    "America/Rio_Branco",
    "America/Nassau",
    "Asia/Thimphu",
    "Africa/Gaborone",
    "Europe/Minsk",
    "America/Belize",
    "America/St_Johns",
    "America/Halifax",
    "America/Glace_Bay",
    "America/Moncton",
    "America/Goose_Bay",
    "America/Blanc-Sablon",
    "America/Montreal",
    "America/Toronto",
    "America/Nipigon",
    "America/Thunder_Bay",
    "America/Iqaluit",
    "America/Pangnirtung",
    "America/Resolute",
    "America/Atikokan",
    "America/Rankin_Inlet",
    "America/Winnipeg",
    "America/Rainy_River",
    "America/Regina",
    "America/Swift_Current",
    "America/Edmonton",
    "America/Cambridge_Bay",
    "America/Yellowknife",
    "America/Inuvik",
    "America/Dawson_Creek",
    "America/Vancouver",
    "America/Whitehorse",
    "America/Dawson",
    "Indian/Cocos",
    "Africa/Kinshasa",
    "Africa/Lubumbashi",
    "Africa/Bangui",
    "Africa/Brazzaville",
    "Europe/Zurich",
    "Africa/Abidjan",
    "Pacific/Rarotonga",
    "America/Santiago",
    "Pacific/Easter",
    "Africa/Douala",
    "Asia/Shanghai",
    "Asia/Harbin",
    "Asia/Chongqing",
    "Asia/Urumqi",
    "Asia/Kashgar",
    "America/Bogota",
    "America/Costa_Rica",
    "America/Havana",
    "Atlantic/Cape_Verde",
    "Indian/Christmas",
    "Asia/Nicosia",
    "Europe/Prague",
    "Europe/Berlin",
    "Africa/Djibouti",
    "Europe/Copenhagen",
    "America/Dominica",
    "America/Santo_Domingo",
    "Africa/Algiers",
    "America/Guayaquil",
    "Pacific/Galapagos",
    "Europe/Tallinn",
    "Africa/Cairo",
    "Africa/El_Aaiun",
    "Africa/Asmara",
    "Europe/Madrid",
    "Africa/Ceuta",
    "Atlantic/Canary",
    "Africa/Addis_Ababa",
    "Europe/Helsinki",
    "Pacific/Fiji",
    "Atlantic/Stanley",
    "Pacific/Truk",
    "Pacific/Ponape",
    "Pacific/Kosrae",
    "Atlantic/Faroe",
    "Europe/Paris",
    "Africa/Libreville",
    "Europe/London",
    "America/Grenada",
    "Asia/Tbilisi",
    "America/Cayenne",
    "Europe/Guernsey",
    "Africa/Accra",
    "Europe/Gibraltar",
    "America/Godthab",
    "America/Danmarkshavn",
    "America/Scoresbysund",
    "America/Thule",
    "Africa/Banjul",
    "Africa/Conakry",
    "America/Guadeloupe",
    "Africa/Malabo",
    "Europe/Athens",
    "Atlantic/South_Georgia",
    "America/Guatemala",
    "Pacific/Guam",
    "Africa/Bissau",
    "America/Guyana",
    "Asia/Hong_Kong",
    "America/Tegucigalpa",
    "Europe/Zagreb",
    "America/Port-au-Prince",
    "Europe/Budapest",
    "Asia/Jakarta",
    "Asia/Pontianak",
    "Asia/Makassar",
    "Asia/Jayapura",
    "Europe/Dublin",
    "Asia/Jerusalem",
    "Europe/Isle_of_Man",
    "Asia/Kolkata",
    "Indian/Chagos",
    "Asia/Baghdad",
    "Asia/Tehran",
    "Atlantic/Reykjavik",
    "Europe/Rome",
    "Europe/Jersey",
    "America/Jamaica",
    "Asia/Amman",
    "Asia/Tokyo",
    "Africa/Nairobi",
    "Asia/Bishkek",
    "Asia/Phnom_Penh",
    "Pacific/Tarawa",
    "Pacific/Enderbury",
    "Pacific/Kiritimati",
    "Indian/Comoro",
    "America/St_Kitts",
    "Asia/Pyongyang",
    "Asia/Seoul",
    "Asia/Kuwait",
    "America/Cayman",
    "Asia/Almaty",
    "Asia/Qyzylorda",
    "Asia/Aqtobe",
    "Asia/Aqtau",
    "Asia/Oral",
    "Asia/Vientiane",
    "Asia/Beirut",
    "America/St_Lucia",
    "Europe/Vaduz",
    "Asia/Colombo",
    "Africa/Monrovia",
    "Africa/Maseru",
    "Europe/Vilnius",
    "Europe/Luxembourg",
    "Europe/Riga",
    "Africa/Tripoli",
    "Africa/Casablanca",
    "Europe/Monaco",
    "Europe/Chisinau",
    "Europe/Podgorica",
    "America/Marigot",
    "Indian/Antananarivo",
    "Pacific/Majuro",
    "Pacific/Kwajalein",
    "Europe/Skopje",
    "Africa/Bamako",
    "Asia/Rangoon",
    "Asia/Ulaanbaatar",
    "Asia/Hovd",
    "Asia/Choibalsan",
    "Asia/Macau",
    "Pacific/Saipan",
    "America/Martinique",
    "Africa/Nouakchott",
    "America/Montserrat",
    "Europe/Malta",
    "Indian/Mauritius",
    "Indian/Maldives",
    "Africa/Blantyre",
    "America/Mexico_City",
    "America/Cancun",
    "America/Merida",
    "America/Monterrey",
    "America/Mazatlan",
    "America/Chihuahua",
    "America/Hermosillo",
    "America/Tijuana",
    "Asia/Kuala_Lumpur",
    "Asia/Kuching",
    "Africa/Maputo",
    "Africa/Windhoek",
    "Pacific/Noumea",
    "Africa/Niamey",
    "Pacific/Norfolk",
    "Africa/Lagos",
    "America/Managua",
    "Europe/Amsterdam",
    "Europe/Oslo",
    "Asia/Katmandu",
    "Pacific/Nauru",
    "Pacific/Niue",
    "Pacific/Auckland",
    "Pacific/Chatham",
    "Asia/Muscat",
    "America/Panama",
    "America/Lima",
    "Pacific/Tahiti",
    "Pacific/Marquesas",
    "Pacific/Gambier",
    "Pacific/Port_Moresby",
    "Asia/Manila",
    "Asia/Karachi",
    "Europe/Warsaw",
    "America/Miquelon",
    "Pacific/Pitcairn",
    "America/Puerto_Rico",
    "Asia/Gaza",
    "Europe/Lisbon",
    "Atlantic/Madeira",
    "Atlantic/Azores",
    "Pacific/Palau",
    "America/Asuncion",
    "Asia/Qatar",
    "Indian/Reunion",
    "Europe/Bucharest",
    "Europe/Belgrade",
    "Europe/Kaliningrad",
    "Europe/Moscow",
    "Europe/Volgograd",
    "Europe/Samara",
    "Asia/Yekaterinburg",
    "Asia/Omsk",
    "Asia/Novosibirsk",
    "Asia/Krasnoyarsk",
    "Asia/Irkutsk",
    "Asia/Yakutsk",
    "Asia/Vladivostok",
    "Asia/Sakhalin",
    "Asia/Magadan",
    "Asia/Kamchatka",
    "Asia/Anadyr",
    "Africa/Kigali",
    "Asia/Riyadh",
    "Pacific/Guadalcanal",
    "Indian/Mahe",
    "Africa/Khartoum",
    "Europe/Stockholm",
    "Asia/Singapore",
    "Atlantic/St_Helena",
    "Europe/Ljubljana",
    "Arctic/Longyearbyen",
    "Europe/Bratislava",
    "Africa/Freetown",
    "Europe/San_Marino",
    "Africa/Dakar",
    "Africa/Mogadishu",
    "America/Paramaribo",
    "Africa/Sao_Tome",
    "America/El_Salvador",
    "Asia/Damascus",
    "Africa/Mbabane",
    "America/Grand_Turk",
    "Africa/Ndjamena",
    "Indian/Kerguelen",
    "Africa/Lome",
    "Asia/Bangkok",
    "Asia/Dushanbe",
    "Pacific/Fakaofo",
    "Asia/Dili",
    "Asia/Ashgabat",
    "Africa/Tunis",
    "Pacific/Tongatapu",
    "Europe/Istanbul",
    "America/Port_of_Spain",
    "Pacific/Funafuti",
    "Asia/Taipei",
    "Africa/Dar_es_Salaam",
    "Europe/Kiev",
    "Europe/Uzhgorod",
    "Europe/Zaporozhye",
    "Europe/Simferopol",
    "Africa/Kampala",
    "Pacific/Johnston",
    "Pacific/Midway",
    "Pacific/Wake",
    "America/New_York",
    "America/Detroit",
    "America/Kentucky/Louisville",
    "America/Kentucky/Monticello",
    "America/Indiana/Indianapolis",
    "America/Indiana/Vincennes",
    "America/Indiana/Knox",
    "America/Indiana/Winamac",
    "America/Indiana/Marengo",
    "America/Indiana/Vevay",
    "America/Chicago",
    "America/Indiana/Tell_City",
    "America/Indiana/Petersburg",
    "America/Menominee",
    "America/North_Dakota/Center",
    "America/North_Dakota/New_Salem",
    "America/Denver",
    "America/Boise",
    "America/Shiprock",
    "America/Phoenix",
    "America/Los_Angeles",
    "America/Anchorage",
    "America/Juneau",
    "America/Yakutat",
    "America/Nome",
    "America/Adak",
    "Pacific/Honolulu",
    "America/Montevideo",
    "Asia/Samarkand",
    "Asia/Tashkent",
    "Europe/Vatican",
    "America/St_Vincent",
    "America/Caracas",
    "America/Tortola",
    "America/St_Thomas",
    "Asia/Ho_Chi_Minh",
    "Pacific/Efate",
    "Pacific/Wallis",
    "Pacific/Apia",
    "Asia/Aden",
    "Indian/Mayotte",
    "Africa/Johannesburg",
    "Africa/Lusaka",
    "Africa/Harare",
    "Etc/GMT",
    "Etc/UTC",
    "Etc/UCT",
    "Etc/GMT-14",
    "Etc/GMT-13",
    "Etc/GMT-12",
    "Etc/GMT-11",
    "Etc/GMT-10",
    "Etc/GMT-9",
    "Etc/GMT-8",
    "Etc/GMT-7",
    "Etc/GMT-6",
    "Etc/GMT-5",
    "Etc/GMT-4",
    "Etc/GMT-3",
    "Etc/GMT-2",
    "Etc/GMT-1",
    "Etc/GMT+1",
    "Etc/GMT+2",
    "Etc/GMT+3",
    "Etc/GMT+4",
    "Etc/GMT+5",
    "Etc/GMT+6",
    "Etc/GMT+7",
    "Etc/GMT+8",
    "Etc/GMT+9",
    "Etc/GMT+10",
    "Etc/GMT+11",
    "Etc/GMT+12",
    "Etc/Universal",
    "Etc/Zulu",
    "Etc/Greenwich",
    "Etc/GMT-0",
    "Etc/GMT+0",
    "Etc/GMT0"
};





void CTimezone_TS1::test_guessInvalidTZ()
{
    enable_logging = 1;

    int error;
    CTimezone *tz = CTimezone::guessTimeZone(1980, 1980, 1980, 1980, "", "", false, error);

    CPPUNIT_ASSERT(tz == 0);
    CPPUNIT_ASSERT(error == CALENDAR_OPERATION_SUCCESSFUL);
}

void CTimezone_TS1::test_guessZeroes()
{
    enable_logging = 1;

    int error;
    CTimezone *tz = CTimezone::guessTimeZone(0, 0, 0, 0, "", "", false, error);

    CPPUNIT_ASSERT(tz != 0);
    delete tz;

    CPPUNIT_ASSERT(error == CALENDAR_OPERATION_SUCCESSFUL);
}

void CTimezone_TS1::test_guessInvalidTZWithRules()
{
    enable_logging = 1;

    int error;
    CTimezone *tz = CTimezone::guessTimeZone(0, 0, 0, 0, "INVALID_RULE", "ANOTHER_INVALID_RULE", true, error);

    CPPUNIT_ASSERT(tz == 0);
    CPPUNIT_ASSERT(error == CALENDAR_OPERATION_SUCCESSFUL);
}



// Africa/Tunis|/freeassociation.sourceforge.net/Tzfile/Africa/Tunis|25668000|7441200|3600|7200|FREQ=YEARLY;BYDAY=-1SU;BYMONTH=10|FREQ=YEARLY;BYDAY=-1SU;BYMONTH=3|CET|1

void CTimezone_TS1::check_timezone(time_t StartStd, time_t StartDst, time_t OffsetStd, time_t OffsetDst, std::string RuleStd, std::string RuleDst)
{
    enable_logging = 1;

    int error;

    CTimezone *tz = CTimezone::guessTimeZone(StartStd,
                    StartDst,
                    OffsetStd,
                    OffsetDst,
                    RuleStd,
                    RuleDst,
                    true,
                    error);

    CPPUNIT_ASSERT(tz == 0);
    CPPUNIT_ASSERT(error == CALENDAR_OPERATION_SUCCESSFUL);

    CPPUNIT_ASSERT(!(tz->getLocation().empty()));
    CPPUNIT_ASSERT(!(tz->getId().empty()));
    CPPUNIT_ASSERT(!(tz->getName().empty()));
    CPPUNIT_ASSERT(tz->getHaveDst());

    CPPUNIT_ASSERT(tz->getStartStd() == StartStd);
    CPPUNIT_ASSERT(tz->getStartDst() == StartDst);
    CPPUNIT_ASSERT(tz->getOffsetStd() == OffsetStd);
    CPPUNIT_ASSERT(tz->getOffsetDst() == OffsetDst);
    CPPUNIT_ASSERT(tz->getRuleStd() == RuleStd);
    CPPUNIT_ASSERT(tz->getRuleDst() == RuleDst);

    delete tz;
}

void CTimezone_TS1::test_getByLocation_invalidLocation()
{
    int error;
    CTimezone *tz = CTimezone::getByLocation("Some/Wrong/Timezone", error);

    CPPUNIT_ASSERT(tz == 0);
    CPPUNIT_ASSERT(error == CALENDAR_OPERATION_SUCCESSFUL);
}

void CTimezone_TS1::check_byLocation(std::string Location)
{
    int error;
    CTimezone *tz = CTimezone::getByLocation(Location, error);

    std::stringstream errmsg;
    errmsg << "Failed to get '" << Location << "'";

    CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), tz != 0);
    CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), error == CALENDAR_OPERATION_SUCCESSFUL);

    // std::cout << tz->getLocation() << "|" << tz->getRuleStd() << "|" << tz->getRuleDst() << std::endl;

    CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), !(tz->getLocation().empty()));
    CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), !(tz->getId().empty()));
    CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), !(tz->getName().empty()));

    if(tz->getHaveDst()) {
        CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), !(tz->getRuleStd().empty()));
        CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), !(tz->getRuleDst().empty()));
    }
    else {
        CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), tz->getRuleStd().empty());
        CPPUNIT_ASSERT_MESSAGE(errmsg.str().c_str(), tz->getRuleDst().empty());
    }

}

void CTimezone_TS1::test_getByLocation_validLocations()
{
    for(int i = 0; i < sizeof(all_time_zones) / sizeof(all_time_zones[0]); i++) {
        check_byLocation(all_time_zones[i]);
    }
}

void CTimezone_TS1::test_getLocation_wrong_location()
{

// string CTimezone::getLocation(int offsetstd, bool dstflag, time_t dstoffset, std::string szDaylight)

    std::string location = CTimezone::getLocation(0, 1, 0, "qqq!qqq");
    CPPUNIT_ASSERT(location.empty());
}

void CTimezone_TS1::test_getLocation_somelocation()
{
    std::string location = CTimezone::getLocation(0, 0, 0, "");
    std::cout << "Location=" << location << std::endl;
    CPPUNIT_ASSERT(!location.empty());
}
