#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.17 2004/05/08 22:39:43 afm Exp $
#
HTML =	ack.html gallery.html prereq.html install.html util.html 	\
	news.html about.html config.html gallery.html
HTMLFILES = meteodata.html meteo-install.html meteo.css $(HTML)
IMAGES = meteo.jpg
TEXTFILES = ../README ../ChangeLog ../NEWS ../AUTHORS ../INSTALL \
	../COPYING ../SETUP
HTMLDIR = @HTMLDIR@
EXTRA_DIST = meteodata.html meteo-install.html meteo.css gallery.xml	\
	about.xml ack.xml config.xml news.xml.in prereq.xml util.xml 	\
	$(IMAGES) meteo.xsl $(HTML) gallery_template.xml 		\
	gallery.xml.in gallery.xsl buildgallery.sh

install-data-local:	html
	@if [ ! -z "$(HTMLDIR)" ];					\
	then								\
		echo "installing the HTML documentation in $(HTMLDIR)";	\
		mkdir -p "$(HTMLDIR)";					\
		cp $(HTMLFILES) $(TEXTFILES) $(IMAGES) "$(HTMLDIR)";	\
	else								\
		echo "info: HTML docs not installed, used --with-html-dir"; \
	fi


%.html : %.xml ; xsltproc meteo.xsl $< > $@

html:	$(HTML)

gallery.xml:	gallery.xml.in gallery.xsl
	./buildgallery.sh
