#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.2 2002/11/18 02:36:42 afm Exp $
#
noinst_DATA = rc.meteo
EXTRA_DIST = meteojob meteooverview How_to_migrate $(noinst_DATA)

install-exec-local:
	cp meteojob meteooverview $(bindir)

bin_PROGRAMS = meteofixtimestamps

meteofixtimestamps_SOURCES = meteofixtimestamps.c
