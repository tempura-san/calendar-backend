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

#include <iostream>
#include <time.h>

#include <clockd/libtime.h>



using namespace std;

int main()
{
#if 0
	time_t utcint;
	time_t timeDiff;

	utcint = time_get_time();
        timeDiff = mktime(localtime(&utcint)) - mktime(gmtime(&utcint));

	cout << timeDiff << endl;

	struct tm ltm, gtm;
	time_get_local (&ltm);
	time_get_utc (&gtm);

	timeDiff = time_mktime (&ltm, 0) - time_mktime (&gtm, 0);

	cout << timeDiff << endl;
#endif

#if 0
	struct tm *ptm = 0;
	time_t utc = time (0);
	cout << utc << endl;
	ptm = localtime (&utc);
	utc = mktime (ptm);
	cout << utc << endl;
#endif

	time_t utc = time (0);
	struct tm *ptm = 0;

	cout << utc << endl;

        ptm = gmtime(&utc);
        utc = mktime(ptm);
	
	cout << utc << endl;
}
		
