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

#ifndef __CEVENT_H__
#define __CEVENT_H__

/* Headers */
#include "CComponentDetails.h"
class CAttendee;
class CProperties;
/**
 * CEvent class
 * @brief This class is used to get/set the event specific properties 
 */

class CEvent:public CComponentDetails {

  public:

    /**
     * this is the default constructor for CEvent class where all variables are being initialised
     */
    CEvent();

    /* Paramterized constructor */
    CEvent(string szSummary, string szDescription, string szLocation, int iDateStart, int iDateEnd);

    /**
     * this is the destructor for CEvent class where all memory is being deallocated or freed
     */
    ~CEvent();

    /**
     * @param : string >> used to set geographical parameters(latitude,longitude) of entry created
     * @return :bool (SUCCESS/FAILURE)
     *
     * This property specifies information related to the global
     *    position for the activity specified by a calendar component.
     */
    bool setGeo(string szGeo);

    /**
     * @param :None
     * @return : string  used to get geographical position  of entry  value is 
     *           like "latitude;longitude" eg 12;13
     *
     * This function is used to get the geographical position  of the entry created.
     */
    string getGeo();

    /**
     * @param :None
     * @return : string transp >> used to get transparency of entry
     *          valid values can be TRANSPARENT/OPAQUE    
     *
     * This function is used to get the transparency of the entry created.
     */
    string getTransparency();
    /**
     * @param : transp(int) >> used to set transparency of entry
     *          valid values can be TRANSPARENT/OPAQUE    
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the transparency of the entry created.
     *  Generally, an entry will take up time on an individual calendar. 
     *  Hence, the event will appear as an opaque interval in a search for busy time. 
     *  Alternately, the event can have its Time Transparency set to "TRANSPARENT" in order to prevent
     *  blocking of the event in searches for busy time.
     */
    bool setTransparency(string szTransparency);
    /**
     * @param :None
     * @return : int used to get priority of entry 
     *
     * This function is used to get the priority of the entry created.
     */
    int getPriority();

    /**
     * @param : int iPriority used to set priority of entry created
     * @return :bool (SUCCESS/FAILURE)
     *
     * This function is used to set the priority of the entry created.
     * The property defines the relative priority for a calendar
     *    component.
     */
    bool setPriority(int iPriority);

    /**
     * copy constructor
     * for Cevent class
     */
     CEvent(CEvent & ref);

    /**
     * overloaded assignment 
     * operator 
     */
    const CEvent & operator=(const CEvent & right);
    /**
     * toString
     */
    string toString();


  private:

    /* Member variables are defined as per RFC */

    string szGeo; /*!< Geo parameter */

    string szTransparency; /*!< Transparency */

    int iPriority; /*!< Priority */
};

#endif
