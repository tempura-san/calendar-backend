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

#ifndef __CCOMPONENTDETAILS__H_
#define __CCOMPONENTDETAILS__H_

/* Headers */
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "CComponent.h"
using namespace std;
#ifndef _QRESULT__
#define _QRESULT__
typedef struct _QueryResult QueryResult;

struct _QueryResult {
  public:
    char **pResult;
    int iRow;
    int iColumn;
};
#endif

/* Forward declaration */
class COrganizer;
class CAttendee;
class COrganizer;
class CAttendee;
class CParameters;
class CCalendarDB;
class CProperties;


/**
 * CComponentDetails class
 * @brief This class holds the complete the information about the Components.
 * Will be used when UI/other parties needs the full details of component.
 */

class CComponentDetails:public CComponent {

  public:

    /* Constructor */
    CComponentDetails();

    /* Parameterized constructor */
    CComponentDetails(string description);

    /* Parameterized constructor */
    CComponentDetails(string summary, int tododue, int status);

    /* Parametrized contructor */
     CComponentDetails(string summary, string description, string location, int dateStart,
                       int dateEnd);

    void initEvent();

    /**
     * CComponent copy constructor
     */
     CComponentDetails(CComponentDetails & ref);

    /**
     * overloaded assignment operator
     */
    const CComponentDetails & operator=(const CComponentDetails & right);

    /**
     * @param: string Class 
     * @return: bool indicating result of operation
     *
     * This function is used to set class parameter for an entry (Event,todo)
     */
    bool setClas(string szClas);

    /**
     * @param: none
     * @return: string class
     *
     * This function is used to get class string
     */
    string getClas();

    /**
     * @param: int time in utc
     * @return: bool indicating result of operation
     *
     * This function is used to setDateStamp
     */
    bool setDateStamp(int iDateStamp);

    /**
     * @param: none
     * @return:int  datestamp  in utc
     *
     * This function is used to get date stamp
     */
    int getDateStamp();

    /**
     * @param: COrganizer* pointer to COrganizer object
     * @return: bool indicating result of operation
     *
     * This function is used to set organizer
     */
    bool setOrganizer(COrganizer *pOrg);

    /**
     * @param: none
     * @return: COrganizer* pointer to Organizer class
     *
     * This function is used to get organizer
     */
    COrganizer *getOrganizer();

    /**
     * @param: none
     * @return: none
     *
     * This function is used to remove organizer
     */
    void removeOrganizer();

    /**
     * @param: vector<CAttendee*> vector of pointers to CAttendee objets 
     * @return: bool indicating result of operation
     *
     * This function is used to set Attendees
     */
    bool setAttendees(vector < CAttendee * > vAttendeeList);

    /**
     * @param: none
     * @return: vector<CAttendee*> vector of pointers to CAttendee class
     *
     * This function is used to get vector of  pointers to CAttendee class 
     */
     vector < CAttendee * >getAttendees();

    /**
     * @param: none
     * @return: none
     *
     * This function is used to remove Attendees 
     */
    void removeAttendees();

    /**
     * @param: vector<string> vector of Attachments  
     * @return: bool indicating result of operation
     *
     * TODO: have to change this to string 
     * This function is used to set Attachments
     */
    bool setAttachments(vector < string > vAttachList);

    /**
     * @param: none
     * @return: vector<string> vector of attachments 
     *
     * This function is used to get attachments
     */
     vector < string >getAttachments();

    /**
     * @param: int sequence
     * @return: bool indicating result of operation
     *
     * This function is used to set sequence
     */
    bool setSequence(int iSequence);

    /**
     * @param: none
     * @return: int sequence
     *
     * This function is used to get sequence
     */
    int getSequence();

    /**
     * @param: int uid
     * @return: bool indicating result of operation
     *
     * This function is used to set uid
     */
    bool setUid(int iUId);

    /**
     * @param: none
     * @return: int Uid
     *
     * This function is used to get Uid might be needed from sync point of view 
     */
    int getUid();

    /**
     * @param: string url
     * @return: bool indicating result of operation
     *
     * This function is used to set url
     */
    bool setUrl(string szUrl);

    /**
     * @param: none
     * @return: string  url
     *
     * This function is used to get url string
     */
    string getUrl();

    /**
     * @param: string szCategories 
     * @return: bool indicating result of operation true/false
     *
     * This function is used to set categories
     */
    bool setCategories(string szCategories);

    /**
     * @param: none
     * @return: string Categories
     *
     * This function is used to get categories string
     */
    string getCategories();

    /**
     * @param: string comments
     * @return: bool indicating result of operation
     *
     * This function is used to set comments
     */
    bool setComments(string szComments);

    /**
     * @param: none
     * @return: string  comments 
     *
     * This function is used to get comments
     */
    string getComments();

    /**
     * @param: string contact
     * @return: bool indicating result of operation
     *
     * This function is used to set contact
     */
    bool setContact(string szContact);

    /**
     * @param: none
     * @return: string contact
     * 
     * This function is used to get Contact
     */
    string getContact();

    /**
     * @param: string Related
     * @return: bool indicating result of operation
     *
     * This function is used to set Related
     */
    bool setRelated(string szRelated);

    /**
     * @param: none
     * @return: string related
     *
     * This function is used to get related
     */
    string getRelated();

    /**
     * @param: string Resources
     * @return: bool indicating result of operation
     *
     * This function is used to set Resources
     */
    bool setResources(string szResources);

    /**
     * @param: none
     * @return: string Resources
     *
     * This function is used to get resources
     */
    string getResources();
    /**
     * @param NONE 
     * @return vector<CAttendee*>  vector of CAttendee pointers
     *
     * Memory is alocated inside the procedure for each of the CAttendee 
     * has to be freed by the API user. Function used to retrieve Attendee details from DB 
     */
    vector <CAttendee *> retrieveAttendeeDetails();
    /**
     * @param NONE 
     * @return COrganizer* 
     *
     * Function used to retrieve organizer details 
     * Memory is allocated inside the prodcedure for COrganizer has to 
     * be freed by the API user.
     */
    COrganizer* retrieveOrganizerDetails();

    /**
     * @param string Xproperties
     * @return bool indicating result of operation
     *
     * This function is used to set Xproperties
     */
    bool setXProperties(vector < CProperties* >);

    /**
     * @param none
     * @return vector <CProperties*> vector of Cpropertied
     * 
     * This function is used to get Xproperties
     */
    vector < CProperties* > getXProperties();

    /**
     * @param: none
     * @return: none
     *
     * This function is used to remove Xproperties
     */
    void removeXProperties();

    /**
     *  Destructor */
     virtual ~ CComponentDetails();

     /**
      * @param: NONE
      * @return: map <string, vector <CParameters*> > 
      *
      * This function is used to get the hash map from the CComponentDetails object.
      */
     map<string,vector <CParameters*> > getHashMap();

    /**
     * @param: hashMap
     * @return bool
     *
     * This function is used to set the hashmap in CComponentDetails object.
     * CComponentDetails takes ownership of the CParameters pointers in hashMap,
     * so you mustn't delete them yourself.
     */
    bool setHashMap(map<string, vector <CParameters*> > hashMap);

    /**
     * @param: none
     * @return: none
     *
     * This function is used to remove hashmap 
     */
    void removeHashMap();

    /**
     * @param : NONE
     * @return : map with string and vector <CParameters*> as (key,value)
     *
     * This function retrieves details from parameters table and returns a hashmap.
     * Memory for vector <CParameters*> will be allocated inside this procedure 
     * has to be freed by the API user.
     */
    map< string, vector < CParameters* > > retrieveParameterDetails();
    map < string,vector < CParameters* > > retrieveXParameterDetails();


    /**
     * @param: QueryResult pQr
     * @return: vector < CParameters* >
     * 
     * This function returns all cparameters.
     */
    vector < CParameters* > getParameter(QueryResult *pQr);
    /**
     * @param: NONE
     * @return: vector <CProperties*> vector of extra properties if any for this event 
     *
     * Memory is allocated for vector <CProperties*> inside this procedure has to be freed 
     * by the API user.
     * Function to retrieve xtra properties.
     */

    vector < CProperties* > retrieveXPropertyDetails();
    /**
     * @param string Property Name 
     * @return vector<string> vector of unique Property values for specific ID 
     *
     * Function used to retrieve MailTo parameters for attendee
     * 
     */
    vector<string> getPropertyValues( string szProperty);
    /**
     * @param vector <CParameters *> 
     * @return CAttendee *
     *
     * Function to convet vector of parameters to CAttendee 
     */
    CAttendee*  convertParamToAttendee(vector<CParameters*> vParam);
    /**
     * @param string
     * @param string
     * @return vector <CParameters*>
     *
     * Function to retrieve parameters for a particular property
     * This is an overloaded method.
     */

    vector <CParameters*> getParameter(string szPropName,string szPropValue);
    /**
     * @param 
     * @return
     *
     * Function to convert vector of parameters to COrganizer
     */
    COrganizer* convertParamToOrganizer(vector<CParameters*> vParam);
    /**
     * toString
     */
    string toString();
	


  private:


    /**
     * @param: hashmap
     * @return none
     *
     * This function is used to set the hashmap in CComponentDetails object.
     */

    void copyHashMap(map<string, vector <CParameters*> > hashMap);


  private:



     int iDateStamp; /*!< Datestamp */

     COrganizer *pOrganizer; /*!< Organizer details */

     /* TODO: Need to change char * to string */

     vector < string >vAttachments; /*!< Attachments */

     vector < CAttendee *>vAttendee; /*!< Attendee */

     int iSequence; /*!< Sequence */

     int iUId; /*!< Uid */

     string szUrl; /*!< Url */

     string szCategory; /*!< Component category */

     string szComments; /*!< Comments */

     string szContacts; /*!< Contacts */

     string szRelated; /*!< Related */

     string szResources; /*!< Resources */

     vector <CProperties*> vXProp; /*!< XProperties */

     string szClas; /*!< Class */

     map< string, vector <CParameters*> > hashMap;
     

};

#endif
