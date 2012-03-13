#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.43 2004/04/30 13:25:13 afm Exp $
#
VERSION = @VERSION@

SUBDIRS = include lib src conf html browser db doc man support \
	wunderground gallery
BUILTSOURCES = README
EXTRA_DIST = shtool README.tmpl SETUP configureit BUGS UPGRADE

README: 	README.tmpl
	(figlet meteo-$(VERSION); cat README.tmpl) > README

