#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.37 2002/11/24 19:48:01 afm Exp $
#
VERSION = @VERSION@

SUBDIRS = include lib src conf html browser db doc man support
BUILTSOURCES = README
EXTRA_DIST = shtool README.tmpl

README: 	README.tmpl
	(figlet meteo-$(VERSION); cat README.tmpl) > README

