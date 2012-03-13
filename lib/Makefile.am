#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.1 2002/01/18 23:34:27 afm Exp $
#

lib_LTLIBRARIES = libmeteo.la
EXTRA_DIST = scanner.l parser.y

METEOCONFFILE = \"@METEOCONFFILE@\"
CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE)

VERSION_FILE = meteo_vers.c

BUILT_SOURCES = scanner.c parser.c

scanner.c:	scanner.l
	$(LEX) -Pmp -oscanner.c scanner.l

parser.c:	parser.y
	$(YACC) -t -b parser -d -p mp parser.y
	mv parser.tab.c parser.c
../include/parser.h:	parser.c
	mv parser.tab.h ../include/parser.h

libmeteo_la_LDFLAGS = -version-info `../shtool version -d libtool $(VERSION_FILE)`

libmeteo_la_SOURCES = crc.c com.c sercom.c tcpcom.c davis.c 		\
	meteodata.c meteo.c conversions.c dloop.c graph.c dewpoint.c	\
	msgque.c dbupdate.c meteosocket.c mconf.c			\
	scanner.c parser.c average.c database.c timestamp.c dograph.c	\
	daemon.c watchdog.c printver.c					\
	$(VERSION_FILE)

meteo_vers.c:	../configure.in
	../shtool version -l c -s @VERSION@ -n Meteo -p meteo_ meteo_vers.c

