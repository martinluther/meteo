#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.21 2003/08/24 14:46:40 afm Exp $
#

noinst_HEADERS =							\
	MeteoException.h MeteoValue.h Vector.h Wind.h Rain.h		\
	Channel.h Timeval.h Station.h Configuration.h			\
	Datasink.h Pidfile.h Converter.h Timelabel.h Dewpoint.h		\
	Tdata.h Query.h Dataset.h Frame.h GraphWindow.h Graphics.h	\
	Datarecord.h							\
	crc.h meteo.h 							\
	msgque.h							\
	meteograph.h xmlconf.h						\
	average.h database.h timestamp.h daemon.h 			\
	printver.h mdebug.h pdebug.h colors.h

VERSION_FILE = meteo_vers.h

EXTRA_DIST = $(VERSION_FILE)

all-local:	$(VERSION_FILE)

meteo_vers.h:	../configure.in
	../shtool version -l c -s @VERSION@ -n Meteo -p meteo_ $(VERSION_FILE)

