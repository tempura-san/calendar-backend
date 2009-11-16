AM_CXXFLAGS =  @PACKAGE_CFLAGS@ -I../include  

AM_CFLAGS = $(ERROR_CFLAGS) @PACKAGE_CFLAGS@ -I../include -I/usr/include/clockd 
	    
AM_LDFLAGS = @PACKAGE_LIBS@ -ltime

calendarbackenddir=/usr/lib
calendarbackend_LTLIBRARIES = libcalendar_backend.la
			       

libcalendar_backend_la_SOURCES = \
    CCalendarDB.cpp \
    CUtility.cpp \
    CCalendar.cpp \
    CMulticalendar.cpp \
    CComponent.cpp \
    CComponentDetails.cpp \
    CEvent.cpp \
    CTodo.cpp \
    CJournal.cpp \
    COrganizer.cpp \
    CAttendee.cpp \
    CParticipant.cpp\
    CRecurrence.cpp\
    CAlarm.cpp \
    CAlarmIface.cpp \
    CRecurrenceRule.cpp\
    CBdayEvent.cpp\
    ICalConverter.cpp\
    VCalConverter.cpp\
    CProperties.cpp\
    CParameters.cpp\
    CCache.cpp \
    CTimezone.cpp

CLEANFILES = $(BUILT_SOURCES)
