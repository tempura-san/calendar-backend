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

#ifndef __CPARAMETERS_H__
#define __CPARAMETERS_H__

/* Headers */
#include <string>
#include <map>

using namespace::std;
using std::string;


typedef struct ParamType{

    int i;
    string szString;
    ParamType(): i(0) {};
}ParamType;
typedef enum {
    
    PARENT = 0,
    CHILD,
    SIBLING
        
}RelationshipType;
    
/**
 * CParameters class
 * @brief This class contains the API's to get/set the parameters for
 * the standard properties of event/todo/journal
 */
class CParameters {

  public:

    /**
     * This is the defult constructor for CParameters class  
     */
    CParameters();
    /**
     * Copy Constructor 
     */
    CParameters(CParameters &);
    /**
     * This is the defult destructor for CParameters class 
     */
    ~CParameters();

    /**
     * @param szParamName string
     * @return bool
     *
     * This function is used to set the param name.
     */
    bool setParamName(string szParamName);

    /**
     * @return string
     *
     * This function is used to return the param name.
     */
    string getParamName();

    /**
     * @param valueType ParamType
     * @return bool
     *
     * This function is used to set the param value.
     */
    bool setParamValue(ParamType valueType);

    /**
     * @return ParamType
     *
     * This function is used to return the param value.
     */
        ParamType getParamValue();

    /**
     * @param szPropParam string
     * @return int
     * 
     * This function returns the Type of the the given parameter (INTEGER/STRING/BOOL)
     */
    int getDataType(string szPropParam);
    
    /**
     * @param Constant reference to CParameters
     * @return Reference to CParameters
     *
     * Overloaded assignment operator for the class 
     *
     */
    const CParameters & operator=(const CParameters & right);
    /**
     * toString
     */
    string toString();

        


  private:

    string szParamName; /*!< param name */
    
    ParamType valueType; /*!< value Type */
           

};

#endif
