#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.26 2002/01/09 23:36:06 afm Exp $
#
lib_LTLIBRARIES = libmeteo.la
man_MANS = meteo.1 meteoavg.1 meteoloop.1 meteodequeue.1 meteograph.1	\
	meteo.conf.5
noinst_DATA = meteo.html meteodata.html rc.meteo
EXTRA_DIST = shtool TODO meteojob meteooverview scanner.l parser.y 	\
	meteo.conf $(noinst_DATA)					\
	meteo.1.in meteoavg.1 meteoloop.1 meteodequeue.1 meteograph.1	\
	meteo.conf.5
METEOCONFFILE = \"@METEOCONFFILE@\"
CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE)

pkgdata_DATA = meteo.conf

LDADD = libmeteo.la
VERSION_FILE = meteo_vers.c

BUILT_SOURCES = scanner.c parser.c parser.h

scanner.c:	scanner.l
	$(LEX) -Pmp -oscanner.c scanner.l

parser.c:	parser.y
	$(YACC) -t -b parser -d -p mp parser.y
	mv parser.tab.c parser.c
parser.h:	parser.c
	mv parser.tab.h parser.h

noinst_HEADERS = com.h crc.h davis.h meteo.h meteodata.h sercom.h 	\
	tcpcom.h conversions.h dloop.h graph.h dewpoint.h msgque.h	\
	dbupdate.h meteosocket.h meteograph.h mconf.h parser.h		\
	average.h database.h timestamp.h dograph.h daemon.h watchdog.h

libmeteo_la_LDFLAGS = -version-info `./shtool version -d libtool $(VERSION_FILE)`

libmeteo_la_SOURCES = crc.c com.c sercom.c tcpcom.c davis.c 		\
	meteodata.c meteo.c conversions.c dloop.c graph.c dewpoint.c	\
	msgque.c dbupdate.c meteosocket.c mconf.c			\
	scanner.c parser.c average.c database.c timestamp.c dograph.c	\
	daemon.c watchdog.c						\
	$(VERSION_FILE)

meteo_vers.c:	configure.in
	./shtool version -l c -s @VERSION@ -n Meteo -p meteo_ meteo_vers.c

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

meteotest_LDADD = libmeteo.la
meteoupdate_LDADD = libmeteo.la
meteoloop_LDADD = libmeteo.la
meteograph_LDADD = libmeteo.la
meteocheck_LDADD = libmeteo.la
meteoavg_LDADD = libmeteo.la
meteodequeue_LDADD = libmeteo.la
meteowatch_LDADD = libmeteo.la

install-exec-local:
	cp meteojob meteooverview $(bindir)

test:	meteotest
	./meteotest -d -a file:///dev/cua/1
test2:	meteoupdate
	./meteoupdate -d -u meteoupdate -p m57cwrt -h timon 		\
		-w file:///dev/cua/1 -s Altendorf
test3:	meteoloop meteo.conf
	./meteoloop -F -d -d -n 10 -w 10 -f `pwd`/meteo.conf
test4:	meteograph meteo.conf
	./meteograph -d -d -d -c images -f `pwd`/meteo.conf 300
