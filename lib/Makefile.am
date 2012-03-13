#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.15 2003/06/12 20:29:33 afm Exp $
#

lib_LTLIBRARIES = libmeteo.la
EXTRA_DIST = meteo_vers.h

METEOCONFFILE = \"@METEOCONFFILE@\"
AM_CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE) @LIBXML_CFLAGS@

VERSION_FILE = meteo_vers.h

libmeteo_la_LDFLAGS = -version-info `../shtool version -d libtool $(VERSION_FILE)`

libmeteo_la_SOURCES =							\
	MeteoException.cc MeteoValue.cc Vector.cc Wind.cc Rain.cc	\
	Converter.cc Channel.cc Timeval.cc Station.cc Configuration.cc	\
	Datasink.cc Pidfile.cc						\
	Frame.cc							\
	crc.c 								\
	meteo.c graph.c dewpoint.c					\
	msgque.c xmlconf.c 						\
	average.c database.c timestamp.c dograph.c			\
	daemon.c printver.c						\
	pdebug.c mdebug.c colors.c					\
	$(VERSION_FILE)	

meteo_vers.h:	../configure.in
	../shtool version -l c -s @VERSION@ -n Meteo -p meteo_ meteo_vers.h

