#! /bin/sh
# 
# buildgallery.sh -- create the gallery.xml file to later construct
#                    the gallery.html page
#
# (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
# $Id: buildgallery.sh,v 1.1 2004/04/30 13:25:13 afm Exp $
#
(
	sed -n -e '1,/GALLERY/p' gallery.xml.in | grep -v GALLERY
	(
		echo '<?xml version="1.0" encoding="UTF-8"?>'
		echo '<gallery>'
		cat ../gallery/*.xml
		echo '</gallery>'
	) | xsltproc gallery.xsl -
	sed -n -e '/GALLERY/,$p' gallery.xml.in | grep -v GALLERY
) > gallery.xml
