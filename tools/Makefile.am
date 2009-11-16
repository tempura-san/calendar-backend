AM_CXXFLAGS =  @PACKAGE_CFLAGS@ -I../include

AM_CFLAGS = $(ERROR_CFLAGS) @PACKAGE_CFLAGS@ -I../include
	    
AM_LDFLAGS = @PACKAGE_LIBS@

bin_PROGRAMS = calendarclt

calendarclt_SOURCES = \
        calendar-tool.cpp

calendarclt_LDADD = ../src/libcalendar_backend.la -lsqlite3 -lical

CLEANFILES = $(BUILT_SOURCES)
