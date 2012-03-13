#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.1 2002/01/18 23:34:25 afm Exp $
#

BUILT_SOURCES = parser.h

../lib/parser.c:	../lib/parser.y
	$(YACC) -t -b parser -d -p mp ../lib/parser.y
	mv parser.tab.c ../lib/parser.c
parser.h:	../lib/parser.c
	mv parser.tab.h parser.h

noinst_HEADERS = com.h crc.h davis.h meteo.h meteodata.h sercom.h 	\
	tcpcom.h conversions.h dloop.h graph.h dewpoint.h msgque.h	\
	dbupdate.h meteosocket.h meteograph.h mconf.h parser.h		\
	average.h database.h timestamp.h dograph.h daemon.h watchdog.h	\
	printver.h

