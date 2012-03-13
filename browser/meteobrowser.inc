<?php
//
//   meteobrowser.inc -- configuration data for the meteobrowser.php
//
//   (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
//
//   $Id: meteobrowser.inc,v 1.3 2002/01/30 12:41:16 afm Exp $
//

// include the following only if your PHP installation lacks gettext support
// not really a problem right now, as the i18n has not been done yet.
// include("dummygettext.inc");

// the start time contains the time of the first data point a station can
// offer, used to prevent navigation into the emty past
$stationstart[Altendorf] = 985898700;
$stationstart[Gurin] = 1009452900;

// the graphlist contains a colon separated list of graphs to be drawn 
// for each station that can be browsed through the current instance of
// the script, see the sample file for instructions.
$graphlist[Altendorf] = "temperature:pressure:wind:rain";
$graphlist[Gurin] = "temperature:pressure:wind:radiation:rain";

// the path the the meteograph binary, could be derived from the configure
// run, but as quite a few things have to be configured in the script anyway,
// doing it manually isn't really more complicated.
$meteograph = "/usr/local/bin/meteograph";

// the directory containing the configuration files, which must have names
// of the form $confpath/meteo-$stationname.conf
$confpath = "/home/afm/Projects/meteo";

// a directory writable by the webserver just below the installation location
// of this script, or at least so that a browser sees it just besides 
// meteobrowser.php
$cachedir = "/home/afm/Projects/meteo/cache";

// the interval after after which a graph must be recomputed, even if it
// is unlikely that new data has arrived.
$rintval = 86400;	// recompute graphs older than a day

// defaults for the script, used to display something useful to the user
// if called without any parameters (so that it can be used as an index
// page)
$level_default = "year";
$midtime_default = time();	// not a particularly good value, but the
				// normalization below will fix that
$station_default = "Altendorf";	// could use the environment variables to
				// find out which station to display so that
				// the script could be used with virtual hosts
$language_default = "en_US";	// default language
?>
