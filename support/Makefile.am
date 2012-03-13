#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.9 2004/05/08 22:07:17 afm Exp $
#
noinst_DATA = rc.meteo
EXTRA_DIST = meteojob meteooverview meteodbmigrate How_to_migrate	\
	How_to_migrate2 README $(noinst_DATA) 				\
	meteo StartupParameters.plist

install-exec-local:
	cp meteojob $(DESTDIR)$(bindir)/meteojob.default
	cp meteooverview $(DESTDIR)$(bindir)/meteooverview.default

bin_PROGRAMS = meteofixtimestamps meteodbmigrate

meteofixtimestamps_SOURCES = meteofixtimestamps.c
meteodbmigrate_SOURCES = meteodbmigrate.cc

testmig:	meteodbmigrate
	./meteodbmigrate -o -10800 -b meteo -u meteoupdate -p m57cwrt \
		-t VantagePro -s Lajeado
