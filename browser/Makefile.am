#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.5 2006/05/07 19:47:22 afm Exp $
#
BROWSERFILES = meteobrowser.php current.php				\
	up.png upup.png upupup.png week.png month.png year.png 		\
	next.png previous.png
BROWSERDIR = @BROWSERDIR@

EXTRA_DIST = README.browser meteobrowser.inc $(BROWSERFILES)

SUBDIRS = po

install-data-local:
	@if [ ! -z "$(BROWSERDIR)" ];					\
	then								\
		echo "installing meteobrowser files in $(BROWSERDIR)";	\
		cp $(BROWSERFILES) ../html/meteo.css "$(BROWSERDIR)";	\
		cp meteobrowser.inc $(BROWSERDIR)/meteobrowser.inc.default; \
	else								\
		echo "warning: meteobrowser not installed, use --with-browser-dir"; \
	fi
