#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.2 2002/01/19 00:36:58 afm Exp $
#
HTMLFILES = meteo.html meteodata.html meteo.css
TEXTFILES = ../README ../ChangeLog ../NEWS ../AUTHORS ../COPYING
HTMLDIR = @HTMLDIR@
EXTRA_DIST = meteo.html.in meteodata.html meteo.css

install-data-local:
	@if [ ! -z "$(HTMLDIR)" ];					\
	then								\
		echo "installing the HTML documentation in $(HTMLDIR)";	\
		cp $(HTMLFILES) $(TEXTFILES) "$(HTMLDIR)";		\
	else								\
		echo "info: HTML docs not installed, used --with-html-dir"; \
	fi
