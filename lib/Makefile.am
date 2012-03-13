#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.23 2003/08/24 14:46:40 afm Exp $
#

lib_LTLIBRARIES = libmeteo.la

METEOCONFFILE = \"@METEOCONFFILE@\"
AM_CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE) @LIBXML_CFLAGS@

VERSION_FILE = ../include/meteo_vers.h

libmeteo_la_LDFLAGS = -version-info `../shtool version -d libtool $(VERSION_FILE)`

libmeteo_la_SOURCES =							\
	MeteoException.cc MeteoValue.cc Vector.cc Wind.cc Rain.cc	\
	Converter.cc Channel.cc Timeval.cc Station.cc Configuration.cc	\
	Datasink.cc Pidfile.cc Dataset.cc Timelabel.cc Dewpoint.cc	\
	Tdata.cc Query.cc Frame.cc GraphWindow.cc Graphics.cc		\
	Datarecord.cc							\
	crc.c 								\
	meteo.c 			 				\
	msgque.c xmlconf.c 						\
	average.c database.c timestamp.c 				\
	daemon.c printver.c						\
	pdebug.c mdebug.c  

