<?php
//
//   meteobrowser.inc -- configuration data for the meteobrowser.php
//
//   (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
//
//   $Id: meteobrowser.inc,v 1.8 2003/07/26 22:39:43 afm Exp $
//

// include the following only if your PHP installation lacks gettext support
// not really a problem right now, as the i18n has not been done yet.
//include("dummygettext.inc");

// the start time contains the time of the first data point a station can
// offer, used to prevent navigation into the emty past
$stationstart[Altendorf] = 985898700;
$stationstart[Gurin] = 1009452900;

// the graphlist contains a colon separated list of graphs to be drawn 
// for each station that can be browsed through the current instance of
// the script, see the sample file for instructions.
$graphlist[Altendorf] = "temperature:pressure:wind:rain";
$graphlist[Gurin] = "temperature:pressure:wind:radiation:rain";

// the conflist array contains configuration file names in case different
// stations need different configuration files (this may happen if scales
// suitable for one station are less useful for some other station). If the
// are does not contain anything for a given station, the standard configuration
// file is used instead.
$conflist[Gurin] = "/usr/local/etc/meteo-Gurin.xml";

// the path to the meteodraw and meteolabel binaries, could be derived
// from the configure run, but as quite a few things have to be configured
// in the script anyway, doing it manually isn't really more complicated.
$meteodraw = "/usr/local/bin/meteodraw";
$meteolabel = "/usr/local/bin/meteolabel";

// the URL path used as a bases for URLs embedded in the generated page
$pageurl = "/meteo/browser/meteobrowser.php";

// the directory containing the configuration files, which must have names
// of the form $confpath/meteo.xml
$confpath = "/usr/local/etc";

// a directory writable by the webserver just below the installation location
// of this script, or at least so that a browser sees it just besides 
// meteobrowser.php
$cachedir = "cache/";

// defaults for the script, used to display something useful to the user
// if called without any parameters (so that it can be used as an index
// page)
$label_default = "m".date("Ym");// default label is the current month
$station_default = "Altendorf";	// could use the environment variables to
				// find out which station to display so that
				// the script could be used with virtual hosts
$language_default = "en_US";	// default language
?>
