#! /bin/sh
#
# This script displays the current station data by reading it from the mapfile
# preconditions:
#   - you need a suitable directory on your web server which should also be
#     your meteo server
#   - run meteopoll with the -m option so that meteopoll writes the map
#     file to meteo.xml in that directory
# setup
#   - copy this script an current.xsl to the same directory
#   - make the script executable as a CGI script to the webserver
#   - make sure meteomap and xsltproc are on the PATH of the webserver,
#     if this is not the case, fully qualify the commands below
#
# suggestion if your meteo server is not the webserver:
#   add meteomap to inetd.conf on your meteo server so that whoever connects
#   to this port gets a dump of the map. Then use netcat on the webserver to
#   read the meteomap from the meteo server.
#   addition to /etc/services:
#
#   meteomap        1353/tcp
#
#   addition to /etc/inetd.conf
#
#   meteomap         stream tcp nowait www /usr/local/bin/meteomap meteomap /var/spool/meteo.xml
#
# (c) 2006 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: current.cgi,v 1.1 2006/05/16 11:19:54 afm Exp $
#
echo "Content-Type: text/html"
echo 
if [ -r meteo.xml ]
then
	# ok, we have meteo.xml, dump it atomically
	meteomap meteo.xml
then
	# well, there is no meteo.xml, but we could try netcat.
	# This works only if
	# 1. netcat is available
	# 2. meteo is a DNS name for the meteo server
	# 3. meteomap is installed as a service in inetd.conf on the
	#    meteo server, as described above
	netcat meteo 1353
fi | xsltproc current.xsl -
