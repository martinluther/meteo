#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.40 2003/06/08 22:50:00 afm Exp $
#
VERSION = @VERSION@

SUBDIRS = include lib src conf html browser db doc man support wunderground
BUILTSOURCES = README
EXTRA_DIST = shtool README.tmpl SETUP configureit

README: 	README.tmpl
	(figlet meteo-$(VERSION); cat README.tmpl) > README

