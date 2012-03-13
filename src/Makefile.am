#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.2 2002/01/27 22:55:20 afm Exp $
#
METEOCONFFILE = \"@METEOCONFFILE@\"
CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE)

LDADD = libmeteo.la

bin_PROGRAMS = meteoupdate meteoloop meteograph meteoavg		\
	meteodequeue meteowatch
noinst_PROGRAMS = meteocheck meteotest

meteotest_SOURCES = meteotest.c
meteoupdate_SOURCES = meteoupdate.c
meteoloop_SOURCES = meteoloop.c
meteograph_SOURCES = meteograph.c
meteocheck_SOURCES = meteocheck.c
meteoavg_SOURCES = meteoavg.c
meteodequeue_SOURCES = meteodequeue.c
meteowatch_SOURCES = meteowatch.c

meteotest_LDADD = ../lib/libmeteo.la
meteoupdate_LDADD = ../lib/libmeteo.la
meteoloop_LDADD = ../lib/libmeteo.la
meteograph_LDADD = ../lib/libmeteo.la
meteocheck_LDADD = ../lib/libmeteo.la
meteoavg_LDADD = ../lib/libmeteo.la
meteodequeue_LDADD = ../lib/libmeteo.la
meteowatch_LDADD = ../lib/libmeteo.la


test:	meteoloop
	./meteoloop -m -l file://`pwd`/logfile -d -F -w 2 -f `pwd`/../conf/meteo.conf
