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

#ifndef __UTILITY_H__
#define __UTILITY_H__


#include <string>
#include <vector>
#include "Common.h"
extern "C" {
#include <libical/ical.h>
}
/* Forward declaration*/
class CProperties;

class CUtility{
    public:
	//Extracts the date from date/time value by
	//discarding the time
	time_t getDateFromTime(time_t startDate);
	/**
	 *Parse ids to vector 
	 */
	vector <string> parseIds(string szIds);

	//Check if a string needs to be encoded
	//using quoted printable
	bool isEncodingRequired(string szInput, bool syncing);

	//Replace content for string with given content
	bool stringReplaceOne(string &toChange, const char *changeThis,
			const char *toThis);
	    
	/* function to encode data in quoted printable 
	*/
        string  encodeQuotedPrintable(string szInput);
	template <class T> void releaseVector(std::vector <T*> vec);
	void releasePropertiesVector(std::vector <CProperties*> vec);
	std::vector < time_t > getRecurrentTimes(std::string szRRule, 
						icaltimetype iDtStart,
						time_t iViewEnd,
						icaltimezone *pTz,
						int& pErrorCode);
	icaltimezone* getSystemTimeZoneAsIcalTimeZone();
	std::string getSystemTimeZone();
	string getApplicationName();
	void setApplicationName(string ApplicationName);

	CUtility();
	~CUtility();
	static CUtility *Instance();
	    

    private :
	
	string AppName;
	static CUtility *pUt;
};


template <class T> void CUtility::releaseVector(vector <T *> vec)
{
	class std::vector <T *>::iterator iIter;
	for (iIter = vec.begin(); iIter < vec.end(); iIter++) {
		delete(*iIter);
		(*iIter) = 0;
	}
	vec.clear();
}

#endif
