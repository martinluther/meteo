#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.12 2003/11/19 22:27:07 afm Exp $
#
HTMLFILES = meteodata.html meteo-install.html meteo.css gallery.html	\
	about.html ack.html config.html index.html inhalt.html		\
	news.html requ.html uti.html get.html
IMAGES = inhaltband.jpg normband.jpg
TEXTFILES = ../README ../ChangeLog ../NEWS ../AUTHORS ../INSTALL \
	../COPYING ../SETUP
HTMLDIR = @HTMLDIR@
EXTRA_DIST = meteodata.html meteo-install.html meteo.css gallery.html	\
	about.html ack.html config.html index.html inhalt.html		\
	news.html.in requ.html uti.html get.html.in $(IMAGES)

install-data-local:
	@if [ ! -z "$(HTMLDIR)" ];					\
	then								\
		echo "installing the HTML documentation in $(HTMLDIR)";	\
		mkdir -p "$(HTMLDIR)";					\
		cp $(HTMLFILES) $(TEXTFILES) "$(HTMLDIR)";		\
	else								\
		echo "info: HTML docs not installed, used --with-html-dir"; \
	fi

