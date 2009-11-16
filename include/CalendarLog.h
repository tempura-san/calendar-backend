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

#ifndef __CALENDAR_SUPPORT_H__
#define __CALENDAR_SUPPORT_H_

#include <syslog.h>
#include <stdlib.h>

#undef DEBUG_LEVEL_1 
/* variable for logging Debug messages */
extern int enable_logging ;

//#define NO_DEBUG_MESSAGES_PLEASE

#ifndef NO_DEBUG_MESSAGES_PLEASE
#ifdef DEBUG_LEVEL_1
#    define CAL_DEBUG_LOG(a...) ULOG_DEBUG_F(a)
#  else
#    define CAL_DEBUG_LOG(fmtstr, args...) \
      if (enable_logging)\
	( syslog(LOG_ERR,"CALENDAR" ":%s: " fmtstr "\n", __func__, ##args))
#  endif
#else
#    define CAL_DEBUG_LOG(a...) {;}
#endif


#ifndef NO_DEBUG_MESSAGES_PLEASE
# define CAL_ERROR_LOG(fmtstr, args...) \
	( syslog(LOG_ERR,"CALENDAR" ":%s: " fmtstr "\n", __func__, ##args))
#else
#    define CAL_ERROR_LOG(a...) {;}
#endif


#define CALENDAR_LOG_ERROR(X,Y)\
    if(X!=CALENDAR_OPERATION_SUCCESSFUL){\
	syslog(LOG_ERR,"Error Code is : %d Error Message : %s",X,Y);\
	return FAILURE;\
    }           


#ifdef DEBUG_LEVEL_2

#define CAL_LOG(ARG...) ULOG_DEBUG_F(ARG)

#else

#define CAL_LOG(ARG...)

#endif

#endif
