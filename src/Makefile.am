#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.42 2003/11/02 21:41:02 afm Exp $
#
METEOCONFFILE = \"@METEOCONFFILE@\"
AM_CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE) @LIBXML_CFLAGS@

LDADD = ../lib/libmeteo.la

bin_PROGRAMS = meteopoll meteoavg meteolast				\
	meteodequeue meteodraw meteolabel meteodbdump meteosensors
 
meteoavg_SOURCES = meteoavg.cc
meteodequeue_SOURCES = meteodequeue.cc
meteopoll_SOURCES = meteopoll.cc
meteodraw_SOURCES = meteodraw.cc
meteolabel_SOURCES = meteolabel.cc
meteolast_SOURCES = meteolast.cc
meteodbdump_SOURCES = meteodbdump.cc
meteosensors_SOURCES = meteosensors.cc

test:	meteopoll
	cp /dev/null /var/tmp/meteopoll.log
	./meteopoll -F -s Altendorf -l file:///var/tmp/meteopoll.log \
		-p `pwd`/meteopoll- -b msgqueue \
		-d -d -d -f `pwd`/../conf/meteo.xml

#
# draw standard graph
#
#		-f `pwd`/../conf/meteo.xml -I -p ../src/ 
testdraw:	meteodraw
	cp /dev/null /var/tmp/meteodraw.log
	DYLD_BIND_AT_LAUNCH=1 ./meteodraw -s Lajeado -d -d \
		-l file:///var/tmp/meteodraw.log \
		-f ../conf/meteo.xml -I -p ../src/ \
		-u "/browser/meteobrowser.php?lang=de" -I \
		-L d20030930 \
		-g Lajeado.rain -g Lajeado.temperature -g Lajeado.humidity \
		-g Lajeado.wind -g Lajeado.radiation

testmap:	meteodraw
	DYLD_BIND_AT_LAUNCH=1 ./meteodraw -s Lajeado -d -d \
		-f ../conf/meteo.xml -I -p ../src/ \
		-u "/browser/meteobrowser.php?lang=de" -I \
		-L m200309 -m \
		-g Lajeado.rain 
	
#
# drawing high resolution graphs uses a different configuration file
#
testdraw60:
	cp /dev/null /var/tmp/meteodraw.log
	DYLD_BIND_AT_LAUNCH=1 ./meteodraw -s Gurin \
		-l file:///var/tmp/meteodraw.log -d -d \
		-f `pwd`/../conf/meteo-60.xml -I -p ../src/ \
		-u "/browser/meteobrowser.php?lang=de" \
		-L d20030719 -g temperature -g temperature_inside -g pressure \
		-g radiation -g rain -g wind -g humidity -g humidity_inside -m

#
# test computing averages
#
testavg:	meteoavg
	cp /dev/null /var/tmp/meteoavg.log
	DYLD_BIND_AT_LAUNCH=1 ./meteoavg -sLajeado \
		-l file:///var/tmp/meteoavg.log \
		-i 300 -f `pwd`/../conf/meteo.xml -a \
		20030901000000 20031002160000

testavgd:	meteoavg
	cp /dev/null /var/tmp/meteoavg.log
	DYLD_BIND_AT_LAUNCH=1 ./meteoavg -sLajeado -p `pwd`/meteoavg- \
		-l file:///var/tmp/meteoavg.log -d \
		-f `pwd`/../conf/meteo.xml

#
# test dequeuer
#
testdequeue:	meteodequeue
	DYLD_BIND_AT_LAUNCH=1 ./meteodequeue \
		-p `pwd`/meteodequeue- \
		-d -f `pwd`/../conf/meteo.xml

#
# test the meteolast program
#
testlast:	meteolast
	DYLD_BIND_AT_LAUNCH=1 ./meteolast -x -d -w 1000000000 \
		-f `pwd`/../conf/meteo.xml Lajeado

#
# test the dump programm
#
testdump:	meteodbdump
	DYLD_BIND_AT_LAUNCH=1 ./meteodbdump -r -s Lajeado \
		-l file:///- -d -b testdump- -f `pwd`/../conf/meteo.xml avg
	
