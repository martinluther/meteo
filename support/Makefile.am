#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.1 2002/01/30 10:41:31 afm Exp $
#
noinst_DATA = rc.meteo
EXTRA_DIST = meteojob meteooverview $(noinst_DATA)

install-exec-local:
	cp meteojob meteooverview $(bindir)

