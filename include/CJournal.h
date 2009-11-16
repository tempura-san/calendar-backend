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

#ifndef __CJOURNAL_H__
#define __CJOURNAL_H__

/* Headers */
#include "CComponentDetails.h"
/**
 * CJournal class
 * @brief This class holds the information related to journal
 * Inherited from CComponentDetails
 */

class CJournal:public CComponentDetails {

  public:

    /* Paramterized constructor */
    CJournal(string szDescription);

    /* Constructor */
    CJournal();

    /* Copy constructor */
    CJournal(CJournal & ref);

    /**
     * overloaded assignment 
     * operator
     */
    CJournal & operator=(CJournal & right);

    /**
     *This is the default destructor of journal class
     */
    ~CJournal();
    /**
     * toString
     */
    string toString();



  private:
};
#endif
