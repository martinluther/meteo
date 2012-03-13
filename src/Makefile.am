#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.11 2003/06/08 22:32:10 afm Exp $
#
METEOCONFFILE = \"@METEOCONFFILE@\"
AM_CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE) @LIBXML_CFLAGS@

LDADD = ../lib/libmeteo.la

bin_PROGRAMS = meteopoll 					\
	meteoupdate meteograph meteoavg				\
	meteodequeue meteowatch

meteoupdate_SOURCES = meteoupdate.cc
meteograph_SOURCES = meteograph.cc
meteoavg_SOURCES = meteoavg.cc
meteodequeue_SOURCES = meteodequeue.cc
meteowatch_SOURCES = meteowatch.cc
meteopoll_SOURCES = meteopoll.cc

test:	meteopoll
	./meteopoll -s Frauenfeld -l file://`pwd`/logfile -b msgqueue -d -d -d -f /usr/local/etc/meteo.xml

