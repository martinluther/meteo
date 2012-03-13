#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.4 2003/07/26 07:42:44 afm Exp $
#
BROWSERDIR = @BROWSERDIR@
LOCDIR = $(BROWSERDIR)/locale

EXTRA_DIST = meteobrowser.pot de.po en.po pt_BR.po
MOFILES = de.mo en.mo pt_BR.mo

de.mo:	de.po
	msgfmt -o $@ $<

en.mo:	en.po
	msgfmt -o $@ $<

pt_BR.mo:	pt_BR.po
	msgfmt -o $@ $<

all-local:	de.mo en.mo pt_BR.mo

install-data-local:	$(MOFILES)
	@if [ ! -z "$(BROWSERDIR)" ];					\
	then								\
		echo "installing meteobrowser files in $(BROWSERDIR)";	\
		../../shtool mkdir -p $(LOCDIR)/en_US/LC_MESSAGES;	\
		cp en.mo $(LOCDIR)/en_US/LC_MESSAGES/meteobrowser.mo;	\
		../../shtool mkdir -p $(LOCDIR)/de_DE/LC_MESSAGES;	\
		cp de.mo $(LOCDIR)/de_DE/LC_MESSAGES/meteobrowser.mo;	\
		../../shtool mkdir -p $(LOCDIR)/pt_BR/LC_MESSAGES;	\
		cp pt_BR.mo $(LOCDIR)/pt_BR/LC_MESSAGES/meteobrowser.mo;\
	fi
