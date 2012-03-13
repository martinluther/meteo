#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.5 2002/11/24 23:16:48 afm Exp $
#
METEOCONFFILE = \"@METEOCONFFILE@\"
CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE)

LDADD = ../lib/libmeteo.la

bin_PROGRAMS = meteoupdate meteoloop meteograph meteoavg		\
	meteodequeue meteowatch meteolast

meteoupdate_SOURCES = meteoupdate.c
meteoloop_SOURCES = meteoloop.c
meteograph_SOURCES = meteograph.c
meteoavg_SOURCES = meteoavg.c
meteodequeue_SOURCES = meteodequeue.c
meteowatch_SOURCES = meteowatch.c
meteolast_SOURCES = meteolast.c

test:	meteoloop
	./meteoloop -m -l file://`pwd`/logfile -d -F -w 2 -f `pwd`/../conf/meteo.conf

