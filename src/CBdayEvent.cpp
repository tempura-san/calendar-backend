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

#include "CComponent.h"
#include "CBdayEvent.h"
#include "Common.h"

CBdayEvent::CBdayEvent(string szUId, string szSummary, int iDateStart) :
        CComponent(szUId, szSummary, iDateStart)
{
}

CBdayEvent::CBdayEvent()
{
}

CBdayEvent::~CBdayEvent()
{
}

bool CBdayEvent::setEBookUid(string szEbookUid)
{
    return setGUid(szEbookUid);
}

string CBdayEvent::getEBookUid()
{
    return getGUid();
}

bool CBdayEvent::setFirstName(string szName)
{
    return setSummary(szName);
}

string CBdayEvent::getFirstName()
{
    return getSummary();
}

bool CBdayEvent::setBirthDate(int iDate)
{
    return setDateStart(iDate);
}

int CBdayEvent::getBirthDate()
{
    return getDateStart();
}

string CBdayEvent::toString()
{
    CAlarm *pAlarm;
    CRecurrence *cRec;
    std::stringstream ss;

    ss << "ID=" << getId();
    ss << ",CalendarId=" << getCalendarId();
    ss << ",Summary=" << getSummary();
    ss << ",Description=" << getDescription();
    ss << ",Location=" << getLocation();
    ss << ",TimeZone=" << getTzid();
    ss << ",UId=" << getGUid();
    ss << ",Type=" << getType();
    ss << ",Flags=" << getFlags();
    ss << ",Status=" << getStatus();
    ss << ",Start date=" << getDateStart();
    ss << ",End date=" << getDateEnd();
    ss << ",Last modified=" << getLastModified();
    ss << ",Created=" << getCreatedTime();
    ss << ",Until=" << getUntil();
    ss << ",All day=" << getAllDay();

    pAlarm = getAlarm();
    if (pAlarm) {
        ss << ",Alarm=" << pAlarm->toString();
    }
    else {
        ss << ",Alarm=NULL";
    }

    cRec = getRecurrence();
    if (cRec) {
        ss << ",Rec=" << cRec->toString();
    }
    else {
        ss << ",Rec=NULL";
    }

    return ss.str();
}
