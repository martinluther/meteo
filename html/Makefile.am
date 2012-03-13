#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.5 2003/06/09 00:27:37 afm Exp $
#
HTMLFILES = meteo.html meteodata.html meteo-install.html meteo.css
TEXTFILES = ../README ../ChangeLog ../NEWS ../AUTHORS ../COPYING
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

