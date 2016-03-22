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

#include <iostream>
#include <calendar-backend/CMulticalendar.h>
#include <calendar-backend/Common.h>
#include <string>

using namespace std;
using std::cout;
using std::endl;
using std::string;
int main(int argc, char *argv[])
{
    /*********** To compile
       g++ import-tool.cpp -o importexe `pkg-config calendar-backend dbus-1 --cflags --libs`
     **********/

    cout << "##################################################################" << endl;
    cout << "###                                                            ###" << endl;
    cout << "###                                                            ###" << endl;
    cout << "###	        WELCOME TO  IMPORT  TOOL	               ###" << endl;
    cout << "###               FOR CALENDAR-ENGINE                          ###" << endl;
    cout << "###                                                            ###" << endl;
    cout << "###                                                            ###" << endl;
    cout << "### Copyright (C) 2008 Nokia Corporation. All rights reserved. ###" << endl;
    cout << "###                                                            ###" << endl;
    cout << "### Contact: Biris Ilias <ilias.biris@nokia.com>               ###" << endl;
    cout << "###                                                            ###" << endl;
    cout << "##################################################################" << endl;
    cout << " BY DEFAULT ALL THE ENTRIES WILL BE IMPORTED TO SYNCHRONIZED CALENDAR " << endl;

    string temp;
    int pErrorCode ;
    int countTask = 0, countEvent = 0, countJournal = 0;
    CMulticalendar *pMc  = 0;

    if(argc != 2) {
        cout << "Too few arguments " << endl;
        return 1;
    }

    /* Cmulticalendar object is instanciated here */
    pMc = CMulticalendar::MCInstance();
    pMc->importIcsFileData(string(argv[1]), 1, temp, countTask, countEvent, countJournal, pErrorCode);

    if(pErrorCode != 500) {
        cout << "Error is " << pErrorCode << "While importing file name: " << argv[1] << endl;

    }

    /* CMulticalendar object is destroyed here */
    pMc->MCdestroy();

}
