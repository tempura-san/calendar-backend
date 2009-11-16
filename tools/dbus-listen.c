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

#include <stdio.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

/* SAMPLE PROGRAM TO RECEIVE THE DATA CHANGE SIGNAL */

/* gcc -o dbus-listen `pkg-config --cflags --libs glib-2.0 dbus-1 dbus-glib-1` dbus-listen.c */

/* Run program : run-standalone.sh ./dbus-signal */

DBusHandlerResult signal_filter 
(DBusConnection *connection, DBusMessage *message, void *user_data);

int  main (int argc, char **argv)
{
	GMainLoop *loop;
	DBusConnection *bus;
	DBusError error;

	loop = g_main_loop_new (NULL, FALSE);

	dbus_error_init (&error);
	bus = dbus_bus_get (DBUS_BUS_SESSION, &error);
	if (!bus) {
		g_warning ("Failed to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free (&error);
		return 1;
	}
	dbus_connection_setup_with_g_main (bus, NULL);

	dbus_bus_add_match (bus, "type='signal',interface='com.nokia.calendar'", NULL);
	if (!dbus_connection_add_filter (bus, signal_filter, NULL, NULL))
		return FALSE;

	g_main_loop_run (loop);
	return 0;
}

DBusHandlerResult
signal_filter (DBusConnection *connection, DBusMessage *message, void *user_data)
{
			printf ("CB Activated ");
	if (dbus_message_is_signal (message,"com.nokia.calendar", "dbChange")) {
		DBusError error;
		char *s = 0;
		char *appName = 0;

		dbus_error_init (&error);
		if (dbus_message_get_args (message, &error, DBUS_TYPE_STRING, &s,DBUS_TYPE_STRING,
					&appName,DBUS_TYPE_INVALID)) {
			printf ("Message received: %s", s);

		} else {
			printf("Message received, but error getting message: %s", error.message);
			dbus_error_free (&error);
		}
		return DBUS_HANDLER_RESULT_HANDLED;
	}


	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}



