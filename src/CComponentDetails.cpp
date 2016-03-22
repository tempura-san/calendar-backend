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

/* header file inclusion
 */
#include "CComponentDetails.h"
#include "Common.h"
#include "CalendarLog.h"

#if 0
static int PROP_COUNT = 9;
/* properties array used in this file  */
static char properties[][12] = {
    "DESCRIPTION",
    "SUMMARY",
    "RESOURCES",
    "CATEGORIES",
    "REALTED_TO",
    "COMMENT",
    "LOCATION",
    "CONTACT",
    "ATTACH"
};
#endif
/**
 * Component()
 * CComponent copy constructor
 */
CComponentDetails::CComponentDetails(CComponentDetails &ref): CComponent
    (ref)
{
    iDateStamp = ref.iDateStamp;
    pOrganizer = 0;

    if(ref.pOrganizer) {
        this->setOrganizer(ref.pOrganizer);
    }

    vAttachments.clear();

    if(ref.vAttachments.size()) {
        this->setAttachments(ref.vAttachments);
    }

    vAttendee.clear();

    if(ref.vAttendee.size()) {
        this->setAttendees(ref.vAttendee);
    }

    iSequence = ref.iSequence;
    iUId = ref.iUId;
    szUrl = ref.szUrl;
    szCategory = ref.szCategory;
    szComments = ref.szComments;
    szContacts = ref.szContacts;
    szRelated = ref.szRelated;
    szResources = ref.szResources;
    vXProp.clear();

    if(ref.vXProp.size()) {
        this->setXProperties(ref.vXProp);
    }

    szClas = ref.szClas;
    this->copyHashMap(ref.hashMap);
}

/**
 * overloaded assignment operator
 */
const CComponentDetails &CComponentDetails::
operator=(const CComponentDetails &right)
{
    if(&right != this) {       //prevent self assignment
        CComponent::operator=(right);
        iDateStamp = right.iDateStamp;
        removeOrganizer();

        if(right.pOrganizer) {
            this->setOrganizer(right.pOrganizer);
        }

        vAttachments.clear();

        if(right.vAttachments.size()) {
            this->setAttachments(right.vAttachments);
        }

        removeAttendees();

        if(right.vAttendee.size()) {
            this->setAttendees(right.vAttendee);
        }

        iSequence = right.iSequence;
        iUId = right.iUId;
        szUrl = right.szUrl;
        szCategory = right.szCategory;
        szComments = right.szComments;
        szContacts = right.szContacts;
        szRelated = right.szRelated;
        szResources = right.szResources;
        removeXProperties();

        if(right.vXProp.size()) {
            this->setXProperties(right.vXProp);
        }

        szClas = right.szClas;
        this->copyHashMap(right.hashMap);
    }

    return *this;
}

/**
 * CComponentDetails
 * Default constructor
 */
CComponentDetails::CComponentDetails(): CComponent()
{
    initEvent();
}

/**
 * CComponentDetails
 * Overloaded parameterized constructor
 */

CComponentDetails::CComponentDetails(string szDescription): CComponent
    (szDescription)
{
    initEvent();
}

/**
 * CComponentDetails
 * Overloaded parameterized constructor
 */
CComponentDetails::CComponentDetails(string szSummary, int iTodoDue, int iStatus):
    CComponent(szSummary, iTodoDue, iStatus)
{
    initEvent();

}


/**
 * CComponentDetails
 * Overloaded parameterized constructor
 */

CComponentDetails::CComponentDetails(string szSummary,
                                     string szDescription,
                                     string szLocation, int iDateStart,
                                     int iDateEnd): CComponent(szSummary,
                                                 szDescription,
                                                 szLocation,
                                                 iDateStart,
                                                 iDateEnd)
{
    initEvent();
}

/**
 * InitEvent
 * Function used to initialize the class members
 */
void CComponentDetails::initEvent()
{
    this->iDateStamp = 0;
    this->pOrganizer = NULL;
    this->vAttendee.clear();
    this->vXProp.clear();
    this->hashMap.clear();
    this->iSequence = 0;
    this->iUId = 0;
}

/**
 * retrieveXPropertyDetails()
 */
vector<CProperties *> CComponentDetails::retrieveXPropertyDetails()
{
    QueryResult *pQr = NULL;
    CCalendarDB *pDb = NULL;
    char *pQuery = NULL;
    int iSqliteError = 0;
    vector<CProperties *> vPropList;
    int iI_PropCount;
    int iJ_PropCount;
    int iK_PropCount;
    PropType val;

    vPropList.clear();
    pDb = CCalendarDB::Instance();

    if((pDb == NULL) || (this->getId().empty())) {
        return vPropList;
    }

    pQuery = sqlite3_mprintf(SELECT_TAB_COND2, XPROP_TABLE,
                             XPROP_FIELD_ID, this->getId().c_str());

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s\n", pQuery);
    pQr = pDb->getRecords(pQuery, iSqliteError);
    sqlite3_free(pQuery);

    if(pQr == 0) {
        CAL_DEBUG_LOG("Query Result is empty");
        return vPropList;
    }

    CProperties *pProp = NULL;

    for(iI_PropCount = 0; iI_PropCount < pQr->iRow; iI_PropCount++) {
        pProp = new CProperties();
        ASSERTION(pProp);

        for(iJ_PropCount = 0; iJ_PropCount < pQr->iColumn; iJ_PropCount++) {
            iK_PropCount = pQr->iColumn + (iI_PropCount * pQr->iColumn);

            if((pQr->pResult[iK_PropCount + iJ_PropCount]) == NULL) {
                continue;
            }

            switch(iJ_PropCount) {

            case DB_COLUMN_ID2:
                CAL_DEBUG_LOG("XpropName =  %s",
                              pQr->pResult[iK_PropCount + iJ_PropCount]);
                pProp->setPropName(pQr->
                                   pResult[iK_PropCount + iJ_PropCount]);
                break;

            case DB_COLUMN_ID3:
                if((pProp->getDataType(pProp->getPropName())) == STRING)
                    val.szString =
                        (pQr->pResult[iK_PropCount + iJ_PropCount]);

                if((pProp->getDataType(pProp->getPropName())) == INTEGER)
                    val.i =
                        atoi(pQr->pResult[iK_PropCount + iJ_PropCount]);

                pProp->setPropValue(val);
                break;

            default:
                break;
            }//switch
        }//j

        vPropList.push_back(pProp);
    }//i

    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;
    return vPropList;
}


/**
 * retrieveParameterDetails()
 * @param : NONE
 * @return : map with string and vector <CParameters*> as (key,value)
 *
 * This function retrieves details from parameters table and returns a hashmap.
 */
map < string,
    vector < CParameters * > > CComponentDetails::retrieveParameterDetails()
{
    QueryResult *pQr = NULL;
    CCalendarDB *pDb = NULL;
    char *pQuery = NULL;
    map < string, vector < CParameters * > >paramHashMap;
    int iSqliteError = 0;
    int iI_ParamCount = 0;
    int iJ_ParamCount = 0;
    int iK_ParamCount = 0;
    CParameters *pParameter = 0;
    CProperties *pProperties = 0;
    pDb = CCalendarDB::Instance();

    if((pDb == NULL) || (this->getId().empty())) {
        return paramHashMap;
    }

    pQuery =
        sqlite3_mprintf(SELECT_TAB_COND_AND2, PARAMS_TABLE,
                        PARAM_FIELD_ID, this->getId().c_str());

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s\n", pQuery);
    pQr = pDb->getRecords(pQuery, iSqliteError);
    sqlite3_free(pQuery);

    if(pQr == 0) {
        CAL_DEBUG_LOG("Query Result is empty");
        return paramHashMap;
    }



    for(iI_ParamCount = 0; iI_ParamCount < pQr->iRow; iI_ParamCount++) {
        pParameter = new CParameters();
        pProperties = new CProperties();
        ASSERTION(pParameter);
        ASSERTION(pProperties);

        for(iJ_ParamCount = 0; iJ_ParamCount < pQr->iColumn; iJ_ParamCount++) {
            iK_ParamCount = pQr->iColumn + (iI_ParamCount * pQr->iColumn);
            ParamType val;
            PropType valProp;

            if((pQr->pResult[iK_ParamCount + iJ_ParamCount]) == NULL) {
                continue;
            }

            switch(iJ_ParamCount) {

            case DB_COLUMN_ID2:
                CAL_DEBUG_LOG("Prop name  %s\n",
                              pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                pProperties->setPropName(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                break;

            case DB_COLUMN_ID3:
                if((pProperties-> getDataType(pProperties->getPropName())) == STRING) {
                    valProp.szString = (pQr-> pResult[iK_ParamCount + iJ_ParamCount]);
                }

                if((pProperties->getDataType(pProperties->getPropName())) == INTEGER) {
                    valProp.i = atoi(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                }

                pProperties->setPropValue(valProp);

                break;

            case DB_COLUMN_ID4:
                CAL_DEBUG_LOG("Param name  %s\n",
                              pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                pParameter->setParamName(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                break;

            case DB_COLUMN_ID5:

                if((pParameter-> getDataType(pParameter->getParamName())) == STRING) {
                    val.szString = (pQr-> pResult[iK_ParamCount + iJ_ParamCount]);
                }

                if((pParameter->getDataType(pParameter->getParamName())) == INTEGER) {
                    val.i = atoi(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                }

                pParameter->setParamValue(val);
                break;

            default:
                break;
            }
        }

        map<string, vector <CParameters * > >::iterator it;
        it = paramHashMap.find(pProperties->getPropName());

        if(it != paramHashMap.end()) {
            (*it).second.push_back(pParameter);
        }

        else {
            vector < CParameters * >vParamList;
            vParamList.push_back(pParameter);
            paramHashMap.insert(pair < string, vector <CParameters * > >(pProperties->getPropName(), vParamList));
            //pUt->releaseParametersVector (vParamList);
        }

        delete pProperties;
    }

    /*
     * fetch the parameter found into the 'right' vector
     * fetch the vector based on the property name, from hashmap,
     * vector = hashmap.lookup(pProperties->setPropName())
     * vector.push(parameter);
     */
    pQuery = NULL;
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;

    return paramHashMap;
}

#if 0
map < string,
vector < CParameters * > > CComponentDetails::retrieveXParameterDetails()
{
    QueryResult *pQr = NULL;
    CCalendarDB *pDb = NULL;
    char *pQuery = NULL;
    map < string, vector < CParameters * > >paramHashMap;
    int iSqliteError = 0;
    int iI_ParamCount = 0;
    int iJ_ParamCount = 0;
    int iK_ParamCount = 0;
    CParameters *pParameter = 0;
    CProperties *pProperties = 0;

    pDb = CCalendarDB::Instance();

    if((pDb == NULL) || (this->getId().empty())) {
        return paramHashMap;
    }

    pQuery =
        sqlite3_mprintf(SELECT_TAB_COND_AND2, PARAMS_TABLE,
                        PARAM_FIELD_ID, this->getId().c_str());

    ASSERTION(pQuery);
    CAL_DEBUG_LOG("query is %s\n", pQuery);
    pQr = pDb->getRecords(pQuery, iSqliteError);
    sqlite3_free(pQuery);

    for(iI_ParamCount = 0; iI_ParamCount < pQr->iRow; iI_ParamCount++) {
        pParameter = new CParameters();
        pProperties = new CProperties();
        ASSERTION(pParameter);

        for(iJ_ParamCount = 0; iJ_ParamCount < pQr->iColumn; iJ_ParamCount++) {
            iK_ParamCount = pQr->iColumn + (iI_ParamCount * pQr->iColumn);
            ParamType val;
            PropType valProp;

            if((pQr->pResult[iK_ParamCount + iJ_ParamCount]) == NULL) {
                continue;
            }

            switch(iJ_ParamCount) {

            case DB_COLUMN_ID2:
                CAL_DEBUG_LOG("Prop name  %s\n",
                              pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                pProperties->setPropName(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                break;

            case DB_COLUMN_ID3:
                if((pProperties-> getDataType(pProperties->getPropName())) == STRING) {
                    valProp.szString = (pQr-> pResult[iK_ParamCount + iJ_ParamCount]);
                }

                if((pProperties->getDataType(pProperties->getPropName())) == INTEGER) {
                    valProp.i = atoi(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                }

                pProperties->setPropValue(valProp);

                break;

            case DB_COLUMN_ID4:
                CAL_DEBUG_LOG("Param name  %s\n",
                              pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                pParameter->setParamName(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                break;

            case DB_COLUMN_ID5:

                if((pParameter-> getDataType(pParameter->getParamName())) == STRING) {
                    val.szString = (pQr-> pResult[iK_ParamCount + iJ_ParamCount]);
                }

                if((pParameter->getDataType(pParameter->getParamName())) == INTEGER) {
                    val.i = atoi(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                }

                pParameter->setParamValue(val);
                break;

            default:
                break;
            }
        }

        map<string, vector <CParameters * > >::iterator it;
        it = paramHashMap.find(pProperties->getPropName());

        if(it != paramHashMap.end()) {
            (*it).second.push_back(pParameter);
        }

        else {
            vector < CParameters * >vParamList;
            vParamList.push_back(pParameter);
            paramHashMap.insert(pair < string, vector <CParameters * > >(pProperties->getPropName(), vParamList));
            //pUt->releaseParametersVector (vParamList);
        }

    }

    pQuery = NULL;
    pQr = NULL;
    sqlite3_free_table(pQr->pResult);
    delete pQr;
    pQr = 0;

    return paramHashMap;


}
#endif

/**
 * getParameter()
 * @param: QueryResult pQr
 * @return: vector < CParameters* >
 *
 * This function returns all cparameters.
 */

vector < CParameters * >CComponentDetails::getParameter(QueryResult *pQr)
{

    vector < CParameters * >vParamList;
    int iI_ParamCount;
    int iJ_ParamCount;
    int iK_ParamCount;

    vParamList.clear();

    if(pQr == 0) {
        return vParamList;
    }

    CParameters *pParameter = NULL;

    for(iI_ParamCount = 0; iI_ParamCount < pQr->iRow; iI_ParamCount++) {
        pParameter = new CParameters();
        ASSERTION(pParameter);

        for(iJ_ParamCount = DB_COLUMN_ID4; iJ_ParamCount < pQr->iColumn; iJ_ParamCount++) {
            iK_ParamCount = pQr->iColumn + (iI_ParamCount * pQr->iColumn);
            ParamType val;

            if((pQr->pResult[iK_ParamCount + iJ_ParamCount]) == NULL) {
                continue;
            }

            switch(iJ_ParamCount) {
            case DB_COLUMN_ID4:
                CAL_DEBUG_LOG("Param name  %s\n",
                              pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                pParameter->setParamName(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                break;

            case DB_COLUMN_ID5:

                /*if string */

                if((pParameter-> getDataType(pParameter->getParamName())) == STRING) {
                    val.szString = (pQr-> pResult[iK_ParamCount + iJ_ParamCount]);
                }

                if((pParameter->getDataType(pParameter->getParamName())) == INTEGER) {
                    val.i = atoi(pQr->pResult[iK_ParamCount + iJ_ParamCount]);
                }

                pParameter->setParamValue(val);
                break;

            default:
                break;
            }//switch end
        }//j end

        vParamList.push_back(pParameter);
    }//i end

    /* freeing pQr->pResult */
    sqlite3_free_table(pQr->pResult);
    /* freeing pQr*/
    delete pQr;
    pQr = 0;

    return vParamList;
}

/**
 * getPropertyValues
 * @param string Property Name
 * @return vector<string> vector of unique Property values for specific ID
 *
 * Function used to retrieve MailTo parameters for attendee
 *
 */
vector < string > CComponentDetails::getPropertyValues(string szProperty)
{


    /*
     * query will be
     * #define SELECT_DISTINCT_PROP "select distinct PropertyValue from Parameters
     * where ComponentId = %s AND PropertyName = %s"
     */

    /*
     * there can be multiple attendees for a single event each of them
     * can be distinguished by the mailto parameter*/
    QueryResult *pQr = NULL;
    int iI_IdCount = 0;
    int iJ_IdCount = 0;
    int iK_IdCount = 0;
    char *pQuery = NULL;
    CCalendarDB *pDb = NULL;
    int iSqliteError = 0;
    vector < string > listId;

    pDb = CCalendarDB::Instance();
    listId.clear();


    if(pDb == NULL) {
        return listId;
    }

    pQuery =
        sqlite3_mprintf(SELECT_DISTINCT_PROP, this->getId().c_str(),
                        szProperty.c_str());
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery, iSqliteError);
    CAL_DEBUG_LOG("query is given by  %s", pQuery);
    sqlite3_free(pQuery);

    if(pQr == 0) {
        CAL_DEBUG_LOG("returning back from distinct \n");

        return listId;
    }

    for(iI_IdCount = 0; iI_IdCount < pQr->iRow; iI_IdCount++) {
        for(iJ_IdCount = 0; iJ_IdCount < pQr->iColumn; iJ_IdCount++) {
            iK_IdCount = pQr->iColumn + (iI_IdCount * pQr->iColumn);

            if(pQr->pResult[iK_IdCount + iJ_IdCount]) {
                listId.push_back(pQr->pResult[iK_IdCount + iJ_IdCount]);
            }
        }
    }

    sqlite3_free_table(pQr->pResult);

    if(pQr) {

        delete pQr;
        pQr = 0;
    }

    return (listId);




}

/**
 * getParameter
 * @param string
 * @param string
 * @return vector <CParameters*>
 *
 * Function to retrieve parameters for a particular property
 */

vector < CParameters * >CComponentDetails::getParameter(string szPropName,
        string szPropValue)
{
    /**
    * #define SELECT_PROP_AND select * from Parametes where ComponentId=  AND
    * PropertyName = %s AND PropertyValue = %s
    */
    QueryResult *pQr = NULL;
    char *pQuery = NULL;
    CCalendarDB *pDb = NULL;
    int iSqliteError = 0;
    vector < string > listId;
    pDb = CCalendarDB::Instance();
    vector < CParameters * >vParam;
    vParam.clear();

    if(pDb == NULL) {
        return vParam;
    }

    pQuery =
        sqlite3_mprintf(SELECT_PROP_AND, this->getId().c_str(),
                        szPropName.c_str(), szPropValue.c_str());
    ASSERTION(pQuery);
    pQr = pDb->getRecords(pQuery, iSqliteError);
    CAL_DEBUG_LOG("query is given by  %s", pQuery);
    sqlite3_free(pQuery);

    /* pQr and pQr->pResult  will be freed inside
     * getParameter procedure*/
    vParam = getParameter(pQr);

    return vParam;

}

/**
 * convertParamToOrganizer
 * @param
 * @return
 *
 * Function to convert vector of parameters to COrganizer
 */
COrganizer *CComponentDetails::convertParamToOrganizer(vector <
        CParameters *
        > vParam)
{
    COrganizer *pOrg = new COrganizer();
    ASSERTION(pOrg);

    for(unsigned int i = 0; i < vParam.size(); i++) {

        if(vParam[i]->getParamName() == "SENTBY") {
            pOrg->setSentBy(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == "CNAME") {
            pOrg->setCommonName(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == "DIRPARAM")
            pOrg->setDirectoryParameter(vParam[i]->getParamValue().
                                        szString);
        else if(vParam[i]->getParamName() == "LANGUAGE") {
            pOrg->setLanguage(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == "MAILTO") {
            pOrg->setMailTo(vParam[i]->getParamValue().szString);
        }

    }

    return pOrg;


}

/**
 * convertParamToAttendee
 * @param vector <CParameters *>
 * @return CAttendee *
 *
 * Function to convert vector of parameters of attendee to
 * Cattendee
 */
CAttendee *CComponentDetails::convertParamToAttendee(vector <
        CParameters * > vParam)
{

    CAttendee *pAtt = new CAttendee();
    ASSERTION(pAtt);

    for(unsigned int i = 0; i < vParam.size(); i++) {

        if(vParam[i]->getParamName() == "CUTYPE")

            pAtt->setCalendarUserType((CalendarUserType) vParam[i]->
                                      getParamValue().i);

        else if(vParam[i]->getParamName() == ATMEMBER) {
            pAtt->setMember(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == ATROLE) {
            pAtt->setRole((ParticipantRole) vParam[i]->getParamValue().i);
        }
        else if(vParam[i]->getParamName() == ATPSTAT)
            pAtt->setParticipationStatus((ParticipantStatus) vParam[i]->
                                         getParamValue().i);
        else if(vParam[i]->getParamName() == ATRSVP) {
            pAtt->setRSVP(vParam[i]->getParamValue().i);
        }
        else if(vParam[i]->getParamName() == ATDELEGATEES) {
            pAtt->setDelegatees(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == ATDELEGATOR) {
            pAtt->setDelegator(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == SENT) {
            pAtt->setSentBy(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == CNAME) {
            pAtt->setCommonName(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == DIRPAR)
            pAtt->setDirectoryParameter(vParam[i]->getParamValue().
                                        szString);
        else if(vParam[i]->getParamName() == LANGUAGE) {
            pAtt->setLanguage(vParam[i]->getParamValue().szString);
        }
        else if(vParam[i]->getParamName() == GNMAILTO) {
            pAtt->setMailTo(vParam[i]->getParamValue().szString);
        }

        else {
            CAL_DEBUG_LOG("invalid value\n");
        }


    }

    return pAtt;


}

/**
 * retrieveAttendeeDetails
 * @param NONE
 * @return vector<CAttendee*>
 * Function used to retrieve Attendee details from DB
 */
vector < CAttendee * >CComponentDetails::retrieveAttendeeDetails()
{

    vector < CAttendee * >listAttendee;
    vector < string > vMailTo;
    vector < string >::iterator iterMailTo;
    vMailTo.clear();
    listAttendee.clear();

    if(this->getId().empty()) {
        return listAttendee;
    }

    /* first get all existing mailto for attendee
     * select unique PropertyValue  from PARAMETERS
     *  where Id =  and PropType = ATTENDEE
     * put them in to list     */

    vMailTo = getPropertyValues(ATTDEE);

    if(vMailTo.size()) {

        for(iterMailTo = vMailTo.begin(); iterMailTo != vMailTo.end();
                iterMailTo++) {

            vector < CParameters * >vParam;
            vector < CParameters * >::iterator vParamIter;
            vParam.clear();
            /* here we get a list of CParameters
             * corresponding to single Attendee */
            vParam = getParameter(ATTDEE, (*iterMailTo));

            CAttendee *pAtemp = 0;

            pAtemp = convertParamToAttendee(vParam);
            pAtemp->setMailTo((*iterMailTo));

            listAttendee.push_back(pAtemp);

            /*free memory allocated for
             *  CParameters list*/
            for(vParamIter = vParam.begin(); vParamIter != vParam.end();
                    vParamIter++) {
                CAL_DEBUG_LOG
                ("Freeing the memory allocated for the parameters \n");

                if(*vParamIter) {
                    delete(*vParamIter);
                }

            }

        }
    }
    else {
        CAL_DEBUG_LOG("Inside Else \n");
    }


    return listAttendee;
}


/**
 * retrieveOrganizerDetails
 * @param NONE
 * @return COrganizer*
 * Function used to retrieve organizer details
 */
COrganizer *CComponentDetails::retrieveOrganizerDetails()
{
    COrganizer *pAtemp = 0;
    vector < string > vMailTo;
    vMailTo.clear();

    if(this->getId().empty()) {
        return NULL;
    }


    vMailTo = getPropertyValues(ORGZER);

    if(vMailTo.size()) {
        vector < CParameters * >vParam;
        vector < CParameters * >::iterator vParamIter;
        vParam.clear();
        /* here we get a list of CParameters
         * corresponding to single Attendee */
        /* There will be only one organizer
         * present in the DB
         */

        vParam = getParameter(ORGZER, vMailTo[0]);

        pAtemp = convertParamToOrganizer(vParam);
        pAtemp->setMailTo(vMailTo[0]);

        /*free memory allocated for
         *  CParameters list*/
        for(vParamIter = vParam.begin(); vParamIter != vParam.end(); vParamIter++) {
            CAL_DEBUG_LOG("Freeing the memory allocated for the parameters \n");

            if(*vParamIter) {
                delete(*vParamIter);
            }
        }

    }
    else {
        CAL_DEBUG_LOG("Organizer not present for the id %s ", this->getId().c_str());
    }

    return pAtemp;
}

/**
 * setDateStamp
 * @param int time in utc
 * @return bool indicating result of operation
 *
 * This function is used to setDateStamp
 */
bool CComponentDetails::setDateStamp(int iDateStamp)
{
    if(iDateStamp <= 0) {
        return false;
    }

    this->iDateStamp = iDateStamp;
    return true;
}

/**
 * getDateStamp
 * @param none
 * @return datestamp  in utc
 * This function is used to get date stamp
 */
int CComponentDetails::getDateStamp()
{
    return this->iDateStamp;
}

/**
 * setOrganizer
 * @param pointer to COrganizer class
 * @return bool indicating result of operation
 *
 * This function is used to set organizer
 */
bool CComponentDetails::setOrganizer(COrganizer *ptrOrganizer)
{

    // it is strange that same variable name is used
    // for class member and the function argument
    // it might create confusion while reviewing
    // not sure why it is followed as standard?

    if(ptrOrganizer == NULL) {
        return false;
    }

    removeOrganizer();
    pOrganizer = new COrganizer;
    ASSERTION(pOrganizer);
    *pOrganizer = *ptrOrganizer;
    // above assignment should invoke the  overloaded assignment operator
    return true;
}

/**
 * getOrganizer
 * @param none
 * @return pointer to Organizer class
 * This function is used to get organizer
 */
COrganizer *CComponentDetails::getOrganizer()
{
    return this->pOrganizer;
}

/**
 * removeOrganizer
 * @param none
 * @return none
 * This function is used to get organizer
 */
void CComponentDetails::removeOrganizer()
{
    if(pOrganizer) {
        delete pOrganizer;
    }

    pOrganizer = 0;
}

/**
 * getHashMap()
 * @param: NONE
 * @return: map <string, vector <CParameters*> >
 *
 * This function is used to get the hash map
 */
map < string, vector < CParameters * > >CComponentDetails::getHashMap()
{
    return this->hashMap;
}

/**
 * setHashMap()
 * @param: hashmap
 * @return bool
 *
 * This function is used to set the hashmap.
 * CComponentDetails takes ownership of the CParameters pointers in hashMap,
 * so you mustn't delete them yourself.
 */
bool CComponentDetails::setHashMap(map < string,
                                   vector < CParameters * > > hashMap)
{
    if(hashMap.size() == 0) {
        return false;
    }

    removeHashMap();
    this->hashMap = hashMap;
    return true;
}

/**
 * copyHashMap()
 * @param: hashmap
 * @return bool
 *
 * This function is used to set the hashmap.
 */
void CComponentDetails::copyHashMap(map<string, vector <CParameters *> > hashMap)
{
    removeHashMap();
    map < string, vector < CParameters * > >::iterator miter;
    vector < CParameters * >::iterator iter;
    CParameters *pParameter = 0;

    for(miter = hashMap.begin(); miter != hashMap.end(); miter++) {
        vector < CParameters * > vParamList;

        for(iter = (*miter).second.begin(); iter != (*miter).second.end(); iter++) {
            pParameter = new CParameters();
            ASSERTION(pParameter);
            *pParameter = *(*iter);
            vParamList.push_back(pParameter);
        }

        this->hashMap.insert(pair < string, vector <CParameters * > >((*miter).first, vParamList));
    }
}

/**
 * removeHashMap
 * @param: none
 * @return: none
 *
 * This function is used to remove the hashmap
 */
void CComponentDetails::removeHashMap()
{
    map < string, vector < CParameters * > >::iterator miter;
    vector < CParameters * >::iterator iter;

    for(miter = hashMap.begin(); miter != hashMap.end(); miter++) {
        for(iter = (*miter).second.begin(); iter != (*miter).second.end(); iter++) {
            delete(*iter);
            (*iter) = 0;
        }

        (*miter).second.clear();
    }

    hashMap.clear();
}

/**
 * setAttendees
 * @param vector of pointers to CAttendee class
 * @return bool indicating result of operation
 *
 * This function is used to set Attendees
 */

bool CComponentDetails::setAttendees(vector < CAttendee * >vAttendee)
{
    if(vAttendee.size() == 0) {
        return false;
    }

    removeAttendees();
    CAttendee *pAtten = 0;

    for(unsigned int iCount = 0; iCount < vAttendee.size(); iCount++) {
        pAtten = new CAttendee();
        ASSERTION(pAtten);
        *pAtten = *vAttendee[iCount];
        this->vAttendee.push_back(pAtten);
    }

    return true;
}

/**
 * getAttendees
 * @param none
 * @return vector of pointers to CAttendee class
 * This function is used to get vector of  pointers to CAttendee class
 */
vector < CAttendee * >CComponentDetails::getAttendees()
{
    return this->vAttendee;
}

/**
 * removeAttendees
 * @param: none
 * @return: none
 *
 * This function is used to remove Attendees
 */
void CComponentDetails::removeAttendees()
{
    vector < CAttendee * >::iterator iter;

    for(iter = vAttendee.begin(); iter != vAttendee.end(); iter++) {
        delete(*iter);
        (*iter) = 0;
    }

    vAttendee.clear();
}

/**
 * setAttachments
 * @param vector of pointers to CAttachments class
 * @return bool indicating result of operation
 *
 * This function is used to set Attachments
 */

bool CComponentDetails::setAttachments(vector < string > vAttachments)
{

    if(vAttachments.size() == 0) {
        return false;
    }

    this->vAttachments = vAttachments;
    return true;
}

/**
 * getAttachments
 * @param none
 * @return vector of pointers to attachments
 * This function is used to get attachments
 */
vector < string > CComponentDetails::getAttachments()
{
    return this->vAttachments;
}

/**
 * setSequence
 * @param int sequence
 * @return bool indicating result of operation
 *
 * This function is used to set sequence
 */
bool CComponentDetails::setSequence(int iSequence)
{
    if(iSequence < 0) {
        return false;
    }

    this->iSequence = iSequence;
    return true;
}

/**
 * getSequence
 * @param none
 * @return int sequence
 * This function is used to get sequence
 */
int CComponentDetails::getSequence()
{
    return this->iSequence;
}

/**
 * setUid
 * @param int uid
 * @return bool indicating result of operation
 *
 * This function is used to set uid
 */
bool CComponentDetails::setUid(int iUId)
{
    if(iUId < 0) {
        return false;
    }

    this->iUId = iUId;
    return true;
}

/**
 * getUid
 * @param none
 * @return int Uid
 * This function is used to get Uid might be needed from sync point of view
 */
int CComponentDetails::getUid()
{
    return this->iUId;
}

/**
 * setUrl
 * @param url string
 * @return bool indicating result of operation
 * This function is used to set url
 */
bool CComponentDetails::setUrl(string szUrl)
{
    this->szUrl = szUrl;
    return true;
}

/**
 * getUrl
 * @param none
 * @return url string
 * This function is used to get url string
 */
string CComponentDetails::getUrl()
{
    return this->szUrl;
}

/**
 * setClas
 * @param Class string
 * @return bool indicating result of operation
 * This function is used to set clas
 */

bool CComponentDetails::setClas(string szClas)
{
    this->szClas = szClas;
    return true;
}

/**
 * getClas
 * @param none
 * @return clas string
 * This function is used to get clas string
 */

string CComponentDetails::getClas()
{
    return this->szClas;
}

/**
 * setCategories
 * @param categories string
 * @return bool indicating result of operation
 * This function is used to set categories
 */

bool CComponentDetails::setCategories(string szCategory)
{
    this->szCategory = szCategory;
    return true;
}

/**
 * getCategories
 * @param none
 * @return Categories string
 * This function is used to get categories string
 */

string CComponentDetails::getCategories()
{
    return this->szCategory;
}

/**
 * setComments
 * @param comments string
 * @return bool indicating result of operation
 * This function is used to set comments
 */
bool CComponentDetails::setComments(string szComments)
{
    this->szComments = szComments;
    return true;
}

/**
 * getComments
 * @param none
 * @return comments string
 * This function is used to get comments
 */
string CComponentDetails::getComments()
{
    return this->szComments;
}

/**
 * setContact
 * @param contact string
 * @return bool indicating result of operation
 * This function is used to set contact
 */
bool CComponentDetails::setContact(string szContacts)
{
    this->szContacts = szContacts;
    return true;
}

/**
 * getContact
 * @param none
 * @return contact string
 * This function is used to get Contact
 */

string CComponentDetails::getContact()
{
    return this->szContacts;
}

/**
 * setRelated
 * @param Related string
 * @return bool indicating result of operation
 * This function is used to set Related
 */
bool CComponentDetails::setRelated(string szRelated)
{
    this->szRelated = szRelated;
    return true;
}

/**
 * getRelated
 * @param none
 * @return related string
 * This function is used to get related
 */
string CComponentDetails::getRelated()
{
    return this->szRelated;
}

/**
 * setResources
 * @param Resources string
 * @return bool indicating result of operation
 * This function is used to set Resources
 */

bool CComponentDetails::setResources(string szResources)
{
    this->szResources = szResources;
    return true;
}

/**
 * getResources
 * @param none
 * @return resources string
 * This function is used to get resources
 */
string CComponentDetails::getResources()
{
    return this->szResources;
}

/**
 * setXproperties
 * @param Xproperties string
 * @return bool indicating result of operation
 * This function is used to set Xproperties
 */
bool CComponentDetails::setXProperties(vector <CProperties *> vXProp)
{
    if(vXProp.size() == 0) {
        return false;
    }

    removeXProperties();
    CProperties *pProp = 0;

    for(unsigned int iCount = 0; iCount < vXProp.size(); iCount++) {
        pProp = new CProperties();
        ASSERTION(pProp);
        *pProp = *vXProp[iCount];
        this->vXProp.push_back(pProp);
    }

    return true;
}

/**
 * getXProperties
 * @param none
 * @return xproperties string
 * This function is used to get Xproperties
 */
vector < CProperties * >CComponentDetails::getXProperties()
{
    return this->vXProp;
}


/**
* removeXProperties
* @param: none
* @return: none
*
* This function is used to remove Xproperties
*/
void CComponentDetails::removeXProperties()
{
    vector < CProperties * >::iterator iter;

    for(iter = vXProp.begin(); iter != vXProp.end(); iter++) {
        delete(*iter);
        (*iter) = 0;
    }

    vXProp.clear();
}


/**
 * ~CComponentDetails
 * Destructor for CComponentDetails class
 */
CComponentDetails::~CComponentDetails()
{
    removeAttendees();
    removeXProperties();
    removeOrganizer();
    removeHashMap();
}

string CComponentDetails::toString()
{
    CAlarm *pAlarm;
    CRecurrence *cRec;
    COrganizer *pOrg;
    string szRet;
    string szTemp;
    time_t temp = 0 ;
    std::stringstream ss;
    ss << "ID=";

    if(getId().c_str()) {
        szTemp = getId().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",CalendarId=";
    ss << getCalendarId();

    ss << ",summary=";

    if(getSummary().c_str()) {
        szTemp = getSummary().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }
    else {
        ss << NULL_STRING;
    }

    ss << ",description=";

    if(getDescription().c_str()) {
        szTemp = getDescription().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",location=";

    if(getLocation().c_str()) {
        szTemp = getLocation().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",TimeZone=";

    if(getTzid().c_str()) {
        szTemp = getTzid().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Type=";
    ss << getType() ;
    ss << ",Flags=";

    switch(getFlags()) {
    case HAS_ATTENDEE:
        ss << "Has Attendee";
        break;

    case HAS_ORGANIZER:
        ss << "Has Organizer";
        break;

    case HAS_RECURRENCE:
        ss << "Has Recurrence";
        break;

    case HAS_ALARM:
        ss << "Has Alarm";
        break;

    case HAS_RECURRENCE_ALARM :
        ss << "Has Recurrent Alarm";
        break;

    case HAS_PARTICIPANT :
        ss << "Has Participant";
        break;

    case HAS_CATEGORIES :
        ss << "Has Categories";
        break;

    case HAS_COMMENT:
        ss << "Has Comment ";
        break;

    case HAS_EXTRA:
        ss << "Has Extra ";
        break;

    default:
        break;
    }

    ss << ",UId=";

    if(getGUid().c_str()) {
        szTemp = getGUid().substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Status=";
    ss << getStatus();
    ss << ",Start date=";
    temp = getDateStart();
    ss << ctime(&temp);
    ss << ",End date=";
    temp = getDateEnd();
    ss << ctime(&temp);
    ss << ",Last modified=";
    temp = getLastModified();
    ss << ctime(&temp);
    ss << ",created=";
    temp = getCreatedTime();
    ss << ctime(&temp);
    ss << ",until=";
    temp = getUntil();
    ss << ctime(&temp);
    ss << ",All day=";
    ss << getAllDay();
    ss << ",Datestamp=";
    ss << iDateStamp;
    ss << ",Sequence=";
    ss << iSequence;
    ss << ",Uid=";
    ss << iUId;
    ss << ",URL=";

    if(szUrl.c_str()) {
        szTemp = szUrl.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Category=";

    if(szCategory.c_str()) {
        szTemp = szCategory.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Comments=";

    if(szComments.c_str()) {
        szTemp = szComments.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Contacts=";

    if(szContacts.c_str()) {
        szTemp = szContacts.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Related=";

    if(szRelated.c_str()) {
        szTemp = szRelated.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Resources=";

    if(szResources.c_str()) {
        szTemp = szResources.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    ss << ",Class=";

    if(szClas.c_str()) {
        szTemp = szClas.substr(0, 100);
        ss << szTemp;
        szTemp.clear();
    }

    else {
        ss << NULL_STRING;
    }

    pAlarm = getAlarm();

    if(pAlarm) {
        ss << pAlarm->toString();
    }
    else {
        ss << ",Alarm=NULL" ;
    }

    cRec = getRecurrence();

    if(cRec) {
        ss << cRec->toString();
    }
    else {
        ss << ",Rec=NULL";
    }


    pOrg = getOrganizer();

    if(pOrg) {
        ss << pOrg->toString();
    }
    else {
        ss << ",Org=NULL";
    }

    if((vAttachments).size() > 0) {
        ss << ",Attachments=";
        vector<string>::iterator it;

        for(it = vAttachments.begin(); it != vAttachments.end(); it++) {
            ss << (*it) << SEMI_COLON;
        }
    }

    if((vAttendee).size() > 0) {
        ss << ",Attendee=";
        vector<CAttendee *>::iterator it;

        for(it = vAttendee.begin(); it != vAttendee.end(); it++) {
            ss << (*it) << SEMI_COLON;
        }
    }

    if((vXProp).size() > 0) {
        ss << ",Xprop=";
        vector<CProperties *>::iterator it;

        for(it = vXProp.begin(); it != vXProp.end(); it++) {
            ss << (*it) << SEMI_COLON;
        }
    }

    szRet.append(ss.str());
    return szRet;
}
