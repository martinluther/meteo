#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.24 2003/08/24 20:26:40 afm Exp $
#
METEOCONFFILE = \"@METEOCONFFILE@\"
AM_CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE) @LIBXML_CFLAGS@

LDADD = ../lib/libmeteo.la

bin_PROGRAMS = meteopoll 	 				\
	meteoavg meteolast					\
	meteodequeue meteowatch meteodraw meteolabel meteodbdump
 
meteoavg_SOURCES = meteoavg.cc
meteodequeue_SOURCES = meteodequeue.cc
meteowatch_SOURCES = meteowatch.cc
meteopoll_SOURCES = meteopoll.cc
meteodraw_SOURCES = meteodraw.cc
meteolabel_SOURCES = meteolabel.cc
meteolast_SOURCES = meteolast.cc
meteodbdump_SOURCES = meteodbdump.cc

test:	meteopoll
	./meteopoll -s Frauenfeld -l file://`pwd`/logfile -b msgqueue -d -d -d -f /usr/local/etc/meteo.xml

#
# draw standard graph
#
#		-f `pwd`/../conf/meteo.xml -I -p ../src/ 
testdraw:	meteodraw
	DYLD_BIND_AT_LAUNCH=1 ./meteodraw -s Gurin -l file:///- -d -d \
		-f /usr/local/etc/meteo.xml -I -p ../src/ \
		-u "/browser/meteobrowser.php?lang=de" -I \
		-L m200304 \
		-g rain 
#
# drawing high resolution graphs uses a different configuration file
#
testdraw60:
	DYLD_BIND_AT_LAUNCH=1 ./meteodraw -s Gurin -l file:///- -d -d \
		-f `pwd`/../conf/meteo-60.xml -I -p ../src/ \
		-u "/browser/meteobrowser.php?lang=de" \
		-L d20030719 -g temperature -g temperature_inside -g pressure \
		-g radiation -g rain -g wind -g humidity -g humidity_inside -m
