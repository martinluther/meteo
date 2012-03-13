#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.1 2002/01/18 23:34:22 afm Exp $
#
BROWSERFILES = meteobrowser.php						\
	up.png upup.png upupup.png week.png month.png year.png 		\
	next.png previous.png						\
	dummygettext.inc
BROWSERDIR = @BROWSERDIR@

EXTRA_DIST = README.browser meteobrowser.inc $(BROWSERFILES)

install-data-local:
	@if [ ! -z "$(BROWSERDIR)" ];					\
	then								\
		echo "installing meteobrowser files in $(BROWSERDIR)";	\
		cp $(BROWSERFILES) ../html/meteo.css "$(BROWSERDIR)";	\
	else								\
		echo "warning: meteobrowser not installed, use --with-browser-dir"; \
	fi
