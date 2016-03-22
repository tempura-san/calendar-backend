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

#include <string.h>
#include<iostream>
#include<vector>
#include<algorithm>
#include "CMulticalendar.h"
#include "ICalConverter.h"
#include "VCalConverter.h"
#include "Common.h"
#include "CBdayEvent.h"
#include <fcntl.h>
#include <sys/types.h>
using namespace std;
using std::string;
using std::vector;
int pErrorCode;
CMulticalendar *CMulticalendar::pMc =  CMulticalendar::MCInstance();
void modifyLocalEvent(CCalendar *);
void modifyLocalTodo(CCalendar *);
void modifyLocalJournal(CCalendar *);
bool addRecurrence(CEvent *pEvent);
std::string generateRrule(int repeattype, time_t until);
string generateRrule(int repeattype, time_t until)
{

    string s1("FREQ=");
    string s2(";UNTIL=");
    string test[5] =
    { "DAILY", "WEEKLY;BYDAY=MO,TU,WE,TH,FR", "WEEKLY", "MONTHLY", "YEARLY" };
    string result;
    struct icaltimetype uical;

    uical = icaltime_from_timet(until, 0);
    string ustring = icaltime_as_ical_string(uical);
    result = s1 + test[repeattype - 1] + s2 + ustring;

    CAL_DEBUG_LOG("Result is %s\n", result.c_str());

    return result;
}



bool addRecurrence(CEvent *pEvent)
{
    char *test = new char[20];
    bool loopexit = false;

    while(!loopexit) {
        cout << "please enter the recurrence type  :" << endl;
        cout << "1. Daily recurrence " << endl;
        cout << "2. WeekDay recurrence" << endl;
        cout << "3. Weekly Recurrence " << endl;
        cout << "4. Monthly Recurrence" << endl;
        cout << "5.Yearly Recurrence" << endl;
        cout << "option here :";
        cin >> test ;

        if(atoi(test) > 5 && atoi(test) < 1) {
            cout << "Invalid option entered onmly 1-5 are supported:" << endl;
        }
        else {


            CRecurrence *pRecur = new CRecurrence();
            int repeattype = atoi(test);
            time_t until = 0;
            vector <string> rules;
            rules.clear();
            cout << "please enter the until time in UTC 1 for \n" << "default value CurrentDate + 3months:\n" << endl;
            cin >> test;
            until = atoi(test);

            if(atoi(test) == 1) {
                until = time(NULL) + 7776000;
            }

            string rrule(generateRrule(repeattype, until));
            rules.push_back(rrule);

            pEvent->setRtype(repeattype);
            pEvent->setUntil(until);
            /* there will be only one rule added from calendar UI
             * also of the prototype
             * FREQ  ...;UNTIL ....;
             * Complex recurrence events ie events added from sync
             * can be displayed in Calendar-UI . If user tries to
             * modify such complex recurrence events then all exceptions
             * for such events will be lost in DB
             **/
            pRecur->setRrule(rules);
            pEvent->setRecurrence(pRecur);
            loopexit = true;
            delete pRecur;
            pRecur = 0 ;
        }

        delete test;
        test = 0;


    }

    return true;


}

void addCal(CMulticalendar *pMcal)
{
    CCalendar *pCal = new CCalendar();
    //int iI;
    int iRes;
    int iVis, iRo;

    cout << "Please enter the Calendar details" << endl;


    cout << "enter calendar name" << endl;
    string szName ;
    cin >> szName;


    cout << "calendar colours available are " << endl;
    cout << "0- RED " << endl << "1 GREEN" << endl << "2 BLUE" << endl <<
         "3 PURPLE" << endl << "4 YELLOW" << endl;
    cout << "enter calendar color in range 0- 4 \n" << endl;
    int Color ;
    cin >> Color;

    cout << "readonly iStatus 0 or 1 bool \n" << endl;
    cin >> iRo;

    cout << " set visibility  0 or 1  bool\n" << endl;
    cin >> iVis;
    cout << "enter Calendar Tune " << endl;
    string szTune;
    cin >> szTune;
    cout << "enter Calendar Version " << endl;
    string ver;
    cin >> ver;

    cout << "Adding calendar data in to DB" << endl;
    CCalendar *pCa = new CCalendar(szName, (CalendarColour)Color, iRo, iVis, (CalendarType)0, szTune, ver);
    iRes = pMcal->addCalendar(pCa, pErrorCode);
    cout << " adding  " << iRes << endl;
    delete pCal;

}

void deleteCal(CMulticalendar *pMcal)
{
    vector <CCalendar *> vC;
    unsigned int iI;
    int iId;
    vC = pMcal->getListCalFromMc();
    cout << " The calendars which can be deleted are " << endl;

    for(iI = 0; iI < vC.size(); iI++) {
        cout << vC[iI]->getCalendarId() << endl;
    }

    cout << " Select the id of the calendar to be deleted" << endl;
    cin >> iId;
    cout << "del =" << iId << endl;
    pMcal->deleteCalendar(iId, pErrorCode);
}

void modifyCal(CMulticalendar *pMcal)
{
    vector <CCalendar *> vC;
    unsigned int iI;
    CCalendar *pCa = NULL;

    bool bModloopexit = false;
    int iId;
    vC = pMcal->getListCalFromMc();

    cout << " The calendars which can be modified are " << endl;

    for(iI = 0; iI < vC.size(); iI++) {
        cout << vC[iI]->getCalendarId() << endl;
    }

    cout << " Select the id of the calendar to be modified" << endl;
    cin >> iId;

    for(iI = 0; iI < vC.size(); iI++) {
        if(iId == vC[iI]->getCalendarId()) {
            pCa = vC[iI];
            break;
        }
    }

    if(pCa == NULL) {
        delete pCa;
        return;
    }

    cout << pCa->getCalendarId() << endl;

    while((1) && (bModloopexit == false)) {
        string szName;
        int Color;
        int iRw, iVi;
        int iChoice;
        cout << "1.Name\n" << endl;
        cout << "2.Color\n" << endl;
        cout << "3.Visibility\n" << endl;
        cout << "4.ReadOnly\n" << endl;
        cout << "5.Calendar Tune\n" << endl;
        cout << "6.exit\n" << endl;

        cout << "select the parameter to modify" << endl;
        cin >> iChoice;

        switch(iChoice) {
        case 1: {
            cout << "enter modified name " << endl;
            cin >> szName;
            pCa->setCalendarName(szName);
            break;
        }

        case 2: {
            cout << "calendar colours available are " << endl;
            cout << "0- RED " << endl << "1 GREEN" << endl << "2 BLUE" << endl <<
                 "3 PURPLE" << endl << "4 YELLOW" << endl;
            cout << "enter calendar color in range 0- 4 \n" << endl;
            cin >> Color;
            pCa->setCalendarColor((CalendarColour)Color);
            break;
        }

        case 3: {
            cout << "change visibility to " << endl;
            cin >> iVi;
            pCa->setCalendarShown(iVi);
            break;
        }

        case 4: {
            cout << "change readonly " << endl;
            cin >> iRw;
            pCa->setCalendarReadOnly(iRw);
            break;
        }

        case 5: {
            cout << "change Calendar Tune " << endl;
            string szTune;
            cin >> szTune;
            pCa->setCalendarTune(szTune);
            break;
        }

        case 6: {
            pCa->setCalendarVersion("version");
            pCa->setProfileType((SyncProfileType)3);
            pMcal->modifyCalendar(pCa, pErrorCode);
            bModloopexit = true;
            exit(0);

        }

        default:
            ;
        }//end of switch
    }//end of while



}

void getCalendars(CMulticalendar *pMcal)
{
    vector <CCalendar *> vC;
    unsigned int iI;

    cout << " Calendars are " << endl;
    vC = pMcal->getListCalFromMc();

    cout << "\n******************************************************************************************";
    cout << "\n\nNumber of Calendars = " << vC.size() << endl;

    for(iI = 0; iI < vC.size(); iI++) {
        cout << "| Id : " << vC[iI]->getCalendarId() << " | ";
        cout << "Name : " << vC[iI]->getCalendarName() << " | " ;
        cout << "Color : " << vC[iI]->getCalendarColor() << " | ";
        cout << "Visibility : " << vC[iI]->IsShown() << " | ";
        cout << "Readonly : " << vC[iI]->IsReadOnly() << " | ";
        cout << "Calendar type : " << vC[iI]->getCalendarType() << endl;
        cout << "Calendar tune : " << vC[iI]->getCalendarTune() << endl;
        cout << "Calendar Version : " << vC[iI]->getCalendarVersion() << endl;
    }

    cout << endl;
    cout << "******************************************************************************************\n\n";
}

bool addBirthDay()
{
    bool bRet;
    CMulticalendar *mc = CMulticalendar::MCInstance();

    string sUid, sfname;
    int date;

    cout << "Enter Birthday details to add" << endl;

    cout << "Enter UID" << endl;
    std :: cin >> sUid;

    cout << "Enter First name" << endl;
    std :: cin >> sfname;

    cout << "Enter Birthdate" << endl;
    std :: cin >> date;

    CBdayEvent *pBday = new CBdayEvent(sUid, sfname, date);
    bRet = mc->addBirthDay(pBday, pErrorCode);


    return bRet;
}


bool modifyBirthDay()
{
    bool bRet = false;
    CMulticalendar *mc = CMulticalendar::MCInstance();

    string sUid, sfname;
    int date = 0;

    cout << "Enter Birthday details to modify" << endl;

    cout << "Enter UID" << endl;
    std :: cin >> sUid;

    cout << "Enter First name" << endl;
    std :: cin >> sfname;

    cout << "Enter Birthdate" << endl;
    std :: cin >> date;

    CBdayEvent *pBday = new CBdayEvent(sUid, sfname, date);
    mc->modifyBirthDay(pBday, pErrorCode);

    return bRet;
}


bool deleteBirthDay()
{
    bool bRet = false;
    string sUid;
    CMulticalendar *mc = CMulticalendar::MCInstance();

    cout << "Enter UID" << endl;
    std :: cin >> sUid;

    mc->deleteBirthDay(sUid, pErrorCode);

    return bRet;
}

void addLocalEvent(CCalendar *pCal)
{

    CEvent  *pCe = new CEvent();
    COrganizer *pOrg = new COrganizer();
    CMulticalendar *mc = CMulticalendar::MCInstance();


    pCe->setType(1);

    //pCe->setCalendarId(pCal->getCalendarId());

    //pCe->setFlags(3);

    cout << "enter organizer details" << endl;
    string szLang ;
    cout << "enter Language" << endl;
    std :: cin >> szLang;
    pOrg->setLanguage(szLang);

    string szSentBy ;
    cout << "enter SentBy\n" << endl;
    cin >> szSentBy;
    pOrg->setSentBy(szSentBy);

    string  szCommonName;
    cout << "enterCommonName\n" << endl;
    std ::cin >> szCommonName;
    pOrg->setCommonName(szCommonName);

    string  szXpro;
    cout << "enter Xparamters\n" << endl;
    cin >> szXpro;
    pOrg->setXparameters(szXpro);

    string  szDirPar;
    cout << "enter DirectoryParameter\n" << endl;
    cin >> szDirPar;
    pOrg-> setDirectoryParameter(szDirPar);

    int stDateStart, stDateEnd, iStatus;
    cout << "enter stDateStart " << endl;
    cin >> stDateStart;
    pCe->setDateStart(stDateStart);

    cout << "enter stDateEnd" << endl;
    cin >> stDateEnd;
    pCe->setDateEnd(stDateEnd);

    cout << "enter iStatus" << endl;
    cin >> iStatus;
    pCe->setStatus(iStatus);

    pCe->setOrganizer(pOrg);


    int iNoat = 0;
    vector <string > vAttach;
    cout << " enter No. of attachments for the event ( 0 or more)" << endl;
    cin >> iNoat;

    for(int iI = 0; iI < iNoat; iI++) {
        //	char * szAttachment = new char[100];
        string szAttachment;
        cout << "enter the attachment " << (iI + 1) << endl;
        cin >> szAttachment;
        vAttach.push_back(szAttachment);
    }

    if(iNoat > 0) {
        if(pCe->setAttachments(vAttach) == true) {
            cout << "attachments set correctly" << endl;
        }
    }

    string  szClas ;
    cout << "enter the class" << endl;
    cin >> szClas;
    pCe->setClas(szClas);
    pCe->setFlags(3);
    int iNo;
    vector <CAttendee *> vAttendees;
    cout << "enter no. of vAttendees for the event (0 or more)" << endl;
    cin >> iNo;

    for(int iI = 0; iI < iNo; iI++) {
        CAttendee *pAtten = new CAttendee();
        cout << endl;

        cout << "enter the attendee " << (iI + 1) << " details" << endl;

        string szUserType ;
        cout << " enter calendar szUserType" << endl;
        cin >> szUserType;
        pAtten->setCalendarUserType((CalendarUserType)atoi(szUserType.c_str()));

        string szMember ;
        cout << " enter szMember" << endl;
        cin >> szMember;
        pAtten->setMember(szMember);

        int szRole ;
        cout << " enter szRole" << endl;
        cin >> szRole;
        pAtten->setRole((ParticipantRole)szRole);

        int szPartStat;
        cout << " enter participation iStatus" << endl;
        cin >> szPartStat;
        pAtten->setParticipationStatus((ParticipantStatus)szPartStat);

        //bool szRSVP ;
        //cout<< " enter szRSVP"<<endl;
        //cin >> szRSVP;
        pAtten->setRSVP(true);

        string  szDelegatees ;
        cout << " enter szDelegatees" << endl;
        cin >> szDelegatees;
        pAtten->setDelegatees(szDelegatees);

        string  szDelegator ;
        cout << " enter Delegator" << endl;
        cin >> szDelegator;
        pAtten-> setDelegator(szDelegator);

        vAttendees.push_back(pAtten);
        pCe->setAttendees(vAttendees);
    }

    pCe->setId("dummy")	;

    string szSummary ;
    cout << "enter szSummary " << endl;
    cin >> szSummary;
    pCe->setSummary(szSummary);
    string szDescr;
    cout << "enter description" << endl;
    cin >> szDescr;
    pCe->setDescription(szDescr);
    string szLoca;
    cout << "enter location" << endl;
    cin >> szLoca;
    pCe->setLocation(szLoca);
    int iSeq, iUid, iPriority;
    cout << "enter sequence" << endl;
    cin >> iSeq;
    pCe->setSequence(iSeq);
    cout << "enter iPriority" << endl;
    cin >> iPriority;
    pCe->setPriority(iPriority);
    cout << "enter iUid" << endl;
    cin >> iUid;
    pCe->setUid(iUid);
    string szCateg;
    cout << "enter category" << endl;
    cin >> szCateg;
    pCe->setCategories(szCateg);
    string szUr;
    cout << "enter url" << endl;
    cin >> szUr;
    pCe->setUrl(szUr);
    string  szComm;
    cout << "enter comments" << endl;
    cin >> szComm;
    pCe->setComments(szComm);
    string  szCon;
    cout << "enter contacts" << endl;
    cin >> szCon;
    pCe->setContact(szCon);
    string  szRel;
    cout << "enter related" << endl;
    cin >> szRel;
    pCe->setRelated(szRel);
    string szRes;
    cout << "enter resources" << endl;
    cin >> szRes;
    pCe->setResources(szRes);
    string szXpr;
    cout << "enter xproperties" << endl;
    cin >> szXpr;
    //pCe->setXProperties(szXpr);
    string szTra;
    cout << "enter transparency" << endl;
    cin >> szTra;
    pCe->setTransparency(szTra);
    string szGeo;
    cout << "enter szGeo" << endl;
    cin >> szGeo;
    pCe->setGeo(szGeo);

    /**
     * following piece of code  is for
     * recurrence support in Calendar-backend
     */
    char *temp = new char[100];
    bool ret;
    cout << "Would you like to make it a recursive event 1-yes/any other for no:" << endl;
    cin >> temp;

    if(atoi(temp) == 1) {
        addRecurrence(pCe);


    }


    //pCe=new CEvent(11,1,pCal->getCalendarId(),1,szSummary,szDescr,szLoca,1,45647,78965,34567,4,5,pOrg,vAttendees,vAttach,iSeq,iUid,szUr,szCateg,(wchar_t *)szComm,(wchar_t*)szCon,(wchar_t*)szRel,szRes,szXpr,szGeo,szTra,iPriority);
    ret = mc->setAutocommitOff();

    if(ret) {
        pCal->addEvent(pCe, pErrorCode);
        mc->commitAllChanges();
    }

    delete pCe;

}
void addLocalTodo(CCalendar *pCal)
{
    CTodo  *pCt = new CTodo();
    COrganizer *pOrg = new COrganizer();

    pCt->setType(2);

    //pCt->setCalendarId(pCal->getCalendarId());

    pCt->setFlags(3);

    cout << "enter organizer details" << endl;
    string szLang ;
    cout << "enter Language" << endl;
    std :: cin >> szLang;
    pOrg->setLanguage(szLang);

    string  szSentBy ;
    cout << "enter SentBy\n" << endl;
    cin >> szSentBy;
    pOrg->setSentBy(szSentBy);

    string  szCommonName;
    cout << "enterCommonName\n" << endl;
    std ::cin >> szCommonName;
    pOrg->setCommonName(szCommonName);

    string  szXpro;
    cout << "enter Xparamters\n" << endl;
    cin >> szXpro;
    pOrg->setXparameters(szXpro);

    string  szDirPar;
    cout << "enter DirectoryParameter\n" << endl;
    cin >> szDirPar;
    pOrg-> setDirectoryParameter(szDirPar);

    pCt->setOrganizer(pOrg);

    int iComp;
    cout << "enter Completed\n" << endl;
    cin >> iComp;
    pCt->setCompleted(iComp);

    int iPercom;
    cout << "enter percent complete\n" << endl;
    cin >> iPercom;
    pCt->setPercentComplete(iPercom);

    int iPri;
    cout << "enter iPriority\n" << endl;
    cin >> iPri;
    pCt->setPriority(iPri);

    int stDue;
    cout << "enter stDue\n" << endl;
    cin >> stDue;
    pCt->setDue(stDue);
    string szGe;
    cout << "enter szGeo" << endl;
    cin >> szGe;
    pCt->setGeo(szGe);
    string szSummary;
    cout << "enter szSummary " << endl;
    cin >> szSummary;
    pCt->setSummary(szSummary);
    string szDescr;
    cout << "enter description" << endl;
    cin >> szDescr;
    pCt->setDescription(szDescr);
    string szLoca;
    cout << "enter location" << endl;
    cin >> szLoca;
    pCt->setLocation(szLoca);
    int iSeq, iUid, iPriority;
    cout << "enter sequence" << endl;
    cin >> iSeq;
    pCt->setSequence(iSeq);

    cout << "enter iPriority" << endl;
    cin >> iPriority;
    pCt->setPriority(iPriority);
    cout << "enter iUid" << endl;
    cin >> iUid;
    pCt->setUid(iUid);
    string szCateg;
    cout << "enter category" << endl;
    cin >> szCateg;
    pCt->setCategories(szCateg);
    string szUr;
    cout << "enter url" << endl;
    cin >> szUr;
    pCt->setUrl(szUr);
    string  szComm;
    cout << "enter comments" << endl;
    cin >> szComm;
    pCt->setComments(szComm);
    string  szCon;
    cout << "enter contacts" << endl;
    cin >> szCon;
    pCt->setContact(szCon);
    string  szRel;
    cout << "enter related" << endl;
    cin >> szRel;
    pCt->setRelated(szRel);
    string szRes;
    cout << "enter resources" << endl;
    cin >> szRes;
    pCt->setResources(szRes);
    string szXpr;
    cout << "enter xproperties" << endl;
    cin >> szXpr;
//	pCt->setXProperties(szXpr);
    int iNo;
    vector <CAttendee *> vAttendees;
    cout << "enter no. of vAttendees for the event (0 or more)" << endl;
    cin >> iNo;

    for(int iI = 0; iI < iNo; iI++) {
        CAttendee *pAtten = new CAttendee();
        cout << endl;

        cout << "enter the attendee " << (iI + 1) << " details" << endl;

        string szUserType;
        cout << " enter calendar szUserType" << endl;
        cin >> szUserType;
        pAtten->setCalendarUserType((CalendarUserType)atoi(szUserType.c_str()));

        string szMember ;
        cout << " enter szMember" << endl;
        cin >> szMember;
        pAtten->setMember(szMember);

        int szRole ;
        cout << " enter szRole" << endl;
        cin >> szRole;
        pAtten->setRole((ParticipantRole) szRole);

        int  szPartStat;
        cout << " enter participation iStatus" << endl;
        cin >> szPartStat;
        pAtten->setParticipationStatus((ParticipantStatus)szPartStat);

        int  iRSVP ;
        cout << " enter RSVP 1 for true 0 for false" << endl;
        cin >> iRSVP;
        pAtten->setRSVP(iRSVP);

        string  szDelegatees;
        cout << " enter szDelegatees" << endl;
        cin >> szDelegatees;
        pAtten->setDelegatees(szDelegatees);

        string  szDelegator ;
        cout << " enter Delegator" << endl;
        cin >> szDelegator;
        pAtten-> setDelegator(szDelegator);

        vAttendees.push_back(pAtten);
        pCt->setAttendees(vAttendees);
    }

    int iNoat;
    vector <string > vAttach;
    cout << " enter No. of attachments for the event ( 0 or more)" << endl;
    cin >> iNoat;

    for(int iI = 0; iI < iNoat; iI++) {
        //char * szAttachment= new char[100];
        string szAttachment;
        cout << "enter the attachment " << (iI + 1) << endl;
        cin >> szAttachment;
        vAttach.push_back(szAttachment);
    }

    cout << "SSSSSSSSSSSSSSSS" << vAttach.size() << endl;

    if(pCt->setAttachments(vAttach) == true) {
        cout << "TTTTTTTTT" << endl;
    }

    pCal->addTodo(pCt, pErrorCode);


}

void addLocalJournal(CCalendar *pCal)
{
    string  szDescr;
    cout << "enter Description\n" << endl;
    cin >> szDescr;
    CJournal *pCe = new CJournal();
    pCe->setDescription(szDescr);
    pCe->setFlags(1);
    pCal->addJournal(pCe, pErrorCode);

}

void modifyLocalJournal(CCalendar *pCal)
{
    CMulticalendar *pCm = CMulticalendar::MCInstance();
    CComponent *pComp = NULL;
    vector <CComponent *> vComplist = pCm->getComponents(pCal->getCalendarId(), 3, -1, -1, pErrorCode);
    cout << "the size is " << vComplist.size() << endl;
    cout << pCal->getCalendarId() << endl;

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        cout << vComplist[iI]->getId() << endl;
    }

    string sId;
    cout << "enter the id  of the journal  which  is to be modified " << endl;
    cin >> sId;

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        if(vComplist[iI]->getId() == sId) {
            pComp = vComplist[iI];
            break;
        }
    }

    if(pComp == NULL) {
        delete pComp;
        return;
    }

    CJournal *pCj = pCal->getJournal(pComp->getId(), pErrorCode);
    int iCho;
    string  szDescr ;
    cout << "enter choice" << endl;
    cout << "1.modify Description 2.exit" << endl;
    cin >> iCho;

    if(iCho == 1) {
        cout << pCj->getDescription() << endl;
        cout << endl << "enter new description" << endl;
        cin >> szDescr;
        pCj->setDescription(szDescr);
        pCal->modifyJournal(pCj, pErrorCode);
    }

}
void modifyLocalEvent(CCalendar *pCal)
{
    CMulticalendar *pCm = CMulticalendar::MCInstance();
    CEvent *pCe;
    bool bModloopexit = false;
    CComponent *pComp = NULL;
    vector <CComponent *> vComplist = pCm->getComponents(pCal->getCalendarId(), 1, -1, -1, pErrorCode);
    cout << "the size is " << vComplist.size() << endl << endl;
    cout << "the calendar id is " << pCal->getCalendarId() << endl;

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        cout << vComplist[iI]->getId() << endl;
    }


    cout << "enter the id  of the event  which  is to be modified " << endl;
    string sId;
    cin >> sId;

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        if(sId == vComplist[iI]->getId()) {
            pComp = vComplist[iI];
            break;
        }
    }

    if(pComp == NULL) {
        delete pComp;
        return;
    }

    pCe = pCal->getEvent(sId, pErrorCode);

    COrganizer *pOrg = pCe->getOrganizer();
    ASSERTION(pOrg);

    while((1) && (bModloopexit == false)) {

        int iCho;
        cout << " modify event" << endl << "enter choice" << endl;
        cout << "1.organzer details\n 2.attachment details\n 3.attendee details\n 4.szSummary\n 5.description\n 6.location\n 7.sequence\n 8.iUid\n 9. iPriority \n10.category\n 11.url \n12.comments \n13.contacts \n14.related \n15.resources\n 16.xproperties\n 17.transparency \n18.szGeo\n 19 exit\n" << endl;
        cin >> iCho;

        switch(iCho) {
        case 1 : {
            int iSubch;
            cout << "modify organizer details" << endl << "enter choice" << endl;
            cout << "1.language \n 2.szSentBy \n 3.szCommonName \n 4.xparameters \n 5.directoryparameter \n 6.done" << endl;
            cin >> iSubch;

            switch(iSubch) {
            case 1: {

                string szLang ;
                cout << "enter Language" << endl;
                std :: cin >> szLang;
                pOrg->setLanguage(szLang);
                break;
            }

            case 2: {
                string  szSentBy ;
                cout << "enter SentBy\n" << endl;
                cin >> szSentBy;
                pOrg->setSentBy(szSentBy);
                break;
            }

            case 3: {
                string  szCommonName;
                cout << "enterCommonName\n" << endl;
                std ::cin >> szCommonName;
                pOrg->setCommonName(szCommonName);
                break;
            }

            case 4: {
                string  szXpro;
                cout << "enter Xparamters\n" << endl;
                cin >> szXpro;
                pOrg->setXparameters(szXpro);
                break;
            }

            case 5: {
                string  szDirPar ;
                cout << "enter DirectoryParameter\n" << endl;
                cin >> szDirPar;
                pOrg-> setDirectoryParameter(szDirPar);
                break;
            }

            case 6: {
                cout << " done with modifying organizer details" << endl;
                pCe->setOrganizer(pOrg);
                break;
            }

            default : {
                cout << "please enter proper choice from 1 to 6" << endl;
                break;
            }
            break;
            }

            case 2: {
                vector <string> vAttach;
                string szModatt;
                vAttach = pCe->getAttachments();

                for(unsigned int iI = 0; iI < vAttach.size(); iI++) {
                    cout << iI << "," << vAttach[iI] << endl;
                }

                unsigned int iAtcho;

                if(vAttach.size() > 0) {
                    cout << "enter the attachment number that you want to modify" << endl;
                    cin >> iAtcho;

                    for(unsigned int iI = 0; iI < vAttach.size(); iI++) {
                        if(iI == iAtcho) {
                            szModatt = vAttach[iI];
                            break;
                        }
                    }

                    //*  szMod = new char[1000];
                    string szMod;
                    cout << "modify the attachment" << endl;
                    cin >> szMod;
                    vAttach.push_back(szMod);
                    pCe->setAttachments(vAttach);
                }

                break;
            }

            case 3: {


                vector <CAttendee *> vAttendees;
                CAttendee *pAtten;
                vAttendees = pCe->getAttendees();

                for(unsigned int iI = 0; iI < vAttendees.size(); iI++) {
                    cout << "the vAttendees are :" << endl;
                    cout << iI << "." << endl;

                    cout << "UserType is " << endl << vAttendees[iI]->getCalendarUserType() << endl;
                    cout << "Member  is " << endl << vAttendees[iI]->getMember() << endl;
                    cout << "Role is " << endl << vAttendees[iI]->getRole();
                    cout << "participant Status is " << endl << vAttendees[iI]->getParticipationStatus();
                    cout << "RSVP is " << endl << vAttendees[iI]->getRSVP();
                    cout << "Delegatees is " << endl << vAttendees[iI]->getDelegatees();
                    cout << "Delegator is " << endl << vAttendees[iI]->getDelegator();
                    cout << endl << endl;
                }

                if(vAttendees.size() > 0) {
                    int iAttno;
                    cout << "enter the attendee no" << endl;
                    cin >> iAttno;

                    for(int iI = 0; iI < iAttno; iI++) {
                        if(iI == iAttno) {
                            pAtten = vAttendees[iI];
                            break;
                        }
                    }

                    int iModch = 0;

                    while(iModch < 8) {
                        cout << "modify " << endl << "1.user type \n2.szMember\n 3.szRole \n4.participantstatus \n5.szRSVP \n6.szDelegatees\n 7.delegators \n8.exit" << endl;
                        cin >> iModch;

                        switch(iModch) {
                        case 1: {
                            string szUserType ;
                            cout << " enter calendar szUserType" << endl;
                            cin >> szUserType;
                            pAtten->setCalendarUserType((CalendarUserType)atoi(szUserType.c_str()));
                            break;
                        }

                        case 2: {
                            string szMember ;
                            cout << " enter szMember" << endl;
                            cin >> szMember;
                            pAtten->setMember(szMember);
                            break;
                        }

                        case 3: {

                            int szRole ;
                            cout << " enter szRole" << endl;
                            cin >> szRole;
                            pAtten->setRole((ParticipantRole)szRole);
                            break;
                        }

                        case 4: {


                            int  szPartStat ;
                            cout << " enter participation iStatus" << endl;
                            cin >> szPartStat;
                            pAtten->setParticipationStatus((ParticipantStatus)szPartStat);
                            break;
                        }

                        case 5: {
                            int  iRSVP ;
                            cout << " enter szRSVP 1-true 0- false" << endl;
                            cin >> iRSVP;

                            pAtten->setRSVP(iRSVP);
                            break;
                        }

                        case 6: {
                            string  szDelegatees;
                            cout << " enter szDelegatees" << endl;
                            cin >> szDelegatees;
                            pAtten->setDelegatees(szDelegatees);
                            break;
                        }

                        case 7: {

                            string  szDelegator ;
                            cout << " enter Delegator" << endl;
                            cin >> szDelegator;
                            pAtten-> setDelegator(szDelegator);
                            break;
                        }

                        case 8: {
                            vAttendees.push_back(pAtten);
                            pCe->setAttendees(vAttendees);
                            break;
                        }

                        default : {
                            cout << "enter choice between 1 and 8" << endl;
                            break;
                        }
                        };
                    }

                }

                break;
            }

            case 4: {
                string szSummary ;
                cout << "enter szSummary " << endl;
                cin >> szSummary;
                pCe->setSummary(szSummary);
                pCe->getSummary();
                break;
            }

            case 5: {
                string szDescr;
                cout << "enter description" << endl;
                cin >> szDescr;
                pCe->setDescription(szDescr);
                break;
            }

            case 6: {
                string szLoca;
                cout << "enter location" << endl;
                cin >> szLoca;
                pCe->setLocation(szLoca);
                break;
            }

            case 7: {
                int iSeq;
                cout << "enter sequence" << endl;
                cin >> iSeq;
                pCe->setSequence(iSeq);
                break;
            }

            case 8: {

                int iPriority;
                cout << "enter iPriority" << endl;
                cin >> iPriority;
                pCe->setPriority(iPriority);
                break;
            }

            case 9: {
                int iUid;
                cout << "enter iUid" << endl;
                cin >> iUid;
                pCe->setUid(iUid);
                break;
            }

            case 10: {

                string szCateg;
                cout << "enter category" << endl;
                cin >> szCateg;
                pCe->setCategories(szCateg);
                break;
            }

            case 11: {

                string szUr;
                cout << "enter url" << endl;
                cin >> szUr;
                pCe->setUrl(szUr);
                cout << "in case 11" << endl << pCe->getUrl() << endl;
                break;
            }

            case 12: {

                string  szComm;
                cout << "enter comments" << endl;
                cin >> szComm;
                pCe->setComments(szComm);
                break;
            }

            case 13: {
                string  szCon;
                cout << "enter contacts" << endl;
                cin >> szCon;
                pCe->setContact(szCon);
                break;
            }

            case 14: {

                string  szRel;
                cout << "enter related" << endl;
                cin >> szRel;
                pCe->setRelated(szRel);
                break;
            }

            case 15: {
                string szRes;
                cout << "enter resources" << endl;
                cin >> szRes;
                pCe->setResources(szRes);
                break;
            }

            case 16: {
                string szXpr;
                cout << "enter xproperties" << endl;
                cin >> szXpr;
                //	pCe->setXProperties(szXpr);
                break;
            }

            case 17: {

                string szTra;
                cout << "enter transparency" << endl;
                cin >> szTra;
                pCe->setTransparency(szTra);
                break;
            }

            case 18: {

                string szGeo;
                cout << "enter szGeo" << endl;
                cin >> szGeo;
                pCe->setGeo(szGeo);
                break;
            }


            case 19: {
                pCal->modifyEvent(pCe, pErrorCode);
                cout << "SUCCESSSSS" << endl;

                bModloopexit = true;
                exit(0);

            }



        }
        }
    }


}
void deleteLocalEvent(CCalendar *pCal)
{
    CMulticalendar *pCm = CMulticalendar::MCInstance();
    vector <CComponent *> vComplist = pCm->getComponents(pCal->getCalendarId(), 1, -1, -1, pErrorCode);
    cout << "the size is " << vComplist.size() << endl;
    cout << "LLLLLLLLLLLLLL" << pCal->getCalendarId() << endl;

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        cout << vComplist[iI]->getId() << endl;
    }

    cout << "enter the id  of the event  which  is to be deleted " << endl;
    string sId;
    cin >> sId;
    cout << sId << endl;
    pCm->deleteEvent(pCal->getCalendarId(), sId, pErrorCode);
}
void modifyLocalTodo(CCalendar *pCal)
{
    CMulticalendar *pCm = CMulticalendar::MCInstance();
    CTodo *pCe;
    bool bModloopexit = false;
    CComponent *pComp = NULL;
    vector <CComponent *> vComplist = pCm->getComponents(pCal->getCalendarId(), 2, -1, -1, pErrorCode);
    cout << "the size is " << vComplist.size() << endl;
    cout << "LLLLLLLLLLLLLL" << pCal->getCalendarId() << endl;

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        cout << vComplist[iI]->getId() << endl;
    }

    cout << "enter the id  of the event  which  is to be modified " << endl;
    string sId;
    cin >> sId;

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        if(vComplist[iI]->getId() == sId) {
            pComp = vComplist[iI];
            break;
        }
    }

    if(pComp == NULL) {
        delete pComp;
        return;
    }

    pCe = pCal->getTodo(sId, pErrorCode);

    COrganizer *pOrg = pCe->getOrganizer();

    if(pOrg == NULL) {
        delete pOrg;
    }

    return;

    while((1) && (bModloopexit == false)) {

        int iCho;
        cout << " modify todo" << endl << "enter choice" << endl;
        cout << "1.organzer details\n 2.attachment details\n 3.attendee details\n 4.szSummary\n 5.description\n 6.location\n 7.sequence\n 8.iUid\n 9. iPriority \n10.category\n 11.url \n12.comments \n13.contacts \n14.related \n15.resources\n 16.xproperties\n 17.szGeo \n18.tododue\n 19 .completed\n 20. percentcomplete\n 21.iPriority\n 22.Exit" << endl;
        cin >> iCho;

        switch(iCho) {
        case 1 : {
            int iSubch;
            cout << "modify organizer details" << endl << "enter choice" << endl;
            cout << "1.language \n 2.szSentBy \n 3.szCommonName \n 4.xparameters \n 5.directoryparameter \n 6.done" << endl;
            cin >> iSubch;

            switch(iSubch) {
            case 1: {

                string szLang ;
                cout << "enter Language" << endl;
                std :: cin >> szLang;
                pOrg->setLanguage(szLang);
                break;
            }

            case 2: {
                string  szSentBy ;
                cout << "enter SentBy\n" << endl;
                cin >> szSentBy;
                pOrg->setSentBy(szSentBy);
                break;
            }

            case 3: {
                string  szCommonName;
                cout << "enterCommonName\n" << endl;
                std ::cin >> szCommonName;
                pOrg->setCommonName(szCommonName);
                break;
            }

            case 4: {
                string  szXpro;
                cout << "enter Xparamters\n" << endl;
                cin >> szXpro;
                pOrg->setXparameters(szXpro);
                break;
            }

            case 5: {
                string  szDirPar ;
                cout << "enter DirectoryParameter\n" << endl;
                cin >> szDirPar;
                pOrg-> setDirectoryParameter(szDirPar);
                break;
            }

            case 6: {
                cout << " done with modifying organizer details" << endl;
                pCe->setOrganizer(pOrg);
                break;
            }

            default : {
                cout << "please enter proper choice from 1 to 6" << endl;
                break;
            }
            break;
            }

            case 2: {
                vector < string > vAttach;
                string szModatt;
                vAttach = pCe->getAttachments();

                for(unsigned int iI = 0; iI < vAttach.size(); iI++) {
                    cout << iI << "," << vAttach[iI] << endl;
                }

                unsigned int iAtcho;

                if(vAttach.size() > 0) {
                    cout << "enter the attachment number that you want to modify" << endl;
                    cin >> iAtcho;

                    for(unsigned int iI = 0; iI < vAttach.size(); iI++) {
                        if(iI == iAtcho) {
                            szModatt = vAttach[iI];
                            break;
                        }
                    }

                    //	char * szMod = new char[1000];
                    string szMod;
                    cout << "modify the attachment" << endl;
                    cin >> szMod;
                    vAttach.push_back(szMod);
                    pCe->setAttachments(vAttach);
                }

                break;
            }

            case 3: {


                vector <CAttendee *> vAttendees;
                CAttendee *pAtten;
                vAttendees = pCe->getAttendees();

                for(unsigned int iI = 0; iI < vAttendees.size(); iI++) {
                    cout << "the vAttendees are :" << endl;
                    cout << iI << "." << endl;

                    cout << "UserType is " << endl << vAttendees[iI]->getCalendarUserType();
                    cout << "Member  is " << endl << vAttendees[iI]->getMember();
                    cout << "Role is " << endl << vAttendees[iI]->getRole();
                    cout << "participant Status is " << endl << vAttendees[iI]->getParticipationStatus();
                    cout << "RSVP is " << endl << vAttendees[iI]->getRSVP();
                    cout << "Delegatees is " << endl << vAttendees[iI]->getDelegatees();
                    cout << "Delegator is " << endl << vAttendees[iI]->getDelegator();
                    cout << endl << endl;
                }

                if(vAttendees.size() > 0) {
                    int iAttno;
                    cout << "enter the attendee no" << endl;
                    cin >> iAttno;

                    for(int iI = 0; iI < iAttno; iI++) {
                        if(iI == iAttno) {
                            pAtten = vAttendees[iI];
                            break;
                        }
                    }

                    int iModch = 0;
                    cout << "modify " << endl << "1.user type 2.szMember 3.szRole 4.participant iStatus 5.szRSVP 6.szDelegatees 7.delegators 8.exit" << endl;

                    switch(iModch) {
                    case 1: {
                        string szUserType;
                        cout << " enter calendar szUserType" << endl;
                        cin >> szUserType;
                        pAtten->setCalendarUserType((CalendarUserType)atoi(szUserType.c_str()));
                        break;
                    }

                    case 2: {
                        string szMember;
                        cout << " enter szMember" << endl;
                        cin >> szMember;
                        pAtten->setMember(szMember);
                        break;
                    }

                    case 3: {

                        int szRole ;
                        cout << " enter szRole" << endl;
                        cin >> szRole;
                        pAtten->setRole((ParticipantRole)szRole);
                        break;
                    }

                    case 4: {


                        int szPartStat;
                        cout << " enter participation iStatus" << endl;
                        cin >> szPartStat;
                        pAtten->setParticipationStatus((ParticipantStatus)szPartStat);
                        break;
                    }

                    case 5: {
                        int  iRSVP ;
                        cout << " enter szRSVP 1 true 0 false" << endl;
                        cin >> iRSVP;
                        pAtten->setRSVP(iRSVP);
                        break;
                    }

                    case 6: {
                        string  szDelegatees;
                        cout << " enter szDelegatees" << endl;
                        cin >> szDelegatees;
                        pAtten->setDelegatees(szDelegatees);
                        break;
                    }

                    case 7: {

                        string  szDelegator;
                        cout << " enter Delegator" << endl;
                        cin >> szDelegator;
                        pAtten-> setDelegator(szDelegator);
                        break;
                    }

                    case 8: {
                        vAttendees.push_back(pAtten);
                        pCe->setAttendees(vAttendees);
                        break;
                    }

                    default : {
                        cout << "enter choice between 1 and 8" << endl;
                        break;
                    }
                    }
                }

                break;
            }

            case 4: {
                string szSummary;
                cout << "enter szSummary " << endl;
                cin >> szSummary;
                pCe->setSummary(szSummary);
                pCe->getSummary();
                break;
            }

            case 5: {
                string szDescr;
                cout << "enter description" << endl;
                cin >> szDescr;
                pCe->setDescription(szDescr);
                break;
            }

            case 6: {
                string szLoca;
                cout << "enter location" << endl;
                cin >> szLoca;
                pCe->setLocation(szLoca);
                break;
            }

            case 7: {
                int iSeq;
                cout << "enter sequence" << endl;
                cin >> iSeq;
                pCe->setSequence(iSeq);
                break;
            }

            case 8: {

                int iPriority;
                cout << "enter iPriority" << endl;
                cin >> iPriority;
                pCe->setPriority(iPriority);
                break;
            }

            case 9: {
                int iUid;
                cout << "enter iUid" << endl;
                cin >> iUid;
                pCe->setUid(iUid);
                break;
            }

            case 10: {

                string szCateg;
                cout << "enter category" << endl;
                cin >> szCateg;
                pCe->setCategories(szCateg);
                break;
            }

            case 11: {

                string szUr;
                cout << "enter url" << endl;
                cin >> szUr;
                pCe->setUrl(szUr);
                cout << "in case 11" << endl << pCe->getUrl() << endl;
                break;
            }

            case 12: {

                string  szComm;
                cout << "enter comments" << endl;
                cin >> szComm;
                pCe->setComments(szComm);
                break;
            }

            case 13: {
                string  szCon;
                cout << "enter contacts" << endl;
                cin >> szCon;
                pCe->setContact(szCon);
                break;
            }

            case 14: {

                string  szRel;
                cout << "enter related" << endl;
                cin >> szRel;
                pCe->setRelated(szRel);
                break;
            }

            case 15: {
                string szRes;
                cout << "enter resources" << endl;
                cin >> szRes;
                pCe->setResources(szRes);
                break;
            }

            case 16: {
                string szXpr;
                cout << "enter xproperties" << endl;
                cin >> szXpr;
                //	pCe->setXProperties(szXpr);
                break;
            }

            case 17: {

                string szGeo;
                cout << "enter szGeo" << endl;
                cin >> szGeo;
                pCe->setGeo(szGeo);
                break;
            }

            //cout<<"1.organzer details\n 2.attachment details\n 3.attendee details\n 4.szSummary\n 5.description\n 6.location\n 7.sequence\n 8.iUid\n 9. iPriority \n10.category\n 11.url \n12.comments \n13.contacts \n14.related \n15.resources\n 16.xproperties\n 17.szGeo \n18.tododue\n 19 .completed\n 20. percentcomplete\n 21.iPriority\n"<<endl;

            case 18: {
                int stDue;
                cout << "enter stDue\n" << endl;
                cin >> stDue;
                pCe->setDue(stDue);
                break;
            }

            case 19: {

                int iComp;
                cout << "enter Completed\n" << endl;
                cin >> iComp;
                pCe->setCompleted(iComp);
                break;
            }

            case 20: {

                int iPercom;
                cout << "enter percent complete\n" << endl;
                cin >> iPercom;
                pCe->setPercentComplete(iPercom);
                break;
            }

            case 21: {

                int iPri;
                cout << "enter iPriority\n" << endl;
                cin >> iPri;
                pCe->setPriority(iPri);
                break;
            }

            case 22: {
                pCal->modifyTodo(pCe, pErrorCode);
                cout << "SUCCESSSSS" << endl;

                bModloopexit = true;
                exit(0);

            }



        }
        }
    }
}


void deleteLocalTodo(CCalendar *pCal)
{
    CMulticalendar *pCm = CMulticalendar::MCInstance();
    vector <CComponent *> vComplist = pCm->getComponents(pCal->getCalendarId(), 2, -1, -1, pErrorCode);
    cout << "the size is " << vComplist.size() << endl;
    cout << "LLLLLLLLLLLLLL" << pCal->getCalendarId();

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        cout << vComplist[iI]->getId() << endl;
    }

    cout << "enter the id  of the Todo  which  is to be deleted " << endl;
    string sId;
    cin >> sId;
    pCal->deleteTodo(sId, pErrorCode);
}
void deleteLocalJournal(CCalendar *pCal)
{
    CMulticalendar *pCm = CMulticalendar::MCInstance();
    vector <CComponent *> vComplist = pCm->getComponents(pCal->getCalendarId(), 3, -1, -1, pErrorCode);
    cout << "the size is " << vComplist.size() << endl;
    cout << "LLLLLLLLLLLLLL" << pCal->getCalendarId();

    for(unsigned int iI = 0; iI < vComplist.size(); iI++) {
        cout << vComplist[iI]->getId() << endl;
    }

    cout << "enter the id  of the Journal  which  is to be deleted " << endl;
    string sId;
    cin >> sId;
    pCal->deleteJournal(sId, pErrorCode);
}

void comp(CMulticalendar *pMcal)
{
    vector <CCalendar *> vC;
    unsigned int iI;
    bool bComploopexit = false;
    int iTemp, iOp;

    int iId;
    CCalendar *pCa = NULL;

    vC = pMcal->getListCalFromMc();

    cout << " The calendars for which components can be added/mod/del " << endl;

    for(iI = 0; iI < vC.size(); iI++) {
        cout << vC[iI]->getCalendarId() << endl;
    }

    cout << " Select the id of the calendar for which component can be add/mod/del" << endl;
    cin >> iId;
    cout << "nput id is given by " << iId << endl;

    for(iI = 0; iI < vC.size(); iI++) {
        if(iId == vC[iI]->getCalendarId()) {
            pCa = vC[iI];
        }
    }

    if(pCa == NULL) {
        cout << "Specific calendar Id is not available in database..Exit\n";
        exit(0);
    }

    bool ret;

    while((1) && (bComploopexit == false)) {
        cout << endl;
        cout << "1. Event " << endl;
        cout << "2. Todo " << endl;
        cout << "3. Journal " << endl;
        cout << "4. Birthday " << endl;
        cout << "5. Exit" << endl;

        cout << "select the Component " << endl;
        cin >> iTemp;

        if(iTemp == 5) {
            bComploopexit = true;
            exit(0);
        }

        cout << endl;
        cout << "1.Add" << endl;
        cout << "2.Modify" << endl;
        cout << "3.Delete" << endl;
        cout << "4.exit" << endl;

        cout << "select the operation to be done on the component" << endl;
        cin >> iOp;

        if(iTemp == 4 && iOp == 1) {
            if(pCa->getCalendarType() != 1) {
                cout << "Add birthday operation in wrong calendar type\n";
                break;
            }

            ret = addBirthDay();

            if(!ret) {
                cout << "Add birthday failed" << endl;
            }
            else {
                cout << "Add birthday success\n";
            }

            break;
        }


        if(iTemp == 4 && iOp == 2) {
            if(pCa->getCalendarType() != 2) {
                cout << "Modify birthday operation in wrong calendar type\n";
                break;
            }

            ret = modifyBirthDay();

            if(!ret) {
                cout << "Modify birthday failed" << endl;
            }
            else {
                cout << "Modify birthday success\n";
            }

            break;
        }


        if(iTemp == 4 && iOp == 3) {

            if(pCa->getCalendarType() != 2) {
                cout << "Delete birthday operation in wrong calendar type\n";
                break;
            }

            ret = deleteBirthDay();

            if(!ret) {
                cout << "Delete birthday failed" << endl;
            }
            else {
                cout << "Delete birthday success\n";
            }

            break;
        }


        if(iTemp == 1 && iOp == 1) {
            addLocalEvent(pCa);
            cout << "in addevent case" << endl;
            break;
        }


        if(iTemp == 1 && iOp == 2) {
            modifyLocalEvent(pCa);
            cout << "in modifyevent case" << endl;
            break;
        }

        if(iTemp == 1 && iOp == 3) {
            deleteLocalEvent(pCa);
            cout << "in delete event case" << endl;
            break;
        }

        if(iTemp == 2 && iOp == 1) {
            addLocalTodo(pCa);
            cout << "in addtodo case" << endl;
            break;
        }

        if(iTemp == 2 && iOp == 2) {
            modifyLocalTodo(pCa);
            cout << "in modify todo case" << endl;
            break;
        }

        if(iTemp == 2 && iOp == 3) {
            deleteLocalTodo(pCa);
            cout << "in delete todo case" << endl;
            break;
        }

        if(iTemp == 3 && iOp == 1) {
            addLocalJournal(pCa);
            cout << "in addjournal case" << endl;
            break;
        }

        if(iTemp == 3 && iOp == 2) {
            modifyLocalJournal(pCa);
            cout << "in modify journal case" << endl;
            break;
        }

        if(iTemp == 3 && iOp == 3) {
            deleteLocalJournal(pCa);
            cout << "in delete journal case" << endl;
            break;
        }

        if(iOp == 5) {

            bComploopexit = true;
            exit(0);
        }
    }


}

void allComponents(CMulticalendar *pMcal)
{
    vector <CCalendar *> vC;
    vector <CComponent *> vComp;
    bool bDisploopexit = false;
    int iId;



    unsigned int iI;

    vC = pMcal->getListCalFromMc();
    cout << " The calendars for which components to be dispalyed " << endl;

    for(iI = 0; iI < vC.size(); iI++) {
        cout << vC[iI]->getCalendarId() << endl;
    }

    cout << " Select the id of the calendar for which components should be displayed" << endl;
    cin >> iId;
    CCalendar *pCa = NULL;

    for(unsigned int iI = 0; iI < vC.size(); iI++) {
        if(iId == vC[iI]->getCalendarId()) {
            pCa = vC[iI];
            break;
        }
    }

    ///////////string  ret =ca->toString();
    ////////////////////////cout<<ret;
    if(pCa == NULL) {
        delete pCa;
        return;
    }

    while((1) && (bDisploopexit == false)) {
        cout << endl;
        cout << "1.Event" << endl;
        cout << "2.Todo" << endl;
        cout << "3.Journal" << endl;
        cout << "4.Birthday" << endl;
        cout << "5.All" << endl;
        cout << "6.Exit" << endl;

        cout << "enter the option to view" << endl;
        int iTemp;
        cin >> iTemp;
        int stDateSt, stDateEnd;

        if(iTemp >= 1 && iTemp <= 4) {
            cout << " enter the start date(-1 in case of no condition)" << endl;
            cin >> stDateSt;
            cout << "enter the end date(-1 in case of no condition)" << endl;
            cin >> stDateEnd;
        }

        switch(iTemp) {

        case 1: {
            vComp = pMcal->getComponents(pCa->getCalendarId(), 1, stDateSt, stDateEnd, pErrorCode);

            if(vComp.size() == 0) {
                cout << "no entries in the specified condition" << endl;
            }

            cout << "in event display" << endl;
            break;
        }

        case 2: {
            vComp = pMcal->getComponents(pCa->getCalendarId(), 2, stDateSt, stDateEnd, pErrorCode);

            if(vComp.size() == 0)

            {
                cout << "no entries in the specified condition" << endl;
            }

#if 0
            else {
                for(int iI = 0; iI < comp.size(); iI++) {
                    CTodo *pCt = comp[iI];

                    cout << endl << pCt->getCompleted();
                    cout << endl << pCt->getPercentComplete();
                    cout << endl << pCt->getPriority();
                    cout << endl << pCt->getDue();
                    cout << endl << pCt->getGeo();
                    cout << endl << pCt->getSummary();
                    cout << endl << pCt->getDescription();
                    cout << endl << pCt->getLocation();
                    cout << endl << pCt->getSequence();
                    cout << endl << pCt->getPriority();
                    cout << endl << pCt->getUid();
                    cout << pCt->setCategories(szCateg);
                    string szUr = new char[100];
                    cout << "enter url" << endl;
                    cin >> szUr;
                    pCt->setUrl(szUr);
                    string  szComm = new char[100];
                    cout << "enter comments" << endl;
                    cin >> szComm;
                    pCt->setComments((wchar_t *)szComm);
                    string  szCon = new char[100];
                    cout << "enter contacts" << endl;
                    cin >> szCon;
                    pCt->setContact((wchar_t *)szCon);
                    string  szRel = new char[100];
                    cout << "enter related" << endl;
                    cin >> szRel;
                    pCt->setRelated((wchar_t *)szRel);
                    string szRes = new char[100];
                    cout << "enter resources" << endl;
                    cin >> szRes;
                    pCt->setResources(szRes);
                    string szXpr = new char[100];
                    cout << "enter xproperties" << endl;
                    cin >> szXpr;
                    pCt->setXProperties(szXpr);
                    int no;
                    vector <CAttendee *> vAttendees;
                    cout << "enter no. of vAttendees for the event (0 or more)" << endl;
                    cin >> no;

                    for(int iI = 0; iI < no; iI++) {
                        CAttendee *pAtten = new CAttendee();
                        cout << endl;

                        cout << "enter the attendee " << (iI + 1) << " details" << endl;

                        string szUserType = new char[100];
                        cout << " enter calendar szUserType" << endl;
                        cin >> szUserType;
                        pAtten->setCalendarUserType(szUserType);

                        string szMember = new char[100];
                        cout << " enter szMember" << endl;
                        cin >> szMember;
                        pAtten->setMember(szMember);

                        string szRole = new char[100];
                        cout << " enter szRole" << endl;
                        cin >> szRole;
                        pAtten->setRole(szRole);

                        string  szPartStat = new char[100];
                        cout << " enter participation iStatus" << endl;
                        cin >> szPartStat;
                        pAtten->setParticipationStatus(szPartStat);

                        string  szRSVP = new char[100];
                        cout << " enter szRSVP" << endl;
                        cin >> szRSVP;
                        pAtten->setRSVP(szRSVP);

                        string  szDelegatees = new char[100];
                        cout << " enter szDelegatees" << endl;
                        cin >> szDelegatees;
                        pAtten->setDelegatees(szDelegatees);

                        string  szDelegator = new char[100];
                        cout << " enter Delegator" << endl;
                        cin >> szDelegator;
                        pAtten-> setDelegator(szDelegator);

                        vAttendees.push_back(pAtten);
                        pCt->setAttendees(vAttendees);
                    }

                    int noat;
                    vector <string > vAttach;
                    cout << " enter No. of attachments for the event ( 0 or more)" << endl;
                    cin >> noat;

                    for(int iI = 0; iI < noat; iI++) {
                        string  attachment = new char[1000];
                        cout << "enter the attachment " << (iI + 1);
                        cin >> attachment;
                        vAttach.push_back(attachment);
                    }

                    cout << "SSSSSSSSSSSSSSSS" << vAttach.size() << endl;

                    if(pCt->setAttachments(vAttach) == true) {
                        cout << "TTTTTTTTT" << endl;
                    }


#endif
                    cout << "in todo display" << endl;
                    break;
                }

                case 3: {
                    vComp = pMcal->getComponents(pCa->getCalendarId(), 3, stDateSt, stDateEnd, pErrorCode);

                    if(vComp.size() == 0) {
                        cout << "no entries in the specified condition" << endl;
                    }

                    cout << "in journal display" << endl;
                    break;
                }

                case 4: {
                    vComp = pMcal->getComponents(pCa->getCalendarId(), 4, stDateSt, stDateEnd, pErrorCode);

                    if(vComp.size() == 0) {
                        cout << "no entries in the specified condition" << endl;
                    }

                    cout << "in birthday display" << endl;
                    break;
                }

                case 5: {
                    vComp = pMcal->getComponents(pCa->getCalendarId(), 0, stDateSt, stDateEnd, pErrorCode);

                    if(vComp.size() == 0) {
                        cout << "no entries in the specified condition" << endl;
                    }

                    cout << "in journal display" << endl;
                    break;
                }


                case 6: {
                    bDisploopexit = true;
                    exit(0);
                }

                default:
                    ;

                }//end of switch



            }//end of while


        }

        /*void modifyLocalEvent(CCalendar * pCal)
          {
        // vector <CComponent *>  getComponents (int calid, int type,  int stDate, int endDate);
        CMulticalendar *pCm = CMulticalendar::MCInstance();
        CEvent * eptr;
        vector <CComponent *> complist = pCm->getComponents(pCal->getCalendarId(),1,-1,-1);
        cout<<"the size is "<<complist.size()<<endl;
        cout<<"LLLLLLLLLLLLLL"<<pCal->getCalendarId();
        for(int iI=0;iI<complist.size();iI++)
        {
        cout<<"PPPPPPPPPPP"<<endl;
        cout<<complist[iI]->getId()<<endl;
        }
        int id;




        cout<<"enter the id  of the calendar from which the event is to be modified"<<endl;
        cin >> id;
        int iI;
        for(iI=0;iI<complist.size();iI++)
        {
        if(id == complist[iI]->getId())
        {
        eptr=(CEvent *)complist[iI];
        break;
        }
        }
        cout<<"stDateStart is "<<eptr->getDateStart()<<endl;
        cout<<"stDateEnd   is "<<eptr->getDateEnd()<<endl;
        cout<<"flags are    "<<eptr->getFlags()<<endl;
        cout<<"szSummary   is "<<eptr->getSummary()<<endl;
        cout<<"description is "<<eptr->getDescription()<<endl;
        cout<<"location  is "<<eptr->getLocation()<<endl;
        cout<<"get iStatus   "<<eptr->getStatus()<<endl;
        cout<<"get Last modified   "<<eptr->getLastModified()<<endl;
        cout<<"get Categories   "<<eptr->getCategories()<<endl;





        int ch;
        cout << " Modify 1.stDateStart 2.stDateEnd 3.szSummary 4.description 5.location 6.getstatus 7.getlast modified 8.exit"<<endl;
        cin >> ch;
        switch (ch)
        {
        case 1:
        {
        int ds;
        cout<<"enter the modified date start"<<endl;
        cin >> ds;
        eptr->setDateStart(ds);
        break;
        }
        case 2:
        {
        int de;
        cout<<"enter the modified date end"<<endl;
        cin >> de;
        eptr->setDateEnd(de);
        break;
        }
        default :
        break;
        }
        pCal->modifyEvent(eptr);
        #if 0
        cout << "Please enter the Organizer details"<<endl;

        cout << "enter Language\n"<<endl;
        cin>> temp;
        org->setLanguage((wchar_t*)temp);

        cout << "enter SentBy\n"<<endl;
        cin>> temp;
        org->setSentBy(temp);

        cout << "enterCommonName\n"<<endl;
        cin>> temp;
        org->setCommonName((wchar_t*)temp);

        cout << "enter Xparamters\n"<<endl;
        cin>> temp;
        org->setXparameters(temp);

        cout << "enter DirectoryParameter\n"<<endl;
        cin>> temp;
        org-> setDirectoryParameter(temp);

        event->setOrganizer(org);

        cout << "Please enter the Event details" << endl;

        cout << "enter created Time\n"<<endl;
        cin>> temp;
        event->setCreatedTime(atoi(temp));
        event->setLastModified(atoi(temp));

        cout << "enter class\n"<<endl;
        cin>> temp;
        event->setClas(temp);

        cout << "enter DateStamp\n"<<endl;
        cin>> temp;
        event->setDateStamp(atoi(temp));

        cout << "enter sequence\n"<<endl;
        cin>> temp;
        event->setSequence(atoi(temp));

        cout << "enter Uid\n"<<endl;
        cin>> temp;
        event->setUid(atoi(temp));

        cout << "enter Url\n"<<endl;
        cin>> temp;
        event->setUrl(temp);

        cout << "enter Categories\n"<<endl;
        cin>> temp;
        event->setCategories(temp);

        cout << "enter Comments\n"<<endl;
        cin>> temp;
        event->setComments((wchar_t*)temp);

        cout << "enter Contact\n"<<endl;
        cin>> temp;
        event->setContact((wchar_t*)temp);

        cout << "enter Related\n"<<endl;
        cin>> temp;
        event->setRelated((wchar_t*)temp);

        cout << "enter Resources\n"<<endl;
        cin>> temp;
        event->setResources(temp);

        cout << "enter Property\n"<<endl;
        cin>> temp;
        event->setXProperties(temp);

        cout << " enter No. of attachments for the event ( 0 or more)"<< endl;
        cin>>temp;

        for(iI=0;iI<atoi(temp);iI++)
        {
        	cout<< "enter the attachment "<<(iI+1);
        	cin >> attachment;
        	vAttach.push_back(attachment);
        }
        event->setAttachments(vAttach);

        event->setType(1);

        int id=pCal->getCalendarId();
        cout<<"id = "<<id<<endl;
        event->setCalendarId(id);

        event->setFlags(3);

        cout<<"enter szSummary\n"<<endl;
        cin >> temp;
        event->setSummary(temp);

        cout<<"enter Description\n"<<endl;
        cin >> temp;
        event->setDescription(temp);

        cout<<"enter Location\n"<<endl;
        cin >> temp;
        event->setLocation(temp);

        cout<<"enter iStatus\n"<<endl;
        cin >> temp;
        event->setStatus(atoi(temp));

        cout<<"enter DateStart\n"<<endl;
        cin >> temp;
        event->setDateStart(atoi(temp));

        cout<<"enter DateEnd\n"<<endl;
        cin >> temp;
        event->setDateEnd(atoi(temp));

        cout<<"enter szGeo\n"<<endl;
        cin >> temp;
        event->setGeo(temp);

        cout<<"enter iPriority\n"<<endl;
        cin >> temp;
        event->setPriority(atoi(temp));

        cout<<"enter transparency\n"<<endl;
        cin >> temp;
        event-> setTransparency("OPAQUE");

        cout<< "enter no. of vAttendees for the event (0 or more)"<<endl;
        cin >> no;
        for(iI=0;iI<atoi(no);iI++)
        {
        	pAtten = new CAttendee();
        	cout<<endl;

        	cout<<"enter the attendee "<<(iI+1)<<" details"<< endl;

        	cout<< " enter calendar szUserType"<<endl;
        	cin >> temp;
        	pAtten->setCalendarUserType(temp);

        	cout<< " enter szMember"<<endl;
        	cin >> temp;
        	pAtten->setMember(temp);

        	cout<< " enter szRole"<<endl;
        	cin >> temp;
        	pAtten->setRole(temp);

        	cout<< " enter participation iStatus"<<endl;
        	cin >> temp;
        	pAtten->setParticipationStatus(temp);

        	cout<< " enter szRSVP"<<endl;
        	cin >> temp;
        	pAtten->setRSVP(temp);

        	cout<< " enter Delegatees"<<endl;
        	cin >> temp;
        	pAtten->setDelegatees(temp);

        	cout<< " enter Delegator"<<endl;
        	cin >> temp;
        	pAtten-> setDelegator(temp);

        	vAttendees.push_back(pAtten);
        #endif

        }*/

        int main() {

            cout << "##################################################################" << endl;
            cout << "###                                                            ###" << endl;
            cout << "###                                                            ###" << endl;
            cout << "###	        WELCOME TO  COMMAND LINE TOOL	               ###" << endl;
            cout << "###                  FOR CALENDAR-ENGINE                       ###" << endl;
            cout << "###                                                            ###" << endl;
            cout << "###                                                            ###" << endl;
            cout << "### Copyright (C) 2008 Nokia Corporation. All rights reserved. ###" << endl;
            cout << "###                                                            ###" << endl;
            cout << "### Contact: Biris Ilias <ilias.biris@nokia.com>               ###" << endl;
            cout << "###                                                            ###" << endl;
            cout << "##################################################################" << endl;
            int iI ;
            bool bLoopexit = false;
            string eventId;
            CMulticalendar *pMcal = CMulticalendar::MCInstance();
# if 0
            pMcal -> importIcsFileDataProgressive("data.ics", 1, eventId, iDuplicateCountTask, iDuplicateCountEvent,
                                                  iDuplicateCountJournal, pErrorCode, bFileReadingOver, iRequestLimit);

            pMcal->enableDisableCalendarLogs(1);
            CCalendar *pCal =  pMcal ->getCalendarById(1, pErrorCode);
            CEvent *pEvent = pCal->getEvent("3" , pErrorCode);
            pEvent->updateUntilInDB(222222, pErrorCode);
#endif

            while((1) && (bLoopexit == false)) {
                cout << "please select the operation for calendar\n" << endl;
                cout << "1.Add Calendar" << endl;
                cout << "2.Delete CAlendar" << endl;
                cout << "3.Modify Calendar" << endl;
                cout << "4.all Calendars" << endl;
                cout << "5.No. of calendars and visible Calendars" << endl;
                cout << "6.Calendar Components " << endl;
                cout << " 7.All Compenents" << endl;
                cout << "8. Exit" << endl;
                cout << "option here : " ;
                cin >> iI;


                if((iI < 1) || (iI > 8))
                    cout << "invalid option entered ( supported values  are 1 to 8)\
					\n please enter options in range  1-8 " << endl;
                else {
                    switch(iI) {
                    case 1: {
                        addCal(pMcal);
                        break;
                    }

                    case 2: {
                        deleteCal(pMcal);
                        break;
                    }

                    case 3: {
                        modifyCal(pMcal);
                        break;
                    }

                    case 4: {
                        getCalendars(pMcal);
                        break;
                    }

                    case 5: {
                        cout << endl << "number of Calendars =" << pMcal->getNoofCalendars() << endl;
                        break;
                    }

                    case 6: {
                        comp(pMcal);
                        break;
                    }

                    case 7: {
                        allComponents(pMcal);
                        break;
                    }

                    case 8: {
                        bLoopexit = true;
                        //	exit(0);
                    }

                    default:
                        ;
                    }
                }
            }

            pMcal->MCdestroy();


            return 0;
        }


