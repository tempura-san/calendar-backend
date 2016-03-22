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

/**
 * This file generates the zone information stored in zoneinfo table.
 * The timezone data is picked from libical configuration and updated in
 * zoneinfotable
 *
 *
 * To get the configuration data make libical configuration parameter
 * (use_built_in_timezone = yes )  in configure.in, install libical so that
 * libical folder is generated in /usr/share/...
 * Tzone executable reads the information from this location and creates a
 * zone info table .
 *
 *
 * Right now there is discrepancy for following cities  ( in libical zone.tab and
 * system zone.tab )
 *
 *  -+0223200 +0882200 Asia/Calcutta
   -+0104500 +1064000 Asia/Saigon
 * FIXME: Its a good idea to change these city names in Libical configuration information
 * ( zone.tab and corresponding ics file) to the latest ones supported in device.
 * So after generating zoneinfo please update the location for these cities
 * in zoneinfo table.
 *
 *   :Asia/Calcutta      to  :Asia/Kolkata
 *   :Asia/Saigon        to  :Asia/Ho_Chi_Minh
 *
 * FIXME: There can be some discrepancies due to city name changes at a later
 * point in time  these challenges are for maintainers to ponder.
 */



#include <iostream>
#include <fstream>
#include <calendar-backend/Common.h>
#include <calendar-backend/CalendarErrors.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>

int timeinsec(string vCalTimeZone);
void readinfo();
int readTzInfo(string file);
CCalendarDB *pCalDb = CCalendarDB::Instance();

/**
 *  g++ Tzone.cpp -o Tzone `pkg-config calendar-backend dbus-1 --cflags --libs`
 */
int timeinsec(string vCalTimeZone)
{
    time_t offset = 0 ;
    time_t offset_dst = 0;
    bool is_negative = false;

    if(vCalTimeZone.find("+") != string::npos) {
        vCalTimeZone = vCalTimeZone.substr(vCalTimeZone.find("+") + 1);
    }
    else {
        if(vCalTimeZone.find("-") != string::npos) {
            is_negative = true;
            vCalTimeZone = vCalTimeZone.substr(vCalTimeZone.find("-") + 1);
        }
    }

    string szMinutes, szHour;
    szMinutes  = vCalTimeZone.substr(2);
    offset = atoi(szMinutes.c_str()) * 60;
    szHour = vCalTimeZone.substr(0, 2);
    offset = offset + atoi(szHour.c_str()) * 3600 ;

    if(is_negative) {
        offset = 0 - offset;
    }

    return offset;

}
void readinfo()
{
    string line;
    string path;
    string zonedir = "/usr/share/libical/zoneinfo/";

//    system("find \"/usr/share/libical/zoneinfo\" -name \"*.ics\" > \"/tmp/info.tmp\"");
    system("awk '{ print $3 }' \"/usr/share/libical/zoneinfo/zones.tab\" > \"/tmp/info.tmp\"");

    ifstream zone("/tmp/info.tmp");

    if(zone.is_open()) {
        while(!zone.eof()) {
            getline(zone, line);

            if(!line.empty()) {
                line = line.append(".ics");
                path = zonedir + line;
                readTzInfo(path);
                path.clear();
            }
        }

        zone.close();
    }
}


int readTzInfo(string file)
{
    char str[100];
    string temp;
    fstream file_op(file.c_str(), ios::in);

    while(!file_op.eof()) {
        file_op.getline(str, 100);
        temp.append(str);
        temp.append(" ");
    }

    file_op.close();
    string loc;
    int e = temp.find("X-LIC-LOCATION:");
    loc = temp.substr(e + 14, 40);
    e = loc.find(" ");
    loc = loc.substr(0, e);
    int t1 = temp.find("BEGIN:DAYLIGHT");
    int t2 = temp.find("END:DAYLIGHT");
    string Day, DayOffsetTo, DayDst;
    int t3 = -1, t4 = -1, iDayOffsetTo = 0;

    if(t1 > 0 && t2 > 0 && t1 < t2) {
        string Day = temp.substr(t1, t2 - t1);
        t3 = Day.find("DTSTART:");
        DayDst = Day.substr(t3 + 8, 15);
        t4 = Day.find("TZOFFSETTO:");
        DayOffsetTo = Day.substr(t4 + 11, 5);
        iDayOffsetTo = timeinsec(DayOffsetTo);
    }

    int s1 = temp.find("BEGIN:STANDARD");
    int s2 = temp.find("END:STANDARD");
    string Std = temp.substr(s1, s2 - s1);
    int s3 = Std.find("DTSTART:");
    string StdDst = Std.substr(s3 + 8, 15);
    int s4 = Std.find("TZOFFSETTO:");
    string StdOffsetTo = Std.substr(s4 + 11, 5);
    int iStdOffsetTo = timeinsec(StdOffsetTo);
    int dstflag = -1;

    if(t1 == -1 && t2 == -1) {
        dstflag = 0;
    }
    else {
        dstflag = 1;
    }

    char *pQuery = 0;
    asprintf(&pQuery, "insert into  Zoneinfo values ( \"%s\",%d, %d, %d,\"%s\",\"%s\")" , loc.c_str(), iStdOffsetTo, iDayOffsetTo, dstflag, DayDst.c_str(), StdDst.c_str());
    pCalDb ->execSQL(pQuery);
    free(pQuery);
    Std.clear();
    StdDst.clear();
    StdOffsetTo.clear();
    iStdOffsetTo = 0;
    Day.clear();
    DayDst.clear();
    DayOffsetTo.clear();
    iDayOffsetTo = 0;
    return 0;
}
int main()
{
    char *szQuery = 0;
    char *szErr_msg = 0;
    int iRet = 0;
    CCalendarDB *pCalendarDb = CCalendarDB::Instance();

    if(pCalendarDb == 0 || pCalDb == 0) {
        printf("Failed to create Calendar database ");
        exit(1);
    }

    szQuery = (CREATE_TIMEZONE);
    iRet = sqlite3_exec(pCalendarDb->getDb(), szQuery, 0, 0, &szErr_msg);

    if(iRet != 0) {
        printf("Failed to create Calendar timezone table ");
        exit(1);
    }

    pCalDb ->execSQL("create table if not exists Zoneinfo(Location TEXT,StdOffset INTEGER, DayLightOffset INTEGER ,DstFlag INTEGER, DaylightSwitch TEXT,StandardSwitch TEXT)");
    readinfo();
}
