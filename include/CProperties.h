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

#ifndef __CPROPERTIES_H__
#define __CPROPERTIES_H__

/* Headers */
#include <string>
#include <map>

using namespace::std;
using std::string;

struct PropType {
    int i;
    string szString;

    PropType(): i(0) {};
}; 

/**
 * CProperties class
 * @brief This class contains the API's to get/set the parameters for
 * the standard properties of event/todo/journal
 */
class CProperties {

  public:

    /**
     * This is the defult constructor for CProperties class 
     */
    CProperties();
    
    /**
     * @param Constant reference to CProperties
     * @return Reference to CProperties
     *
     * Overloaded assignment operator for the class 
     *
     */
    const CProperties & operator=(const CProperties & right);
    
    /**
     * Copy Constructor 
     */
    CProperties(CProperties &);

    /**
     * This is the defult destructor for CProperties class 
     */
    ~CProperties();

    /**
     * @param szPropName string
     * @return bool
     *
     * This function is used to set the property name.
     */
    bool setPropName(string szPropName);

    /**
     * @return string
     *
     * This function is used to return the property name.
     */
    string getPropName();

    /**
     * @param valueType PropType
     * @return bool
     *
     * This function is used to set the property value.
     */
    bool setPropValue(PropType valueType);

    /**
     * @return PropType
     *
     * This function is used to return the property value.
     */
        PropType getPropValue();
    
    /**
     * @param szPropParam string
     * @return int
     * 
     * This function returns the Type of the the given property (INTEGER/STRING/BOOL)
     */
    int getDataType(string szProp);
    /**
     * toString
     */
    string toString();
	
        

  private:

    string szPropName; /*!< prop name */
    PropType valueType; /*!< value Type */
};

#endif
