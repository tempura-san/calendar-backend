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

#include "ical.h"
#include <time.h>

int main()
{

    const char *rrule = "FREQ=DAILY;UNTIL=20080820T143500Z";
    icaltimetype dtstart = icaltime_from_timet(time(0) - (2 * 24 * 3600), 0);
    time_t viewend;

    struct icalrecurrencetype recur;
    time_t utc_tim;
    time_t dtst_utc;
    bool loopexit = false;

    recur = icalrecurrencetype_from_string(rrule);

    printf("date is given by %d:%d:%d\n\n", dtstart.year, dtstart.month, dtstart.day);

    dtst_utc = icaltime_as_timet(dtstart);
    icalrecur_iterator *ritr;
    ritr = icalrecur_iterator_new(recur, dtstart);


    struct icaltimetype next;
    next = icalrecur_iterator_next(ritr);

    while((!icaltime_is_null_time(next)) && (!loopexit)) {

        utc_tim = icaltime_as_timet(next);

        if(time(0) < utc_tim) {
            printf("Recurrent time : %s\n", ctime(&utc_tim));
            loopexit = true;
        }

        next = icalrecur_iterator_next(ritr);
    }

    icalrecur_iterator_free(ritr);
}
