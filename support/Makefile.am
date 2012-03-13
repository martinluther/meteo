#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.3 2003/08/01 00:01:52 afm Exp $
#
noinst_DATA = rc.meteo
EXTRA_DIST = meteojob meteooverview How_to_migrate $(noinst_DATA)

install-exec-local:
	cp meteojob $(bindir)/meteojob.default
	cp meteooverview $(bindir)/meteooverview.default

bin_PROGRAMS = meteofixtimestamps

meteofixtimestamps_SOURCES = meteofixtimestamps.c
