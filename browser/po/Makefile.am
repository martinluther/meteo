#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.1 2003/05/29 18:23:34 afm Exp $
#
BROWSERDIR = @BROWSERDIR@
LOCDIR = $(BROWSERDIR)/locale

EXTRA_DIST = meteobrowser.pot de.po en.po
MOFILES = de.mo en.mo

de.mo:	de.po
	msgfmt $< -o $@
en.mo:	en.po
	msgfmt $< -o $@

install-data-local:	$(MOFILES)
	@if [ ! -z "$(BROWSERDIR)" ];					\
	then								\
		echo "installing meteobrowser files in $(BROWSERDIR)";	\
		../../shtool mkdir -p $(LOCDIR)/en_US/LC_MESSAGES;	\
		cp en.mo $(LOCDIR)/en_US/LC_MESSAGES/meteobrowser.mo;	\
		../../shtool mkdir -p $(LOCDIR)/de_DE/LC_MESSAGES;	\
		cp de.mo $(LOCDIR)/de_DE/LC_MESSAGES/meteobrowser.mo;	\
	fi
