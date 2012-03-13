#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.4 2003/06/06 07:04:04 afm Exp $
#
EXTRA_DIST = Appendix.txt Techref.txt faq.txt vtechref.txt uds10

uds10:	uds10.in
	nroff uds10.in | uniq > uds10
