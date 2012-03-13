#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.41 2003/10/31 21:15:51 afm Exp $
#
VERSION = @VERSION@

SUBDIRS = include lib src conf html browser db doc man support wunderground
BUILTSOURCES = README
EXTRA_DIST = shtool README.tmpl SETUP configureit BUGS

README: 	README.tmpl
	(figlet meteo-$(VERSION); cat README.tmpl) > README

