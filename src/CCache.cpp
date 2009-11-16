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

/* Headers */
#include "CCache.h"
/* default constructor */
CCache::CCache()
{
	//iStartDate = 0;
	memset(&iStartDate, 0, sizeof(iStartDate));
}

string CCache::getCommaSeperatedIdString()
{
	string szOutput;
	int iCount = 0;
	string szSeparator(",");
	int iLength = vListId.size();
	
	if(iLength == 0) {
		return szOutput;
	}
	
	for (iCount = 0 ; iCount < iLength; iCount++) {
		szOutput.append(szSeparator);
		szOutput.append(vListId[iCount]);
	 }
	 //something has been added. 
	 //Then end it with comma
	 if(!szOutput.empty())
	 	szOutput.append(szSeparator);
	return szOutput;
}
	
CCache::CCache(time_t startDate, vector <string> vList)
{
	iStartDate = startDate;
	vListId = vList;
}

vector <string> CCache::getCacheIds()
{
	return vListId;
}

time_t CCache::getCacheStartDate()
{
	return iStartDate;
}

void CCache::setCacheStartDate(time_t startDate)
{
	iStartDate = startDate;
}

void CCache::setCacheIds(vector <string> vList)
{
	vListId = vList;
}
	
CCache::~CCache()
{
}
