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

#include<stdio.h>
#include <stdlib.h>
#include <clockd/libtime.h>
#include <ctime>

int main()
{
    char *string = new char[200];
    int ret;
    time_get_synced();

//	time_set_timezone ("Europe/Helsinki");

    time_set_timezone("Asia/Calcutta");

    ret = time_get_timezone(string, 200);
    printf("Current timezone is %s\n",  string);

//	printf ("Current timezone is %s\n",  time_set_timezone(const char *tz));

    return 1;
}
