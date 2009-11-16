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

/*
 * New attributes added 
 *   - "target_time_offset"
 *   - "calendar_event_type" 
 */
const char* TARGET_TIME_OFFSET =  "target_time_offset";
const char* CAL_EVENT_TYPE = "calendar_event_type";
 /**
  * event type values:
  *   - "normalevent"
  *   - "allday"
  *   - "task"
  *   - "birthday"
  */
const char* ANORMALEVENT = "timedevent";
const char* AALLDAY = "allday";
const char* ATASK = "task";
const char* ABIRTHDAY = "birthday";
/**
 * enumeration fro entryType 
 */
enum eventType {
	E_NORMAL_EVENT= 0,
	E_ALLDAY,
	E_TASK,
	E_BIRTHDAY
};

/*constants used in CAlarm.cpp*/
const char *CALENDAR_INTERFACE = "com.nokia.calendar";
const char *CALENDAR_SERVICE = "com.nokia.calendar";
const char *CALENDAR_PATH = "/com/nokia/calendar";
const char *CALENDAR_AUTOCLEAN_METHOD = "calendar_alarm_autoclean";
const char *CALENDAR_LAUNCH_METHOD = "launch_view";
const char *CLOSE = "cloc_bd_stop";
const char *VIEW = "cloc_bd_view";
const char *SNOOZE = "cloc_bd_alarm_notification_snooze";
const char *CLOCK_TEXT_DOMAIN = "osso-clock";
const char *ALARM_APPID = "Calendar";
const char *TARGET_TIME = "target_time";
const char *EVENT_LOCATION = "location";
const char *EVENT_DESCRIPTION = "Description";
const char *SET_NEXT_ALARM = "/usr/bin/set_next_alarm";
const int OPEN_EVENT_VIEW = 4;
const int OPEN_TASK_VIEW = 5;
const int ALARM_ACTION_COUNT = 4;
const int ALARM_ACTION_ONE = 1;
const char *ALARM_ICON_PATH =
"/usr/share/icons/hicolor/128x128/hildon/clock_calendar_alarm.png";
const int ALARM_ACTION_SNOOZE = 0;
const int ALARM_ACTION_VIEW = 1;
const int ALARM_ACTION_STOP = 2;
const int ALARM_ACTION_MISSED = 3;
const int ALARM_INVALID_COOKIE = 0;
const int DBUS_ARGUMENT_0 = 0;
const int DBUS_ARGUMENT_1 = 1;
