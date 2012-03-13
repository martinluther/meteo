#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.10 2003/11/01 18:00:30 afm Exp $
#
HTMLFILES = meteodata.html meteo-install.html meteo.css gallery.html	\
	about.html ack.html config.html index.html inhalt.html		\
	news.html requ.html uti.html
IMAGES = inhaltband.png normband.png
TEXTFILES = ../README ../ChangeLog ../NEWS ../AUTHORS ../INSTALL \
	../COPYING ../SETUP
HTMLDIR = @HTMLDIR@
EXTRA_DIST = meteodata.html meteo-install.html meteo.css gallery.html	\
	about.html ack.html config.html index.html inhalt.html		\
	news.html.in requ.html uti.html $(IMAGES)

install-data-local:
	@if [ ! -z "$(HTMLDIR)" ];					\
	then								\
		echo "installing the HTML documentation in $(HTMLDIR)";	\
		mkdir -p "$(HTMLDIR)";					\
		cp $(HTMLFILES) $(TEXTFILES) "$(HTMLDIR)";		\
	else								\
		echo "info: HTML docs not installed, used --with-html-dir"; \
	fi

