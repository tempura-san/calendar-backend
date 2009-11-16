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
const char CHIME[] = "";
const char SYNCHRONIZED_CAL[] = "sync_va_calendar_synced";
const char PRIVATE_CAL[] = "!!Private";
using namespace std;
using std::cout;
using std::endl;
using std::string;
int main()
{    
	/*********** To compile 
	   g++ perf-tool.cpp -o perfexe `pkg-config calendar-backend dbus-1 --cflags --libs`
	 **********/  

	cout <<"##################################################################"<<endl;
	cout <<"###                                                            ###"<<endl;
	cout <<"###                                                            ###"<<endl;
	cout <<"###	        WELCOME TO  PERFORMACE  TOOL	               ###"<<endl;
	cout <<"###                  FOR CALENDAR-ENGINE                       ###"<<endl;
	cout <<"###                                                            ###"<<endl;
	cout <<"###                                                            ###"<<endl;
	cout <<"### Copyright (C) 2008 Nokia Corporation. All rights reserved. ###"<<endl;
	cout <<"###                                                            ###"<<endl;
	cout <<"### Contact: Biris Ilias <ilias.biris@nokia.com>               ###"<<endl;
	cout <<"###                                                            ###"<<endl;
	cout <<"##################################################################"<<endl;
	cout << " READ ME PLEASE:-): \n"<<endl;
	cout <<"##################################################################"<<endl;
	cout << " $$    - By default there are 3 calendars                       $$" <<endl;
	cout << " $$    - All events/tasks /journals will be                     $$"<<endl;  
	cout << " $$      added to these calendars                               $$"<<endl;
	cout <<"##################################################################"<<endl;


	string temp;
	cout << " Please select one action from the set below " << endl;
	cout << " 1. Add calendars: " <<endl;
	cout << " 2. Add Events      " <<endl;
	cout << " 3. Add Tasks       " <<endl;
	cout << " 4. Add Journals    " <<endl;
	cout << "Enter your choice here  : "<<endl;
	cin >> temp;
	int choice = atoi(temp.c_str());
	int i = 0;

	if (choice < 1 || choice > 4 ){
		cout << " Invalid option entered so exiting from here \n"<<endl;
		exit(1);
	}
	CMulticalendar* pMc  = 0;
	/* Cmulticalendar object is instanciated here */
	pMc = CMulticalendar::MCInstance();
	string s;
	int k;
	time_t t= time(0);
	CCalendar* pCal= pMc->getCalendarById(1,k);
	CEvent *pEvent = new CEvent("summary", "description",
			"location",time(0) ,(time(0)+100) );
	CTodo *pTodo = new CTodo("Task summary", time(0), 0);
	CJournal *pJournal = new CJournal();
	pJournal->setSummary("Journal Summary");
	int errval ;	
	int count = 0; 	
       /* Make sure that three calendars are added here 
	*/

	int pErrorCode = 0;
	CCalendar* pCal1 =  new CCalendar(SYNCHRONIZED_CAL,COLOUR_RED,
			0,1,DEFAULT_SYNC,CHIME,"1.0");
	CCalendar* pCal2 =  new CCalendar(PRIVATE_CAL,COLOUR_GREEN,
			0,1,DEFAULT_PRIVATE,CHIME,"1.0");
	CCalendar* pCal3 =  new CCalendar("Cal1",COLOUR_YELLOW,
			0,1,LOCAL_CALENDAR,CHIME,"1.0");

	pMc->addCalendar (pCal1,pErrorCode); 
	pMc->addCalendar(pCal2,pErrorCode); 
	pMc->addCalendar(pCal3,pErrorCode); 
        char *str ;


	switch (choice)
	{
		case (1):	
			cout << " Please enter number of calendars to be added : " << endl;
			cin >> temp;
			i =atoi(temp.c_str());
			static int calendarid = 2;
			while(i>0)
			{
				char *str ;
				asprintf(&str,"Cal %d %d",time(0),calendarid++);
	                        CCalendar* ptCal =  new CCalendar("C ",COLOUR_YELLOW,
			                               0,1,LOCAL_CALENDAR,CHIME,"1.0");

				ptCal->setCalendarName(str);
				pMc->addCalendar(ptCal,pErrorCode);
				cout << " ErrorCode is:"<<pErrorCode<<endl;
				free(str);
				i--;
			}
			break;
		case (2):

			cout << "please select number of events to be added  :\n"<<endl;
			cin >> count;
			while (count >0)
			{
				static int c = 1;
				t= t+ 3600;
				pEvent->setDateStart(t);
				pEvent->setDateEnd(t+100);
				asprintf(&str,"Event %d %d" ,time(0),c++);
				pEvent->setSummary(str);
				free(str);

				if  (count % 3 == 0)
					pMc->addEvent(pEvent, 1, errval);
				else if(count %3 == 1)
					pMc->addEvent(pEvent,2, errval);
				else
					pMc->addEvent(pEvent, 3, errval);
				count--;
			}
			break;
		case (3):

			cout << "please select number of Tasks to be added  :\n"<<endl;
			count = 0;
			t = time (0);
			cin>> count ;
			while (count >0)
			{
				static int d = 1 ;
				t= t+ 15000;
				pTodo->setDateStart(t);
				pTodo->setDateEnd(t+100);
				asprintf(&str,"Task %d %d" ,time(0),d++);
				pTodo->setSummary(str);
				if  (count % 3 == 0)
					pMc->addTodo(pTodo, 1, errval);
				else if(count %3 == 1)
					pMc->addTodo(pTodo, 2, errval);
				else
					pMc->addTodo(pTodo, 3, errval);
				count--;
			}

			break;
		case (4):

			cout << "please select number of journals  to be added  :\n"<<endl;
			t = time (0);
			count = 0;
			cin>> count ;
			while (count >0)
			{
				t= t+ 15000;
				pJournal->setDateStart(t);
				pJournal->setDateEnd(t+100);
				if  (count % 3 == 0)
					pMc->addJournal(pJournal, 1, errval);
				else if(count %3 == 1)
					pMc->addJournal(pJournal, 2, errval);
				else
					pMc->addJournal(pJournal, 3, errval);
				count--;
			}
			break;
		default:
			cout << " cant do anything here \n"<<endl;
	};

if (pEvent)
	delete pEvent;
if (pTodo)
	delete pTodo;
if (pJournal)
	delete pJournal;

  pMc->MCdestroy();

}
