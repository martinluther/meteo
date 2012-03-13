#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.4 2003/08/03 21:24:38 afm Exp $
#
noinst_DATA = rc.meteo
EXTRA_DIST = meteojob meteooverview How_to_migrate $(noinst_DATA)

install-exec-local:
	cp meteojob $(DESTDIR)$(bindir)/meteojob.default
	cp meteooverview $(DESTDIR)$(bindir)/meteooverview.default

bin_PROGRAMS = meteofixtimestamps

meteofixtimestamps_SOURCES = meteofixtimestamps.c
