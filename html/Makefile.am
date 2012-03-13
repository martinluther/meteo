#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.6 2003/06/10 04:19:12 afm Exp $
#
HTMLFILES = meteo.html meteodata.html meteo-install.html meteo.css
TEXTFILES = ../README ../ChangeLog ../NEWS ../AUTHORS ../INSTALL \
	../COPYING ../SETUP
HTMLDIR = @HTMLDIR@
EXTRA_DIST = meteo.html.in meteodata.html meteo-install.html meteo.css

install-data-local:
	@if [ ! -z "$(HTMLDIR)" ];					\
	then								\
		echo "installing the HTML documentation in $(HTMLDIR)";	\
		cp $(HTMLFILES) $(TEXTFILES) "$(HTMLDIR)";		\
	else								\
		echo "info: HTML docs not installed, used --with-html-dir"; \
	fi

