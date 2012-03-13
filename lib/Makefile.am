#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.4 2002/11/24 19:48:01 afm Exp $
#

lib_LTLIBRARIES = libmeteo.la
EXTRA_DIST = scanner.l parser.y

METEOCONFFILE = \"@METEOCONFFILE@\"
CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE) @LIBXML_CFLAGS@

VERSION_FILE = meteo_vers.c

libmeteo_la_LDFLAGS = -version-info `../shtool version -d libtool $(VERSION_FILE)`

libmeteo_la_SOURCES = crc.c com.c sercom.c tcpcom.c davis.c 		\
	meteodata.c meteo.c conversions.c dloop.c graph.c dewpoint.c	\
	msgque.c dbupdate.c meteosocket.c xmlconf.c 			\
	average.c database.c timestamp.c dograph.c			\
	daemon.c watchdog.c printver.c					\
	pdebug.c mdebug.c colors.c					\
	$(VERSION_FILE)

meteo_vers.c:	../configure.in
	../shtool version -l c -s @VERSION@ -n Meteo -p meteo_ meteo_vers.c

