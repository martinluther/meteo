#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.6 2003/10/19 22:07:34 afm Exp $
#
EXTRA_DIST = Appendix.txt Techref.txt faq.txt vtechref.txt uds10 \
	architecture sensors

uds10:	uds10.in
	nroff uds10.in | uniq > uds10
