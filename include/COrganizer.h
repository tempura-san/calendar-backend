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

#ifndef __CALORGANIZER__H_
#define __CALORGANIZER__H_

/* Headers */
#include "CParticipant.h"

/** 
 * COrganizer class
 * @brief Class which contains the details of Organizers
 * Inherited from CParticipant
 */
class COrganizer:public CParticipant {

  public:

    /**
     * default constructor 
     */
    COrganizer();

    /**
     * Overloaded assignment operator
     */
    COrganizer & operator=(COrganizer & right);

    /**
     * copy Constructor 
     */
    COrganizer(COrganizer & Orgref);

    /* Destructor */
    ~COrganizer();
    /**
     * toString
     */
    string toString();



};

#endif
